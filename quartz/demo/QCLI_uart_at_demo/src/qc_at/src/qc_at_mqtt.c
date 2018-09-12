/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qc_api_mqtt.h"
#include "qc_at_mqtt.h"
#include "qosa_util.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
extern QCLI_Context_t QCLI_Context;

/*-------------------------------------------------------------------------
 * Below are the wrapper functions for the UART AT MQTT commands
 *-----------------------------------------------------------------------*/
QCLI_Command_Status_t qc_at_mqtt_help(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (TAKE_LOCK(QCLI_Context.CLI_Mutex))
    {
        Display_Help(QCLI_Context.Executing_Group, Parameter_Count, Parameter_List);
        RELEASE_LOCK(QCLI_Context.CLI_Mutex);
        LOG_AT_OK();
        return QCLI_STATUS_SUCCESS_E;
    }

    LOG_AT_ERROR();
    return QCLI_STATUS_ERROR_E;
}

QCLI_Command_Status_t qc_at_mqtt_client_init(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    char *ca_file = NULL;

    if (Parameter_Count > 0)
    {
        if (!Parameter_List || Parameter_List[0].Integer_Is_Valid){
            LOG_AT_ERROR();
            return QCLI_STATUS_USAGE_E;
        }
        ca_file = Parameter_List[0].String_Value;
    }

    if (0 != qc_api_mqttc_init(Parameter_Count, ca_file))
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_ERROR_E;
    }

    LOG_AT_OK();
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t qc_at_mqtt_client_shut(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (0 != qc_api_mqttc_shut())
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_ERROR_E;
    }

    LOG_AT_OK();
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t qc_at_mqtt_client_new(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (Parameter_Count > 0  && Parameter_Count < 2)
    {
        if (!Parameter_List || Parameter_List[0].Integer_Is_Valid ||
                !Parameter_List[1].Integer_Is_Valid){
            LOG_AT_ERROR();
            return QCLI_STATUS_USAGE_E;
        }
    }

    if (0 != qc_api_mqttc_new(Parameter_Count, Parameter_List))
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_ERROR_E;
    }

    LOG_AT_OK();
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t qc_at_mqtt_client_destroy(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (Parameter_Count != 1 || !Parameter_List || !Parameter_List[0].Integer_Is_Valid)
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if (0 != qc_api_mqttc_destroy(Parameter_Count, Parameter_List[0].Integer_Value))
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_ERROR_E;
    }

    LOG_AT_OK();
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t qc_at_mqtt_client_config(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (Parameter_Count == 3)
    {
        if (!Parameter_List || !Parameter_List[0].Integer_Is_Valid || Parameter_List[1].Integer_Is_Valid || Parameter_List[2].Integer_Is_Valid)
        {
            LOG_AT_ERROR();
            return QCLI_STATUS_USAGE_E;
        }
    }
    else if (Parameter_Count > 3 && Parameter_Count == 6)
    {
        if (!Parameter_List || !Parameter_List[0].Integer_Is_Valid || Parameter_List[1].Integer_Is_Valid || Parameter_List[2].Integer_Is_Valid || Parameter_List[3].Integer_Is_Valid || Parameter_List[4].Integer_Is_Valid || !Parameter_List[5].Integer_Is_Valid)
        {
            LOG_AT_ERROR();
            return QCLI_STATUS_USAGE_E;
        }
    }
    else
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if (0 != qc_api_mqttc_config(Parameter_Count, Parameter_List))
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_ERROR_E;
    }

    LOG_AT_OK();
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t qc_at_mqtt_client_connect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (Parameter_Count == 2)
    {
        if (!Parameter_List || !Parameter_List[0].Integer_Is_Valid || Parameter_List[1].Integer_Is_Valid)
        {
            LOG_AT_ERROR();
            return QCLI_STATUS_USAGE_E;
        }
    }
    else if (Parameter_Count > 2 && Parameter_Count == 7)
    {
        if (!Parameter_List || !Parameter_List[0].Integer_Is_Valid || Parameter_List[1].Integer_Is_Valid || !Parameter_List[2].Integer_Is_Valid || !Parameter_List[3].Integer_Is_Valid || !Parameter_List[4].Integer_Is_Valid || !Parameter_List[5].Integer_Is_Valid || Parameter_List[6].Integer_Is_Valid)
        {
            LOG_AT_ERROR();
            return QCLI_STATUS_USAGE_E;
        }
    }
    else
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if (0 != qc_api_mqttc_connect(Parameter_Count, Parameter_List))
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_ERROR_E;
    }

    LOG_AT_OK();
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t qc_at_mqtt_client_sub(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (Parameter_Count == 2)
    {
        if (!Parameter_List || !Parameter_List[0].Integer_Is_Valid || Parameter_List[1].Integer_Is_Valid)
        {
            LOG_AT_ERROR();
            return QCLI_STATUS_USAGE_E;
        }
    }
    else if (Parameter_Count > 2 && Parameter_Count == 3)
    {
        if (!Parameter_List || !Parameter_List[0].Integer_Is_Valid || Parameter_List[1].Integer_Is_Valid || !Parameter_List[2].Integer_Is_Valid)
        {
            LOG_AT_ERROR();
            return QCLI_STATUS_USAGE_E;
        }
    }
    else
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if (0 != qc_api_mqttc_sub(Parameter_Count, Parameter_List))
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_ERROR_E;
    }

    LOG_AT_OK();
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t qc_at_mqtt_client_pub(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (Parameter_Count == 2)
    {
        if (!Parameter_List || !Parameter_List[0].Integer_Is_Valid ||
                Parameter_List[1].Integer_Is_Valid)
        {
            LOG_AT_ERROR();
            return QCLI_STATUS_USAGE_E;
        }
    }
    else if (Parameter_Count > 2 && Parameter_Count == 6)
    {
        if (!Parameter_List || !Parameter_List[0].Integer_Is_Valid || Parameter_List[1].Integer_Is_Valid || !Parameter_List[2].Integer_Is_Valid || Parameter_List[3].Integer_Is_Valid || !Parameter_List[4].Integer_Is_Valid || !Parameter_List[5].Integer_Is_Valid)
        {
            LOG_AT_ERROR();
            return QCLI_STATUS_USAGE_E;
        }
    }
    else
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if (0 != qc_api_mqttc_pub(Parameter_Count, Parameter_List))
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_ERROR_E;
    }

    LOG_AT_OK();
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t qc_at_mqtt_client_unsub(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (Parameter_Count == 1)
    {
        if (!Parameter_List || !Parameter_List[0].Integer_Is_Valid)
        {
            LOG_AT_ERROR();
            return QCLI_STATUS_USAGE_E;
        }
    }
    else if (Parameter_Count > 1 && Parameter_Count == 2)
    {
        if (!Parameter_List || !Parameter_List[0].Integer_Is_Valid || Parameter_List[1].Integer_Is_Valid)
        {
            LOG_AT_ERROR();
            return QCLI_STATUS_USAGE_E;
        }
    }
    else
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if (0 != qc_api_mqttc_unsub(Parameter_Count, Parameter_List))
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_ERROR_E;
    }

    LOG_AT_OK();
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t qc_at_mqtt_client_disconnect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (Parameter_Count != 1 || !Parameter_List || !Parameter_List[0].Integer_Is_Valid)
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if (0 != qc_api_mqttc_disconnect(Parameter_Count, Parameter_List[0].Integer_Value))
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_ERROR_E;
    }

    LOG_AT_OK();
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t qc_at_mqtt_client_ssl_config(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (Parameter_Count < 1 || !Parameter_List || !Parameter_List[0].Integer_Is_Valid)
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if (0 != qc_api_mqttc_sslconfig(Parameter_Count, Parameter_List))
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_ERROR_E;
    }

    LOG_AT_OK();
    return QCLI_STATUS_SUCCESS_E;
}

