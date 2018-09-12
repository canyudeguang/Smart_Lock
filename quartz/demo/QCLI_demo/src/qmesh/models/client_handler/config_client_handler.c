/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/


 /* config Model Headers */
#include "config_client_handler.h"
#include "qmesh_demo_utilities.h"
#include "qmesh_demo_debug.h"
#include "qmesh_demo_composition.h"

#define QCLI_DFLT_TTL 10
/* Enable Config client Model */
#define QMESH_CONFIG_CLIENT_MODEL_ENABLE 1

/* Upper 4 bits of net idx represents provisioned net ID */
#define PROVISIONED_NET_ID_MASK 0xF000

/* Lower 12 bits of net idx represents 12 bit global app/net key ID */
#define KEY_ID_MASK 0x0FFF


extern QCLI_Group_Handle_t     mesh_group;

/* QMesh config client Model Commands */
const QCLI_Command_t qmesh_config_model_cmd_list[] =
{

/* Config client Model APIs */
#if (QMESH_CONFIG_CLIENT_MODEL_ENABLE == 1)
    {
        ConfigBeaconGet,
        false,
        "ConfigBeaconGet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)] [dst addr(2 octets)]",
        "send ConfigbeaconGet cmd to peer device"
    },
    {
       ConfigBeaconSet,
       false,
       "ConfigBeaconSet" ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)] [dst addr(2 octets)] \
        [beacon state(1octet)]",
       "send ConfigBeaconSet cmd to peer device"
   },
   {
        ConfigGetCompData ,
        false,
        "ConfigGetCompData",
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)] [dst addr(2 octets)] \
         [page number(1 octet)]",
        "Send ConfigGetCompData to peer device"
    },
    {
        ConfigDefaultTtlSet,
        false,
        "ConfigDefaultTtlSet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
         [ttl(1octet)]",
        "send ConfigDefaultTtlSet  cmd to peer device"
    },
    {
       ConfigDefaultTtlget,
       false,
       "ConfigDefaultTtlget" ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)]",
       "send ConfigDefaultTtlget cmd to peer device"
    },
    {
        ConfigRelayGet,
        false,
        "ConfigRelayGet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
         [elm addr(2octets)] ",
        "send ConfigRelayGet cmd to peer device"
    },
    {
        ConfigRelaySet,
        false,
        "ConfigRelaySet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
         [relay state (1octet)] [rtx cnt(1octet 3bits)] [rtx step(1octet 5bits)]",
        "send ConfigRelaySet cmd to peer device "
    },
    {
        ConfigNetKeyAdd,
        false,
        "ConfigNetKeyAdd" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
         [Netkey idx (2octets)] [NetKey (16octets in hex)]",
        "Add App keys associated with given netkey"
    },
    {
        ConfigNetKeyUpdate,
        false,
        "ConfigNetKeyUpdate" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
         [Netkey idx(2octets)][Netkey(16octets)]",
        "send ConfigNetKeyUpdate cmd to peer device"
    },
    {
        ConfigNetKeyDelete,
        false,
        "ConfigNetKeyDelete" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
         [NetKey idx(2octets)]",
        "send ConfigNetKeyDelete cmd to peer device"
    },
    {
        ConfigNetKeyGet,
        false,
        "ConfigNetKeyGet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)]",
        "send Network key request "
    },
    {
        ConfigAddAppKey ,
        false,
        "ConfigAddAppKey",
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)] [dst addr(2 octets)] \
         [appkeyIdx (2octets)] [netkeyIdx (2octets)] [AppKey(16octet in hex))]",
        "send ConfigAddAppKey cmd to peer device"
    },
    {
        ConfigGetAppKey,
        false,
        "ConfigGetAppKey" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
         [netkey idx(2octets)]",
        "get App keys associated with given netkey"
    },
    {
        ConfigAppKeyUpdate,
        false,
        "ConfigAppKeyUpdate" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
         [appkey idx(2octets)] [netkey idx(2octets)][appkey(16octets)]",
        "update App key associated with given netkey"
    },
    {
       ConfigAppKeyDelete,
       false,
       "ConfigAppKeyDelete" ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
        [appkey idx(2octets)][Netkey idx(2octets)]",
       "send ConfigAppKeyDelete cmd to peer device"
    },
    {
       ConfigKeyRefreshPhaseGet,
       false,
       "ConfigKeyRefreshPhaseGet" ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
        [Netkey idx(2octets)] ",
       " send ConfigKeyRefreshPhaseGet for given netkey"
    },
    {
       ConfigKeyRefreshPhaseSet,
       false,
       "ConfigKeyRefreshPhaseSet" ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
        [Netkey idx(2octets)] [phase transistion(1octet)]",
       "key refresh phase set for given netkey"
    },
    {
        ConfigGattProxyStateGet,
        false,
        "ConfigGattProxyStateGet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)]",
        "send ConfigGattProxyStateGet cmd to peer device"
    },
    {
        ConfigGattProxyStateSet ,
        false,
        "ConfigGattProxyStateSet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
         [new Gattproxy state(1octet)]",
        "send ConfigGattProxyStateSet cmd to peer device"
    },
    {
       ConfigNodeIdentityGet,
       false, "ConfigNodeIdentityGet" ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
        [NetkeyIdx (2octets)]",
       "send ConfigNodeIdentityGet cmd to peer device"
   },
   {
        ConfigNodeIdentitySet,
        false,
        "ConfigNodeIdentitySet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
         [netkey idx(2octets)][New Identity state(1octet)]",
        "send ConfigNodeIdentitySet cmd to peer device"
    },
    {
        ConfigPublicationGet,
        false,
        "ConfigPublicationGet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
         [elem addr (2octets)] [Model id (2octets)]",
        "send ConfigPublicationGet cmd to peer device"
    },
   {
        ConfigPublicationSet,
        false,
        "ConfigPublicationSet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] \
        [elm addr(2octets)] [Appkey Idx (2bytes)][cred_flag(0/1 1octet)] \
        [publish_ttl(1octet)] [publish_period(1octet)] [pub_rtx_cnt(1octet)] \
        [pub_rtx_int_stp 1] [model Id (2bytes)] [publish addr(2octets)]",
        "send ConfigPublicationSet cmd to peer device"
    },
    {
        ConfigPublicationVirtualAddrSet ,
        false,
        "ConfigPublicationVirtualAddrSet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] \
        [elm addr(2octets)] [AppkeyIdx(2octets)][cred_flag(0/1)] \
        [publish_ttl(1octet)] [publish_period(1octet)] [pub_rtx_cnt(1octet)] [pub_rtx_int_stp(1octet)] \
        [model_id(2octets)] [virAaddr(16octets)]",
        "send ConfigPublicationVirtualAddrSet cmd to peer device"
    },
    {
        ConfigSubcriptionVirtualAddrSet,
        false,
        "ConfigSubcriptionVirtualAddrSet  - operation values  add-0/delete-1/overwrite-2/delete all-3 " ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
         [elm addr(2octets)] [model id 4octets)][operation(0/1/2/3) (1octet)] [virt addr(16octets)] ",
        "send Config SubcriptionVirtualAddrSet cmd to peer device"
    },
    {
        ConfigSubscriptionDeleteAll ,
        false,
        "ConfigSubscriptionDeleteAll" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] \
         [elm addr(2octets)] [model id (2octets)]",
        "Send ConfigSubscriptionDeleteAll cmd to peer device"
    },
    {
       ConfigSubcriptionGet,
       false,
       "ConfigSubcriptionGet" ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] \
        [elm addr(2octets)] [model id 2octets)] ",
       "send ConfigSubcriptionGet cmd to peer device"
    },
    {
       ConfigSubscriptionSet ,
       false,
       "ConfigSubscriptionSet  - operation values  add-0/delete-1/overwrite-2/delete all-3 " ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] \
        [elm addr(2octets)] [model id 2octets)] [operation(0/1/2/3)(1octet)] [addr (2octets)]",
       " send ConfigSubscriptionSet cmd to peer device"
   },
   {
        ConfigVendorSubscriptionGet,
        false,
        "ConfigVendorSubscriptionGet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
         [elm addr(2octets)] [model id (4octets)]",
        "Get ConfigVendorSubscriptionGet cmd to peer device"
    },
    {
        ConfigBindAppKey,
        false,
        "ConfigBindAppKey" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] \
         [elm addr(2octets)] [model type(0-SIG, 1-Vendor)] [model id (2octets)] [appkey idx(2octets)]",
        "send ConfigBindAppKey cmd to peer device"
    },
    {
        ConfigAppUnbind,
        false,
        "ConfigAppUnbind" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] \
         [elm addr(2octets)] [model id (4octects)] [appkey idx(2octets)]",
        "send ConfigAppUnbind cmd to peer device"
    },
    {
       ConfigModelAppGet,
       false,
       "ConfigModelAppGet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] \
         [elm addr(2octets)] [model id(2octets)]",
       "send ConfigModelAppGet cmd to peer device"
   },
   {
       ConfigVendorModelAppGet,
       false,
       "ConfigVendorModelAppGet" ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] \
        [elm addr(2octets)] [model id(4octets)]",
       "send ConfigVendorModelAppGet cmd to peer device"
   },
   {
        ConfigNodeReset,
        false,
        "configNodeReset" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)]",
        "send ConfigNodeReset cmd to remote device "
    },
    {
        ConfigFriendGet,
        false,
        "configFriendGet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)]",
        " send ConfigFriendGet cmd to peer device"
    },
    {
       ConfigFriendSet,
       false,
       "configFriendSet" ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2 octets)] \
        [frnd state(1octet) 0/1]",
       "Send ConfigFriendSet cmd to peer device"
   },
   {
        ConfigHeartBeatPublicGet,
        false,
        "configHeartBeatPublicGet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)]",
        "send ConfigHeartBeatPublicGet cmd to peer device"
    },
    {
        ConfigHeartBeatPublicSet,
        false,
        "configHeartBeatPublicSet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] \
         [destaddr(2octet)][features(2octets)] [netkey idx(2octets)] [ count log(1octet)] [period log(1octet)] \
         [ttl(1octet)]",
        "send ConfigHeartBeatPublicSet cmd to peer device"
    },
    {
        ConfigHeartBeatSubscribeGet,
        false,
        "configHeartBeatSubscribeGet" ,
        "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] ",
        "send ConfigHeartBeatSubscribeGett cmd  to peer device"
    },
    {
       ConfigHeartBeatSubscribeSet,
       false,
       "configHeartBeatSubscribeSet" ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] \
        [sub_dest addr(2octets)][src addr(2octets)] [period log(1octet)]",
       "send Config Heart BeatSubscribe set cmd to peer device"
    },
    {
       ConfigLpnPollTimeoutGet,
       false,
       "ConfigLpnPollTimeoutGet" ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] \
        [unicast addr of lpn(2octets)]",
       "send ConfigLpnPollTimeoutGet cmd to peer device"
   },
   {
       ConfigNwTransmitGet,
       false,
       "ConfigNwTransmitGet" ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)]",
       "send ConfigNwTransmitGet cmd to peer device"
   },
   {
       ConfigNwTransmitSet,
       false,
       "ConfigNwTransmitSet" ,
       "[net info(2 octets) LS 12 bits - Global Key ID, MS 4 bits - Provisioned Net ID)][dst addr(2octets)] \
        [Tx count(1octet 3bits)] [Tx init step(1octet 5bits)]",
       "send ConfigNwTransmitSet cmd to peer device"
   }
 #endif

};

