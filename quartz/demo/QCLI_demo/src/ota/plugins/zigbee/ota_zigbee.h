/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _FW_UPGRADE_ZIGBEE_H_
#define _FW_UPGRADE_ZIGBEE_H_

typedef struct plugin_Zigbee_Init_s
{
   uint8_t Endpoint;
} plugin_Zigbee_Init_t;

qapi_Fw_Upgrade_Status_Code_t plugin_Zigbee_Init(const char* interface_name, const char *url, void *init_param);
qapi_Fw_Upgrade_Status_Code_t plugin_Zigbee_Finish(void);
qapi_Fw_Upgrade_Status_Code_t plugin_Zigbee_Recv_Data(uint8_t *buffer, uint32_t buf_len, uint32_t *ret_size);
qapi_Fw_Upgrade_Status_Code_t plugin_Zigbee_Abort(void);
qapi_Fw_Upgrade_Status_Code_t plugin_Zigbee_Resume(const char* interface_name, const char *url, uint32_t offset);

#endif /*_FW_UPGRADE_ZIGBEE_H_ */