const QCLI_Command_t atm_cmd_list[] =
{
    /*  cmd function                  flag     cmd_string             usage_string                                Description      */
    { qc_at_mqtt_help,                 false,   "HELP",                "",                                         "Display the MQTT commands."   },
    { qc_at_mqtt_client_init,          false,   "CINIT",               "[<ca_file>]",                              "Initialize the MQTT service."   },
    { qc_at_mqtt_client_shut,          false,   "CSHUT",               "",                                "Shutdown MQTT service."   },
    { qc_at_mqtt_client_new,           false,   "CNEW",                "[<client_id>,<session_ID>]",               "Creates a MQTT session, taking the client name and the session ID."   },
    { qc_at_mqtt_client_destroy,       false,   "CDESTROY",            "<session_id>",                             "Destroys the MQTT session, taking the session ID as the parameter for which session to destroy."   },
    { qc_at_mqtt_client_config,        false,   "CCONFIG",             "<session_id>,><user_name>,<password>,[<topic>,<message>,<qos>]",    "Configures the MQTT client parameters like username, password, topic, message, QOS."   },
    { qc_at_mqtt_client_connect,       false,   "CCONNECT",            "<session_id>,<server_IP>,[<ss>,<nb>,<keep_alive>,<wait_time>,<interface_name>]",                                 "Connect to the MQTT server."   },
    { qc_at_mqtt_client_sub,           false,   "CSUB",                "<session_id>,<topic_filter>,[<qos>]",       "Subscribe the topic."   },
    { qc_at_mqtt_client_pub,           false,   "CPUB",                "<session_id>,<topic_filter>,[<qos>,<message>,<retain>,<duplicate>]",                                                                "Publish the message about some certain topics."   },
    { qc_at_mqtt_client_unsub,         false,   "CUNSUB",              "<session_id>,[topic_filter]",             "Un-subscribe from a topic."   },
    { qc_at_mqtt_client_disconnect,    false,   "CDISCONNECT",         "<session_id>",                  "Disconnect from the MQTT server."   },
    { qc_at_mqtt_client_ssl_config,    false,   "CSSLCONFIG",          "<session_id>[<keyword_1>,<value_1>],[<keyword_2>,<keyword_2>].....",    "Get/set Country code to be set."   }
};

const QCLI_Command_Group_t atm_cmd_group =
{
    "ATMQTT",
    (sizeof(atm_cmd_list)/sizeof(atm_cmd_list[0])),
    atm_cmd_list
};

/* This function is used to register the ATWLAN Command Group  */
void Initialize_ATM_Demo(void)
{
    /* Attempt to register the Command Groups with the UART AT framework.*/
    QCLI_Register_Command_Group(NULL, &atm_cmd_group);
}
