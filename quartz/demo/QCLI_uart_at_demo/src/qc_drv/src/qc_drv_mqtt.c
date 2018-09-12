/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include  "qc_drv_mqtt.h"

qapi_Status_t qc_drv_net_mqtt_init(qc_drv_context *qc_drv_ctx, char *ca_file)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_init)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_init(ca_file);
	}
	return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_net_mqtt_shutdown(qc_drv_context *qc_drv_ctx)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_shutdown)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_shutdown();
	}
	return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_net_mqtt_new(qc_drv_context *qc_drv_ctx, const char *client_id,
			qbool_t clean_session, qapi_Status_t *status)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_new)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_new(client_id, clean_session, status);
	}
	return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_net_mqtt_destroy(qc_drv_context *qc_drv_ctx, int32_t handle)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_destroy)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_destroy(handle);
	}
	return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_net_mqtt_set_username_password(qc_drv_context *qc_drv_ctx, int32_t handle,
			const char *user, size_t user_len, const char *pw, size_t pw_len,
			qapi_Status_t *status)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_set_username_password)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_set_username_password(handle, user,
								user_len, pw, pw_len, status);
	}
	return QCLI_STATUS_ERROR_E;
}


qapi_Status_t qc_drv_net_mqtt_set_will(qc_drv_context *qc_drv_ctx, int32_t handle,
			const char *topic, size_t topic_len, const char *msg, size_t msg_len,
			uint32_t qos, qbool_t retained, qapi_Status_t *status)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_set_will)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_set_will(handle, topic, topic_len, msg,
				msg_len, qos, retained, status);
	}
	return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_net_mqtt_set_keep_alive(qc_drv_context *qc_drv_ctx, int32_t handle,
			uint16_t keepalive_sec)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_set_keep_alive)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_set_keep_alive(handle, keepalive_sec);
	}
	return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_net_mqtt_set_connack_wait_time(qc_drv_context *qc_drv_ctx, int32_t handle,
			uint16_t max_conn_pending_sec)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_set_connack_wait_time)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_set_connack_wait_time(handle,
			max_conn_pending_sec);
	}
	return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_net_mqtt_set_ssl_config(qc_drv_context *qc_drv_ctx, int32_t handle,
			qapi_Net_SSL_Config_t *mqttc_sslcfg, qapi_Status_t *status)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_set_ssl_config)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_set_ssl_config(handle, mqttc_sslcfg,
			status);
	}
	return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_net_mqtt_connect(qc_drv_context *qc_drv_ctx, int32_t handle, char *host,
			qbool_t secure_session, qbool_t nonblocking_connect, char *bind_if,
			qapi_Status_t *status)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_connect)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_connect(handle, host, secure_session,
			nonblocking_connect, bind_if, status);
	}
	return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_net_mqtt_subscribe(qc_drv_context *qc_drv_ctx, int32_t handle,
			const char *topic, uint32_t qos, qapi_Status_t *status)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_subscribe)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_subscribe(handle, topic, qos, status);
	}
	return QCLI_STATUS_ERROR_E;
}
qapi_Status_t qc_drv_net_mqtt_publish(qc_drv_context *qc_drv_ctx, int32_t handle, const char *topic,
			size_t topic_len, const char *msg, uint32_t msg_len, uint32_t qos,
			qbool_t retained, qbool_t dup, qapi_Status_t *status)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_publish)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_publish(handle, topic, topic_len,
			msg, msg_len, qos, retained, dup, status);
	}
	return QCLI_STATUS_ERROR_E;
}
qapi_Status_t qc_drv_net_mqtt_unsubscribe(qc_drv_context *qc_drv_ctx, int32_t handle,
			const char *topic, qapi_Status_t *status)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_unsubscribe)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_unsubscribe(handle, topic, status);
	}
	return QCLI_STATUS_ERROR_E;
}

qapi_Status_t qc_drv_net_mqtt_disconnect(qc_drv_context *qc_drv_ctx, int32_t handle,
			qapi_Status_t *status)
{
	if (is_drv_cb_valid(qc_drv_net_mqtt_disconnect)) {
		return qc_drv_ctx->drv_ops->qc_drv_net_mqtt_disconnect(handle, status);
	}
	return QCLI_STATUS_ERROR_E;
}

