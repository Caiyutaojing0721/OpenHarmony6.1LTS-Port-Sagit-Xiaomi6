/*
 * Copyright 2012 The Android Open Source Project
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

/******************************************************************************
 *
 *  Filename:      hardware.c
 *
 *  Description:   Contains controller-specific functions, like
 *                      firmware patch download
 *                      low power mode operations
 *
 ******************************************************************************/

#include "bt_hci_bdroid.h"
#include "bt_vendor_qcom.h"
#include "hci_uart.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "upio.h"

#include <utils/Log.h>
#define MAX_CNT_RETRY 100

#define HCI_CMD_MAX_LEN 258

#define HCI_RESET 0x0C03
#define HCI_VSC_WRITE_UART_CLOCK_SETTING 0xFC45
#define HCI_VSC_UPDATE_BAUDRATE 0xFC18
#define HCI_READ_LOCAL_NAME 0x0C14
#define HCI_VSC_DOWNLOAD_MINIDRV 0xFC2E
#define HCI_VSC_WRITE_FIRMWARE 0xFC4C
#define HCI_VSC_WRITE_BD_ADDR 0xFC01
#define HCI_VSC_WRITE_SLEEP_MODE 0xFC27
#define HCI_VSC_WRITE_SCO_PCM_INT_PARAM 0xFC1C
#define HCI_VSC_WRITE_PCM_DATA_FORMAT_PARAM 0xFC1E
#define HCI_VSC_WRITE_I2SPCM_INTERFACE_PARAM 0xFC6D
#define HCI_VSC_ENABLE_WBS 0xFC7E
#define HCI_VSC_LAUNCH_RAM 0xFC4E
#define HCI_READ_LOCAL_BDADDR 0x1009

#define HCI_EVT_CMD_CMPL_STATUS_RET_BYTE 5
#define HCI_EVT_CMD_CMPL_LOCAL_NAME_STRING 6
#define HCI_EVT_CMD_CMPL_LOCAL_BDADDR_ARRAY 6
#define HCI_EVT_CMD_CMPL_OPCODE 3
#define LPM_CMD_PARAM_SIZE 12
#define UPDATE_BAUDRATE_CMD_PARAM_SIZE 6
#define HCI_CMD_PREAMBLE_SIZE 3
#define HCD_REC_PAYLOAD_LEN_BYTE 2
#define LOCAL_NAME_BUFFER_LEN 32
#define LOCAL_BDADDR_PATH_BUFFER_LEN 256
#define DELAY_50_MS (50)
#define BAUDRATE_3000000 (3000000)

/* low power mode parameters */
typedef struct {
    uint8_t sleep_mode;                     /* 0(disable),1(UART),9(H5) */
    uint8_t host_stack_idle_threshold;      /* Unit scale 300ms/25ms */
    uint8_t host_controller_idle_threshold; /* Unit scale 300ms/25ms */
    uint8_t bt_wake_polarity;               /* 0=Active Low, 1= Active High */
    uint8_t host_wake_polarity;             /* 0=Active Low, 1= Active High */
    uint8_t allow_host_sleep_during_sco;
    uint8_t combine_sleep_mode_and_lpm;
    uint8_t enable_uart_txd_tri_state; /* UART_TXD Tri-State */
    uint8_t sleep_guard_time;          /* sleep guard time in 12.5ms */
    uint8_t wakeup_guard_time;         /* wakeup guard time in 12.5ms */
    uint8_t txd_config;                /* TXD is high in sleep state */
    uint8_t pulsed_host_wake;          /* pulsed host wake if mode = 1 */
} bt_lpm_param_t;

static bt_lpm_param_t lpm_param = {
    LPM_SLEEP_MODE,
    LPM_IDLE_THRESHOLD,
    LPM_HC_IDLE_THRESHOLD,
    LPM_BT_WAKE_POLARITY,
    LPM_HOST_WAKE_POLARITY,
    LPM_ALLOW_HOST_SLEEP_DURING_SCO,
    LPM_COMBINE_SLEEP_MODE_AND_LPM,
    LPM_ENABLE_UART_TXD_TRI_STATE,
    0, /* not applicable */
    0, /* not applicable */
    0, /* not applicable */
    LPM_PULSED_HOST_WAKE
};

/*******************************************************************************
**
** Function        hw_lpm_enable
**
** Description     Enalbe/Disable LPM
**
** Returns         TRUE/FALSE
**
*******************************************************************************/
uint8_t hw_lpm_enable(uint8_t turn_on)
{
    HILOGD("entering hw_lpm_enable11");
    HC_BT_HDR *p_buf = NULL;
    uint8_t *p;
    uint8_t ret = FALSE;

    if (bt_vendor_cbacks) {
        p_buf = (HC_BT_HDR *)bt_vendor_cbacks->alloc(BT_HC_HDR_SIZE +
                                                     HCI_CMD_PREAMBLE_SIZE +
                                                     LPM_CMD_PARAM_SIZE);
    }

    if (p_buf) {
        p_buf->event = MSG_STACK_TO_HC_HCI_CMD;
        p_buf->offset = 0;
        p_buf->layer_specific = 0;
        p_buf->len = HCI_CMD_PREAMBLE_SIZE + LPM_CMD_PARAM_SIZE;

        p = (uint8_t *)(p_buf + 1);
        UINT16_TO_STREAM(p, HCI_VSC_WRITE_SLEEP_MODE);
        *p++ = LPM_CMD_PARAM_SIZE; /* parameter length */

        if (turn_on) {
            memcpy_s(p, LPM_CMD_PARAM_SIZE, &lpm_param, LPM_CMD_PARAM_SIZE);
            upio_set(UPIO_LPM_MODE, UPIO_ASSERT, 0);
        } else {
            memset_s(p, LPM_CMD_PARAM_SIZE, 0, LPM_CMD_PARAM_SIZE);
            upio_set(UPIO_LPM_MODE, UPIO_DEASSERT, 0);
        }

        if ((ret = bt_vendor_cbacks->xmit_cb(HCI_VSC_WRITE_SLEEP_MODE, p_buf)) <= 0) {
            bt_vendor_cbacks->dealloc(p_buf);
        }
    }

    HILOGD("hw_lpm_enable ret:%{public}d", ret);
    return ret;
}

