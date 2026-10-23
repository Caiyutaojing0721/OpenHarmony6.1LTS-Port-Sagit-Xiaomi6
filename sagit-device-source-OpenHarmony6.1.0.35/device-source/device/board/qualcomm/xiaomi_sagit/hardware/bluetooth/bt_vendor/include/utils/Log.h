/*
 * Copyright (C) 2021–2022 Beijing OSWare Technology Co., Ltd
 * This file contains confidential and proprietary information of
 * OSWare Technology Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BT_VENDOR_LOG_H
#define BT_VENDOR_LOG_H
#include <string.h>
#include <stdint.h>
#include "hilog/log.h"
#include "stdio.h"
#ifdef HDF_LOG_TAG
#undef HDF_LOG_TAG
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#undef LOG_TAG
#undef LOG_DOMAIN
#define LOG_TAG "BTVENDOR"
#define LOG_DOMAIN 0xD001400

#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)

#ifndef HILOGD
#define HILOGD(format, ...)                                                                                     \
    do {                                                                                                              \
        HILOG_DEBUG(LOG_CORE, "[%{public}s@%{public}s:%{public}d] " format "\n",                                      \
                __FUNCTION__, __FILENAME__, __LINE__,                                                                 \
                ##__VA_ARGS__);                                                                                                             \
    } while (0)
#endif

#ifndef HILOGI
#define HILOGI(format, ...)                                                                                     \
    do {                                                                                                              \
        HILOG_INFO(LOG_CORE, "[%{public}s@%{public}s:%{public}d] " format "\n", __FUNCTION__, __FILENAME__, __LINE__, \
            ##__VA_ARGS__);                                                                                           \
    } while (0)
#endif

#ifndef HILOGW
#define HILOGW(format, ...)                                                                                     \
    do {                                                                                                              \
        HILOG_WARN(LOG_CORE, "[%{public}s@%{public}s:%{public}d] " format "\n", __FUNCTION__, __FILENAME__, __LINE__, \
            ##__VA_ARGS__);                                                                                           \
    } while (0)
#endif

#ifndef HILOGE
#define HILOGE(format, ...)                                 \
    do {                                                          \
        HILOG_ERROR(LOG_CORE,                                     \
            "[%{public}s@%{public}s:%{public}d] " format "\n",    \
            __FUNCTION__, __FILENAME__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* LOG_H */