/* Command group for SIG Model Config Model Clients */
const QCLI_Command_Group_t qmesh_ConfigModel_group =
{
    "QMesh Config Model",
    (sizeof(qmesh_config_model_cmd_list) / sizeof(qmesh_config_model_cmd_list[0])),
    qmesh_config_model_cmd_list
};

static void UpdateMsgInfoData(QMESH_CONFIG_CLIENT_MSG_INFO_T *msg_info, QCLI_Parameter_t *Parameter_List)
{
    /* Update Key Info */
    msg_info->key_info.prov_net_idx = (Parameter_List[0].Integer_Value & PROVISIONED_NET_ID_MASK) >> 12;
    msg_info->key_info.key_idx = (Parameter_List[0].Integer_Value & KEY_ID_MASK);
    /* Update Src and Dst Address */
    msg_info->src_addr = DemoGetConfigClientElemAddr();
    msg_info->dst_addr = Parameter_List[1].Integer_Value;
    /* Update TTL */
    msg_info->ttl = QCLI_DFLT_TTL ;
}

static QCLI_Command_Status_t ConfigGetCompData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 3) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.page_num = Parameter_List[2].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_COMPOSITION_DATA_GET,
                                          &cmd_data);

        if (result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigGetCompData failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}

static QCLI_Command_Status_t ConfigAddAppKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    uint16_t appKey[QMESH_APP_KEY_SIZE_HWORDS] = {0};
    uint8_t val , len;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 5) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid) &&
                                    (Parameter_List[1].Integer_Is_Valid)&&(Parameter_List[2].Integer_Is_Valid)
                                        &&(Parameter_List[3].Integer_Is_Valid) )
    {
        len = strlen((char *) Parameter_List[4].String_Value);
        if (len != 32)
        {
            return QCLI_STATUS_ERROR_E;
        }

        val = AsciiToHex((char *)Parameter_List[4].String_Value, len, (uint8_t*)appKey);
        if (val == 0xFF)
        {
            return QCLI_STATUS_ERROR_E;
        }

        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.app_key_add.app_key_idx = Parameter_List[2].Integer_Value;
        cmd_data.app_key_add.net_key_idx = Parameter_List[3].Integer_Value;
        QmeshMemCopyPack(cmd_data.app_key_add.app_key, (uint8_t*)appKey, QMESH_APP_KEY_SIZE_OCTETS);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_APP_KEY_ADD,
                                          &cmd_data);

        if (result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigAddAppKey failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}

