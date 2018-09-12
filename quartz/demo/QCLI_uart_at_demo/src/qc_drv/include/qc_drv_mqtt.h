/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QC_DRV_MQTT_H
#define __QC_DRV_MQTT_H

#include "qc_drv_main.h"

qapi_Status_t qc_drv_net_mqtt_init(qc_drv_context *qc_drv_ctx, char *ca_file);

qapi_Status_t qc_drv_net_mqtt_shutdown(qc_drv_context *qc_drv_ctx);

qapi_Status_t qc_drv_net_mqtt_new(qc_drv_context *qc_drv_ctx, const char *client_id, qbool_t clean_session, qapi_Status_t *status);

qapi_Status_t qc_drv_net_mqtt_destroy(qc_drv_context *qc_drv_ctx, int32_t handle);

qapi_Status_t qc_drv_net_mqtt_set_username_password(qc_drv_context *qc_drv_ctx, int32_t handle, const char *user, size_t user_len, const char *pw, size_t pw_len, qapi_Status_t *status);

qapi_Status_t qc_drv_net_mqtt_set_will(qc_drv_context *qc_drv_ctx, int32_t handle, const char *topic, size_t topic_len, const char *msg, size_t msg_len, uint32_t qos, qbool_t retained, qapi_Status_t *status);

qapi_Status_t qc_drv_net_mqtt_set_keep_alive(qc_drv_context *qc_drv_ctx, int32_t handle, uint16_t keepalive_sec);

qapi_Status_t qc_drv_net_mqtt_set_connack_wait_time(qc_drv_context *qc_drv_ctx, int32_t handle, uint16_t max_conn_pending_sec);

qapi_Status_t qc_drv_net_mqtt_set_ssl_config(qc_drv_context *qc_drv_ctx, int32_t handle, qapi_Net_SSL_Config_t *mqttc_sslcfg, qapi_Status_t *status);

qapi_Status_t qc_drv_net_mqtt_connect(qc_drv_context *qc_drv_ctx, int32_t handle, char *host, qbool_t secure_session, qbool_t nonblocking_connect, char *bind_if, qapi_Status_t *status);

qapi_Status_t qc_drv_net_mqtt_subscribe(qc_drv_context *qc_drv_ctx, int32_t handle, const char *topic, uint32_t qos, qapi_Status_t *status);

qapi_Status_t qc_drv_net_mqtt_publish(qc_drv_context *qc_drv_ctx, int32_t handle, const char *topic, size_t topic_len, const char *msg, uint32_t msg_len, uint32_t qos, qbool_t retained, qbool_t dup, qapi_Status_t *status);

qapi_Status_t qc_drv_net_mqtt_unsubscribe(qc_drv_context *qc_drv_ctx, int32_t handle, const char *topic, qapi_Status_t *status);

qapi_Status_t qc_drv_net_mqtt_disconnect(qc_drv_context *qc_drv_ctx, int32_t handle, qapi_Status_t *status);

#endif