/*******************************************************************************
**
** Function        hw_lpm_set_wake_state
**
** Description     Assert/Deassert BT_WAKE
**
** Returns         None
**
*******************************************************************************/
void hw_lpm_set_wake_state(uint8_t wake_assert)
{
    uint8_t state = (wake_assert) ? UPIO_ASSERT : UPIO_DEASSERT;

    upio_set(UPIO_BT_WAKE, state, lpm_param.bt_wake_polarity);
}

/*******************************************************************************
**
** Function         hw_lpm_ctrl_cback
**
** Description      Callback function for lpm enable/disable rquest
**
** Returns          None
**
*******************************************************************************/
void hw_lpm_ctrl_cback(void *p_mem)
{
    HC_BT_HDR *p_evt_buf = (HC_BT_HDR *)p_mem;
    bt_op_result_t status = BTC_OP_RESULT_FAIL;

    if (*((uint8_t *)(p_evt_buf + 1) + HCI_EVT_CMD_CMPL_STATUS_RET_BYTE) == 0) {
        status = BTC_OP_RESULT_SUCCESS;
    }
}

void hw_config_start(void)
{
    bt_vendor_cbacks->init_cb(BTC_OP_RESULT_SUCCESS);
/*
    HC_BT_HDR *p_buf = NULL;
    uint8_t *p;

    HILOGI("hw config start");

    if (bt_vendor_cbacks) {
        p_buf = (HC_BT_HDR *)bt_vendor_cbacks->alloc(BT_HC_HDR_SIZE +
                                                     HCI_CMD_PREAMBLE_SIZE);
    }

    if (p_buf) {
        p_buf->event = MSG_STACK_TO_HC_HCI_CMD;
        p_buf->offset = 0;
        p_buf->layer_specific = 0;
        p_buf->len = HCI_CMD_PREAMBLE_SIZE;

        p = (uint8_t *)(p_buf + 1);
        UINT16_TO_STREAM(p, HCI_RESET);
        *p = 0;

        HILOGI("HCI_RESET");

        // hw_cfg_cb.state = HW_CFG_START;
        bt_vendor_cbacks->xmit_cb(HCI_RESET, p_buf);
        bt_vendor_cbacks->init_cb(BTC_OP_RESULT_SUCCESS);
    } else {
        if (bt_vendor_cbacks) {
            HILOGE("vendor lib fw conf aborted [no buffer]");
            bt_vendor_cbacks->init_cb(BTC_OP_RESULT_FAIL);
        }
    }
*/
}

void hw_process_event(HC_BT_HDR *p_buf)
{
    uint16_t opcode;
    uint8_t *p = (uint8_t *)(p_buf + 1) + HCI_EVT_CMD_CMPL_OPCODE;
    STREAM_TO_UINT16(opcode, p);

    HILOGI("%s, opcode:0x%{public}04x", __FUNCTION__, opcode);
    switch (opcode) {
        case HCI_VSC_WRITE_BD_ADDR:
#if (USE_CONTROLLER_BDADDR == TRUE)
        case HCI_READ_LOCAL_BDADDR:
#endif
        case HCI_READ_LOCAL_NAME:
        case HCI_VSC_DOWNLOAD_MINIDRV:
        case HCI_VSC_WRITE_FIRMWARE:
        case HCI_VSC_LAUNCH_RAM:
        case HCI_RESET:
        case HCI_VSC_WRITE_UART_CLOCK_SETTING:
        case HCI_VSC_UPDATE_BAUDRATE:
            //hw_config_cback(p_buf);
            HILOGI("hw_config_cback");
            break;

        case HCI_VSC_WRITE_SCO_PCM_INT_PARAM:
        case HCI_VSC_WRITE_PCM_DATA_FORMAT_PARAM:
        case HCI_VSC_WRITE_I2SPCM_INTERFACE_PARAM:
            //hw_sco_i2spcm_cfg_cback(p_buf);
            HILOGI("hw_sco_i2spcm_cfg_cback");
            break;

        case HCI_VSC_WRITE_SLEEP_MODE:
            hw_lpm_ctrl_cback(p_buf);
            break;

        case HCI_VSC_ENABLE_WBS:
            break;
    }

    HILOGI("%s, Complete", __FUNCTION__);
}
