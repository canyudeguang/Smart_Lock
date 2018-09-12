/*
* Copyright (c) 2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/**
 * @file util.c
 * @brief File contains utility functions.
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <qapi_ver.h>
#include <qcli_api.h>
#include <qapi_ble_gap.h>
#include <qapi_ble_bttypes.h>
#include <qapi_reset.h>

#include "util.h"
#include "log_util.h"
#include "ble_util.h"

#define IOT_INFO          LOG_INFO
#define IOT_ERROR         LOG_ERROR
#define IOT_WARN          LOG_WARN
#define IOT_VERBOSE       LOG_VERBOSE

#define BOARD_NAME        CHIPSET_VARIANT
#define DEVID_STA          1

int32_t wlan_get_device_mac_address(uint32_t mode, char *mac);

void change_to_upper(char *s)
{
    while(*s)
    {
        if (*s >='a' && *s <='z')
            *s = *s - ('a' - 'A');
        s++;
    }
}

void get_dev_fw_version(char *buf, int32_t buf_size)
{
    qapi_FW_Info_t  info;
    if (qapi_Get_FW_Info(&info) == QAPI_OK)
    {
        snprintf(buf, buf_size, "\"QAPI Ver: %d.%d.%d CRM Num: %d\"",
                (int) (info.qapi_Version_Number&__QAPI_VERSION_MAJOR_MASK)>>__QAPI_VERSION_MAJOR_SHIFT,
                ((int) info.qapi_Version_Number&__QAPI_VERSION_MINOR_MASK)>>__QAPI_VERSION_MINOR_SHIFT,
                ((int) info.qapi_Version_Number&__QAPI_VERSION_NIT_MASK)>>__QAPI_VERSION_NIT_SHIFT, (unsigned int) info.crm_Build_Number);
    }
}

int32_t get_localdevice_name(char *buf, int32_t buf_size)
{
    char board_name[FW_VER_BUF] = {0};
    int rc = SUCCESS;

    memcpy(board_name, BOARD_NAME, sizeof(BOARD_NAME));
    change_to_upper(board_name);
#ifdef ONBOARD_VIA_WIFI
    {
        char mac[6] = { 0 };
        rc = wlan_get_device_mac_address(DEVID_STA, mac); 
        snprintf(buf+strlen(buf), buf_size,"\"%s_%02x%02x%02x\"", board_name, mac[3], mac[4], mac[5]);
    }
#endif
#ifdef ONBOARD_VIA_BLE
    {
        qapi_BLE_BD_ADDR_t BD_ADDR;
        if (!qapi_BLE_GAP_Query_Local_BD_ADDR(GetBluetoothStackID(), &BD_ADDR))
        {
			LOG_INFO("BD_ADDR: %02x:%02x:%02x:%02x:%02x:%02x\n", BD_ADDR.BD_ADDR5, BD_ADDR.BD_ADDR4, BD_ADDR.BD_ADDR3,
                        BD_ADDR.BD_ADDR2, BD_ADDR.BD_ADDR1, BD_ADDR.BD_ADDR0);
            snprintf(buf+strlen(buf), buf_size,"\"%s_%02x%02x%02x\"", board_name,
                       BD_ADDR.BD_ADDR2, BD_ADDR.BD_ADDR1, BD_ADDR.BD_ADDR0);
        }
	else
            rc = FAILURE;
    }
#endif
    
    return rc;
}

int32_t get_ble_localdevice_name(char *buf, int32_t buf_size)
{
    char board_name[FW_VER_BUF] = {0};
    int rc = SUCCESS;

    memcpy(board_name, BOARD_NAME, sizeof(BOARD_NAME));
    change_to_upper(board_name);
#ifdef ONBOARD_VIA_WIFI
    {
        char mac[6] = { 0 };
        rc = wlan_get_device_mac_address(DEVID_STA, mac);
        snprintf(buf+strlen(buf), buf_size,"\"%s_%02x%02x%02x\"", board_name, mac[3], mac[4], mac[5]);
    }
#endif
#ifdef ONBOARD_VIA_BLE
    {
        qapi_BLE_BD_ADDR_t BD_ADDR;
        if (!qapi_BLE_GAP_Query_Local_BD_ADDR(GetBluetoothStackID(), &BD_ADDR))
        {
            LOG_INFO("BD_ADDR: %02x:%02x:%02x:%02x:%02x:%02x\n", BD_ADDR.BD_ADDR5, BD_ADDR.BD_ADDR4, BD_ADDR.BD_ADDR3,
                    BD_ADDR.BD_ADDR2, BD_ADDR.BD_ADDR1, BD_ADDR.BD_ADDR0);
            snprintf(buf+strlen(buf), buf_size,"%s_%02x%02x%02x", board_name,
                    BD_ADDR.BD_ADDR2, BD_ADDR.BD_ADDR1, BD_ADDR.BD_ADDR0);
        }
        else
            rc = FAILURE;
    }
#endif

    return rc;
}

void restart_device(void)
{
    qapi_System_Reset();
}

