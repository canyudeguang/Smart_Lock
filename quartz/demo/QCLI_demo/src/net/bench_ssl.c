/* Copyright (c) 2011-2016.  Qualcomm Atheros, Inc.
All rights reserved.

Qualcomm Atheros Confidential and Proprietary.

*/
#include "bench.h"

#ifdef CONFIG_NET_SSL_DEMO
SSL_INST ssl_inst[MAX_SSL_INST];

#ifdef CONFIG_NET_TXRX_DEMO

extern QCLI_Group_Handle_t qcli_net_handle; /* Handle for Net Command Group. */
qapi_Net_SSL_Role_t ssl_role[MAX_SSL_INST];

int32_t bench_ssl_IsInstanceValid(uint32_t instance)
{
	int error = 0;
	if (0 == ssl_role[instance]) {
		error = -1;
		goto end;
	}

	if (ssl_inst[instance].sslCtx == QAPI_NET_SSL_INVALID_HANDLE ||
		ssl_inst[instance].role != QAPI_NET_SSL_SERVER_E) {
		QCLI_Printf(qcli_net_handle, "ERROR: SSL server not started (Use 'ssl start server' first).\n");
		error = -1;
		goto end;
	}

end:
	return error;
}

int bench_ssl_IsDTLS(int instance) 
{
	if ((ssl_inst[instance].config_set == 1) &&
		((ssl_inst[instance].config.protocol == QAPI_NET_SSL_PROTOCOL_DTLS_1_0) ||
		(ssl_inst[instance].config.protocol == QAPI_NET_SSL_PROTOCOL_DTLS_1_2))) {
		return 1;
	}

	return 0;
}

int bench_ssl_GetProtocol()
{
	int ssl_proto;

	if (bench_ssl_IsInstanceValid(SSL_SERVER_INST) < 0) {
		return -1;
	}

	if (bench_ssl_IsDTLS(SSL_SERVER_INST)) {
		ssl_proto = UDP;
	}
	else {
		ssl_proto = TCP;
	}


	return ssl_proto;
}

void bench_ssl_InitInstance(uint32_t instance, uint32_t value)
{
	ssl_role[instance] = value;
}

void bench_ssl_ResetInstance(uint32_t instance)
{
	ssl_role[instance] = (qapi_Net_SSL_Role_t)0;
}

SSL_INST * bench_ssl_GetInstance(int instance)
{
	return &ssl_inst[instance];
}

qapi_Net_SSL_Role_t* bench_ssl_GetSSLRole(int instance)
{
	return &ssl_role[instance];
}

int bench_ssl_Con_Get_Status(bench_ssl_server_inst_t *srv)
{
	int result;
	time_struct_t current_time;
	uint32_t handshake_time_in_ms = 0;

	result = qapi_Net_SSL_Con_Get_Status(srv->connHandle);
	if (result == QAPI_SSL_HS_IN_PROGRESS) {
		app_get_time(&current_time);
		handshake_time_in_ms = app_get_time_difference(&(srv->hs_start_time), &current_time);
		if (handshake_time_in_ms > SSL_ACCEPT_TIMEOUT_IN_MS) {
			result = QAPI_ERR_TIMEOUT;
		}
	}

	return result;
}

