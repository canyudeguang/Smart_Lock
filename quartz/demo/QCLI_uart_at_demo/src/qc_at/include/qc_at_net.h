/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#ifndef __QC_AT_NET_H
#define __QC_AT_NET_H

#include <qcli_api.h>
#include <qcli_util.h>
#include "qc_drv_net.h"
#include "qc_api_main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "qurt_timer.h"
#include "qurt_thread.h"
#include "qcli.h"
#include "qapi_socket.h"
#include "qapi_netservices.h"
#include "qapi_wlan_bridge.h"
#include "net_bench.h"

//#include "net_demo.h"
//#include "net_iperf.h"

//#include "qapi_netprofile.h"
//#include "qapi_omtm.h"
//#include "qapi_httpsvr.h"
//#include "qapi_netbuf.h"
//#include "qapi_ns_utils.h"
//#include "qapi_ns_gen_v4.h"
//#include "qapi_crypto.h"

#include "qc_api_net.h"
#include "qc_drv_net.h"
#include "qc_util.h"
#include "qcli.h"

#define MAX_FQDN_LEN   255
#define DHCP_INFINITE_LEASE_TIME "infinite"

QCLI_Group_Handle_t atn_group;
qapi_Net_SSL_Config_t * httpsvr_sslcfg;
void Initialize_ATN_Demo(void);

QCLI_Command_Status_t ssl_start(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ssl_stop(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
void sslconfig_help(const char *str);
void ssl_free_config_parameters(qapi_Net_SSL_Config_t *cfg);
QCLI_Command_Status_t ssl_parse_config_parameters(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, qapi_Net_SSL_Config_t *cfg, qapi_Net_SSL_Obj_Hdl_t ssl_hdl, qbool_t server);
QCLI_Command_Status_t ssl_config(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ssl_add_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ssl_set_dtls_server_max_clients(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ssl_set_dtls_server_idle_timer(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ssl_set_ecjpake_params(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ssl_add_psk_table(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t store_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t delete_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t list_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t get_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t hash_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t generate_csr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

#endif
