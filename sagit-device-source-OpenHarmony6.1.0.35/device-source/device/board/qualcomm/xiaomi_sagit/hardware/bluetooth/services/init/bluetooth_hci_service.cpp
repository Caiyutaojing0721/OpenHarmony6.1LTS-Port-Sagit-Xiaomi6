/*
 * Copyright (c) 2023 AlgoIdeas
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "cutils/sockets.h"

#include <thread_ex.h>
#include <iostream>
#include <cstdio>
#include <sys/prctl.h>
#include <signal.h>
#include "hilog/log.h"

#undef LOG_TAG
#undef LOG_DOMAIN
#define LOG_TAG "BluetoothHciSvr"
#define LOG_DOMAIN 0xD002100

#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)

#ifndef BT_HCI_LOGI
#define BT_HCI_LOGI(format, ...)                                                                                     \
    do {                                                                                                              \
        HILOG_INFO(LOG_CORE, "[%{public}s@%{public}s:%{public}d] " format "\n", __FUNCTION__, __FILENAME__, __LINE__, \
            ##__VA_ARGS__);                                                                                           \
    } while (0)
#endif

#define BTPROTO_HCI 1
#define HCIDEVUP    _IOW('H', 201, int)
#define HCIDEVDOWN  _IOW('H', 202, int)

struct HciSockAddr
{
    sa_family_t hciFamily;
    unsigned short hciDev;
    unsigned short hciChannel;
};

/* HCI Packet types */
#define HCI_COMMAND_PKT 0x01
#define HCI_ACLDATA_PKT 0x02
#define HCI_SCODATA_PKT 0x03
#define HCI_EVENT_PKT 0x04
#define HCI_ISODATA_PKT 0x05
#define HCI_VENDOR_PKT 0xff

#define HCI_FLT_TYPE_BITS 31
#define HCI_FLT_EVENT_BITS 63
#define HCI_FLT_OGF_BITS 63
#define HCI_FLT_OCF_BITS 127

#define cmd_opcode_pack(ogf, ocf) (uint16_t)((ocf & 0x03ff) | (ogf << 10))

#define HCI_EVENT_HDR_SIZE    2
#define HCI_COMMAND_HDR_SIZE  3

#define SOL_HCI     0
#define HCI_FILTER  2

struct HciFilter
{
    uint32_t typeMask;
    uint32_t eventMask[2];
    uint16_t opcode;
};

using ThreadRunFunc = bool (*)(int &sock, int &btSock);

static int btSock_ = -1;

class BTHciThread : public OHOS::Thread
{
public:
    BTHciThread()
        : sock_(0), btSock_(0){};
    ~BTHciThread() {}

    void setRunFunc(const int sock, const int btSock, ThreadRunFunc runFunc)
    {
        sock_ = sock;
        btSock_ = btSock;
        runFunc_ = runFunc;
    }

    void setBtRunFunc(const int sock, ThreadRunFunc runFunc)
    {
        btSock_ = sock;
        btRunFunc_ = runFunc;
    }

    void setSock(const int sock)
    {
        sock_ = sock;
    }

    int sock_;
    int btSock_;

protected:
    bool Run() override;

private:
    ThreadRunFunc runFunc_ = nullptr;
    ThreadRunFunc btRunFunc_= nullptr;
};

bool BTHciThread::Run()
{
    if (runFunc_ != nullptr)
    {
        return (*runFunc_)(sock_, btSock_);
    }

    if (btRunFunc_ != nullptr)
    {
        return (*btRunFunc_)(sock_, btSock_);
    }

    return false;
}

static inline void hciFilterClear(struct HciFilter *f)
{
    memset(f, 0, sizeof(*f));
}

static inline void hciFilterAllPtypes(struct HciFilter *f)
{
    memset((void *) &f->typeMask, 0xff, sizeof(f->typeMask));
}

static inline void hciFilterAllEvents(struct HciFilter *f)
{
    memset((void *)f->eventMask, 0xff, sizeof(f->eventMask));
}

bool BtRecvRun(int &sock, int &btSock)
{
    char buf[260];
    int n;

    n = read(btSock, buf, sizeof(buf));
    if (n > 0)
    {
        write(sock, buf, n);
    }

    return true;
}

bool ClientRun(int &sock, int &btSock)
{
    char buf[260];
    int n;

    n = read(sock, buf, sizeof(buf));
    if (n > 0)
    {
        if (write(btSock, buf, n) <= 0)
        {
            BT_HCI_LOGI("write btSock failed, btSock: %{public}d, msg: %{public}s", btSock, strerror(errno));
            return false;
        }
    }
    else if (errno != EAGAIN && errno != EINTR)
    {
        BT_HCI_LOGI("client task finish\n");
        return false;
    }

    return true;
}

static BTHciThread *btHciThread;

static int BTHciInit(void)
{
    struct HciSockAddr addr;
    int hdev = 0;
    struct HciFilter flt;

    signal(SIGPIPE, SIG_IGN);
    btSock_ = socket(AF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC, BTPROTO_HCI);
    if (btSock_ < 0)
    {
        BT_HCI_LOGI("Socket creation failure\n");
        return -1;
    }

    /* Bind socket to the HCI device */
    memset(&addr, 0, sizeof(addr));
    addr.hciFamily = AF_BLUETOOTH;
    addr.hciDev = hdev;
    if (bind(btSock_, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        close(btSock_);
        BT_HCI_LOGI("Socket bind failure");
        return -1;
    }

    BT_HCI_LOGI("bt socket %{public}d\n", btSock_);

    /* Setting filter */
    hciFilterClear(&flt);
    hciFilterAllPtypes(&flt);
    hciFilterAllEvents(&flt);

    if (setsockopt(btSock_, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0)
    {
        BT_HCI_LOGI("HCI filter setup failed");
        return -1;
    }

    btHciThread = new BTHciThread();
    btHciThread->setBtRunFunc(btSock_, BtRecvRun);
    btHciThread->Start("bluetooth_hci_server", OHOS::THREAD_PROI_NORMAL, 1024);

    return 0;
}

int BTCreatLocalSocket()
{
    int sk = -1;

    sk = socket_local_server("bluetooth_hci", ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
    if (sk < 0)
    {
        close(sk);
        BT_HCI_LOGI("Socket bind failure: %{public}s (%{public}d)\n", strerror(errno), errno);
        return -1;
    }

    BT_HCI_LOGI("bind to local socket: %{public}d\n", sk);

    return sk;
}

int main(int argc, char *argv[])
{
    int sock;
    int clientSock;
    socklen_t clientAddrLen;
    struct sockaddr_in clientAddr;
    clientAddrLen = sizeof(clientAddr);
    BTHciThread *client;

    BTHciInit();

    sock = BTCreatLocalSocket();
    BT_HCI_LOGI("BTCreatLocalSocket\n");

    for (;;)
    {
        clientSock = accept(sock, (struct sockaddr *)&clientAddr, &clientAddrLen);
        BT_HCI_LOGI("connect success\n");
        btHciThread->setSock(clientSock);

        client = new BTHciThread();
        client->setRunFunc(clientSock, btSock_, ClientRun);
        client->Start("bluetooth_hci_client", OHOS::THREAD_PROI_NORMAL, 1024);
        sleep(1);
    }

    BT_HCI_LOGI("bluetooth_hci service Exit!\n");

    close(btSock_);
    close(sock);

    return 0;
}