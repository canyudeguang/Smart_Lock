/*
* Copyright (c) 2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "qapi_status.h"
#include "net_bench.h"
#include "qapi_netservices.h"
#include "qapi_crypto.h"
#include "qcli_api.h"
#include "qc_api_main.h"
#include "qc_drv_net.h"
#include "qosa_util.h"

#define MAX_PASSWORD_SIZE_IN_BYTES 256

/* TODO Create default certificate/CA list to be used for testing */
const uint16_t ssl_default_calist_len;

QCLI_Command_Status_t ssl_set_dtls_server_max_clients(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ssl_set_dtls_server_idle_timer(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ssl_set_ecjpake_params(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

/*QCLI_Command_Status_t ssl_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if(Parameter_Count < 1)
    {
        return QCLI_STATUS_USAGE_E;
    }

    if (0 == strncmp(Parameter_List[0].String_Value, "start", 3)) {
        return ssl_start(Parameter_Count - 1, &Parameter_List[1]);
    }
    else if (0 == strncmp(Parameter_List[0].String_Value, "stop", 3)) {
        return ssl_stop(Parameter_Count - 1, &Parameter_List[1]);
    }
    else if (0 == strncmp(Parameter_List[0].String_Value, "config", 3)) {
        return ssl_config(Parameter_Count - 1, &Parameter_List[1]);
    }
    else if (0 == strncmp(Parameter_List[0].String_Value, "cert", 3)) {
        return ssl_add_cert(Parameter_Count - 1, &Parameter_List[1]);
    }
    else if (0 == strncmp(Parameter_List[0].String_Value, "psk", 3)) {
        return ssl_add_psk_table(Parameter_Count - 1, &Parameter_List[1]);
    }
    else if (0 == strncmp(Parameter_List[0].String_Value, "ecjpake", 7)) {
        return ssl_set_ecjpake_params(Parameter_Count - 1, &Parameter_List[1]);
    }
    else if (0 == strncmp(Parameter_List[0].String_Value, "max_clients", 10)) {
        return ssl_set_dtls_server_max_clients(Parameter_Count - 1, &Parameter_List[1]);
    }
    else if (0 == strncmp(Parameter_List[0].String_Value, "idle_timer", 10)) {
        return ssl_set_dtls_server_idle_timer(Parameter_Count - 1, &Parameter_List[1]);
    }
	else
    {
        return QCLI_STATUS_USAGE_E;
    }
}*/

QCLI_Command_Status_t ssl_start(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    SSL_INST *ssl;
    qapi_Net_SSL_Role_t role;

    if(Parameter_Count < 1)
    {
usage:
        LOG_AT("\nUsage: ssl start <server|client>\n");
        return QCLI_STATUS_ERROR_E;
    }

    if (0 == strncmp(Parameter_List[0].String_Value, "server", 3))
    {
        ssl = &ssl_inst[SSL_SERVER_INST];
        role = QAPI_NET_SSL_SERVER_E;
    }
    else if (0 == strncmp(Parameter_List[0].String_Value, "client", 3))
    {
        ssl = &ssl_inst[SSL_CLIENT_INST];
        role = QAPI_NET_SSL_CLIENT_E;
    }
    else
    {
       goto usage;
    }

    if (ssl->sslCtx != QAPI_NET_SSL_INVALID_HANDLE)
    {
        LOG_ERR("ERROR: one SSL context only!\n");
        return QCLI_STATUS_ERROR_E;
    }

    // Create SSL context
    memset(ssl, 0, sizeof(SSL_INST));
    ssl->role = role;
    ssl->sslCtx = qc_drv_net_SSL_Obj_New(qc_api_get_qc_drv_context(), role);

    if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
    {
        LOG_ERR("ERROR: Unable to create SSL context\n");
        return QCLI_STATUS_ERROR_E;
    }

    // Reset config struct
    memset(&ssl->config, 0, sizeof(qapi_Net_SSL_Config_t));

    // Done
    LOG_AT("SSL %s started\n", (role == QAPI_NET_SSL_CLIENT_E) ? "client" : "server");
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t ssl_stop(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    SSL_INST *ssl;
    qapi_Net_SSL_Role_t role;

    if(Parameter_Count < 1)
    {
usage:
        LOG_AT("\nUsage: ssl stop <server|client>\n");
        return QCLI_STATUS_ERROR_E;
    }

    if (0 == strncmp(Parameter_List[0].String_Value, "server", 3))
    {
        ssl = &ssl_inst[SSL_SERVER_INST];
        role = QAPI_NET_SSL_SERVER_E;
    }
    else if (0 == strncmp(Parameter_List[0].String_Value, "client", 3))
    {
        ssl = &ssl_inst[SSL_CLIENT_INST];
        role = QAPI_NET_SSL_CLIENT_E;
    }
    else
    {
        goto usage;
    }

    if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE || role != ssl->role)
    {
        LOG_ERR("ERROR: SSL %s not started\n", (role == QAPI_NET_SSL_CLIENT_E) ? "client" : "server");
        return QCLI_STATUS_ERROR_E;
    }

    if (ssl->ssl)
    {
        qc_drv_net_SSL_Shutdown(qc_api_get_qc_drv_context(), ssl->ssl);
        ssl->ssl = QAPI_NET_SSL_INVALID_HANDLE;
    }

    if (ssl->sslCtx)
    {
        qc_drv_net_SSL_Obj_Free(qc_api_get_qc_drv_context(), ssl->sslCtx);
        ssl->sslCtx = QAPI_NET_SSL_INVALID_HANDLE;
    }

    ssl_free_config_parameters(&ssl->config);

    LOG_AT("SSL %s stopped\n", (role == QAPI_NET_SSL_CLIENT_E) ? "client" : "server");
    return QCLI_STATUS_SUCCESS_E;
}

void sslconfig_help(const char *str)
{
    LOG_INFO("%s [<keyword_1> <value_1>] [<keyword_2> <value_2>] ...\n", str);
    LOG_INFO(" where <keyword> <value> are:\n");
    LOG_INFO("       protocol <protocol> = select protocol: TLS (default), TLS1.0, TLS1.1, TLS1.2, DTLS, DTLS1.0, DTLS1.2\n");
    LOG_INFO("       time 0|1            = disable|enable certificate time validation (optional)\n");
    LOG_INFO("       domain 0|<name>     = disable|enable validation of peer's domain name against <name>\n");
    LOG_INFO("       alert 0|1           = disable|enable sending of SSL alert if certificate validation fails.\n");
    LOG_INFO("       cipher <cipher>     = select <cipher> (enum name) suite to use, can be repeated 8 times (optional)\n");
    LOG_INFO("       max_frag_len <bytes>= max fragment length in bytes\n");
    LOG_INFO("       neg_disable 0|1     = disable|enable maximum fragment length negotiation\n");
    LOG_INFO("       sni <name>          = configure name for server name indication (SNI)\n");
    LOG_INFO("       alpn <protocol_name>= specify protocol name for ALPN, e.g. \"h2\", \"http/1.1\"\n");
}

void ssl_free_config_parameters(qapi_Net_SSL_Config_t *cfg)
{
    if (cfg == NULL)
    {
        return;
    }

	if (cfg->sni_Name != NULL)
    {
		free(cfg->sni_Name);
        cfg->sni_Name = NULL;
        cfg->sni_Name_Size = 0;
	}

    /* ignore 'alpn' and 'extensions' for now */
}

/*****************************************************************************
 * [0]      [1]     [2]   [3] [4]    [5]
 * protocol TLS1.1  time   1  cipher QAPI_NET_TLS_RSA_WITH_AES_256_GCM_SHA384
 *****************************************************************************/
QCLI_Command_Status_t ssl_parse_config_parameters(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, qapi_Net_SSL_Config_t *cfg, qapi_Net_SSL_Obj_Hdl_t ssl_hdl, qbool_t server)
{
    uint32_t argn;

    memset(cfg, 0, sizeof(*cfg));

    /* Set the default protocol to TLS */
    cfg->protocol = QAPI_NET_SSL_TLS_E;

    for (argn = 0; argn < Parameter_Count; ++argn)
    {
        if (argn == Parameter_Count-1)
        {
            LOG_AT("What is value of %s?\n", Parameter_List[argn].String_Value);
            return QCLI_STATUS_ERROR_E;
        }

        /* 'protocol TLS1.2' */
        if (0 == strncmp("protocol", Parameter_List[argn].String_Value, 8))
        {
            argn++;

            // Setting of protocol version is supported for SSL client only,
            // but SSL server uses this to set the protocol type (DTLS/TLS).
            if (server)
            {
                LOG_WARN("Warning: Server uses only protocol type (DTLS/TLS) and ignores version (1.1, 1.2, ...).\n");
            }

            if (0 == strcmp("TLS1.0", Parameter_List[argn].String_Value))
            {
                cfg->protocol = QAPI_NET_SSL_PROTOCOL_TLS_1_0;
            }
            else if (0 == strcmp("TLS1.1", Parameter_List[argn].String_Value))
            {
                cfg->protocol = QAPI_NET_SSL_PROTOCOL_TLS_1_1;
            }
            else if (0 == strcmp("TLS1.2", Parameter_List[argn].String_Value))
            {
                cfg->protocol = QAPI_NET_SSL_PROTOCOL_TLS_1_2;
            }
            else if (0 == strcmp("TLS", Parameter_List[argn].String_Value))
            {
                cfg->protocol = QAPI_NET_SSL_TLS_E;
            }
            else if (0 == strcmp("DTLS1.0", Parameter_List[argn].String_Value))
            {
                cfg->protocol = QAPI_NET_SSL_PROTOCOL_DTLS_1_0;
            }
            else if (0 == strcmp("DTLS1.2", Parameter_List[argn].String_Value))
            {
                cfg->protocol = QAPI_NET_SSL_PROTOCOL_DTLS_1_2;
            }
            else if (0 == strcmp("DTLS", Parameter_List[argn].String_Value))
            {
                cfg->protocol = QAPI_NET_SSL_DTLS_E;
            }
            else
            {
                LOG_ERR("ERROR: Invalid protocol: %s\n", Parameter_List[argn].String_Value);
                return QCLI_STATUS_ERROR_E;
            }
        }

        /* 'time 1' */
        else if (0 == strncmp("time", Parameter_List[argn].String_Value, 4))
        {
            argn++;

            if (0 == strcmp("1", Parameter_List[argn].String_Value))
            {
                cfg->verify.time_Validity = 1;
            }
            else if (0 == strcmp("0", Parameter_List[argn].String_Value))
            {
                cfg->verify.time_Validity = 0;
            }
            else
            {
                LOG_ERR("ERROR: Invalid option: %s\n", Parameter_List[argn].String_Value);
                return QCLI_STATUS_ERROR_E;
            }
        }

        /* 'alert 0 ' */
        else if (0 == strncmp("alert", Parameter_List[argn].String_Value, 5))
        {
            argn++;

            if (0 == strcmp("1", Parameter_List[argn].String_Value))
            {
                cfg->verify.send_Alert = 1;
            }
            else if (0 == strcmp("0", Parameter_List[argn].String_Value))
            {
                cfg->verify.send_Alert = 0;
            }
            else
            {
                LOG_ERR("ERROR: Invalid option: %s\n", Parameter_List[argn].String_Value);
                return QCLI_STATUS_ERROR_E;
            }
        }

        /* 'domain 0' */
        else if (0 == strncmp("domain", Parameter_List[argn].String_Value, 6))
        {
            argn++;

            if (0 == strcmp("0", Parameter_List[argn].String_Value))
            {
                cfg->verify.domain = 0;
                cfg->verify.match_Name[0] = '\0';
            }
            else
            {
                cfg->verify.domain = 1;
                if (strlen(Parameter_List[argn].String_Value) >= sizeof(cfg->verify.match_Name))
                {
                    LOG_ERR("ERROR: %s too long (max %d chars)\n", Parameter_List[argn].String_Value, sizeof(cfg->verify.match_Name));
                    return QCLI_STATUS_ERROR_E;
                }
                strcpy(cfg->verify.match_Name, Parameter_List[argn].String_Value);
            }
        }

        /* 'cipher QAPI_NET_TLS_RSA_WITH_AES_256_GCM_SHA384' */
        else if (0 == strncmp("cipher", Parameter_List[argn].String_Value, 6))
        {
            qbool_t is_valid_cipher = true;
            qapi_Status_t cipher_selection_status = QAPI_OK;

            argn++;

            if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_256_GCM_SHA384", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_RSA_WITH_AES_256_GCM_SHA384);
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_256_CBC_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_RSA_WITH_AES_256_CBC_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_256_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_RSA_WITH_AES_256_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_128_GCM_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_RSA_WITH_AES_128_GCM_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_128_CBC_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_RSA_WITH_AES_128_CBC_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_128_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_RSA_WITH_AES_128_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384);
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_128_CCM", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_RSA_WITH_AES_128_CCM);
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_256_CCM", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_RSA_WITH_AES_256_CCM);
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CCM", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CCM);
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CCM", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CCM);
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_128_CCM_8", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_RSA_WITH_AES_128_CCM_8);
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_256_CCM_8", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_RSA_WITH_AES_256_CCM_8);
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CCM_8", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CCM_8);
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CCM_8", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CCM_8);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECJPAKE_WITH_AES_128_CCM_8", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECJPAKE_WITH_AES_128_CCM_8);
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_128_GCM_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_PSK_WITH_AES_128_GCM_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_256_GCM_SHA384", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_PSK_WITH_AES_256_GCM_SHA384);
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA256);
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA384", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA384);
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA);
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_128_CCM_8", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_PSK_WITH_AES_128_CCM_8);
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256", Parameter_List[argn].String_Value))
            {
                cipher_selection_status = qc_drv_net_SSL_Cipher_Add(qc_api_get_qc_drv_context(), cfg, QAPI_NET_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256);
            }
            else
            {
                LOG_WARN("Warning: Ignoring invalid cipher: %s\n", Parameter_List[argn].String_Value);
                is_valid_cipher = false;
            }

            if (is_valid_cipher && QAPI_OK != cipher_selection_status)
            {
                LOG_WARN("Warning: Unsupported cipher: %s\n", Parameter_List[argn].String_Value);
            }
        }

        /* 'max_frag_len 1000' */
        else if (0 == strncmp("max_frag_len", Parameter_List[argn].String_Value, 12))
        {
            argn++;
            cfg->max_Frag_Len = atoi(Parameter_List[argn].String_Value);
        }

        /* 'neg_disable 1' */
        else
        if (0 == strncmp("neg_disable", Parameter_List[argn].String_Value, 11))
        {
            argn++;
            cfg->max_Frag_Len_Neg_Disable = atoi(Parameter_List[argn].String_Value);
        }

        /* 'sni xxxx' */
        else if(0 == strncmp("sni", Parameter_List[argn].String_Value, 3))
        {
            argn++;

            if (server)
            {
                LOG_WARN("Warning: ignoring SNI option, this only valid for clients\n");
            }
            else
            {
                cfg->sni_Name_Size = strlen(Parameter_List[argn].String_Value);
                cfg->sni_Name = malloc(cfg->sni_Name_Size);
                if (cfg->sni_Name == NULL)
                {
                    LOG_ERR("Error: unable to allocate memory for password\n");
                    return QCLI_STATUS_ERROR_E;
                }
                memcpy(cfg->sni_Name, Parameter_List[argn].String_Value, cfg->sni_Name_Size);
            }
        }

        /* 'alpn http/1.1' */
        else if (strncmp("alpn", Parameter_List[argn].String_Value, 4) == 0) {
        	int ret = 0;
        	argn++;

            /* ignore 'alpn' parameter if ssl object handle is zero */
            if (ssl_hdl != 0)
            {
                ret = qc_drv_net_SSL_ALPN_Protocol_Add(qc_api_get_qc_drv_context(), ssl_hdl, Parameter_List[argn].String_Value);
                if (ret != 0) {
                    LOG_ERR("Error: ALPN Protocol add failed %d\n", ret);
                    return QCLI_STATUS_ERROR_E;
                }
            }
        }

    } /*for*/

    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t ssl_config(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    SSL_INST *ssl;
    int32_t res;
    qbool_t server = false;
    QCLI_Command_Status_t status;

    if(Parameter_Count < 1)
    {
        goto usage;
    }

    if (0 == strncmp(Parameter_List[0].String_Value, "server", 3))
    {
        ssl = &ssl_inst[SSL_SERVER_INST];
        server = true;
    }
    else if (0 == strncmp(Parameter_List[0].String_Value, "client", 3))
    {
        ssl = &ssl_inst[SSL_CLIENT_INST];
    }
    else
    {
        goto usage;
    }

    if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
    {
        LOG_ERR("ERROR: SSL %s not started\n", Parameter_List[0].String_Value);
        return QCLI_STATUS_ERROR_E;
    }

    ssl->config_set = 0;
    /* Parse SSL config parameters from command line */
    status = ssl_parse_config_parameters(
                    Parameter_Count-1,
                    &Parameter_List[1],
                    &ssl->config,
                    ssl->sslCtx,
                    server);
    if (status == QCLI_STATUS_USAGE_E)
    {
        goto usage;
    }
    else if (status != QCLI_STATUS_SUCCESS_E)
    {
        return QCLI_STATUS_ERROR_E;
    }

    if (ssl->ssl == QAPI_NET_SSL_INVALID_HANDLE)
    {
        // Create SSL connection object
        qapi_Net_SSL_Protocol_t proto;
        switch(ssl->config.protocol)
        {
            case QAPI_NET_SSL_PROTOCOL_TLS_1_0:
            case QAPI_NET_SSL_PROTOCOL_TLS_1_1:
            case QAPI_NET_SSL_PROTOCOL_TLS_1_2:
                proto = QAPI_NET_SSL_TLS_E;
                break;
            case QAPI_NET_SSL_PROTOCOL_DTLS_1_0:
            case QAPI_NET_SSL_PROTOCOL_DTLS_1_2:
                proto = QAPI_NET_SSL_DTLS_E;
                break;
            default:
                /* No specific protocol, negotiate */
                proto = ssl->config.protocol;
                ssl->config.protocol = 0;
                break;
        }
        ssl->ssl = qc_drv_net_SSL_Con_New(qc_api_get_qc_drv_context() ,ssl->sslCtx, proto);
        if (ssl->ssl == QAPI_NET_SSL_INVALID_HANDLE)
        {
            LOG_ERR("ERROR: SSL configure failed (Unable to create SSL context)\n");
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        if (ssl->config.protocol <= QAPI_NET_SSL_DTLS_E)
            ssl->config.protocol = 0;
    }

    // configure the SSL connection
    ssl->config_set = 1;
    res = qc_drv_net_SSL_Configure(qc_api_get_qc_drv_context(), ssl->ssl, &ssl->config);
    if (res < 0)
    {
        LOG_ERR("ERROR: SSL configure failed (%d)\n", res);
        return QCLI_STATUS_ERROR_E;
    }
    LOG_AT("SSL %s configuration changed\n", Parameter_List[0].String_Value);
    return QCLI_STATUS_SUCCESS_E;

usage:
    sslconfig_help("\nssl config <server|client>");
    return QCLI_STATUS_ERROR_E;
}

QCLI_Command_Status_t ssl_add_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    SSL_INST *ssl;
    qapi_Net_SSL_Cert_Type_t type;
    char *name = NULL;

    if(Parameter_Count < 3)
    {
usage:
        LOG_AT("\nUsage: ssl cert <server|client> <certificate|calist> <name>\r\n"
                    " This adds a certificate or CA list to either SSL server or client.\r\n"
                    "      where <name> = name of file to load from secure storage repository.\r\n");
        return QCLI_STATUS_ERROR_E;
    }

    if (0 == strncmp(Parameter_List[0].String_Value, "server", 3))
    {
        ssl = &ssl_inst[SSL_SERVER_INST];
        if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
        {
            LOG_ERR("ERROR: SSL server not started\n");
            return QCLI_STATUS_ERROR_E;
        }
    }
    else if (0 == strncmp(Parameter_List[0].String_Value, "client", 3))
    {
        ssl = &ssl_inst[SSL_CLIENT_INST];
        if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
        {
            LOG_ERR("ERROR: SSL client not started\n");
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        goto usage;
    }

    if (0 == strncmp("certificate", Parameter_List[1].String_Value, 3))
    {
        type = QAPI_NET_SSL_CERTIFICATE_E;
    }
    else if (0 == strncmp("calist", Parameter_List[1].String_Value, 3))
    {
        type = QAPI_NET_SSL_CA_LIST_E;
    }
    else
    {
        goto usage;
    }

    name = Parameter_List[2].String_Value;

    // Load/add certificate
    if (name != NULL)
    {
        if (qc_drv_net_SSL_Cert_Load(qc_api_get_qc_drv_context(), ssl->sslCtx, type, name) < 0)
        {
            LOG_ERR("ERROR: Unable to load %s from secure storage repository\n" , name);
            return QCLI_STATUS_ERROR_E;
        }
        LOG_INFO("%s loaded from secure storage repository\n", name);
    }
    else
    {
        LOG_ERR("ERROR: Must specify name for certificate / CA list\r\n");
        goto usage;
    }
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t ssl_set_dtls_server_max_clients(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if(Parameter_Count < 1)
    {
        LOG_AT("\nUsage: ssl max_clients <max_Clients> \r\n"
                    " Sets DTLS maximum number of clients per server connection.  Defaults to 1 if not set.\r\n");
        return QCLI_STATUS_ERROR_E;
    }

    SSL_INST *ssl;
    ssl = &ssl_inst[SSL_SERVER_INST];
    if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
    {
        LOG_ERR("ERROR: SSL server not started\n");
        return QCLI_STATUS_ERROR_E;
    }

    qapi_Status_t status = qc_drv_net_SSL_Max_Clients_Set(qc_api_get_qc_drv_context(), ssl->sslCtx, Parameter_List[0].Integer_Value);
	if(status != QAPI_OK) {
        LOG_ERR("ERROR: error setting max_clients, (status = %d)\n", status);
        return QCLI_STATUS_ERROR_E;
	}

	return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t ssl_set_dtls_server_idle_timer(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if(Parameter_Count < 1)
    {
        LOG_AT("\nUsage: ssl idle_timer <idle_timeout> \r\n"
                    " For DTLS servers, sets the number of seconds to wait before closing an idle client connection.\r\n");
        return QCLI_STATUS_ERROR_E;
    }

    SSL_INST *ssl;
    ssl = &ssl_inst[SSL_SERVER_INST];
    if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
    {
        LOG_ERR("ERROR: SSL server not started\n");
        return QCLI_STATUS_ERROR_E;
    }

    qapi_Status_t status = qc_drv_net_SSL_Idle_Timeout_Set(qc_api_get_qc_drv_context(), ssl->sslCtx, Parameter_List[0].Integer_Value);
	if(status != QAPI_OK) {
        LOG_ERR("ERROR: error setting idle_timeout, (status = %d)\n", status);
        return QCLI_STATUS_ERROR_E;
	}

	return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t ssl_set_ecjpake_params(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if(Parameter_Count < 2)
    {
usage:
        LOG_AT("\nUsage: ssl ecjpake <server|client> <password>\r\n"
                    " Sets the password for ECJPAKE cipher suite (ECJPAKE).\r\n");
        return QCLI_STATUS_ERROR_E;
    }

	SSL_INST *ssl;
    if (0 == strncmp(Parameter_List[0].String_Value, "server", 3))
    {
        ssl = &ssl_inst[SSL_SERVER_INST];
        if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
        {
            LOG_ERR("ERROR: SSL server not started\n");
            return QCLI_STATUS_ERROR_E;
        }
    }
    else if (0 == strncmp(Parameter_List[0].String_Value, "client", 3))
    {
        ssl = &ssl_inst[SSL_CLIENT_INST];
        if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
        {
            LOG_ERR("ERROR: SSL client not started\n");
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        goto usage;
    }

    qapi_Net_SSL_ECJPAKE_Params_t ecjpake_params = {0};
    ecjpake_params.password = (uint8_t *)Parameter_List[1].String_Value;
    ecjpake_params.password_Size = strlen(Parameter_List[1].String_Value);
    qapi_Status_t status = qc_drv_net_SSL_ECJPAKE_Parameters_Set(qc_api_get_qc_drv_context(), ssl->sslCtx, &ecjpake_params);
	if(status != QAPI_OK) {
        LOG_ERR("ERROR: error setting ecjpake parameters (status = %d)\n", status);
        return QCLI_STATUS_ERROR_E;
	}

	return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t ssl_add_psk_table(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if(Parameter_Count < 3)
    {
usage:
        LOG_AT("\nUsage: ssl psk <server|client> <identity> <psk>\r\n"
                    " This creates a psk table for either the SSL server or client.\r\n");
        return QCLI_STATUS_ERROR_E;
    }

	SSL_INST *ssl;
    if (0 == strncmp(Parameter_List[0].String_Value, "server", 3))
    {
        ssl = &ssl_inst[SSL_SERVER_INST];
        if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
        {
            LOG_ERR("ERROR: SSL server not started\n");
            return QCLI_STATUS_ERROR_E;
        }
    }
    else if (0 == strncmp(Parameter_List[0].String_Value, "client", 3))
    {
        ssl = &ssl_inst[SSL_CLIENT_INST];
        if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
        {
            LOG_ERR("ERROR: SSL client not started\n");
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        goto usage;
    }

	qapi_Net_SSL_PSK_t psk_list =
	{
		.identity = (uint8_t *)Parameter_List[1].String_Value,
		.identity_Size = strlen(Parameter_List[1].String_Value),
		.psk = (uint8_t *)Parameter_List[2].String_Value,
		.psk_Size = strlen(Parameter_List[2].String_Value)
	};
	if(qc_drv_net_SSL_PSK_Table_Set(qc_api_get_qc_drv_context(), ssl->sslCtx, &psk_list, 1) != QAPI_OK) {
        LOG_ERR("ERROR: error setting psk table\n");
        return QCLI_STATUS_ERROR_E;
	}

	return QCLI_STATUS_SUCCESS_E;
}
