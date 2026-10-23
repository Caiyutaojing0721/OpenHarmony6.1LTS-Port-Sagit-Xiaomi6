/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "sockets.h"

int socket_close(int sock) {
    return close(sock);
}

ssize_t socket_send_buffers(cutils_socket_t sock,
                            const cutils_socket_buffer_t* buffers,
                            size_t num_buffers) {
    if (num_buffers > SOCKET_SEND_BUFFERS_MAX_BUFFERS) {
        return -1;
    }

    iovec iovec_buffers[SOCKET_SEND_BUFFERS_MAX_BUFFERS];
    for (size_t i = 0; i < num_buffers; ++i) {
        // It's safe to cast away const here; iovec declares non-const
        // void* because it's used for both send and receive, but since
        // we're only sending, the data won't be modified.
        iovec_buffers[i].iov_base = const_cast<void*>(buffers[i].data);
        iovec_buffers[i].iov_len = buffers[i].length;
    }

    return writev(sock, iovec_buffers, num_buffers);
}


int android_get_control_socket(const char*) {
    return -1;
}