void  bench_ssl_Print_SSL_Handshake_Status(int status)
{
	if (status == QAPI_SSL_OK_HS)
	{
		/** The handshake completed successfully */
		QCLI_Printf(qcli_net_handle, "The handshake completed successfully\n");
	}
	else if (status == QAPI_ERR_SSL_CERT_CN)
	{
		/** The peer's SSL certificate is trusted, CN matches the host name, time is expired */
		QCLI_Printf(qcli_net_handle, "ERROR: The certificate is expired\n");
	}
	else if (status == QAPI_ERR_SSL_CERT_TIME)
	{
		/** The peer's SSL certificate is trusted, CN does NOT match the host name, time is valid */
		QCLI_Printf(qcli_net_handle, "ERROR: The certificate is trusted, but the host name is not valid\n");
	}
	else if (status == QAPI_ERR_SSL_CERT_NONE)
	{
		/** The peer's SSL certificate is trusted, CN does NOT match host name, time is expired */
		QCLI_Printf(qcli_net_handle, "ERROR: The certificate is expired and the host name is not valid\n");
	}
	else if  (status == QAPI_ERR_SSL_ALERT_FATAL)
	{
		/** An alert was sent this maybe a response to a close notify from the peer */
		QCLI_Printf(qcli_net_handle, "ERROR: An alert was sent (this may be in response to a close notify from the peer)\n");
	}
	else if (status == QAPI_ERR_SSL_ALERT_RECV)
	{
		/** An alert was received from the peer */
		QCLI_Printf(qcli_net_handle, "ERROR: An alert was received from the peer\n");
	}
	else
	{
		QCLI_Printf(qcli_net_handle, "ERROR: SSL handshake failed (%d)\n", status);
	}
}

qapi_Net_SSL_Con_Hdl_t bench_ssl_rx_setup(SSL_INST *ssl,
											int sockfd,
											qapi_Net_SSL_Con_Hdl_t *sslConnHandle,
											int so_blocking)
{
    int result=0;

    if (*sslConnHandle == QAPI_NET_SSL_INVALID_HANDLE) {
		// Create SSL connection object
		if(bench_ssl_IsDTLS(SSL_SERVER_INST))
			*sslConnHandle = qapi_Net_SSL_Con_New(ssl->sslCtx, QAPI_NET_SSL_DTLS_E);
		else
			*sslConnHandle = qapi_Net_SSL_Con_New(ssl->sslCtx, QAPI_NET_SSL_TLS_E);

		if (*sslConnHandle == QAPI_NET_SSL_INVALID_HANDLE)
		{
			QCLI_Printf(qcli_net_handle, "ERROR: Unable to create SSL context\n");
			return -1;
		}
    }

	result = qapi_Net_SSL_Fd_Set(*sslConnHandle, sockfd);
    if (result < QAPI_OK)
    {
        QCLI_Printf(qcli_net_handle, "ERROR: Unable to add socket handle to SSL\n");
        goto ssl_error;
    }

	// configure the SSL connection
	if (ssl->config_set)
	{
		result = qapi_Net_SSL_Configure(*sslConnHandle, &ssl->config);
		if (result < QAPI_OK)
		{
			QCLI_Printf(qcli_net_handle, "ERROR: SSL configure failed (%d)\n", result);
			goto ssl_error;
		}
	}

    // SSL handshake with server
    if(!bench_ssl_IsDTLS(SSL_SERVER_INST))
    {
		result = qapi_Net_SSL_Accept(*sslConnHandle);

		if (so_blocking) {
			int total_time_waiting_for_accept=0;
			while((result == QAPI_SSL_HS_IN_PROGRESS) && (total_time_waiting_for_accept < SSL_ACCEPT_TIMEOUT_IN_MS)) {
				app_msec_delay(SSL_ACCEPT_SLEEP_IN_MS);
				total_time_waiting_for_accept += SSL_ACCEPT_SLEEP_IN_MS;
				result = qapi_Net_SSL_Con_Get_Status(ssl->ssl);
			}

			if((result != QAPI_SSL_OK_HS) && (total_time_waiting_for_accept >= SSL_ACCEPT_TIMEOUT_IN_MS)) {
				result = QAPI_ERR_TIMEOUT;
			}
		}

		if((result == QAPI_ERROR) || (result == QAPI_ERR_TIMEOUT))
		{
			QCLI_Printf(qcli_net_handle, "ERROR: SSL accept failed (%d)\n", result);
			goto ssl_error;
		}
    }

    return result;

ssl_error:
    qapi_Net_SSL_Shutdown(*sslConnHandle);
    *sslConnHandle = QAPI_NET_SSL_INVALID_HANDLE;
    return result;
}
#endif
#endif
