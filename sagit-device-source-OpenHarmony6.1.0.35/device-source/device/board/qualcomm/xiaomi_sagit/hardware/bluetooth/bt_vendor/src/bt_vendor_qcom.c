/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  Filename:      bt_vendor_qcom.c
 *
 *  Description:   Qcom vendor specific library implementation
 *
 ******************************************************************************/

#define LOG_TAG "bt_vendor"

#include <utils/Log.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <errno.h>
#include "upio.h"
#include "hci_uart.h"
#include "bt_vendor_qcom.h"
#include "cutils/sockets.h"

/******************************************************************************
**  Externs
******************************************************************************/

uint8_t hw_lpm_enable(uint8_t turn_on);
void hw_lpm_set_wake_state(uint8_t wake_assert);
void hw_config_start(void);

/******************************************************************************
**  Variables
******************************************************************************/

bt_vendor_callbacks_t *bt_vendor_cbacks = NULL;
uint8_t vnd_local_bd_addr[BD_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/******************************************************************************
**  Local type definitions
******************************************************************************/

/******************************************************************************
**  Static Variables
******************************************************************************/

static const tUSERIAL_CFG userial_init_cfg = {
    (USERIAL_DATABITS_8 | USERIAL_PARITY_NONE | USERIAL_STOPBITS_1),
    USERIAL_BAUD_115200
};

/******************************************************************************
**  Functions
******************************************************************************/

/*****************************************************************************
**
**   BLUETOOTH VENDOR INTERFACE LIBRARY FUNCTIONS
**
*****************************************************************************/
/** LPM disable/enable request */
typedef enum {
    BT_VND_LPM_DISABLE,
    BT_VND_LPM_ENABLE,
} bt_vendor_lpm_mode_t;

static int init(const bt_vendor_callbacks_t *p_cb, unsigned char *local_bdaddr)
{
    HILOGI("init, bdaddr:%{public}02x%{public}02x:%{public}02x%{public}02x:%{public}02x%{public}02x", 
        local_bdaddr[0], local_bdaddr[1], local_bdaddr[2],
        local_bdaddr[3], local_bdaddr[4], local_bdaddr[5]);

    if (p_cb == NULL) {
        HILOGE("init failed with no user callbacks!");
        return -1;
    }

#if (VENDOR_LIB_RUNTIME_TUNING_ENABLED == TRUE)
    HILOGW("*****************************************************************");
    HILOGW("*****************************************************************");
    HILOGW("** Warning - BT Vendor Lib is loaded in debug tuning mode!");
    HILOGW("**");
    HILOGW("** If this is not intentional, rebuild libbt-vendor.so ");
    HILOGW("** with VENDOR_LIB_RUNTIME_TUNING_ENABLED=FALSE and ");
    HILOGW("** check if any run-time tuning parameters needed to be");
    HILOGW("** carried to the build-time configuration accordingly.");
    HILOGW("*****************************************************************");
    HILOGW("*****************************************************************");
#endif

    userial_vendor_init();
    upio_init();

    /* store reference to user callbacks */
    bt_vendor_cbacks = (bt_vendor_callbacks_t *)p_cb;

#if (BRCM_A2DP_OFFLOAD == TRUE)
    brcm_vnd_a2dp_init(bt_vendor_cbacks);
#endif

    /* This is handed over from the stack */
    return memcpy_s(vnd_local_bd_addr, BD_ADDR_LEN, local_bdaddr, BD_ADDR_LEN);
}

int hci_up()
{
    int hdev = 0;
    int sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (sock < 0) {
        HILOGE("Can't open hci device");
        return sock;
    }

    /* Start HCI device */
    if (ioctl(sock, HCIDEVUP, hdev) < 0) {
        if (errno != EALREADY) {
            HILOGE("Can't init device hci%{public}d: %{public}s (%{public}d)\n", hdev, strerror(errno), errno);
            close(sock);
            return -1;
        }
    }

    close(sock);
    return 0;
}

int hci_down()
{
    int hdev = 0;
    int sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (sock < 0) {
        HILOGE("Can't open hci device");
        return sock;
    }

    /* Stop HCI device */
    if (ioctl(sock, HCIDEVDOWN, hdev) < 0) {
        HILOGE("Can't down device hci%{public}d: %{public}s (%{public}d)\n", hdev, strerror(errno), errno);
        close(sock);
        return -1;
    }

    close(sock);
    return 0;
}

int connect_to_local_socket() {
    struct sockaddr_hci addr;
    int sk = -1;
    int hdev = 0;

    HILOGE("%{public}s: ACCEPT ", __func__);
    sk  = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sk < 0) {
        HILOGE("Socket creation failure");
        return -1;
    }

    if(socket_local_client_connect(sk, "bluetooth_hci",
            ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM) < 0)
    {
        HILOGE("failed to connect (%s)\n", strerror(errno));
        close(sk);
        return -1;
    } else {
        HILOGI("%s: Connection succeeded\n", __func__);
    }

    return sk;
}