static QCLI_Command_Status_t ConfigNetKeyUpdate(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    uint16_t net_key[QMESH_NETWORK_KEY_SIZE_HWORDS] = {0};
    uint8_t val , len;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 4) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid) &&
                                    (Parameter_List[1].Integer_Is_Valid)&&(Parameter_List[2].Integer_Is_Valid))

    {
        len = strlen((char *) Parameter_List[3].String_Value);
        if(len != 32)
            return QCLI_STATUS_ERROR_E;

        val = AsciiToHex((char *)Parameter_List[3].String_Value, len, (uint8_t*)net_key);
        if(val == 0xFF)
            return QCLI_STATUS_ERROR_E;

        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.net_key_update.net_key_idx = Parameter_List[2].Integer_Value;
        QmeshMemCopyPack(cmd_data.net_key_update.net_key, (uint8_t*)net_key, QMESH_NETWORK_KEY_SIZE_OCTETS);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_NETWORK_KEY_UPDATE,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigNetKeyUpdate failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}

static QCLI_Command_Status_t ConfigNetKeyDelete(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 3) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid) &&
                                    (Parameter_List[1].Integer_Is_Valid)&&(Parameter_List[2].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.net_key_delete = Parameter_List[2].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_NETWORK_KEY_DELETE,
                                                            &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigNetKeyDelete failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}


