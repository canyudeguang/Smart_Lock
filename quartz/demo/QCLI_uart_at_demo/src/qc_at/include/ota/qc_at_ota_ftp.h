/* Copyright (c) 2011-2016.  Qualcomm Atheros, Inc.
 * All rights reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#ifndef _FW_UPGRADE_FTP_H_
#define _FW_UPGRADE_FTP_H_

#include "qapi_firmware_upgrade.h"

/*
 * Firmware Upgrade FTP Status codes
 */
typedef enum {
        QCOM_FW_UPGRADE_ERR_FTP_SESSION_ALREADY_START_E = 2000,
        QCOM_FW_UPGRADE_ERR_FTP_SESSION_NOT_START_E,
        QCOM_FW_UPGRADE_ERR_FTP_GET_LOCAL_ADDRESS_E, 
        QCOM_FW_UPGRADE_ERR_FTP_CREATE_SOCKET_E,
        QCOM_FW_UPGRADE_ERR_FTP_CONTROL_SOCKET_CLOSED_E,
        QCOM_FW_UPGRADE_ERR_FTP_DATA_SOCK_CLOSED_E,        
        QCOM_FW_UPGRADE_ERR_FTP_URL_FORMAT_E,
        QCOM_FW_UPGRADE_ERR_FTP_NO_MEMORY_E,
        QCOM_FW_UPGRADE_ERR_FTP_CONNECT_FAIL_E,
        QCOM_FW_UPGRADE_ERR_FTP_BIND_FAIL_E,
        QCOM_FW_UPGRADE_ERR_FTP_PEER_CLOSED_E,
        QCOM_FW_UPGRADE_ERR_FTP_SEND_COMMAND_E,
        QCOM_FW_UPGRADE_ERR_FTP_LISTEN_DATA_PORT_E,
        QCOM_FW_UPGRADE_ERR_FTP_DATA_CONNECTION_TIMEOUT_E,
        QCOM_FW_UPGRADE_ERR_FTP_ACCEPT_DATA_CONNECT_E,
        QCOM_FW_UPGRADE_ERR_FTP_FILE_NOT_COMPLETE_E,
        QCOM_FW_UPGRADE_ERR_FTP_SYST_E,
        QCOM_FW_UPGRADE_ERR_FTP_LOGIN_INCORRECT_E,
        QCOM_FW_UPGRADE_ERR_FTP_SET_TYPE_E,
        QCOM_FW_UPGRADE_ERR_FTP_SET_PORT_E,
        QCOM_FW_UPGRADE_ERR_FTP_FILE_NOT_FOUND_E,
        QCOM_FW_UPGRADE_ERR_FTP_DIR_NOT_EXIST_E,
        QCOM_FW_UPGRADE_ERR_FTP_RESTART_NOT_SUPPORT_E,
} QCOM_FW_UPGRADE_FTP_STATUS_CODE_t;

/*****************************************************************************************************************/
qapi_Fw_Upgrade_Status_Code_t plugin_Ftp_Init(const char* interface_name, const char *url, void *init_param);
qapi_Fw_Upgrade_Status_Code_t plugin_Ftp_Fin(void);
qapi_Fw_Upgrade_Status_Code_t plugin_Ftp_Recv_Data(uint8_t *buffer, uint32_t buf_len, uint32_t *ret_size);
qapi_Fw_Upgrade_Status_Code_t plugin_Ftp_Abort(void);
qapi_Fw_Upgrade_Status_Code_t plugin_Ftp_Resume(const char* interface_name, const char *url, uint32_t offset);

#endif /*_FW_UPGRADE_FTP_H_ */
