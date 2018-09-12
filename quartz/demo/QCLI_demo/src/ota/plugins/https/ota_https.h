/* Copyright (c) 2011-2016.  Qualcomm Atheros, Inc.
 * All rights reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#ifndef _OTA_HTTPS_H_
#define _OTA_HTTPS_H_

/*
 * OTA HTTPS Status codes
 */
typedef enum {
        OTA_ERR_HTTPS_SESSION_ALREADY_START_E = 3000,
        OTA_ERR_HTTPS_SESSION_NOT_START_E,
        OTA_ERR_HTTPS_GET_LOCAL_ADDRESS_E, 
        OTA_ERR_HTTPS_CREATE_SOCKET_E,
        OTA_ERR_HTTPS_URL_FORMAT_E,
        OTA_ERR_HTTPS_NO_MEMORY_E,
        OTA_ERR_HTTPS_CONNECT_FAIL_E,
        OTA_ERR_HTTPS_SSL_CREATE_CTX_ERR_E,
        OTA_ERR_HTTPS_SSL_CONNECT_FAIL_E,  
        OTA_ERR_HTTPS_SSL_ADD_SOCKET_FAIL_E,
        OTA_ERR_HTTPS_BIND_FAIL_E,
        OTA_ERR_HTTPS_PEER_CLOSED_E,
        OTA_ERR_HTTPS_SEND_E,
        OTA_ERR_HTTPS_FILE_NOT_COMPLETE_E,
        OTA_ERR_HTTPS_FILE_NOT_FOUND_E,
        OTA_ERR_HTTPS_FILE_NAME_TOO_LONG_E,
        OTA_ERR_HTTPS_DIR_NOT_EXIST_E,  
        OTA_ERR_HTTPS_IMAGE_DOWNLOAD_FAIL_E,        
        OTA_ERR_HTTPS_SERVER_RSP_TIMEOUT_E,
        OTA_ERR_HTTPS_HEADER_ERROR_E,
} QCOM_OTA_HTTPS_STATUS_CODE_t;

/*****************************************************************************************************************/
ota_Status_Code_t ota_Plugin_Https_Init(const char* interface_name, const char *url);
ota_Status_Code_t ota_Plugin_Https_Fini(void);
ota_Status_Code_t ota_Plugin_Https_Recv_Data(uint8_t *buffer, uint32_t buf_len, uint32_t *ret_size);
ota_Status_Code_t ota_Plugin_Https_Abort(void);

#endif /*_OTA_HTTPS_H_ */