static QCLI_Command_Status_t ConfigBindAppKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_ERROR_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 6) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid) && (Parameter_List[1].Integer_Is_Valid) &&
                        (Parameter_List[2].Integer_Is_Valid)&&(Parameter_List[3].Integer_Is_Valid) &&(Parameter_List[4].Integer_Is_Valid) &&
                        (Parameter_List[5].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.app_key_bind.ele_info.ele_addr = Parameter_List[2].Integer_Value;
        cmd_data.app_key_bind.ele_info.model_type = Parameter_List[3].Integer_Value;
        cmd_data.app_key_bind.ele_info.model_id = Parameter_List[4].Integer_Value;
        cmd_data.app_key_bind.app_key_idx = Parameter_List[5].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_APP_BIND, &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigBindAppKey failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}

static QCLI_Command_Status_t ConfigGetAppKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 3) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid) &&
                                    (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.app_key_get= Parameter_List[2].Integer_Value;
        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_APP_KEY_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigGetAppKey failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}

static QCLI_Command_Status_t ConfigNetKeyAdd(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    uint16_t net_key[QMESH_NETWORK_KEY_SIZE_HWORDS] = {0};
    uint8_t len, val;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
   if((Parameter_Count == 4) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid))
    {
        len = strlen((char *) Parameter_List[3].String_Value);
        if(len != 32)
            return QCLI_STATUS_ERROR_E;

        val = AsciiToHex((char *)Parameter_List[3].String_Value, len, (uint8_t*)net_key);
        if(val == 0xFF)
            return QCLI_STATUS_ERROR_E;

        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.net_key_add.net_key_idx = Parameter_List[2].Integer_Value;
        QmeshMemCopyPack(cmd_data.net_key_add.net_key, (uint8_t*)net_key, QMESH_NETWORK_KEY_SIZE_OCTETS);
        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_NETWORK_KEY_ADD,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigNetKeyAdd failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}

