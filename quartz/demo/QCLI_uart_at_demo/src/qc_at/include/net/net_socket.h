/*
* Copyright (c) 2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/


#ifndef _NET_SOCKET_H_
#define _NET_SOCKET_H_

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

#define SIZEOF_IP                           16
#define MAX_NO_OF_SESSIONS                  12

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
typedef struct sock_test_struct{
    int32_t sock_id;
    uint32_t proto;
    uint16_t port_no;
    int32_t session_id;
    char *server_ip;
}sock_buf;



#endif /* _NET_SOCKET_H_ */
