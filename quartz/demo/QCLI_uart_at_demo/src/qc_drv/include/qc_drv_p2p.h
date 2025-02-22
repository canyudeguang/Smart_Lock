/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QC_DRV_P2P_H
#define __QC_DRV_P2P_H

#include "qc_drv_main.h"

qapi_Status_t qc_drv_p2p_enable(qc_drv_context *qc_drv_ctx, uint32_t deviceId, int flag);
qapi_Status_t qc_drv_p2p_disable(qc_drv_context *qc_drv_ctx, uint32_t deviceId, int flag);

qapi_Status_t qc_drv_p2p_set_config(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
		qapi_WLAN_P2P_Config_Params_t *p2pConfig, int size, int flag);

qapi_Status_t qc_drv_p2p_connect(qc_drv_context *qc_drv_ctx, uint32_t deviceId, uint8_t wps_Method,
		uint8_t *peer_Addr, uint8_t p2p_pers_go);
qapi_Status_t qc_drv_p2p_listen(qc_drv_context *qc_drv_ctx, uint32_t deviceId, uint32_t timeout_val);
qapi_Status_t qc_drv_p2p_cancel(qc_drv_context *qc_drv_ctx, uint32_t deviceId);

qapi_Status_t qc_drv_p2p_join(qc_drv_context *qc_drv_ctx, uint32_t deviceId, uint8_t wps_Method,
		uint8_t *p2p_join_mac_addr, char *p2p_wps_pin, uint16_t go_Oper_Freq);

qapi_Status_t qc_drv_p2p_auth(qc_drv_context *qc_drv_ctx, uint32_t deviceId, uint8_t dev_Auth,
		qapi_WLAN_P2P_WPS_Method_e wps_Method, uint8_t *peer_Addr, uint8_t p2p_persistent_go);

qapi_Status_t qc_drv_p2p_start_go(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
		qapi_WLAN_P2P_Go_Params_t *goParams, int32_t go_chan, uint8_t persistent_Group);

qapi_Status_t qc_drv_p2p_invite(qc_drv_context *qc_drv_ctx, uint32_t deviceId, const char *string,
        uint8_t wps_Method, uint8_t *peer_Addr, uint8_t is_Persistent, uint8_t p2p_invite_role);

qapi_Status_t qc_drv_p2p_get_node_list(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
		qapi_WLAN_P2P_Node_List_Params_t *p2pNodeList, uint32_t *dataLen);

qapi_Status_t qc_drv_p2p_get_network_list(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
		qapi_WLAN_P2P_Network_List_Params_t *p2pNetworkList, uint32_t *dataLen);

qapi_Status_t qc_drv_p2p_set_oops_params(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
		qapi_WLAN_P2P_Opps_Params_t *opps, int size, int flag);

qapi_Status_t qc_drv_p2p_set_noa_params(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
		qapi_WLAN_P2P_Noa_Params_t *noaParams, int size, int flag);

qapi_Status_t qc_drv_p2p_set_operating_class(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
		qapi_WLAN_P2P_Config_Params_t *p2pConfig, int size, int flag);

qapi_Status_t qc_drv_p2p_stop_find(qc_drv_context *qc_drv_ctx, uint32_t deviceId);
qapi_Status_t qc_drv_p2p_set_ssidparam(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
		qapi_WLAN_P2P_Go_Params_t *ssidParams, int size, int flag);

qapi_Status_t qc_drv_p2p_set(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
		uint8_t config_Id, int *val, uint32_t len, int flag);

qapi_Status_t qc_drv_p2p_provision(qc_drv_context *qc_drv_ctx, uint32_t deviceId, uint16_t wps_Method, uint8_t *peer);

qapi_Status_t qc_drv_p2p_set_param(qc_drv_context *qc_drv_ctx, uint32_t deviceId, uint8_t *p2pmode, int size, int flag);

qapi_Status_t qc_drv_p2p_find(qc_drv_context *qc_drv_ctx, uint32_t deviceId, uint8_t type, uint32_t timeout);

qapi_Status_t qc_drv_p2p_invite_auth(qc_drv_context *qc_drv_ctx, uint8_t device_Id, const qapi_WLAN_P2P_Invite_Info_t  *invite_Info);

#endif