static QCLI_Command_Status_t ConfigNetKeyGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_NETWORK_KEY_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigNetKeyGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}



static QCLI_Command_Status_t ConfigAppKeyUpdate(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    uint16_t app_key[QMESH_APP_KEY_SIZE_HWORDS] = {0};
    uint8_t len, val;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 5) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
            && (Parameter_List[3].Integer_Is_Valid) )
     {
        len = strlen((char *) Parameter_List[4].String_Value);
        if(len != 32)
            return QCLI_STATUS_ERROR_E;

        val = AsciiToHex((char *)Parameter_List[4].String_Value, len, (uint8_t*)app_key);
        if(val == 0xFF)
            return QCLI_STATUS_ERROR_E;

        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.app_key_update.app_key_idx = Parameter_List[2].Integer_Value;
        cmd_data.app_key_update.net_key_idx = Parameter_List[3].Integer_Value;
        QmeshMemCopyPack(cmd_data.app_key_update.app_key, (uint8_t*)app_key, QMESH_APP_KEY_SIZE_OCTETS);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_APP_KEY_UPDATE,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigAppKeyUpdate failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigAppKeyDelete(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 4) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.app_key_delete.app_key_idx = Parameter_List[2].Integer_Value;;
        cmd_data.app_key_delete.net_key_idx = Parameter_List[3].Integer_Value;;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_APP_KEY_DELETE,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigAppKeyDelete failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}

static QCLI_Command_Status_t ConfigKeyRefreshPhaseGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 3) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.key_refresh_phase_get = Parameter_List[2].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_KEY_REFRESH_PHASE_GET,
                                                            &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigKeyRefreshPhaseGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}



static QCLI_Command_Status_t ConfigKeyRefreshPhaseSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 4) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                     && (Parameter_List[3].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.key_refresh_phase_set.net_key_idx = Parameter_List[2].Integer_Value;
        cmd_data.key_refresh_phase_set.phase = Parameter_List[3].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_KEY_REFRESH_PHASE_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigKeyRefreshPhaseSet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigBeaconGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_BEACON_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigBeaconGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigBeaconSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 3) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.beacon_state = Parameter_List[2].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_BEACON_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigBeaconSet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigDefaultTtlSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 3) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.default_ttl = Parameter_List[2].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_DEFAULT_TTL_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigDefaultTtlSet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}

