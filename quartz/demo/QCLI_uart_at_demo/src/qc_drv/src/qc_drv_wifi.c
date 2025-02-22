/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "qc_drv_wifi.h"

qapi_Status_t qc_drv_wlan_enable(qc_drv_context *qc_drv_ctx)
{
    if (is_drv_cb_valid(qc_drv_wlan_enable)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_enable();
    }
    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_disable(qc_drv_context *qc_drv_ctx)
{
    if (is_drv_cb_valid(qc_drv_wlan_disable)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_disable();
    }
    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_add_device(qc_drv_context *qc_drv_ctx, uint8_t device_ID)
{
    if (is_drv_cb_valid(qc_drv_wlan_add_device)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_add_device(device_ID);
    }
    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_remove_device(qc_drv_context *qc_drv_ctx, uint8_t device_ID)
{
    if (is_drv_cb_valid(qc_drv_wlan_remove_device)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_remove_device(device_ID);
    }
    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_encr_type(qc_drv_context *qc_drv_ctx, uint32_t deviceId, void *encrType, int size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_encr_type)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_encr_type(deviceId, encrType, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_auth_mode(qc_drv_context *qc_drv_ctx, uint32_t deviceId, qapi_WLAN_Auth_Mode_e *authMode, int size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_auth_mode)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_auth_mode(deviceId, authMode, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_op_mode(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        uint32_t *wifimode, uint32_t *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_op_mode)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_op_mode(deviceId, wifimode, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_mac_addr(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        uint8_t *own_Interface_Addr, uint32_t *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_mac_addr)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_mac_addr(deviceId, own_Interface_Addr, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_mac_addr(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        uint8_t *own_Interface_Addr, uint32_t dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_mac_addr)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_mac_addr(deviceId, own_Interface_Addr, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_ssid(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        uint8_t *ssid, uint32_t ssid_len, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_ssid)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_ssid(deviceId, ssid, ssid_len, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_passphrase(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        void *passphrase, uint32_t passphrase_len, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_passphrase)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_passphrase(deviceId, passphrase,
                passphrase_len,
                flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_security_pmk(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        void *passphrase, uint32_t passphrase_len, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_security_pmk)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_security_pmk(deviceId, passphrase,
                passphrase_len,
                flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_wps_credentials(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        qapi_WLAN_WPS_Credentials_t *wpsCred, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_wps_credentials)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_wps_credentials(deviceId, wpsCred,
                size,
                flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_op_mode(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        qapi_WLAN_Dev_Mode_e *opMode, int size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_op_mode)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_op_mode(deviceId, opMode, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_power_param(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        void *pwrMode, int size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_power_param)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_power_param(deviceId, pwrMode, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_channel(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        uint32_t *channel_val, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_channel)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_channel(deviceId, channel_val, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_listen_interval(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        int32_t *listen_time, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_listen_interval)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_listen_interval(deviceId, listen_time, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_sta_keep_alive(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        int32_t *keep_alive, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_sta_keep_alive)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_sta_keep_alive(deviceId, keep_alive, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_rssi_threshold(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        qapi_WLAN_Rssi_Threshold_Params_t *qrthresh, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_rssi_threshold)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_rssi_threshold(deviceId, qrthresh, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_probe_req_fwd_to_host(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        int32_t *enable, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_probe_req_fwd_to_host)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_probe_req_fwd_to_host(deviceId,
                enable, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_wep_key_index(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        int32_t *key_idx, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_wep_key_index)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_wep_key_index(deviceId,
                key_idx, size, flag);
    }

    return QCLI_STATUS_ERROR_E;

}

qapi_Status_t qc_drv_wlan_set_wep_key_pair(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        qapi_WLAN_Security_Wep_Key_Pair_Params_t *keyPair, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_wep_key_pair)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_wep_key_pair(deviceId,
                keyPair, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_rate(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        qapi_WLAN_Bit_Rate_t *rateIndex, uint32_t size, uint8_t flag)

{
    if (is_drv_cb_valid(qc_drv_wlan_set_rate)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_rate(deviceId,
                rateIndex, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}


qapi_Status_t qc_drv_wlan_set_ap_wps_flag(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        uint8_t *wps_flag, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_ap_wps_flag)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_ap_wps_flag(deviceId,
                wps_flag, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_tx_power_in_dbm(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        int32_t *power_in_dBm, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_tx_power_in_dbm)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_tx_power_in_dbm(deviceId,
                power_in_dBm, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_phy_mode(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        qapi_WLAN_Phy_Mode_e *phyMode, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_phy_mode)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_phy_mode(deviceId,
                phyMode, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_ht_config(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        qapi_WLAN_11n_HT_Config_e *htconfig, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_ht_config)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_ht_config(deviceId,
                htconfig, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_ap_enable_hidden_mode(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        uint8_t *hidden_flag, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_ap_enable_hidden_mode)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_ap_enable_hidden_mode(deviceId,
                hidden_flag, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_beacon_interval_in_tu(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        uint32_t *beacon_int_in_tu, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_beacon_interval_in_tu)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_beacon_interval_in_tu(deviceId,
                beacon_int_in_tu, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_ap_dtim_interval(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        uint32_t *dtim_period, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_ap_dtim_interval)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_ap_dtim_interval(deviceId,
                dtim_period, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_country_code(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        char *country_code, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_country_code)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_country_code(deviceId,
                country_code, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_ap_country_code(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        char *country_code, uint32_t size, uint8_t flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_ap_country_code)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_ap_country_code(deviceId,
                country_code, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_power_param(qc_drv_context *qc_drv_ctx, uint32_t deviceId,
        uint32_t *mode, uint32_t *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_power_param)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_power_param(deviceId, mode, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_start_scan(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        const qapi_WLAN_Start_Scan_Params_t  *scan_Params,
        qapi_WLAN_Store_Scan_Results_e store_Scan_Results)
{
    if (is_drv_cb_valid(qc_drv_wlan_start_scan)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_start_scan(deviceId, scan_Params, store_Scan_Results);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_scan_results(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        qapi_WLAN_BSS_Scan_Info_t  *scan_Info,
        int16_t  *num_Results)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_scan_results)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_scan_results(deviceId, scan_Info, num_Results);
    }

    return QCLI_STATUS_ERROR_E;
}


qapi_Status_t qc_drv_wlan_get_phy_mode(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        qapi_WLAN_Phy_Mode_e *phyMode,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_phy_mode)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_phy_mode(deviceId, phyMode, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_ht_config(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        qapi_WLAN_11n_HT_Config_e *htConfig,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_ht_config)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_ht_config(deviceId, htConfig, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_ssid(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        void * ssid,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_ssid)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_ssid(deviceId, ssid, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_channel(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        uint16_t *channel_val,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_channel)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_channel(deviceId, channel_val, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_wep_key_pair(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        qapi_WLAN_Security_Wep_Key_Pair_Params_t *key_pair,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_wep_key_pair)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_wep_key_pair(deviceId, key_pair, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_wep_key_index(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        uint32_t *key_index,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_wep_key_index)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_wep_key_index(deviceId, key_index, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_rssi(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        uint8_t *rssi,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_rssi)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_rssi(deviceId, rssi, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}


qapi_Status_t qc_drv_wlan_get_reg_domain_channel_list(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        qapi_WLAN_Get_Channel_List_t *wlanChannelList,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_reg_domain_channel_list)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_reg_domain_channel_list(deviceId,
                wlanChannelList, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_wireless_stats(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        qapi_WLAN_Get_Statistics_t *getStats,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_wireless_stats)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_wireless_stats(deviceId,
                getStats, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_rate(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        int32_t *rate_index,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_rate)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_rate(deviceId,
                rate_index, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_firmware_version(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        qapi_WLAN_Firmware_Version_String_t *versionstr,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_firmware_version)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_firmware_version(deviceId,
                versionstr, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_reg_domain(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        uint32_t *regDomain,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_reg_domain)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_reg_domain(deviceId,
                regDomain, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_last_error(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        int32_t *err,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_last_error)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_last_error(deviceId,
                err, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_get_tx_status(qc_drv_context *qc_drv_ctx, uint8_t deviceId,
        uint32_t *tx_status,
        uint32_t  *dataLen)
{
    if (is_drv_cb_valid(qc_drv_wlan_get_tx_status)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_get_tx_status(deviceId,
                tx_status, dataLen);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_callback(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        qapi_WLAN_Callback_t callback,
        const void *application_Context)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_callback)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_callback(device_ID,
                callback,
                application_Context);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_wps_connect(qc_drv_context *qc_drv_ctx, uint8_t device_ID)
{
    if (is_drv_cb_valid(qc_drv_wlan_wps_connect)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_wps_connect(device_ID);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_wps_await_completion(qc_drv_context *qc_drv_ctx, uint32_t device_ID,
        qapi_WLAN_Netparams_t *net_Params)
{
    if (is_drv_cb_valid(qc_drv_wlan_wps_await_completion)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_wps_await_completion(device_ID,
                net_Params);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_disconnect(qc_drv_context *qc_drv_ctx, uint8_t device_ID)
{
    if (is_drv_cb_valid(qc_drv_wlan_disconnect)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_disconnect(device_ID);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_commit(qc_drv_context *qc_drv_ctx, uint8_t device_ID)
{
    if (is_drv_cb_valid(qc_drv_wlan_commit)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_commit(device_ID);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_raw_send(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        const qapi_WLAN_Raw_Send_Params_t  *raw_Params)
{
    if (is_drv_cb_valid(qc_drv_wlan_raw_send)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_raw_send(device_ID,
                raw_Params);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_suspend_start(qc_drv_context *qc_drv_ctx, uint32_t suspend_Time_In_Ms)
{
    if (is_drv_cb_valid(qc_drv_wlan_suspend_start)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_suspend_start(suspend_Time_In_Ms);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_wps_start(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        qapi_WLAN_WPS_Connect_Action_e connect_Action,
        qapi_WLAN_WPS_Mode_e mode,
        const char  *pin)
{
    if (is_drv_cb_valid(qc_drv_wlan_wps_start)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_wps_start(device_ID,
                connect_Action, mode, pin);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_get_auth_mode(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *authMode, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_get_auth_mode)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_get_auth_mode(device_ID,
                authMode, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_promiscuous_mode(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *mode, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_promiscuous_mode)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_promiscuous_mode(device_ID,
                mode, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_force_assert(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *data, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_force_assert)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_force_assert(device_ID,
                data, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_scan_param(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *scanParam, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_scan_param)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_scan_param(device_ID,
                scanParam, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_inactivity_time(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *time, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_inactivity_time)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_inactivity_time(device_ID,
                time, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_ap_ps_buf(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *ps_val, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_ap_ps_buf)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_ap_ps_buf(device_ID,
                ps_val, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_ap_enable_uapsd(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *uapsd, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_ap_enable_uapsd)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_ap_enable_uapsd(device_ID,
                uapsd, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_sta_uapsd(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *uapsd, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_sta_uapsd)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_sta_uapsd(device_ID,
                uapsd, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_sta_max_sp_len(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *maxsp, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_sta_max_sp_len)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_sta_max_sp_len(device_ID,
                maxsp, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_wmm_config(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *wmm, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_wmm_config)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_wmm_config(device_ID,
                wmm, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_ap_bss_mip(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *period, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_ap_bss_mip)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_ap_bss_mip(device_ID,
                period, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_sta_sp(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *period, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_sta_sp)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_sta_sp(device_ID,
                period, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_ap_sr(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *response, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_ap_sr)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_ap_sr(device_ID,
                response, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_app_ie(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *ie_params, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_app_ie)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_app_ie(device_ID,
                ie_params, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_sta_bmiss_config(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *bmiss, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_sta_bmiss_config)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_sta_bmiss_config(device_ID,
                bmiss, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_suspend_resume(qc_drv_context *qc_drv_ctx, uint8_t device_ID,
        void *suspend, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_suspend_resume)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_suspend_resume(device_ID,
                suspend, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_ka_offload_scfg(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_TCP_Offload_Config_Params_t *cfg, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_ka_offload_scfg)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_ka_offload_scfg(device_Id,
                cfg, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_ka_offload_enable(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_TCP_Offload_Enable_t *enable, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_ka_offload_enable)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_ka_offload_enable(device_Id,
                enable, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_nw_offload_enable(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Preferred_Network_Offload_Config_t *param, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_nw_offload_enable)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_nw_offload_enable(device_Id,
                param, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_nw_profile(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Preferred_Network_Profile_t *param, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_nw_profile)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_nw_profile(device_Id,
                param, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_aggr_tid(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Aggregation_Params_t *param, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_aggr_tid)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_aggr_tid(device_Id,
                param, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_aggrx_config(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Rx_Aggrx_Params_t *param, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_aggrx_config)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_aggrx_config(device_Id,
                param, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_pktlog_enable(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Pktlog_Enable_t *pktlog, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_pktlog_enable)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_pktlog_enable(device_Id,
                pktlog, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_pktlog_start(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Pktlog_Start_Params_t *pktlog, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_pktlog_start)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_pktlog_start(device_Id,
                pktlog, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_dblog_enable(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Dbglog_Enable_t *dbglog, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_dblog_enable)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_dblog_enable(device_Id,
                dbglog, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_dblog_config(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Dbglog_Config_t *config, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_dblog_config)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_dblog_config(device_Id,
                config, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_dblog_module_conf(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Dbglog_Module_Config_t *config, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_dblog_module_conf)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_dblog_module_conf(device_Id,
                config, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_drv_reg_query(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Driver_RegQuery_Params_t *query, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_drv_reg_query)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_drv_reg_query(device_Id,
                query, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_ap_channel_switch(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Channel_Switch_t *chnl, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_ap_channel_switch)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_ap_channel_switch(device_Id,
                chnl, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_event_filter(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Event_Filter_t *filter, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_event_filter)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_event_filter(device_Id,
                filter, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_pw_mode_policy(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Power_Policy_Params_t *pm, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_pw_mode_policy)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_pw_mode_policy(device_Id,
                pm, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_enable_roaming(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        uint32_t *roaming, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_enable_roaming)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_enable_roaming(device_Id,
                roaming, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_enable_green_tx(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        uint32_t *entx, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_enable_green_tx)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_enable_green_tx(device_Id,
                entx, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_low_pw_listen(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        uint32_t *enlpw, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_low_pw_listen)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_low_pw_listen(device_Id,
                enlpw, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_enable_wake_wireless(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        uint32_t *wake, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_enable_wake_wireless)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_enable_wake_wireless(device_Id,
                wake, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_enable_pkt_filter(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        uint32_t *filter, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_enable_pkt_filter)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_enable_pkt_filter(device_Id,
                filter, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_add_pattern(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Add_Pattern_t *pattern, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_add_pattern)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_add_pattern(device_Id,
                pattern, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_delete_pattern(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Delete_Pattern_t *pattern, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_delete_pattern)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_delete_pattern(device_Id,
                pattern, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_wlan_set_change_def_filter_action(qc_drv_context *qc_drv_ctx, uint8_t device_Id,
        qapi_WLAN_Change_Default_Filter_Action_t *filter, size_t size, int flag)
{
    if (is_drv_cb_valid(qc_drv_wlan_set_change_def_filter_action)) {
        return qc_drv_ctx->drv_ops->qc_drv_wlan_set_change_def_filter_action(device_Id,
                filter, size, flag);
    }

    return QCLI_STATUS_ERROR_E;
}

