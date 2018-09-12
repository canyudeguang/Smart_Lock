/*
* Copyright (c) 2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/


#ifndef _SHADOW_SAMPLE_H_

#define _SHADOW_SAMPLE_H_

#include "aws_iot_shadow_json.h"
#include "qcli_api.h"
#include "qapi_ssl.h"
#include "qapi_socket.h"
#include "qapi_ns_utils.h"
#include "qapi_netbuf.h"
#include "netutils.h"       /* time_struct_t */

#undef A_OK
#define A_OK                    QAPI_OK

#undef A_ERROR
#define A_ERROR                 -1

#define AWS_TASK_PRIORITY    10 
#define AWS_TASK_STACK_SIZE  4096 
#define MAX_JSON_OBJ_COUNT   10

#define AWS_MAX_DOMAIN_SIZE 128
#define AWS_MAX_THING_SIZE  128
#define AWS_MAX_FILE_NAME   128
#define AWS_MAX_CERT_NAME   128
#define AWS_MAX_STRING_SIZE 128

/* max key size 63 + 1 for null termination */
#define MAX_JSON_KEY_SIZE    64

/* Customer structure is used to take care of const char* usage for key variable */
struct jsonStruct_q {
    char *pKey; ///< JSON key
    void *pData; ///< pointer to the data (JSON value)
    JsonPrimitiveType type; ///< type of JSON
    jsonStructCallback_t cb; ///< callback to be executed on receiving the Key value pair
};

struct aws_params {
    char                      *host_name;
    char                      *thing_name;
    uint32_t                   num_objects;
    char                      *file_name;
    char                      *schema_string;
    struct qapi_fs_stat_type  *sbuf;
    char                      *client_cert;
    uint32_t                   update_timer;
};
#endif /* _SHADOW_SAMPLE_H_ */