static QCLI_Command_Status_t ConfigDefaultTtlget(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_DEFAULT_TTL_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigDefaultTtlget failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigRelayGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_RELAY_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigRelayGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
       res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigRelaySet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 5) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid) && (Parameter_List[4].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.relay_set.relay_state = Parameter_List[2].Integer_Value;
        cmd_data.relay_set.relay_rtx_cnt = Parameter_List[3].Integer_Value;
        cmd_data.relay_set.relay_rtx_int_stp = Parameter_List[4].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_RELAY_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigRelaySet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigGattProxyStateGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_GATT_PROXY_STATE_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigGattProxyStateGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigGattProxyStateSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 3) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.gatt_proxy_state = Parameter_List[2].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_GATT_PROXY_STATE_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigGattProxyStateSet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}


static QCLI_Command_Status_t ConfigNodeIdentityGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 3) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid) )
    {

        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.node_identity_get = Parameter_List[2].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_NODE_IDENTITY_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigNodeIdentityGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigNodeIdentitySet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 4) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.node_identity_set.net_key_idx = Parameter_List[2].Integer_Value;
        cmd_data.node_identity_set.identity_state = Parameter_List[3].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_NODE_IDENTITY_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigNodeIdentitySet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigPublicationGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 4) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.pub_get.ele_addr = Parameter_List[2].Integer_Value;;
        cmd_data.pub_get.model_id = Parameter_List[3].Integer_Value;;
        cmd_data.pub_get.model_type = QMESH_MODEL_TYPE_SIG;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_PUBLICATION_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigPublicationGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigPublicationSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 11) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
        && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)&&(Parameter_List[3].Integer_Is_Valid)
        && (Parameter_List[4].Integer_Is_Valid) && (Parameter_List[5].Integer_Is_Valid)&&(Parameter_List[6].Integer_Is_Valid)
        && (Parameter_List[7].Integer_Is_Valid) && (Parameter_List[8].Integer_Is_Valid)&&(Parameter_List[9].Integer_Is_Valid)
        && (Parameter_List[10].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.pub_set.ele_addr = Parameter_List[2].Integer_Value;
        cmd_data.pub_set.app_key_idx = Parameter_List[3].Integer_Value;
        cmd_data.pub_set.credential_flag = Parameter_List[4].Integer_Value;
        cmd_data.pub_set.publish_ttl = Parameter_List[5].Integer_Value;
        cmd_data.pub_set.publish_period = Parameter_List[6].Integer_Value;
        cmd_data.pub_set.pub_rtx_cnt =Parameter_List[7].Integer_Value;
        cmd_data.pub_set.pub_rtx_int_stp = Parameter_List[8].Integer_Value;
        cmd_data.pub_set.model_type = QMESH_MODEL_TYPE_SIG;
        cmd_data.pub_set.model_id = Parameter_List[9].Integer_Value;
        cmd_data.pub_set.pub_addr.addr = Parameter_List[10].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_PUBLICATION_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigPublicationSet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigPublicationVirtualAddrSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    uint8_t vir_addr[16] = {0};
    uint8_t len, val;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 11) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
        && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)&&(Parameter_List[3].Integer_Is_Valid)
        && (Parameter_List[4].Integer_Is_Valid) && (Parameter_List[5].Integer_Is_Valid)&&(Parameter_List[6].Integer_Is_Valid)
        && (Parameter_List[7].Integer_Is_Valid) && (Parameter_List[8].Integer_Is_Valid)&&(Parameter_List[9].Integer_Is_Valid))

    {
        len = strlen((char *) Parameter_List[10].String_Value);

        if(len != 32)
            return QCLI_STATUS_ERROR_E;

        val = AsciiToHex((char *)Parameter_List[10].String_Value, len, vir_addr);

        if(val == 0xFF)
            return QCLI_STATUS_ERROR_E;

        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.pub_vir_set.ele_addr = Parameter_List[2].Integer_Value;
        cmd_data.pub_vir_set.app_key_idx = Parameter_List[3].Integer_Value;
        cmd_data.pub_vir_set.credential_flag = Parameter_List[4].Integer_Value;
        cmd_data.pub_vir_set.publish_ttl = Parameter_List[5].Integer_Value;
        cmd_data.pub_vir_set.publish_period = Parameter_List[6].Integer_Value;
        cmd_data.pub_vir_set.pub_rtx_cnt = Parameter_List[7].Integer_Value;
        cmd_data.pub_vir_set.pub_rtx_int_stp = Parameter_List[8].Integer_Value;
        cmd_data.pub_vir_set.model_type = QMESH_MODEL_TYPE_SIG;
        cmd_data.pub_vir_set.model_id = Parameter_List[9].Integer_Value;
        QmeshMemCopyPack(cmd_data.pub_vir_set.pub_addr.label_uuid, vir_addr, QMESH_UUID_SIZE_OCTETS);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_PUBLICATION_VIRTUAL_ADDR_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigPublicationVirtualAddrSet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigSubcriptionVirtualAddrSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    uint8_t vir_addr[16] = {0};
    uint8_t len, val;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 6) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
        && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)&&(Parameter_List[3].Integer_Is_Valid)
        && (Parameter_List[4].Integer_Is_Valid))
    {
        len = strlen((char *) Parameter_List[5].String_Value);


        if(len != 32)
            return QCLI_STATUS_ERROR_E;

        val = AsciiToHex((char *)Parameter_List[5].String_Value, len, vir_addr);

        if(val == 0xFF)
            return QCLI_STATUS_ERROR_E;

        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.vir_sub_set.ele_info.ele_addr = Parameter_List[2].Integer_Value;
        cmd_data.vir_sub_set.ele_info.model_id = Parameter_List[3].Integer_Value;
        cmd_data.vir_sub_set.ele_info.model_type = QMESH_MODEL_TYPE_SIG;
        cmd_data.vir_sub_set.operation = Parameter_List[4].Integer_Value;
        QmeshMemCopyPack(cmd_data.vir_sub_set.sub_addr.label_uuid, vir_addr, QMESH_UUID_SIZE_OCTETS);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_SUBSCRIPTION_VIRTUAL_ADDR_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigSubcriptionVirtualAddrSet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigSubscriptionDeleteAll(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 4) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.sub_del_all.ele_addr = Parameter_List[2].Integer_Value;
        cmd_data.sub_del_all.model_id = Parameter_List[3].Integer_Value;
        cmd_data.sub_del_all.model_type = QMESH_MODEL_TYPE_SIG;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_SUBSCRIPTION_DEL_ALL,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigSubscriptionDeleteAll failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigSubcriptionGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 4) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.sub_get.ele_addr = Parameter_List[2].Integer_Value;
        cmd_data.sub_get.model_id = Parameter_List[3].Integer_Value;
        cmd_data.sub_get.model_type = QMESH_MODEL_TYPE_SIG;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_SUBSCRIPTION_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigSubcriptionGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
       res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigSubscriptionSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 6) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid) && (Parameter_List[4].Integer_Is_Valid)
                    && (Parameter_List[4].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.sub_set.ele_info.ele_addr = Parameter_List[2].Integer_Value;
        cmd_data.sub_set.ele_info.model_id = Parameter_List[3].Integer_Value;
        cmd_data.sub_set.ele_info.model_type = QMESH_MODEL_TYPE_SIG;
        cmd_data.sub_set.operation = Parameter_List[4].Integer_Value;
        cmd_data.sub_set.sub_addr.addr = Parameter_List[5].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_SUBSCRIPTION_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigSubscriptionSet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigVendorSubscriptionGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 4) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.sub_get.ele_addr = Parameter_List[2].Integer_Value;
        cmd_data.sub_get.model_id = Parameter_List[3].Integer_Value;
        cmd_data.sub_get.model_type = QMESH_MODEL_TYPE_VENDOR;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_VENDOR_SUBSCRIPTION_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigVendorSubscriptionGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}


