/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _AWS_UTIL_H_
#define _AWS_UTIL_H_

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#define MAX_LENGTH_OF_UPDATE_JSON_BUFFER   600
#define MAX_LENGTH_OF_RX_JSON_BUFFER       1024

//AWS Server Details
#define HOST_ID    "ABCD_xxx.xxxxxx.amazonaws.com"
#define THING_NAME "AB_xx"
#define AWS_CERT_LOC "cert"
#define AWS_CALIST_LOC "calist"


#define SNTPC_SVR_ADDR  "132.163.97.4"
//Macro related to Certificate
#define SIZE_CERT 3650
#define hex_to_dec_nibble(hex_nibble) ( (hex_nibble >= 'a') ? (hex_nibble-'a'+10) : ((hex_nibble >= 'A') ? (hex_nibble-'A'+10) : (hex_nibble-'0')) )

extern QCLI_Group_Handle_t qcli_onboard;

#undef IOT_INFO
#define IOT_INFO(...)           QCLI_Printf(qcli_onboard, __VA_ARGS__)
#undef IOT_ERROR
#define IOT_ERROR(...)          QCLI_Printf(qcli_onboard, __VA_ARGS__)
#undef IOT_WARN
#define IOT_WARN(...)           QCLI_Printf(qcli_onboard, __VA_ARGS__)
#undef IOT_VERBOSE
#define IOT_VERBOSE(...)        QCLI_Printf(qcli_onboard, __VA_ARGS__)

/*----------------------------------------------------
 * AWS Events
 *---------------------------------------------------*/
#define SIGNAL_AWS_START_EVENT                   (1<<0)
#define SIGNAL_AWS_STOP_EVENT                    (1<<1)

boolean is_aws_running(void);
int32_t Start_aws(void);
int32_t Stop_aws(void);
int32_t Initialize_aws(void);

typedef struct aws_json_data{
    char device_name[128];
    char sensor_name[128];
    char sensor_val[128];
}json_rx;

#endif