static int bt_vendor_rfkill(int block)
{
    struct rfkill_event event;
    int fd, len;

    HILOGI("%s", __func__);

    fd = open("/dev/rfkill", O_WRONLY);
    if (fd < 0) {
        HILOGE("Unable to open /dev/rfkill");
        return -1;
    }

    memset(&event, 0, sizeof(struct rfkill_event));
    event.op = RFKILL_OP_CHANGE_ALL;
    event.type = RFKILL_TYPE_BLUETOOTH;
    event.hard = block;
    event.soft = block;

    len = write(fd, &event, sizeof(event));
    if (len < 0) {
        HILOGE("Failed to change rfkill state");
        close(fd);
        return 1;
    }

    close(fd);

    return 0;
}

/** Requested operations */
static int op(bt_opcode_t opcode, void *param)
{
    int retval = 0;

    switch (opcode) {
        case BT_OP_POWER_ON:  // BT_VND_OP_POWER_CTRL
            HILOGI("Bluetooth BT_OP_POWER_ON");
            retval = bt_vendor_rfkill(0);
            hci_up();
            break;

        case BT_OP_POWER_OFF:  // BT_VND_OP_POWER_CTRL
            HILOGI("Bluetooth BT_OP_POWER_OFF");
            hci_down();
            retval = bt_vendor_rfkill(1);
            break;

        case BT_OP_HCI_CHANNEL_OPEN: {  // BT_VND_OP_USERIAL_OPEN
            HILOGI("Bluetooth BT_OP_HCI_CHANNEL_OPEN");
            int(*fd_array)[] = (int(*)[])param;
            int fd, idx;
            vnd_userial.fd = fd = connect_to_local_socket();
            if (fd != -1) {
                HILOGI("received the socket fd: %{public}d \n", fd);
                for (idx = 0; idx < HCI_MAX_CHANNEL; idx++)
                    (*fd_array)[idx] = fd;
                retval = 1;
            }
            else
            {
                HILOGE("Unable to connect to BT Server Socket!!!");
                retval = -1;
            }
            /* retval contains numbers of open fd of HCI channels */
            break;
        }
        case BT_OP_HCI_CHANNEL_CLOSE:  // BT_VND_OP_USERIAL_CLOSE
            HILOGI("Bluetooth BT_OP_HCI_CHANNEL_CLOSE");
            userial_vendor_close();
            break;

        case BT_OP_INIT:  // BT_VND_OP_FW_CFG
            HILOGI("Bluetooth BT_OP_INIT");
            hw_config_start();
            break;

        case BT_OP_GET_LPM_TIMER: { // BT_VND_OP_GET_LPM_IDLE_TIMEOUT
            HILOGI("Bluetooth BT_OP_GET_LPM_TIMER");
            uint32_t *timeout_ms = (uint32_t *) param;
            *timeout_ms = 10000;
            break;
        }

        case BT_OP_LPM_ENABLE:
            HILOGI("Bluetooth BT_OP_LPM_ENABLE");
            //retval = hw_lpm_enable(BT_VND_LPM_ENABLE);
            break;

        case BT_OP_LPM_DISABLE:
            HILOGI("Bluetooth BT_OP_LPM_DISABLE");
            //retval = hw_lpm_enable(BT_VND_LPM_DISABLE);
            break;

        case BT_OP_WAKEUP_LOCK:
            HILOGI("Bluetooth BT_OP_WAKEUP_LOCK");
            //hw_lpm_set_wake_state(TRUE);
            break;
        case BT_OP_WAKEUP_UNLOCK:
            HILOGI("Bluetooth BT_OP_WAKEUP_UNLOCK");
            //hw_lpm_set_wake_state(FALSE);
            break;
        case BT_OP_EVENT_CALLBACK:
            HILOGI("Bluetooth BT_OP_EVENT_CALLBACK");
            hw_process_event((HC_BT_HDR *)param);
            break;
    }

    return retval;
}

/** Closes the interface */
static void cleanup(void)
{
    HILOGI("cleanup");
    upio_cleanup();
    bt_vendor_cbacks = NULL;
}

// Entry point of DLib
const bt_vendor_interface_t BLUETOOTH_VENDOR_LIB_INTERFACE = {
    sizeof(bt_vendor_interface_t),
    init,
    op,
    cleanup};