static QCLI_Command_Status_t ConfigAppUnbind(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 5) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid) && (Parameter_List[4].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.app_key_bind.ele_info.ele_addr = Parameter_List[2].Integer_Value;
        cmd_data.app_key_bind.ele_info.model_id = Parameter_List[3].Integer_Value;
        cmd_data.app_key_bind.ele_info.model_type = QMESH_MODEL_TYPE_SIG;
        cmd_data.app_key_bind.app_key_idx = Parameter_List[4].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_APP_UNBIND,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigAppUnbind failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigModelAppGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 4) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.app_get.ele_addr = Parameter_List[2].Integer_Value;
        cmd_data.app_get.model_id = Parameter_List[3].Integer_Value;
        cmd_data.app_get.model_type = QMESH_MODEL_TYPE_SIG;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_SIG_MODEL_APP_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigModelAppGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}

static QCLI_Command_Status_t ConfigVendorModelAppGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 4) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.app_get.ele_addr = Parameter_List[2].Integer_Value;
        cmd_data.app_get.model_id = Parameter_List[3].Integer_Value;
        cmd_data.app_get.model_type = QMESH_MODEL_TYPE_VENDOR;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_VENDOR_MODEL_APP_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigVendorModelAppGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}


static QCLI_Command_Status_t ConfigNodeReset(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_NODE_RESET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigNodeReset failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigFriendGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_FRIEND_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigFriendGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }
    return(res);

}
static QCLI_Command_Status_t ConfigFriendSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 3) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.friend_state = Parameter_List[2].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_FRIEND_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigFriendSet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigHeartBeatPublicGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_HEART_BEAT_PUBLIC_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigHeartBeatPublicGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigHeartBeatPublicSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 8) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid) && (Parameter_List[4].Integer_Is_Valid)
                    && (Parameter_List[5].Integer_Is_Valid) && (Parameter_List[6].Integer_Is_Valid)
                    && (Parameter_List[7].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.heart_beat_pub_set.dest_addr = Parameter_List[2].Integer_Value;
        cmd_data.heart_beat_pub_set.features = Parameter_List[3].Integer_Value;
        cmd_data.heart_beat_pub_set.net_key_idx = Parameter_List[4].Integer_Value;
        cmd_data.heart_beat_pub_set.count_log = Parameter_List[5].Integer_Value;
        cmd_data.heart_beat_pub_set.period_log = Parameter_List[6].Integer_Value;
        cmd_data.heart_beat_pub_set.ttl = Parameter_List[7].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_HEART_BEAT_PUBLIC_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigHeartBeatPublicSet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigHeartBeatSubscribeGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_HEART_BEAT_SUBCRIBE_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigHeartBeatSubscribeGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigHeartBeatSubscribeSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 5) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid) && (Parameter_List[4].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.heart_beat_sub_set.dest_addr = Parameter_List[2].Integer_Value;
        cmd_data.heart_beat_sub_set.src_addr = Parameter_List[3].Integer_Value;
        cmd_data.heart_beat_sub_set.period_log = Parameter_List[4].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_HEART_BEAT_SUBCRIBE_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigHeartBeatSubscribeSet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigLpnPollTimeoutGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 3) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) &&(Parameter_List[2].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.lpn_poll_timeout_get = Parameter_List[2].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_LPN_POLL_TIMEOUT_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigLpnPollTimeoutGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigNwTransmitGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) )
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_NETWORK_TRANSMIT_GET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigNwTransmitGet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}
static QCLI_Command_Status_t ConfigNwTransmitSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t res = QCLI_STATUS_SUCCESS_E;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;


    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 4) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid)
                    && (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid)
                    && (Parameter_List[3].Integer_Is_Valid) && (Parameter_List[4].Integer_Is_Valid))
    {
        UpdateMsgInfoData(&msg_info, Parameter_List);
        /* Update Command Data */
        cmd_data.network_tx_set.nw_tx_count = Parameter_List[2].Integer_Value;
        cmd_data.network_tx_set.nw_tx_int_step =Parameter_List[3].Integer_Value;

        result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_NETWORK_TRANSMIT_SET,
                                          &cmd_data);

        if(result != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nConfigNwTransmitSet failed res=%d\n", result);
            res = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        res = QCLI_STATUS_USAGE_E;
    }

    return(res);

}

