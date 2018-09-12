/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/*
* 
* Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
* NOT A CONTRIBUTION
*/

#ifndef __NETWORK_PLATFORM_H_
#define __NETWORK_PLATFORM_H_

#include "qapi_socket.h"
#include "qapi_ns_utils.h"
#include "qapi_netbuf.h"
#include "qapi_netservices.h"

#include "qapi_ns_gen_v4.h"
#include "qapi_ns_gen_v6.h"

#define AWS_INVALID_SOCKET_FD -1

#ifndef TRUE
   #define TRUE (1 == 1)
#endif

#ifndef FALSE
   #define FALSE (0 == 1)
#endif

typedef struct iot_address_t {
  int size;           /**< size of addr */
  union {
    struct sockaddr         sa;
    struct sockaddr_in      sin;
    struct sockaddr_in6     sin6;
  } addr;
} iot_address_t;

typedef struct IOT_SSL_INST
{
    qapi_Net_SSL_Obj_Hdl_t sslCtx;
    qapi_Net_SSL_Con_Hdl_t ssl;
    qapi_Net_SSL_Config_t   config;
    uint8_t      config_set;
    qapi_Net_SSL_Role_t role;
} IOT_SSL_INST;

#endif

