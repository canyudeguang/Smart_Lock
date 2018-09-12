/* Copyright (c) 2018 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _OTA_HTTP_H_
#define _OTA_HTTP_H_

/*
 * Firmware Upgrade HTTP Status codes
 */
typedef enum {
        FW_UPGRADE_ERR_HTTP_SESSION_ALREADY_START_E = 3000,
        FW_UPGRADE_ERR_HTTP_SESSION_NOT_START_E,
        FW_UPGRADE_ERR_HTTP_GET_LOCAL_ADDRESS_E, 
        FW_UPGRADE_ERR_HTTP_URL_FORMAT_E,
        FW_UPGRADE_ERR_HTTP_NO_MEMORY_E,
        FW_UPGRADE_ERR_HTTP_CONNECT_FAIL_E,
        FW_UPGRADE_ERR_HTTP_SSL_CREATE_CTX_ERR_E,
        FW_UPGRADE_ERR_HTTP_SSL_CONNECT_FAIL_E,  
        FW_UPGRADE_ERR_HTTP_SSL_ADD_SOCKET_FAIL_E,
        FW_UPGRADE_ERR_HTTP_SEND_E,
        FW_UPGRADE_ERR_HTTP_FILE_NOT_COMPLETE_E,
        FW_UPGRADE_ERR_HTTP_FILE_NOT_FOUND_E,
        FW_UPGRADE_ERR_HTTP_FILE_NAME_TOO_LONG_E,
        FW_UPGRADE_ERR_HTTP_DIR_NOT_EXIST_E,  
        FW_UPGRADE_ERR_HTTP_IMAGE_DOWNLOAD_FAIL_E,        
        FW_UPGRADE_ERR_HTTP_SERVER_RSP_TIMEOUT_E,
        FW_UPGRADE_ERR_HTTP_HEADER_ERROR_E,
} FW_UPGRADE_HTTP_STATUS_CODE_t;

/*****************************************************************************************************************/
qapi_Fw_Upgrade_Status_Code_t plugin_Http_Init(const char* interface_name, const char *url, void *init_param);
qapi_Fw_Upgrade_Status_Code_t plugin_Http_Fin(void);
qapi_Fw_Upgrade_Status_Code_t plugin_Http_Recv_Data(uint8_t *buffer, uint32_t buf_len, uint32_t *ret_size);
qapi_Fw_Upgrade_Status_Code_t plugin_Http_Abort(void);
qapi_Fw_Upgrade_Status_Code_t plugin_Http_Resume(const char* interface_name, const char *url, uint32_t offset);

#endif /*_OTA_HTTP_H_ */
