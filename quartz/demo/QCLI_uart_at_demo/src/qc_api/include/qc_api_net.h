/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*-------------------------------------------------------------------------
 *  Include Files
 *-----------------------------------------------------------------------*/
#ifndef __QC_API_NET_H
#define __QC_API_NET_H

#include <stdarg.h>

#include "malloc.h"
#include "string.h"
#include "stringl.h"
#include "qurt_timer.h"

#include "qcli_api.h"
#include "qcli_util.h"
#include "qapi_socket.h"
#include "qapi_ns_utils.h"

int net_sock_info(int32_t sid);
int net_sock_send_data(char *tx_data, uint32_t data_len);
int net_sock_set_active_session(int32_t sid, uint16_t portnum, uint8_t *ipaddr);
void net_sock_close(uint32_t id);
int net_sock_read(int32_t sid);
int net_sock_open(uint8_t server, uint32_t family, uint32_t proto, uint16_t portnum, uint8_t *ipaddr);
int net_sock_initialize(void);

extern QCLI_Command_Status_t multi_sockv4_create(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
extern QCLI_Command_Status_t multi_sock_create(QCLI_Parameter_t *Parameter_List);
extern QCLI_Command_Status_t multi_sock_close(void);
extern QCLI_Command_Status_t ssl_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t qc_api_net_Help(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_Ping(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_Ifconfig(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_DhcpV4Client(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_DhcpV4server(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_AutoIp(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_PingV6(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_DhcpV6Client(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_SntpClient(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_DnsClient(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_Bridge(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_SockClose(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_TcpV6Connect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_TcpConnect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_UdpV6Connect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_UdpConnect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_TcpV6Server(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_TcpServer(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_UdpServer(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_UdpV6Server(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_SockInfo(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_TxData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_RxData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_SendData(char *tx_data, uint32_t len);
QCLI_Command_Status_t qc_api_net_HttpClient(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_HttpServer(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_MsockCreate(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_MsockClose(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_cert_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_ssl_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_benchtx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_net_benchrx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

#endif // _NET_API_H_
