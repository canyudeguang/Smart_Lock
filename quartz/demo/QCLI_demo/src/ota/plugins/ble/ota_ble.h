/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _FW_UPGRADE_BLE_H_
#define _FW_UPGRADE_BLE_H_

#define QAPI_USE_BLE

#include "qapi.h"

typedef struct plugin_BLE_Init_s
{
   uint32_t           BluetoothStackID;
   uint32_t           Version;
   qapi_BLE_BD_ADDR_t RemoteDevice;
   unsigned int       ConnectionID;
} plugin_BLE_Init_t;

qapi_Fw_Upgrade_Status_Code_t plugin_BLE_Init(const char* interface_name, const char *url, void *init_param);
qapi_Fw_Upgrade_Status_Code_t plugin_BLE_Finish(void);
qapi_Fw_Upgrade_Status_Code_t plugin_BLE_Recv_Data(uint8_t *buffer, uint32_t buf_len, uint32_t *ret_size);
qapi_Fw_Upgrade_Status_Code_t plugin_BLE_Abort(void);
qapi_Fw_Upgrade_Status_Code_t plugin_BLE_Resume(const char* interface_name, const char *url, uint32_t offset);

#endif /*_FW_UPGRADE_BLE_H_ */
