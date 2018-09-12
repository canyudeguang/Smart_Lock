/******************************************************************************
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
#include "qmesh_data_types.h"
#include "qmesh_demo_core.h"
#include "qmesh_gw_debug.h"
#include "qmesh_demo_composition.h"
#include "qmesh_demo_config.h"
#include "qmesh_demo_utilities.h"
#include "qmesh_demo_nvm_utilities.h"
#include "qmesh_ps.h"
#include "qmesh_soft_timers.h"
#include <stdlib.h>
#include "qmesh_health_server.h"
#include "qapi_reset.h"
#include "qmesh_model_nvm.h"
#include "qapi_heap_status.h"
#include "qmesh_light_utilities.h"

#define HEALTH_NO_FAULT_VALUE                       (0)
#define HEALTH_ATTENTION_TIMER_OFF                  (0)
#define QMESH_HEALTH_PERIOD_DIVISOR_INVALID         (0x10u)
#define QMESH_HEALTH_PUB_PERIOD_DISABLED            (0x00000000u)
#define QMESH_HEALTH_PUB_TIMER_INVALID_HANDLE       (0)

#define ATT_HDR_OCTETS                              (3)

#define QMESH_MSG_SIZE_MAX                          (380)

static QMESHAPP_GATT_PROXY_CONN_ADVRT_INFO_t gatt_proxy_conn_advrt_info;

static QMESHAPP_GATT_PROXY_CLIENT_INFO_T gatt_proxy_client_context;

const QMESH_DEVICE_COMPOSITION_DATA_T* dev_comp;
#define QMESH_APP_NUM_SOFT_TIMERS (4)
QMESH_TIMER_GROUP_HANDLE_T app_timer_ghdl = QMESH_TIMER_INVALID_GROUP_HANDLE;
uint32_t attention_time = 0x00000000;
static uint8_t prov_net_idx;

/******************************************************************************
 *Provisioning Data
 *****************************************************************************/

static const char *prov_status[QMESH_PROV_STATUS_MAX + 1] =
{
    "\r\nPROV_STATUS: START",
    "\r\nPROV_STATUS: CAPABILITIES",
    "\r\nPROV_STATUS: PUB_KEY",
    "\r\nPROV_STATUS: IP_COMP",
    "\r\nPROV_STATUS: CONFIRMATION",
    "\r\nPROV_STATUS: RANDOM",
    "\r\nPROV_STATUS: DATA",
    "\r\nPROV_STATUS: UNKNOWN"
};

/* Application Context */
static QMESHAPP_CONTEXT_T app_context;

/* Remote Device Element Count */
static uint8_t remote_num_elms = 0;

static uint16_t      dev_pub_key[32];
static bool provisioner_role_enabled = FALSE;

int8_t TTL = QCLI_DFLT_TTL;

app_association_state   prov_state;             /* Local Device Provisioning state */
QMESH_TIMER_HANDLE_T    unprov_dev_beacon_tid;
QMESH_TIMER_HANDLE_T    key_refresh_tid;

static QMESH_TIMER_HANDLE_T    device_disc_tid;

uint32_t                 BluetoothStackID;      /* bluetooth stack ID. Used for
                                               Bearer and GATT operations */

QCLI_Group_Handle_t     mesh_group;            /* Handle for our main QCLI Command*/
QCLI_Group_Handle_t     mesh_model_group;      /* Handle for our main QCLI Command*/


/*Device  provisioning capabiltiy information values as per MESH spec version 1.0 Sec 5.4*/
/*
        // PTS_MESH_NODE_PROV_UPD_BV_01_C
        pubkey_oob_mask = QMESH_PUBLIC_KEY_MASK_NONE;
        static_oob_mask = QMESH_STATIC_MASK_NONE;
        output_oob_size = 0x01;
        output_action_mask = QMESH_OUTPUT_OOB_ACTION_ALPHANUMERIC;
        input_oob_size = 0x00;
        input_action_mask = QMESH_INPUT_OOB_ACTION_MASK_NONE;

        //PTS_MESH_NODE_PROV_UPD_BV_02_C
        pubkey_oob_mask = QMESH_PUBLIC_KEY_MASK_NONE;
        static_oob_mask = QMESH_STATIC_MASK_NONE;
        output_oob_size = 0;
        output_action_mask = QMESH_OUTPUT_OOB_ACTION_BLINK;
        input_oob_size = 0x01;
        input_action_mask = QMESH_INPUT_OOB_ACTION_MASK_ALPHANUMERIC;

        //PTS_MESH_NODE_PROV_UPD_BV_03_C
        pubkey_oob_mask = QMESH_PUBLIC_KEY_MASK_NONE;
        static_oob_mask = QMESH_STATIC_MASK_OOB;
        output_oob_size = 0;
        output_action_mask = QMESH_OUTPUT_OOB_ACTION_BLINK;
        input_oob_size = 0x00;
        input_action_mask = QMESH_INPUT_OOB_ACTION_MASK_NONE;

        // PTS_MESH_NODE_PROV_UPD_BV_04_C
        pubkey_oob_mask = QMESH_PUBLIC_KEY_MASK_OOB;
        static_oob_mask = QMESH_STATIC_MASK_NONE;
        output_oob_size = 1;
        output_action_mask = QMESH_OUTPUT_OOB_ACTION_ALPHANUMERIC;
        input_oob_size = 0x00;
        input_action_mask = QMESH_INPUT_OOB_ACTION_MASK_NONE;

        // PTS_MESH_NODE_PROV_UPD_BV_05_C
        pubkey_oob_mask = QMESH_PUBLIC_KEY_MASK_OOB;
        static_oob_mask = QMESH_STATIC_MASK_NONE;
        output_oob_size = 0x00;
        output_action_mask = QMESH_OUTPUT_OOB_ACTION_BLINK;
        input_oob_size = 0x01;
        input_action_mask = QMESH_INPUT_OOB_ACTION_MASK_ALPHANUMERIC;

        // PTS_MESH_NODE_PROV_UPD_BV_06_C
        pubkey_oob_mask = QMESH_PUBLIC_KEY_MASK_OOB;
        static_oob_mask = QMESH_STATIC_MASK_OOB;
        output_oob_size = 0;
        output_action_mask = QMESH_OUTPUT_OOB_ACTION_BLINK;
        input_oob_size = 0x00;
        input_action_mask = QMESH_INPUT_OOB_ACTION_MASK_NONE;

        // PTS_MESH_NODE_PROV_UPD_BV_07_C
        pubkey_oob_mask = QMESH_PUBLIC_KEY_MASK_NONE;
        static_oob_mask = QMESH_STATIC_MASK_NONE;
        output_oob_size = 0;
        output_action_mask = QMESH_OUTPUT_OOB_ACTION_BLINK;
        input_oob_size = 0x00;
        input_action_mask = QMESH_INPUT_OOB_ACTION_MASK_NONE;

        // PTS_MESH_NODE_PROV_UPD_BV_08_C
        pubkey_oob_mask = QMESH_PUBLIC_KEY_MASK_OOB;
        static_oob_mask = QMESH_STATIC_MASK_NONE;
        output_oob_size = 0;
        output_action_mask = QMESH_OUTPUT_OOB_ACTION_BLINK;
        input_oob_size = 0x00;
        input_action_mask = QMESH_INPUT_OOB_ACTION_MASK_NONE;

        // PTS_MESH_NODE_PROV_UPD_BV_10_C
        pubkey_oob_mask = QMESH_PUBLIC_KEY_MASK_NONE;
        static_oob_mask = QMESH_STATIC_MASK_NONE;
        output_oob_size = 0;
        output_action_mask = QMESH_OUTPUT_OOB_ACTION_BLINK;
        input_oob_size = 0x01;
        input_action_mask = QMESH_INPUT_OOB_ACTION_MASK_ALPHANUMERIC;
*/

/*
 * Device Data
 */
static QMESH_DEVICE_IDENTIFIER_DATA_T device_data =
{
    {0x691B, 0xDC08, 0x1021, 0x0B0E,
     0x0A0C, 0x000B, 0x0E0A, 0x0C69},   /* uuid */
    0,                                  /* uri_length */
    NULL,                               /* uri */
    0,                                  /* oob_info_mask  */
    {                                   /* dev_caps */
        0,                                  /* pubkey_oob_mask */
        0,                                  /* static_oob_mask */
        0,                                  /* output_oob_size */
        0,                                  /* input_oob_size */
        0,                                  /* output_action_mask */
        0,                                  /* input_action_mask */
        0                                   /* prov_algos_mask */
    },
    dev_pub_key                         /* oob_pub_key */
};

extern QMESH_GENERIC_LEVEL_CONTEXT_T          g_gen_lvl_context[3];
extern QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T    g_onoff_model_context;
extern QMESH_GEN_POWERONOFF_DATA_T            g_pwronoff_context;
extern QMESH_LIGHTNESS_MODEL_CONTEXT_T        g_lightness_server_context;
extern QMESH_LIGHT_HSL_MODEL_CONTEXT_T        g_hsl_server_context;
extern QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T    g_hsl_hue_server_context;
extern QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T    g_hsl_sat_server_context;

/* GATT Service */
static uint8_t gatt_proxy_dev_index;
static QMESHAPP_DEVICE_PROVISIONING_INFO_t gatt_dev_prov_info;
static QMESHAPP_PROXY_DEVICE_FOUND_LIST_T gatt_proxy_dev_found_list[PROXY_DEVICE_FOUND_LIST];
static QMESHAPP_UNPROV_DEVICE_FOUND_LIST_t gatt_unprov_dev_found_list[UNPROVISIONED_DEVICE_FOUND_LIST];

static uint8_t gatt_unprov_dev_index;

/******************************************************************************
 *Private Function Prototypes
 *****************************************************************************/
static void CreateBeaconHandlerTimer(void);
static void QmeshProcessMeshEvent (QMESH_APP_EVENT_DATA_T event_data_cb);
static QmeshResult StartGattProxyService(void);
static QmeshResult StopGattProxyService(void);
static QmeshResult CreateGattProxyAdvertTimer(void);
void GapLeEventCallback (uint32_t BluetoothStackID,
                         qapi_BLE_GAP_LE_Event_Data_t *gap_le_event_data,
                         uint32_t callback_param);
void GattConnEventCallback (uint32_t BluetoothStackID,
                            qapi_BLE_GATT_Connection_Event_Data_t *gatt_conn_event_data,
                            uint32_t callback_param);
static bool AddGATTBearer(QMESH_GATT_BEARER_SERVICE_TYPE_T srvcType,
                            QMESHAPP_DEVICE_INFO_T *devInfo);
static void DeleteGATTBearer(qapi_BLE_BD_ADDR_t *addr);

static bool _checkIfDevPresent(qapi_BLE_BD_ADDR_t *addr);
static void _addToUnProvDevList(qapi_BLE_GAP_LE_Advertising_Report_Data_t *adv_report, uint8_t indx);
static bool _checkIfProxyDevPresent(QMESHAPP_PROXY_DEVICE_FOUND_LIST_T *dev_info);
static void _addToProxyDevList(QMESHAPP_PROXY_DEVICE_FOUND_LIST_T *dev_info);
static void _parseConnAdvtFrmUnprovDev(qapi_BLE_GAP_LE_Advertising_Report_Data_t *adv_report);
static void _parseConnAdvtForProxySrvc(qapi_BLE_GAP_LE_Advertising_Report_Data_t *adv_report);
static void EnableProxyServiceWithStack(void *ptr);
static void InitiateProvisioning(void *ptr);
static bool _startDeviceDiscoveryForGATT(uint8_t scanForProv);
static void deviceDiscoveryTimeoutHandler(QMESH_TIMER_HANDLE_T tid, void* data);

static void _updateConnectionData(uint32_t conn_id, uint16_t mtu, qapi_BLE_BD_ADDR_t *addr, bool is_conn);
static QMESHAPP_DEVICE_INFO_T* _getDeviceInfo(uint32_t conn_id);
static QMESHAPP_DEVICE_INFO_T* _getDeviceInfoForDevAddr(qapi_BLE_BD_ADDR_t *dev_addr);
static uint8_t _getAvailableDeviceInfoIndex(void);

#define APP_IS_PROXY_SUPPORTED() ((dev_comp->feature_mask & PROXY_FEATURE_MASK) == PROXY_FEATURE_MASK)

/******************************************************************************
 *Application Model Callback Handler
 *****************************************************************************/
static void QmeshHandleModelEvents(uint16_t elem_addr,uint16_t app_opcode,void *data)
{
    /* Handle Model Messages */
    switch (app_opcode)
    {
        case  QAPP_GEN_ONOFF_UPDATED:
        {
           /*Todo:  Turn the device on/off */
        }
        break;
        case QAPP_LVL_UPDATED:
        {
            /* Get the level data */
            int16_t lvl = (* (int16_t *)data);

            if(lvl + 32768 > 0)
            {               
                 /*To Do Turn on/off the light */
            }
            else
            {
                 /*To Do Turn on/off the light */
            }
        }
        break;
        case QAPP_LIGHTNESS_ACTUAL_UPDATED:
        {
           /*  Retrieve the lightness actual */
           uint16_t light_actual = (* (uint16_t *)data);
           if(light_actual != 0)
           {    
                /*ToDo Turn on/off the light */
           }
           else
           {
                /*ToDo Turn on/off the light */
           }
        }
        break;
        case QAPP_LIGHT_HSL_UPDATED:
        { 
            /* call QmeshConvertHSLtoRGB to convert HSL value to RGB Value */
            /* QMESH_RGB_COLOR rgb = QmeshConvertHSLtoRGB(Hue,Sat,Lightness);*/
            /* Call Light HW routine to update  brightness */
        }
        break;
        case QAPP_LIGHT_HUE_UPDATED:
        {
            /* call QmeshConvertHSLtoRGB to convert HSL value to RGB Value */
            /* QMESH_RGB_COLOR rgb = QmeshConvertHSLtoRGB(Hue,Sat,Lightness);*/
            /* Call Light HW routine to update  color */
        }
        break;
        case QAPP_LIGHT_SAT_UPDATED:
        { 
            /* call QmeshConvertHSLtoRGB to convert HSL value to RGB Value */
            /* QMESH_RGB_COLOR rgb = QmeshConvertHSLtoRGB(Hue,Sat,Lightness);*/
            /* Call Light HW routine to update  saturation */
        }
        break;
        case QAPP_ONPOWERUP_UPDATED:
        {
            /* Generic PowerOn/Off state updated. Preserve the OnPowerup State value */
            onbootpowerup  = (* (uint8_t *)data);
        }
        break;
        default:
        break;
    }
}

/******************************************************************************
 *Provisioning Event Handlers
 *****************************************************************************/
/* Handle QMESH_UNPROVISIONED_DEVICE_EVENT */
static void HandleUnprovisionedDeviceEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    uint16_t *rcvd_uuid_packed = (uint16_t *) event_data.param;
    uint8_t i;

    if(!provisioner_role_enabled)
        return;

    for(i = 0; i < QMESH_MAX_NUM_OF_PROVISIONABLE_DEVICES; i++)
    {
        if(memcmp(app_context.unprovisioned_devices[i], rcvd_uuid_packed, sizeof(QMESH_UUID_T)) == 0)
        {
            return;
        }
    }

    QCLI_LOGE (mesh_group, "\r\nReceived MESH_UNPROVISIONED_DEVICE_EVENT\r\n");
    QCLI_LOGE (mesh_group, "Saved at index %u:", app_context.dev_idx);

    memcpy(app_context.unprovisioned_devices[app_context.dev_idx], rcvd_uuid_packed, sizeof(QMESH_UUID_T));
    app_context.dev_idx = (app_context.dev_idx + 1) % QMESH_MAX_NUM_OF_PROVISIONABLE_DEVICES;

    for(i = 0; i < QMESH_UUID_SIZE_HWORDS; i++)
    {
        QCLI_LOGE (mesh_group, " %04x", rcvd_uuid_packed[i]);
    }
    QCLI_LOGE (mesh_group, "\r\n");
}

static void HandleDevicePubKeyEvent(void)
{
    QmeshResult status;
    const uint16_t oob_pub_key[] = {0xF465u, 0xE43Fu, 0xF23Du, 0x3F1Bu, 0x9DC7u, 0xDFC0u, 0x4DA8u, 0x7581u,
                                  0x84DBu, 0xC966u, 0x2047u, 0x96ECu, 0xCF0Du, 0x6CF5u, 0xE165u, 0x00CCu,
                                  0x0201u, 0xD048u, 0xBCBBu, 0xD899u, 0xEEEFu, 0xC424u, 0x164Eu, 0x33C2u,
                                  0x01C2u, 0xB010u, 0xCA6Bu, 0x4D43u, 0xA8A1u, 0x55CAu, 0xD8ECu, 0xB279u};

    if(!provisioner_role_enabled)
    return;

    status = QmeshSetDevicePublicKey(oob_pub_key);
    if(status != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGI(mesh_group, "\r\n QmeshSetDevicePublicKey failed: %u", status);
    }
}

static void ResetSubnetData(void)
{
    uint32_t idx;
    for (idx = 0; idx < MAX_NUM_OF_SUB_NETWORK; idx++)
    {
        if (app_context.subnet_data[idx].valid)
        {
#if (QMESH_PS_IFCE_ENABLED == 1)
            NVMWrite_SubNetIdentityUpdate(&app_context.subnet_data[idx], true);
#endif
        }
    }

    memset(app_context.subnet_data, 0x00, sizeof(app_context.subnet_data));
}

static void AddSubnetKey(QMESH_SUBNET_KEY_IDX_INFO_T key_info)
{
    uint32_t idx;
    uint32_t free_slot = MAX_NUM_OF_SUB_NETWORK;

    for (idx = 0; idx < MAX_NUM_OF_SUB_NETWORK; idx++)
    {
        if (!app_context.subnet_data[idx].valid)
        {
            if (MAX_NUM_OF_SUB_NETWORK == free_slot)
            {
                free_slot = idx;
            }
        }
        else if ((app_context.subnet_data[idx].netkey_idx.prov_net_idx == key_info.prov_net_idx) &&
                 (app_context.subnet_data[idx].netkey_idx.key_idx == key_info.key_idx))
        {
            free_slot = MAX_NUM_OF_SUB_NETWORK;
            break;
        }
    }

    if (MAX_NUM_OF_SUB_NETWORK != free_slot)
    {
        app_context.subnet_data[free_slot].valid = TRUE;
        app_context.subnet_data[free_slot].node_identity = (APP_IS_PROXY_SUPPORTED())?
                                                           (QMESH_NODE_IDENTITY_STOPPED):
                                                           (QMESH_NODE_IDENTITY_NOT_SUPPORTED);
        app_context.subnet_data[free_slot].netkey_idx = key_info;

#if (QMESH_PS_IFCE_ENABLED == 1)
        NVMWrite_SubNetIdentityUpdate(&app_context.subnet_data[idx], false);
#endif

        QCLI_LOGI(mesh_group, "Add Subnet : 0x%01x : 0x%03x\n", key_info.prov_net_idx, key_info.key_idx);
    }
}

static void RemoveSubnetKey(QMESH_SUBNET_KEY_IDX_INFO_T key_info)
{
    uint32_t idx;

    QCLI_LOGI(mesh_group, "Remove Subnet : %d : %d\n", key_info.prov_net_idx, key_info.key_idx);

    for (idx = 0; idx < MAX_NUM_OF_SUB_NETWORK; idx++)
    {
        if ((app_context.subnet_data[idx].valid) &&
            (app_context.subnet_data[idx].netkey_idx.prov_net_idx == key_info.prov_net_idx) &&
            (app_context.subnet_data[idx].netkey_idx.key_idx == key_info.key_idx))
        {
#if (QMESH_PS_IFCE_ENABLED == 1)
            NVMWrite_SubNetIdentityUpdate(&app_context.subnet_data[idx], true);
#endif
            memset(&app_context.subnet_data[idx], 0x00, sizeof(APP_SUBNET_DATA_T));
            break;
        }
    }
}

static void GetNodeIdentityState(QMESH_CONFIG_NODE_IDENTITY_INFO_T *node_id_info)
{
    uint32_t idx;

    if (!node_id_info)
    {
        return;
    }

    node_id_info->status = QMESH_RESULT_INVALID_NET_KEY;
    for (idx = 0; idx < MAX_NUM_OF_SUB_NETWORK; idx++)
    {
        if ((app_context.subnet_data[idx].valid) &&
            (app_context.subnet_data[idx].netkey_idx.prov_net_idx == node_id_info->net_key_index.prov_net_idx) &&
            (app_context.subnet_data[idx].netkey_idx.key_idx == node_id_info->net_key_index.key_idx))
        {
            node_id_info->status   = QMESH_RESULT_SUCCESS;
            node_id_info->identity = app_context.subnet_data[idx].node_identity;
            break;
        }
    }
}

static void SetNodeIdentityState(QMESH_CONFIG_NODE_IDENTITY_INFO_T *node_id_info)
{
    uint32_t idx;

    if (!node_id_info)
    {
        return;
    }

    QCLI_LOGI(mesh_group, "Search For NI : %d, Prov Net Index : %d, Node ID : %d\n", 
        node_id_info->net_key_index.key_idx, node_id_info->net_key_index.prov_net_idx, node_id_info->identity);

    node_id_info->status = QMESH_RESULT_INVALID_NET_KEY;
    for (idx = 0; idx < MAX_NUM_OF_SUB_NETWORK; idx++)
    {
        QCLI_LOGI(mesh_group, "V: %d PI: %d NI: %d\n", app_context.subnet_data[idx].valid,
                              app_context.subnet_data[idx].netkey_idx.prov_net_idx,
                              app_context.subnet_data[idx].netkey_idx.key_idx);

        if ((app_context.subnet_data[idx].valid) &&
            (app_context.subnet_data[idx].netkey_idx.prov_net_idx == node_id_info->net_key_index.prov_net_idx) &&
            (app_context.subnet_data[idx].netkey_idx.key_idx == node_id_info->net_key_index.key_idx))
        {
            if (QMESH_NODE_IDENTITY_NOT_SUPPORTED != app_context.subnet_data[idx].node_identity)
            {
                if (QMESH_NODE_IDENTITY_NOT_SUPPORTED == node_id_info->identity)
                {
                    node_id_info->status = QMESH_RESULT_CONFIG_STATUS_CANNOT_SET;
                }
                else
                {
                    if (gatt_proxy_conn_advrt_info.is_proxy_advrt_enabled)
                    {
                        node_id_info->status = QMESH_RESULT_SUCCESS;
                        app_context.subnet_data[idx].node_identity = node_id_info->identity;
                    }
                    else
                    {
                        node_id_info->status = QMESH_RESULT_CONFIG_STATUS_TEMPLY_UNABLE_TO_CHANGE_STATE;
                        app_context.subnet_data[idx].node_identity = QMESH_NODE_IDENTITY_STOPPED;
                    }
                 }
            }
            else
            {
                node_id_info->status = QMESH_RESULT_SUCCESS;
            }

            node_id_info->identity = app_context.subnet_data[idx].node_identity;
            break;
        }
    }
}

/* Handle QMESH_GET_DEVICE_KEY_EVENT */
static void HandleGetDeviceKeyEvent(QMESH_APP_EVENT_DATA_T eventDataCallback)
{
    QMESH_DEVICE_KEY_INFO_T         *device_key_info;
    device_key_info = (QMESH_DEVICE_KEY_INFO_T *)eventDataCallback.param;

    QMESH_PEER_DEVICE_KEY_LIST *peer_device_list;
    peer_device_list = app_context.peer_device_key_list;

    while(peer_device_list)
    {
        /* Device key can be used by models other than the Config Model. For intance
         * Time Server Model uses device key & can be present on any element. So the
         * requested element addresss should be checked against all the element address
         * for the specified provisioned network.
         */
        if((peer_device_list->prov_net_id == device_key_info->prov_net_id) &&
           ((device_key_info->elm_addr >= peer_device_list->elem_addr) &&
            (device_key_info->elm_addr < peer_device_list->elem_addr + peer_device_list->num_elems)))
        {
            device_key_info->device_key = peer_device_list->device_key;
            break;
        }
        peer_device_list = peer_device_list->next;
    }
}

/* This function handles the Beacon advertisement timeout event.
 * Send the beacon again on timeout.
 */
void UnProvDeviceBeaconAdvertTimeoutHandler(QMESH_TIMER_HANDLE_T tid, void* data)
{
    QmeshResult result;
    if(tid == unprov_dev_beacon_tid)
    {
        /* Delete the current timer */
        QmeshTimerDelete (&app_timer_ghdl, &tid);
        unprov_dev_beacon_tid = QMESH_TIMER_INVALID_HANDLE;
        if(prov_state == app_state_not_provisioned)
        {
            /* TODO Using the data set in "SendUnProvDeviceBeacon" command */
            result = QmeshSendUnProvDeviceBeacon(0u, FALSE);
            if(result == QMESH_RESULT_SUCCESS)
            {
                /*Restart timer*/
                CreateBeaconHandlerTimer ();
            }
        }
        else
        {
            /* Device is now associated so no need to start the timer again */
            QmeshTimerDelete (&app_timer_ghdl, &unprov_dev_beacon_tid);
            unprov_dev_beacon_tid = QMESH_TIMER_INVALID_HANDLE;
        }
    }
}

/*
 *Create Beacon timer. Beacon is sent once every 2 seconds
 */
static void CreateBeaconHandlerTimer(void)
{
    /* Generate a random delay between 0 to 511 ms */
    uint32_t random_delay = ((uint32_t)(QmeshRandom16() & 0x0FFF));
    prov_state = app_state_not_provisioned;

    /* Start a timer to send Device ID messages periodically to get
     * associated to a network
     */
    unprov_dev_beacon_tid = QmeshTimerCreate(&app_timer_ghdl,
                                             UnProvDeviceBeaconAdvertTimeoutHandler,
                                             NULL,
                                             (random_delay + 2000));
    if (unprov_dev_beacon_tid == QMESH_TIMER_INVALID_HANDLE)
    {
        QCLI_LOGE (mesh_group, "%d: Failed to create beacon timer!",__FUNCTION__);
        return;
    }
}

static QmeshResult SetRole(uint8_t device_instance, bool prov)
{
    QmeshResult res;

    /* Set provision parameters */
    res = QmeshSetProvisioningRole(device_instance, prov);

    if(res == QMESH_RESULT_SUCCESS)
    {
        provisioner_role_enabled = prov;
        prov_net_idx = device_instance;
        if(!prov && device_data.oob_pub_key != NULL)
        {
            uint16_t i;

            QCLI_LOGE (mesh_group, "\nDevice public key: ");
            for(i = 0; i < 32; i++)
            {
                QCLI_LOGE (mesh_group, "%04x", device_data.oob_pub_key[i]);
            }
        }
    }
    else
    {
        QCLI_LOGE (mesh_group, "QmeshSetProvisioningRole failed (0x%x)\n", res);
    }

    return res;
}


/*Uncomment the below function to handle QMESH_PROVISION_INVITE_EVENT - */
/*static void HandleDeviceInviteEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    if(app_context.prov_via_gatt)
    {
        QCLI_LOGE (mesh_group, "\r\n HandleDeviceInviteEvent\n");
        QmeshSchedUserAdvertDisable();
    }
    else
    {
        QCLI_LOGE (mesh_group, "%d: User-advert not disabled\n",__FUNCTION__);
    }
}*/


/* Handle QMESH_DEVICE_CAPABILITY_EVENT Event */
static void HandleDeviceCapabilityEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_REMOTE_NODE_CAPS_DATA_T *dev_cap_info;

    dev_cap_info = (QMESH_REMOTE_NODE_CAPS_DATA_T *) event_data.param;

    QCLI_LOGI (mesh_group, "\r\n dev_cap_info->num_elm %d", dev_cap_info->num_elements);
    QCLI_LOGI (mesh_group, "\r\n dev_cap_info->supp_algos %d", dev_cap_info->caps.prov_algos_mask);
    QCLI_LOGI (mesh_group, "\r\n dev_cap_info->pubkey_oob_mask %d", dev_cap_info->caps.pubkey_oob_mask);
    QCLI_LOGI (mesh_group, "\r\n dev_cap_info->static_oob_mask %d", dev_cap_info->caps.static_oob_mask);
    QCLI_LOGI (mesh_group, "\r\n dev_cap_info->output_oob_size %d", dev_cap_info->caps.output_oob_size);
    QCLI_LOGI (mesh_group, "\r\n dev_cap_info->output_action_mask %d", dev_cap_info->caps.output_action_mask);
    QCLI_LOGI (mesh_group, "\r\n dev_cap_info->input_oob_size %d", dev_cap_info->caps.input_oob_size);
    QCLI_LOGI (mesh_group, "\r\n dev_cap_info->input_action_mask %d", dev_cap_info->caps.input_action_mask);
    QCLI_LOGI (mesh_group, "\t\t %d, %d", 16*2, 0x01);
    QCLI_LOGI (mesh_group, "\nUse 'SelectPeerCapability' command to select capability for OOB action\n");
    remote_num_elms = dev_cap_info->num_elements;
}

/* Handle QMESH_OOB_ACTION_EVENT Event */
static void HandleDeviceOobActionEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    if(app_context.handle_oob_act_evt)
    {
        uint8_t oob_data[16] = {0};
        QmeshResult res;

        QCLI_LOGI (mesh_group, "\r\n OOB Action event received\n");

        oob_data[15] = 7;
        res = QmeshSetOobData(oob_data);
        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGI (mesh_group, "QmeshSetOobData: %d\n", res);
        }
    }
    else
    {
        QCLI_LOGI (mesh_group, "\r\n User needs to take action on OOB event\n");
    }
}


/* Handle QMESH_PROV_COMPLETE_DATA event */
static void HandleProvisionCompleteEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_PROV_COMPLETE_DATA_T *prov_info;
    QMESHAPP_DEVICE_INFO_T *app_dev_info;

    prov_info = (QMESH_PROV_COMPLETE_DATA_T *) event_data.param;
    if(QMESH_RESULT_SUCCESS == event_data.status)
    {
        if(provisioner_role_enabled)
        {
            QMESH_PEER_DEVICE_KEY_LIST *prov_dev_info;

            prov_dev_info = (QMESH_PEER_DEVICE_KEY_LIST*) malloc(sizeof(QMESH_PEER_DEVICE_KEY_LIST));
            if (prov_dev_info != NULL)
            {
                prov_dev_info->prov_net_id = prov_info->key_info.prov_net_idx;
                prov_dev_info->elem_addr = prov_info->elm_addr;
                prov_dev_info->num_elems = remote_num_elms;
                memcpy(prov_dev_info->device_key, prov_info->device_key, QMESH_SECURITY_KEY_HWORDS * sizeof(uint16_t));
                prov_dev_info->next = app_context.peer_device_key_list;
                app_context.peer_device_key_list = prov_dev_info;
            }

            if(app_context.prov_via_gatt)
            {
                InitiateGattDisconnection(gatt_unprov_dev_found_list[gatt_unprov_dev_index].device_addr);
            }
       }
       else
       {
            /* Set the application state to Provisioned */
            app_context.device_provisioned = TRUE;
            prov_state = app_state_provisioned;
            QCLI_LOGI (mesh_group, "\r\nPrim Elm Addr: 0x%x, prov_via_gatt: %d\n", prov_info->elm_addr, app_context.prov_via_gatt);
#if (QMESH_PS_IFCE_ENABLED == 1)
            NVMWrite_ProvState(true);
            StoreElementSequenceNo ();
            StartElementSequenceNoTimer ();
            NVMWrite_PrimaryElementAddress(prov_info->elm_addr);
#endif
            QmeshUpdateModelWithUnicastAddr();
            AddSubnetKey(prov_info->key_info);

            /* Start GATT Proxy Service on Provisioning Complete via ADV
             * only if Proxy feature is supported.
             */
            if(!app_context.prov_via_gatt && (dev_comp->feature_mask & PROXY_FEATURE_MASK))
            {
                QCLI_LOGE (mesh_group, "\r\nStarting GATT Proxy Service\n");
                StartGattProxyService();
            }
        }

        /* Printing device key */
        if (NULL != prov_info->device_key)
        {
            uint8_t indx;
            QCLI_LOGI (mesh_group, "\r\nDevice key is ");
            for(indx = 0; indx < 8; indx++)
            {
                QCLI_LOGI (mesh_group, "0x%4x ", prov_info->device_key[indx]);
            }
            QCLI_LOGI (mesh_group, "\n");
        }
    }
    else
    {
        QCLI_LOGI(mesh_group, "Provisioning failed with res: 0x%x\n", event_data.status);
        prov_state = app_state_not_provisioned;
        app_context.device_provisioned = FALSE;

        if (app_context.prov_via_gatt)
        {
            if (!provisioner_role_enabled)
            {
                app_dev_info = _getDeviceInfo(app_context.prov_conn_id);
                if (NULL != app_dev_info)
                {
                    QCLI_LOGI(mesh_group, "PB-GATT Disconnection : Device address: 0x%02x%02x%02x%02x%02x%02x\n",
                                   app_dev_info->remote_addr.BD_ADDR5, app_dev_info->remote_addr.BD_ADDR4,
                                   app_dev_info->remote_addr.BD_ADDR3, app_dev_info->remote_addr.BD_ADDR2,
                                   app_dev_info->remote_addr.BD_ADDR1, app_dev_info->remote_addr.BD_ADDR0);

                    InitiateGattDisconnection(app_dev_info->remote_addr);
                }
            }
            else
            {
                InitiateGattDisconnection(gatt_unprov_dev_found_list[gatt_unprov_dev_index].device_addr);
            }
        }

        /* Set Role again if Node */
        if (!provisioner_role_enabled)
        {
            if (SetRole(prov_net_idx, FALSE) == QMESH_RESULT_SUCCESS)
            {
                QCLI_LOGI (mesh_group, "Use SendUnProvDeviceBeacon command to provision again\n");
            }
        }
    }
}

static void HandleDeviceLinkCloseEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    QCLI_LOGI(mesh_group, "\r\nHandling Link close event\n");
}

/* Handle QMESH_PROXY_FILTER_STATUS_EVENT event */
static void HandleProxyFilterStatusEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_PROXY_FILTER_STATUS_T *filter_status;
    filter_status = (QMESH_PROXY_FILTER_STATUS_T *)event_data.param;
    QCLI_LOGI (mesh_group, "\r\n\tFilter Type: %d", filter_status->type);
    QCLI_LOGI (mesh_group, "\r\n\tSrc Addr: 0x%04x", filter_status->src_addr);
    QCLI_LOGI (mesh_group, "\r\n\tNum of addresses: %d", filter_status->num_addresses);
}

/* Handle QMESH_NODE_RESET_EVENT event */
static void HandleNodeResetEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    uint8_t net_idx = *((uint8_t *)event_data.param);

    /* Reset the provisioning status */
    app_context.device_provisioned = FALSE;

#if (QMESH_PS_IFCE_ENABLED == 1)
    NVMWrite_ProvState(false);
#endif

    /* Disable User Advert Data Advertisement */
    QmeshSchedUserAdvertDisable();

    if(IsGATTProxyConnected())
    {
        uint8_t idx;
        QCLI_LOGI (mesh_group, "Removing GATT Proxy connection\n");
        for (idx = 0; idx < MAX_NUM_OF_GATT_CONN; idx++)
        {
            if (app_context.deviceInfo[idx].connection_id > 0)
            {
                InitiateGattDisconnection(app_context.deviceInfo[idx].remote_addr);
            }
        }
    }

    /* Reset all the subnetwork data. */
    ResetSubnetData();

    /* Re-initialize the App Key and Subscription lists in the composition data. */
    DemoCompAppKeyListInit();
    DemoCompSubsListInit();

    if(gatt_proxy_conn_advrt_info.proxy_advrt_tmr_hndl != QMESH_TIMER_INVALID_HANDLE)
    {
        QCLI_LOGI (mesh_group, "Deleting connectable adv timer for this network\n");
        QmeshTimerDelete (&app_timer_ghdl, &gatt_proxy_conn_advrt_info.proxy_advrt_tmr_hndl);
    }

    /* Set provision parameters */
    if(SetRole(net_idx, FALSE) == QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGI (mesh_group, "Use SendUnProvDeviceBeacon command to provision again\n");
    }
}

static void SetGattProxyConnAdvertParams(void)
{
    uint8_t uuid_array[] = MESH_PROXY_SERVICE_UUID;
    /*Set Advertisement Parameters */
    gatt_proxy_conn_advrt_info.advert_params.Scan_Request_Filter = QAPI_BLE_FP_NO_FILTER_E;
    gatt_proxy_conn_advrt_info.advert_params.Connect_Request_Filter = QAPI_BLE_FP_NO_FILTER_E;
    gatt_proxy_conn_advrt_info.advert_params.Advertising_Channel_Map = 0x07; /*All 3 channels*/
    gatt_proxy_conn_advrt_info.advert_params.Advertising_Interval_Min = 1000; /* 1000 ms */
    gatt_proxy_conn_advrt_info.advert_params.Advertising_Interval_Max = 1000; /* 1000 ms */
    /* Set Connection Parameters */
    gatt_proxy_conn_advrt_info.conn_params.Connectability_Mode = QAPI_BLE_LCM_CONNECTABLE_E;
    gatt_proxy_conn_advrt_info.conn_params.Own_Address_Type = QAPI_BLE_LAT_PUBLIC_E;
    gatt_proxy_conn_advrt_info.conn_params.Direct_Address_Type = QAPI_BLE_LAT_PUBLIC_E;

    /*Set Proxy Service UUID in Advertisement Data*/
    gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[0] = 0x02;          /*Length of the AD structure*/
    gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[1] = 0x01;          /*AD_Type ==> Flags*/
    gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[2] = 0x06;          /*Flags- Value*/
    gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[3] = 0x03;          /*Length of the AD structure*/
    gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[4] = 0x03;          /*AD_Type ==> Complete List of 16-bit Service Class UUIDs*/
    gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[5] = uuid_array[0]; /*Proxy SERVICE UUID0*/
    gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[6] = uuid_array[1]; /*Proxy SERVICE UUID1*/
    gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[7] = 0x0C;          /*Length of the AD structure for Network-ID*/
    gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[8] = 0x16;          /*AD_Type ==>  «Service Data» */
    gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[9] = uuid_array[0]; /*Proxy SERVICE UUID byte0*/
    gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[10] = uuid_array[1]; /*Proxy SERVICE UUID byte1*/

}



/******************************************************************************
 * Config Model Event Handlers
 *****************************************************************************/
/* Display the network header information for the specefied config message */
static void DisplayNetworkData(QMESH_SUBNET_KEY_IDX_INFO_T subnet_info, const QMESH_NW_HEADER_INFO_T *nw_hdr)
{
    QCLI_LOGI (mesh_group, "\r\n\tSource Address:%d\t", nw_hdr->src_addr);
#if 0 /* Enable below prints if required */
    QCLI_LOGI (mesh_group, "\r\n\tProvisioned Net ID:%d\t", subnet_info.prov_net_idx);
    QCLI_LOGI (mesh_group, "\r\n\tGlobal Net Key Idx:%d\t", subnet_info.key_idx);
    QCLI_LOGI (mesh_group, "\r\n\tSequence Number:%d\t", nw_hdr->seq_num);
    QCLI_LOGI (mesh_group, "\r\n\tDestination Number:%d\t", nw_hdr->dst_addr);
    QCLI_LOGI (mesh_group, "\r\n\tReceived TTL of the message:%d\t", nw_hdr->ttl);
#endif
}

/* Handle QMESH_CONFIG_NET_KEY_STATUS_EVENT event */
static void HandleNetKeyStatusEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tstatus: Status=%d, NetKeyInde=%d",
               cli_evt_data->event_info.net_key_status.status,
               cli_evt_data->event_info.net_key_status.glob_net_key_idx);
}

/* Handle QMESH_CONFIG_APP_KEY_STATUS_EVENT event */
static void HandleAppKeyStatusEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tstatus: %d", cli_evt_data->event_info.app_key_status.status);
    QCLI_LOGI (mesh_group, "\r\n\tglobal net Key Index: ",
               cli_evt_data->event_info.app_key_status.glob_net_key_idx);
    QCLI_LOGI (mesh_group, "\r\n\tglobal net Key Index: ",
               cli_evt_data->event_info.app_key_status.glob_app_key_idx);

}

/* Handle QMESH_CONFIG_APP_KEY_LIST_EVENT */
static void HandleConfigAppKeyListEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tstatus: %d", cli_evt_data->event_info.app_key_list.status);
    QCLI_LOGI (mesh_group, "\r\n\tglobal net Key Index: %d",
               cli_evt_data->event_info.app_key_list.glob_net_key_idx);
    QCLI_LOGI (mesh_group, "\r\n\tNum indexes: %d", cli_evt_data->event_info.app_key_list.num_keys);
    QCLI_LOGI (mesh_group, "\r\nApp Key index list: ");
    QCLI_ARR_LOGI (mesh_group, cli_evt_data->event_info.app_key_list.app_key_index,
                   cli_evt_data->event_info.app_key_list.num_keys);
}

/* Handle QMESH_CONFIG_NET_KEY_LIST_EVENT */
static void HandleConfigNetKeyListEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tNum indexes: %d", cli_evt_data->event_info.net_key_list.num_keys);
    QCLI_LOGI (mesh_group, "\r\nNet Key index list: ");
    QCLI_ARR_LOGI (mesh_group, cli_evt_data->event_info.net_key_list.net_key_index,
                   cli_evt_data->event_info.net_key_list.num_keys)
}

/* Handle QMESH_CONFIG_MODEL_PUB_STATUS_EVENT */
static void HandleConfigModelPubStatusEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tStatus:%d\t", cli_evt_data->event_info.pub_status.status);
    QCLI_LOGI (mesh_group, "\r\n\tElement Address:%d\t", cli_evt_data->event_info.pub_status.elm_addr);
    QCLI_LOGI (mesh_group, "\r\n\tPublish Address:%d\t", cli_evt_data->event_info.pub_status.pub_addr);
    QCLI_LOGI (mesh_group, "\r\n\tApp key index:%d\t", cli_evt_data->event_info.pub_status.app_key_idx);
    QCLI_LOGI (mesh_group, "\r\n\tCredential Flag:%d\t", cli_evt_data->event_info.pub_status.credential_flag);
    QCLI_LOGI (mesh_group, "\r\n\tPublish TTL:%d\t", cli_evt_data->event_info.pub_status.pub_ttl);
    QCLI_LOGI (mesh_group, "\r\n\tPublish period:%d\t", cli_evt_data->event_info.pub_status.pub_period);
    QCLI_LOGI (mesh_group, "\r\n\tModel ID:%d\t", cli_evt_data->event_info.pub_status.model_id);
}

/* Handle QMESH_CONFIG_NODE_IDENTITY_STATUS_EVENT */
static void HandleConfigNodeIdStatusEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tstatus: %d, NetKeyIndex %d, Identity State %d",
               cli_evt_data->event_info.node_identity_status.status,
               cli_evt_data->event_info.node_identity_status.glob_net_key_idx,
               cli_evt_data->event_info.node_identity_status.identity_state);
}

/* Handle QMESH_CONFIG_MODEL_SUB_STATUS_EVENT */
static void HandleConfigModelSubStatusEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tstatus:%d, Element addr=%d, Sub Addr=%d, Model ID=%d\t",
               cli_evt_data->event_info.sub_status.status,
               cli_evt_data->event_info.sub_status.elm_addr,
               cli_evt_data->event_info.sub_status.sub_addr,
               cli_evt_data->event_info.sub_status.model_id);
}

/* Handle QMESH_CONFIG_HEART_BEAT_PUB_STATUS_EVENT */
static void HandleConfigHeartRatePubStatusEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tstatus: %d", cli_evt_data->event_info.hb_pub_status.status);
    QCLI_LOGI (mesh_group, "\r\n\tDest Addr: %d", cli_evt_data->event_info.hb_pub_status.dest_addr);
    QCLI_LOGI (mesh_group, "\r\n\tCount Log: %d", cli_evt_data->event_info.hb_pub_status.count_log);
    QCLI_LOGI (mesh_group, "\r\n\tperiod Log: %d", cli_evt_data->event_info.hb_pub_status.period);
    QCLI_LOGI (mesh_group, "\r\n\tTTL: %d", cli_evt_data->event_info.hb_pub_status.ttl);
    QCLI_LOGI (mesh_group, "\r\n\tNet Key Index: %d", cli_evt_data->event_info.hb_pub_status.net_key_idx);
    QCLI_LOGI (mesh_group, "\r\n\tFeatures: %d", cli_evt_data->event_info.hb_pub_status.features);
}

/*Handle QMESH_CONFIG_HEART_BEAT_SUB_STATUS_EVENT */
static void HandleConfigHeartBeatSubStatusEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tstatus: %d", cli_evt_data->event_info.hb_sub_status.status);
    QCLI_LOGI (mesh_group, "\r\n\tSrc addr: %d", cli_evt_data->event_info.hb_sub_status.src_addr);
    QCLI_LOGI (mesh_group, "\r\n\tDest Addr: %d", cli_evt_data->event_info.hb_sub_status.dest_addr);
    QCLI_LOGI (mesh_group, "\r\n\tCount Log: %d", cli_evt_data->event_info.hb_sub_status.count_log);
    QCLI_LOGI (mesh_group, "\r\n\tperiod Log: %d", cli_evt_data->event_info.hb_sub_status.period_log);
    QCLI_LOGI (mesh_group, "\r\n\tMin Hops: %d", cli_evt_data->event_info.hb_sub_status.min_hops);
    QCLI_LOGI (mesh_group, "\r\n\tMax hops: %d", cli_evt_data->event_info.hb_sub_status.max_hops);
}

/* Handle QMESH_CONFIG_MODEL_APP_STATUS_EVENT */
static void HandleConfigModelAppStatusEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\nQMESH_CONFIG_MODEL_APP_STATUS_EVENT\r\n\tstatus: %d",
               cli_evt_data->event_info.mdl_app_status.status);
    QCLI_LOGI (mesh_group, "\r\n\tele addr: %d", cli_evt_data->event_info.mdl_app_status.ele_addr);
    QCLI_LOGI (mesh_group, "\r\n\tapp key index: %d", cli_evt_data->event_info.mdl_app_status.app_key_idx);
}

/* Handle QMESH_CONFIG_COMPOSITION_DATA_STATUS_EVENT */
static void HandleConfigCompDataStatusEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\nDevice Composition details:");
    QCLI_LOGI (mesh_group, "\r\n\tpage num:\t0\r\n\tCID:%d\t", cli_evt_data->event_info.comp_data.cid);
    QCLI_LOGI (mesh_group, "\r\n\tPID:\t", cli_evt_data->event_info.comp_data.pid);
    QCLI_LOGI (mesh_group, "\r\n\tVID:\t", cli_evt_data->event_info.comp_data.vid);
    QCLI_LOGI (mesh_group, "\r\n\tCRPL:\t", cli_evt_data->event_info.comp_data.crpl);
    QCLI_LOGI (mesh_group, "\r\n\tFeatures:\t", cli_evt_data->event_info.comp_data.features);
    QCLI_LOGI (mesh_group, "\r\nElement List:\r\n\t[ ");
    QCLI_ARR_LOGI (mesh_group, cli_evt_data->event_info.comp_data.ele_list,
                   cli_evt_data->event_info.comp_data.ele_list_len);
}

/* Handle QMESH_CONFIG_KEY_REFRESH_PHASE_STATUS_EVENT */
static void HandleConfigKeyRefreshPhStatusEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tstatus: %d", cli_evt_data->event_info.key_refresh_status.status);
    QCLI_LOGI (mesh_group, "\r\n\tGlobal Net Key Index: %d",
               cli_evt_data->event_info.key_refresh_status.glob_net_key_index);
    QCLI_LOGI (mesh_group, "\r\n\tPhase: %d", cli_evt_data->event_info.key_refresh_status.phase);
}

/* Handle QMESH_CONFIG_SIG_MODEL_APP_LIST_EVENT and QMESH_CONFIG_VENDOR_MODEL_APP_LIST_EVENT */
static void HandleConfigModelAppListEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tstatus: %d", cli_evt_data->event_info.model_app_list.status);
    QCLI_LOGI (mesh_group, "\r\n\tElemen Address: %d", cli_evt_data->event_info.model_app_list.ele_addr);
    QCLI_LOGI (mesh_group, "\r\n\tModel ID: %d", cli_evt_data->event_info.model_app_list.model_id);
    QCLI_LOGI (mesh_group, "\r\n\tModel Type: %d", cli_evt_data->event_info.model_app_list.type);
    QCLI_LOGI (mesh_group, "\r\n\tapp index list: [ ");
    QCLI_ARR_LOGI (mesh_group, cli_evt_data->event_info.model_app_list.app_idx_list,
                   cli_evt_data->event_info.model_app_list.num_keys);
}

/* Handle QMESH_CONFIG_SIG_MODEL_SUBSCRIBE_LIST_EVENT and CONFIG_VENDOR_MODEL_SUBSCRIPTION_LIST*/
static void HandleConfigModelSubListEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tstatus: %d", cli_evt_data->event_info.model_sub_list.status);
    QCLI_LOGI (mesh_group, "\r\n\tElemen Address: %d", cli_evt_data->event_info.model_sub_list.ele_addr);
    QCLI_LOGI (mesh_group, "\r\n\tModel ID: %d", cli_evt_data->event_info.model_sub_list.model_id);
    QCLI_LOGI (mesh_group, "\r\n\tModel Type: %d", cli_evt_data->event_info.model_sub_list.type);
    QCLI_LOGI (mesh_group, "\r\n\tSubscription list: [ ");
    QCLI_ARR_LOGI (mesh_group, cli_evt_data->event_info.model_sub_list.subscribe_list,
                   cli_evt_data->event_info.model_sub_list.num_addr);
}

/* Handle QMESH_CONFIG_IDENTITY_GET_EVENT */
static void HandleConfigNodeIdentityGetEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_NODE_IDENTITY_INFO_T *info;
    info = (QMESH_CONFIG_NODE_IDENTITY_INFO_T *)event_data.param;
    QCLI_LOGI (mesh_group,"\r\n\tProv Net Index %x, netKeyIndex %x, value %x\n",
        info->net_key_index.prov_net_idx, info->net_key_index.key_idx, info->identity);
    /* Need to update the applicaton stored identity value for the net_key_index. */
    GetNodeIdentityState(info);
}

/* Handle QMESH_CONFIG_IDENTITY_SET_EVENT */
static void HandleConfigNodeIdentitySetEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_NODE_IDENTITY_INFO_T *info;

    info = (QMESH_CONFIG_NODE_IDENTITY_INFO_T *)event_data.param;
    QCLI_LOGI (mesh_group,"\r\n\tProv Net Index %x, netKeyIndex %x, value %x\n",
        info->net_key_index.prov_net_idx, info->net_key_index.key_idx, info->identity);

    SetNodeIdentityState(info);
}

/* Handle QMESH_CONFIG_GATT_PROXY_GET_EVENT */
static void HandleConfigGattProxyGetEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_GATT_PROXY_INFO_T *proxyInfo;
    proxyInfo = (QMESH_CONFIG_GATT_PROXY_INFO_T *)event_data.param;
    QCLI_LOGI (mesh_group, "\r\n\tgatt proxy state %d\n", gatt_proxy_conn_advrt_info.proxy_state);
    proxyInfo->gp_state = gatt_proxy_conn_advrt_info.proxy_state;
}

/* Handle QMESH_CONFIG_GATT_PROXY_SET_EVENT */
static void HandleConfigGattProxySetEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_GATT_PROXY_INFO_T *proxyInfo;

    proxyInfo = (QMESH_CONFIG_GATT_PROXY_INFO_T *)event_data.param;

    /* gatt_proxy_conn_advrt_info.proxy_state stores current state of gatt proxy operation.
     * application needs to update this value to QMESH_GATT_PROXY_STATE_DISABLED
     * after 60 seconds of network ID advertisment.
     */
    QCLI_LOGI (mesh_group, "\r\n\tstate=0x%x\n", proxyInfo->gp_state);
    gatt_proxy_conn_advrt_info.proxy_state = proxyInfo->gp_state;

    if (gatt_proxy_conn_advrt_info.proxy_state == QMESH_GATT_PROXY_STATE_RUNNING)
    {
        /* Binding with GATT Proxy State
         * Set Node Identity State for all subnet to 1 */
        /* Start GATT Proxy Service */
        StartGattProxyService();
    }
    else
    {
        /* Binding with GATT Proxy State
         * Set Node Identity State for all subnet to 0 */
        /* Stop GATT Proxy Service */
        StopGattProxyService();
    }
}

/* Handle QMESH_CONFIG_NETWORK_TRANSMIT_STATUS_EVENT */
static void HandleConfigNetworkTransmitStatusEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tNetwork Transmit Count: %d", cli_evt_data->event_info.nw_transmit_status.nw_tx_cnt);
    QCLI_LOGI (mesh_group, "\r\n\tNetwork Transmit Interval Steps: %d", cli_evt_data->event_info.nw_transmit_status.nw_tx_int_step);
}

/* Handle QMESH_CONFIG_RELAY_STATUS_EVENT */
static void HandleConfigRelayStatusEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;
    DisplayNetworkData(cli_evt_data->net_key_info, cli_evt_data->nw_hdr);
    QCLI_LOGI (mesh_group, "\r\n\tRelay State: %d", cli_evt_data->event_info.relay_status.relay_state);
    QCLI_LOGI (mesh_group, "\r\n\tRelay Retransmit Count: %d", cli_evt_data->event_info.relay_status.relay_rtx_cnt);
    QCLI_LOGI (mesh_group, "\r\n\tRelay Retransmit Interval Steps: %d", cli_evt_data->event_info.relay_status.relay_rtx_int_stp);
}

/* Handle QMESH_CONFIG_APP_BIND_UNBIND_EVENT */
static void HandleAppKeyBindEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_APP_BIND_EVENT_T *bind_event;
    const QMESH_ELEMENT_CONFIG_T *p_ele;
    bind_event = (QMESH_CONFIG_APP_BIND_EVENT_T *)event_data.param;
    p_ele = bind_event->ele_model_info.p_ele;
    QCLI_LOGI (mesh_group, "\r\n\tBind state: %d", bind_event->bind_state);
    QCLI_LOGI (mesh_group, "\r\n\tElement Address: %d", p_ele->element_data->unicast_addr);
    QCLI_LOGI (mesh_group, "\r\n\tApp Key Index: %d", bind_event->app_key_idx);
    QCLI_LOGI (mesh_group, "\r\n\tModel ID: %d", bind_event->ele_model_info.model_id);
    QCLI_LOGI (mesh_group, "\r\n\tModel Type: %d", bind_event->ele_model_info.model_type);
#if (QMESH_PS_IFCE_ENABLED == 1)
    uint8_t elementid, modelid;
    const QMESH_MODEL_DATA_T *p_model;
    p_model = bind_event->ele_model_info.p_model;
    elementid =  p_ele - &dev_comp->elements[QMESH_PRIMARY_ELEMENT_INDEX];
    modelid = p_model - &dev_comp->elements[elementid].element_data->model_data[0];
    if (QMESH_CONFIG_APP_BIND == bind_event->bind_state)
        NVMWrite_ModelAppKeyAdd (elementid, modelid, bind_event->app_key_idx);
    else
        NVMWrite_ModelAppKeyDelete (elementid, modelid, bind_event->app_key_idx);
#endif
}

/* Handle QMESH_CONFIG_SUBSCRIPTION_SET_EVENT */
static void HandleSubscriptionSetEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_SUB_SET_EVENT_T *sub_set_event;
    const QMESH_ELEMENT_CONFIG_T *p_ele;
    uint8_t idx;
    sub_set_event = (QMESH_CONFIG_SUB_SET_EVENT_T *)event_data.param;
    p_ele = sub_set_event->ele_model_info.p_ele;
    QCLI_LOGI (mesh_group, "\r\n\tElement Address: %d", p_ele->element_data->unicast_addr);
    QCLI_LOGI (mesh_group, "\r\n\tSub Operation : %d", sub_set_event->sub_operation);
    QCLI_LOGI (mesh_group, "\r\n\tSub Addr : %d", sub_set_event->sub_addr);
    QCLI_LOGI (mesh_group, "\r\n\tModel ID: %d", sub_set_event->ele_model_info.model_id);
    QCLI_LOGI (mesh_group, "\r\n\tModel Type: %d", sub_set_event->ele_model_info.model_type);
    if (sub_set_event->label_uuid != NULL)
    {
        QCLI_LOGI (mesh_group, "\r\n\tLabel UUID : ");
        for(idx=0; idx < QMESH_UUID_SIZE_HWORDS; idx++)
        {
            QCLI_LOGI (mesh_group, "\r\n\tLabel UUID[%d]=0x%04x", idx, sub_set_event->label_uuid[idx]);
        }
    }

#if (QMESH_PS_IFCE_ENABLED == 1)
    uint8_t elementid, modelid;
    const QMESH_MODEL_DATA_T *p_model;
    p_model = sub_set_event->ele_model_info.p_model;
    elementid =  p_ele - &dev_comp->elements[QMESH_PRIMARY_ELEMENT_INDEX];
    modelid = p_model - &dev_comp->elements[elementid].element_data->model_data[0];
    if (sub_set_event->sub_operation == QMESH_MODEL_SUB_EVENT_DELETE_ALL)
    {
        if (sub_set_event->label_uuid != NULL)
        {
            NVMWrite_LabelUUID (elementid, modelid, (uint8_t*)sub_set_event->label_uuid,
                                QMESH_MODEL_SUB_EVENT_DELETE_ALL);
        }
        NVMWrite_ModelSubscriptionDeleteAll (elementid, modelid);
    }
    else
    {
        if (sub_set_event->label_uuid != NULL)
        {
            NVMWrite_LabelUUID (elementid, modelid, (uint8_t*)sub_set_event->label_uuid, sub_set_event->sub_operation);
        }
        else
        {
            switch (sub_set_event->sub_operation)
            {
            case QMESH_MODEL_SUB_EVENT_ADD:
                NVMWrite_ModelSubscriptionAdd (elementid, modelid, sub_set_event->sub_addr);
                break;
            case QMESH_MODEL_SUB_EVENT_DELETE:
                NVMWrite_ModelSubscriptionDelete (elementid, modelid, sub_set_event->sub_addr);
                break;
            case QMESH_MODEL_SUB_EVENT_OVERWRITE:
                NVMWrite_ModelSubscriptionUpdate (elementid, modelid, sub_set_event->sub_addr);
                break;
            case QMESH_MODEL_SUB_EVENT_DELETE_ALL:
                /*Handled above*/
                break;
            }
        }
    }
#endif
}

/* Handle QMESH_CONFIG_PUBLICATION_SET_EVENT */
static void HandlePublicationSetEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_PUBLICATION_SET_EVENT_T *pub_set_event;
    const QMESH_ELEMENT_CONFIG_T *p_ele;
    pub_set_event = (QMESH_CONFIG_PUBLICATION_SET_EVENT_T *)event_data.param;
    p_ele = pub_set_event->p_ele;
    QCLI_LOGI (mesh_group, "\r\n\tElement Address: %d", p_ele->element_data->unicast_addr);
    QCLI_LOGI (mesh_group, "\r\n\tModel ID: %d", pub_set_event->model_id);
    QCLI_LOGI (mesh_group, "\r\n\tModel Type: %d", pub_set_event->model_type);
#if (QMESH_PS_IFCE_ENABLED == 1)
    const QMESH_MODEL_DATA_T *p_model;
    uint8_t elementid, modelid;
    p_model = pub_set_event->p_model;
    elementid =  p_ele - &dev_comp->elements[QMESH_PRIMARY_ELEMENT_INDEX];
    modelid = p_model - &dev_comp->elements[elementid].element_data->model_data[0];
    NVMWrite_ModelPublicationStatus(elementid, modelid, p_model->publish_state);
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshAppProcessMeshEvent
 *
 *  DESCRIPTION
 *   The Mesh stack calls this call-back to notify asynchronous
 *   events to applications.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void QmeshProcessMeshEvent (QMESH_APP_EVENT_DATA_T event_data_cb)
{
    QMESH_EVENT_T operation_event = event_data_cb.event;

    switch(operation_event)
    {
        case QMESH_UNPROVISIONED_DEVICE_EVENT:
        {
            HandleUnprovisionedDeviceEvent(event_data_cb);
        }
        break;
        case QMESH_PROVISION_INVITE_EVENT:
        {
            QMESH_PROV_INVITE_DATA_T *prov_inv_info = (QMESH_PROV_INVITE_DATA_T *)event_data_cb.param;
            QCLI_LOGE (mesh_group, "\r\nReceived   MESH_PROVISION_INVITE_EVENT Attention-Duration=%d\n", prov_inv_info->att_duration);
            //HandleDeviceInviteEvent(eventDataCallback);
        }
        break;
        case QMESH_DEVICE_CAPABILITY_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_DEVICE_CAPABILITY_EVENT\n");
            HandleDeviceCapabilityEvent (event_data_cb);
        }
        break;
        case QMESH_DEVICE_PUBLIC_KEY_ACTION_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_DEVICE_PUBLIC_KEY_ACTION_EVENT\n");
            HandleDevicePubKeyEvent ();
        }
        break;
        case QMESH_OOB_ACTION_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_OOB_ACTION_EVENT");
            HandleDeviceOobActionEvent(event_data_cb);
        }
        break;

        case QMESH_PROXY_FILTER_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_PROXY_FILTER_STATUS_EVENT");
            HandleProxyFilterStatusEvent(event_data_cb);
        }
        break;

        case QMESH_TEST_PTS_ACCESS_PDU_EVT:
        {
            QMESH_TEST_PTS_ACCESS_PDU_T *access_test_pdu = (QMESH_TEST_PTS_ACCESS_PDU_T *)event_data_cb.param;
            uint16_t idx;

            if ((access_test_pdu) && (QMESH_RESULT_SUCCESS == event_data_cb.status))
            {
                QCLI_LOGI(mesh_group, "\r\n==> ACC PAYLOAD : %d :", access_test_pdu->payload_len);
                for (idx = 0; idx < access_test_pdu->payload_len; idx++)
                {
                    QCLI_LOGI(mesh_group, " %02x", access_test_pdu->payload[idx]);
                }
                QCLI_LOGI(mesh_group, "\r\n");
            }
        }
        break;

        case QMESH_APP_PAYLOAD_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\n Received QMESH_APP_PAYLOAD_EVENT: 0x%04x", event_data_cb.status);
        }
        break;

        case QMESH_PROVISION_STATUS_EVENT:
        {
            QMESH_PROV_STATUS_T st = *(QMESH_PROV_STATUS_T *) event_data_cb.param;
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_PROVISION_STATUS: %s (%d)",
                       prov_status[st >= QMESH_PROV_STATUS_MAX ? QMESH_PROV_STATUS_MAX : st ], st);
        }
        break;
        case QMESH_PROVISION_COMPLETE_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_PROVISION_COMPLETE");
            HandleProvisionCompleteEvent (event_data_cb);
        }
        break;
        case QMESH_PROV_LINK_OPEN_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_OPERATION_LINK_OPEN");
            prov_state = app_state_provision_started;
        }
        break;
        case QMESH_PROV_LINK_CLOSE_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_PROV_LINK_CLOSE_EVENT");
            HandleDeviceLinkCloseEvent(event_data_cb);
        }
        break;
        case QMESH_NODE_RESET_EVENT:
        {
           QCLI_LOGI (mesh_group, "\r\nReceived QMESH_NODE_RESET_EVENT");
           if(!provisioner_role_enabled)
           {
                HandleNodeResetEvent(event_data_cb);
           }
        }
        break;
        case QMESH_CONFIG_NET_KEY_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_NET_KEY_STATUS_EVENT");
            HandleNetKeyStatusEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_APP_KEY_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_APP_KEY_STATUS_EVENT");
            HandleAppKeyStatusEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_APP_KEY_LIST_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_APP_KEY_LIST_EVENT");
            HandleConfigAppKeyListEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_NET_KEY_LIST_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_NET_KEY_LIST_EVENT");
            HandleConfigNetKeyListEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_BEACON_STATUS_EVENT:
        {
            QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
            cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data_cb.param;
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_BEACON_STATUS_EVENT: %d",
                       cli_evt_data->event_info.beacon_status.beacon_state);
        }
        break;
        case QMESH_CONFIG_DEFAULT_TTL_STATUS_EVENT:
        {
            QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
            cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data_cb.param;
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_DEFAULT_TTL_STATUS_EVENT: %d",
                       cli_evt_data->event_info.default_ttl_status.ttl_state);
        }
        break;
        case QMESH_CONFIG_RELAY_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_RELAY_STATUS_EVENT");
            HandleConfigRelayStatusEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_GATT_PROXY_STATUS_EVENT:
        {
            QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
            cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data_cb.param;
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_GATT_PROXY_STATUS_EVENT");
            QCLI_LOGI (mesh_group, "\r\n\tProxy Status: %d",
                       cli_evt_data->event_info.gatt_proxy_status.gatt_proxy_state);
        }
        break;
        case QMESH_CONFIG_NODE_IDENTITY_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_NODE_IDENTITY_STATUS_EVENT");
            HandleConfigNodeIdStatusEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_FRIEND_STATUS_EVENT:
        {
            QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
            cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data_cb.param;
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_FRIEND_STATUS_EVENT");
            QCLI_LOGI (mesh_group, "\r\n\tfrnd state: %d",
                       cli_evt_data->event_info.friend_status.friend_state);
        }
        break;
        case QMESH_CONFIG_MODEL_PUB_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_MODEL_PUB_STATUS_EVENT");
            HandleConfigModelPubStatusEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_MODEL_SUB_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_MODEL_SUB_STATUS_EVENT");
            HandleConfigModelSubStatusEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_NODE_RESET_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nQMESH_CONFIG_NODE_RESET_STATUS_EVENT");
        }
        break;
        case QMESH_CONFIG_HEART_BEAT_PUB_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_HEART_BEAT_PUB_STATUS_EVENT");
            HandleConfigHeartRatePubStatusEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_HEART_BEAT_SUB_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_HEART_BEAT_SUB_STATUS_EVENT");
            HandleConfigHeartBeatSubStatusEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_MODEL_APP_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_MODEL_APP_STATUS_EVENT");
            HandleConfigModelAppStatusEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_COMPOSITION_DATA_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_COMPOSITION_DATA_STATUS_EVENT");
            HandleConfigCompDataStatusEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_KEY_REFRESH_PHASE_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_KEY_REFRESH_PHASE_STATUS_EVENT");
            HandleConfigKeyRefreshPhStatusEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_SIG_MODEL_APP_LIST_EVENT:
        case QMESH_CONFIG_VENDOR_MODEL_APP_LIST_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_MODEL_APP_LIST_EVENT");
            HandleConfigModelAppListEvent (event_data_cb);
        }
        break;
        case QMESH_CONFIG_SIG_MODEL_SUBSCRIBE_LIST_EVENT:
        case QMESH_CONFIG_VENDOR_MODEL_SUBSCRIBE_LIST_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_MODEL_SUBSCRIBE_LIST_EVENT");
            HandleConfigModelSubListEvent (event_data_cb);
        }
        break;

        case QMESH_CONFIG_IDENTITY_SET_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_IDENTITY_SET_EVENT");
            HandleConfigNodeIdentitySetEvent (event_data_cb);
        }
        break;

        case QMESH_CONFIG_IDENTITY_GET_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_IDENTITY_GET_EVENT");
            HandleConfigNodeIdentityGetEvent (event_data_cb);
        }
        break;

        case QMESH_CONFIG_GATT_PROXY_SET_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_GATT_PROXY_SET_EVENT");
            HandleConfigGattProxySetEvent (event_data_cb);
        }
        break;

        case QMESH_CONFIG_GATT_PROXY_GET_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_GATT_PROXY_GET_EVENT");
            HandleConfigGattProxyGetEvent (event_data_cb);
        }
        break;

        case QMESH_CONFIG_NETWORK_TRANSMIT_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_NETWORK_TRANSMIT_STATUS_EVENT");
            HandleConfigNetworkTransmitStatusEvent(event_data_cb);
        }
        break;

        case QMESH_CONFIG_NETKEY_ADD_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_NETKEY_ADD_EVENT");
            if (event_data_cb.status == QMESH_RESULT_SUCCESS)
            {
                AddSubnetKey(*(QMESH_SUBNET_KEY_IDX_INFO_T *)event_data_cb.param);
            }
        }
        break;

        case QMESH_CONFIG_NETKEY_DELETE_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_NETKEY_DELETE_EVENT");
            if (event_data_cb.status == QMESH_RESULT_SUCCESS)
            {
                RemoveSubnetKey(*(QMESH_SUBNET_KEY_IDX_INFO_T *)event_data_cb.param);
            }
        }
        break;
        case QMESH_GET_DEVICE_KEY_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_GET_DEVICE_KEY_EVENT");
            HandleGetDeviceKeyEvent(event_data_cb);
        }
        break;

        case QMESH_CONFIG_APP_BIND_UNBIND_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_APP_BIND_UNBIND_EVENT");
            HandleAppKeyBindEvent(event_data_cb);
        }
        break;

        case QMESH_CONFIG_SUBSCRIPTION_SET_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_SUBSCRIPTION_SET_EVENT");
            HandleSubscriptionSetEvent(event_data_cb);
        }
        break;

        case QMESH_CONFIG_PUBLICATION_SET_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_PUBLICATION_SET_EVENT");
            HandlePublicationSetEvent(event_data_cb);
        }
        break;

        case QMESH_HEALTH_FAULT_GET_EVENT:
        {
            /* Application may choose to update current_fault with
             * some known faults and current running test id.
             */
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_HEALTH_FAULT_GET_EVENT");
        }
        break;

        case QMESH_HEALTH_FAULT_CLEAR_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_HEALTH_FAULT_CLEAR_EVENT");
            /* Clear all faults reported from previous tests */
            uint8_t i;
            for(i = 0; i < current_fault.len_fault_arr; i++)
            {
                current_fault.fault_arr[i] = HEALTH_NO_FAULT_VALUE;
            }
        }
        break;

        case QMESH_HEALTH_FAULT_TEST_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_HEALTH_FAULT_TEST_EVENT");
            QMESH_HEALTH_TEST_ID_INFO_T *tid_info =
                        (QMESH_HEALTH_TEST_ID_INFO_T *)(event_data_cb.param);
            /* Test id to be tested */
            current_fault.test_id = tid_info->test_id;
            /* PTS expects TID 0x01 to be invalid . Hence for a
             * TID val 0x01 updating tid_info status to invalid
             * will prevent health server responding with fault
             * status message.
             */
            if(current_fault.test_id == 0x01)
            {
                tid_info->status = QMESH_HEALTH_INVALID_TEST_ID;
            }
            current_fault.fault_arr[0] = 0x1B;
        }
        break;
        case QMESH_HEALTH_ATTENTION_SET_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_HEALTH_ATTENTION_SET_EVENT");
            /* attention state (need to start a timer for the specified time) */
            current_fault.attention_state = *((uint8_t *)event_data_cb.param);
            attention_time = QmeshGetCurrentTimeInSecs();
        }
        break;
        case QMESH_HEALTH_ATTENTION_GET_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_HEALTH_ATTENTION_GET_EVENT");
            /* Example implementation application may choose to use */
            uint8_t rem_time;
            if(current_fault.attention_state)
            {
                rem_time = (uint8_t)(QmeshGetCurrentTimeInSecs() - attention_time);

                if(rem_time > current_fault.attention_state)
                {
                    current_fault.attention_state =
                                    HEALTH_ATTENTION_TIMER_OFF;
                }
                else
                {
                    current_fault.attention_state = rem_time;
                }
            }
        }
        break;

        default:
            QCLI_LOGI (mesh_group, "Received unhandled event=0x%x\n", operation_event);
            break;
    }

}


static void HandleGattDisconnection(qapi_BLE_BD_ADDR_t *dev_addr)
{
    if(!provisioner_role_enabled)
    {
        /* Handling for GATT Server */
        QCLI_LOGI(mesh_group, "HandleGattDisconnection: prov_via_gatt:%d\n",
            app_context.prov_via_gatt);

        if (app_context.prov_via_gatt)
        {
            QCLI_LOGI(mesh_group, "Received disconnection event for provisioning service\n");
            app_context.prov_via_gatt = FALSE;

#if (APP_PTS_MODE == 0)
            /*Provisioning done, remove the provisioning service*/
            if(UnregisterGattBearerService(QMESH_GATT_BEARER_PROV_SVC_E))
            {
                DeleteGATTBearer(dev_addr);
            }
            else
            {
                QCLI_LOGE (mesh_group, "Failed to Unregsiter Provisioning GATT bearer service\n");
            }
#else
            /*For PTS do not unregister service*/
            {
                QCLI_LOGI(mesh_group, "APP_PTS_MODE\n");
                DeleteGATTBearer(dev_addr);
            }
#endif
            _updateConnectionData(0, 0, dev_addr, FALSE);
            /* Start Proxy Service after provisioning is successfuly completed */
            if(app_context.device_provisioned && (dev_comp->feature_mask & PROXY_FEATURE_MASK))
            {
                QCLI_LOGI(mesh_group, "\r\nStarting GATT Proxy Service\n");
                StartGattProxyService();
            }
        }
        else if (app_context.is_proxy_srvc_enabled)
        {
             /* Remove Device GATT Bearer in Mesh Stack */
            DeleteGATTBearer(dev_addr);
             _updateConnectionData(0, 0, dev_addr, FALSE);
        }
    }
    else
    {
        /* Handling for GATT Client */
        QCLI_LOGI(mesh_group, "APP : GATT disconnection handling for GATT Srvc\n");
        gatt_unprov_dev_index = 0;
        app_context.prov_via_gatt = FALSE;

        DeleteGATTBearer(dev_addr);
        _updateConnectionData(0, 0, dev_addr, FALSE);
    }
}

static void _updateConnectionData(uint32_t conn_id, uint16_t mtu, qapi_BLE_BD_ADDR_t *addr, bool is_conn)
{
    QMESHAPP_DEVICE_INFO_T *devInfo = NULL;
    uint8_t idx;
    if (is_conn && (NULL != addr))
    {
        devInfo = _getDeviceInfo(conn_id);
        if (NULL == devInfo)
        {
            idx = _getAvailableDeviceInfoIndex();
            if (idx < MAX_NUM_OF_GATT_CONN)
            {
                devInfo = &app_context.deviceInfo[idx];
                devInfo->bearer_handle = QMESH_BEARER_HANDLE_INVALID;
            }
        }
        if (NULL != devInfo)
        {
            devInfo->is_connected = is_conn;
            devInfo->connection_id = conn_id;
            devInfo->mtu = mtu;
            devInfo->remote_addr_type = 0xFF;
            memcpy(&devInfo->remote_addr, addr, sizeof(qapi_BLE_BD_ADDR_t));
        }
    }
    else if (addr != NULL)
    {
        devInfo = _getDeviceInfoForDevAddr(addr);
        if (NULL != devInfo)
        {
            devInfo->is_connected = is_conn;
            devInfo->connection_id = 0;
            devInfo->mtu = 0;
            devInfo->remote_addr_type = 0xFF;
            memset(&devInfo->remote_addr, 0, sizeof(qapi_BLE_BD_ADDR_t));
            devInfo->bearer_handle = QMESH_BEARER_HANDLE_INVALID;
        }
    }
}

static QMESHAPP_DEVICE_INFO_T* _getDeviceInfo(uint32_t conn_id)
{
    QMESHAPP_DEVICE_INFO_T *devInfo = NULL;
    uint8_t idx;

    for (idx = 0; idx < MAX_NUM_OF_GATT_CONN; idx++)
    {
        if (app_context.deviceInfo[idx].connection_id == conn_id)
        {
            QCLI_LOGI(mesh_group, "_getDeviceInfo : idx %x, connId %x\n",idx, conn_id);
            devInfo = &app_context.deviceInfo[idx];
            break;
        }
    }

    return devInfo;
}

static uint8_t _getAvailableDeviceInfoIndex(void)
{
    uint8_t idx;
    /* Check for available index */
    for (idx = 0; idx < MAX_NUM_OF_GATT_CONN; idx++)
    {
        if (app_context.deviceInfo[idx].connection_id == 0)
        {
            QCLI_LOGI(mesh_group, "_getAvailableDeviceInfoIndex : idx %x\n",idx);
            break;
        }
    }

    return idx;
}

static QMESHAPP_DEVICE_INFO_T* _getDeviceInfoForDevAddr(qapi_BLE_BD_ADDR_t *addr)
{
    QMESHAPP_DEVICE_INFO_T *dev_info = NULL;
    uint8_t idx;

    for (idx = 0; idx < MAX_NUM_OF_GATT_CONN; idx++)
    {
        if (memcmp(&app_context.deviceInfo[idx].remote_addr, addr, sizeof(qapi_BLE_BD_ADDR_t)) == 0)
        {
            dev_info = &app_context.deviceInfo[idx];
            break;
        }
    }

    return dev_info;
}

void _setProvCapData(uint8_t publicKeyOobMask, uint8_t staticOobMask, uint8_t outputActionMask, uint8_t inputActionMask)
{
    QCLI_LOGI (mesh_group, "_setProvCapData input data is %x %x %x %x\n",
            publicKeyOobMask, staticOobMask, outputActionMask, inputActionMask);

    device_data.dev_caps.pubkey_oob_mask = publicKeyOobMask;
    device_data.dev_caps.static_oob_mask = staticOobMask;
    if(outputActionMask > 0)
        device_data.dev_caps.output_oob_size = 1;
    else
        device_data.dev_caps.output_oob_size = 0;

    device_data.dev_caps.output_action_mask = outputActionMask;

    if(inputActionMask > 0)
        device_data.dev_caps.input_oob_size = 1;
    else
        device_data.dev_caps.input_oob_size = 0;
    device_data.dev_caps.input_action_mask = inputActionMask;

    /*Enable flag based on capabilities to handle OOB action event*/
    if((device_data.dev_caps.pubkey_oob_mask != QMESH_PUBLIC_KEY_MASK_NONE &&
        device_data.dev_caps.input_oob_size == 0) ||
       device_data.dev_caps.static_oob_mask != QMESH_STATIC_MASK_NONE ||
       device_data.dev_caps.output_oob_size != 0)
    {
        app_context.handle_oob_act_evt = TRUE;
        QCLI_LOGI (mesh_group, "Enabling OOB action event handling\n");
    }
    else
    {
        app_context.handle_oob_act_evt = FALSE;
    }
}

 /* Health server data Initialiation procedure */
static void QmeshInitHealthServerData(const QMESH_DEVICE_COMPOSITION_DATA_T *dev_comp)
{
    /* Following example is shown considering health server is registed only in primary element */
    g_health_server_state.company_id = dev_comp->cid;
    g_health_server_state.pub_info = &server_elm_light_hsl_publish_state [1];
    g_health_server_state.fault = (QMESH_HEALTH_MODEL_DATA_T *) &current_fault;
    g_health_server_state.fast_period_divisor = QMESH_HEALTH_PERIOD_DIVISOR_INVALID;
    g_health_server_state.pub_interval = QMESH_HEALTH_PUB_PERIOD_DISABLED;
    g_health_server_state.pub_tid = QMESH_HEALTH_PUB_TIMER_INVALID_HANDLE;
}

/******************************************************************************
 *  Public Functions called from the Command Line Interface
 *****************************************************************************/
 /* All Initialiation of mesh done here */
/* Params: [Device UUID (16 octets in hex (32 chars))(Optional)]
           [
            [Public Key OOB Mask (1 octet)]
            [Static OOB Mask (1 octet)]
            [Output OOB Action (1 octet)]
            [Input OOB Action (1 octet)]
            (Optional)
           ]
*/
QCLI_Command_Status_t InitializeMesh(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res = QCLI_STATUS_SUCCESS_E;
    QMESH_RESULT_T r;
    static int32_t bt_stack_id;

    if((Parameter_Count != 0) && (Parameter_Count != 1) && (Parameter_Count != 4)
    && (Parameter_Count != 5))
    {
        QCLI_LOGE(mesh_group, "Invalid number of parameters\n");
        return QCLI_STATUS_USAGE_E;
    }

    if((Parameter_Count == 0) || (Parameter_List))
    {
        uint8_t str_len = strlen((char *) Parameter_List[0].String_Value);
        uint8_t word_len = 0;
        uint8_t param_index = 0;

        /* Here Device UUID is optional and
         * Other 4 parameters are optional in a group i.e. either all or none will be present.
         * This means Device UUID will be present if and only if either 1 param (only Device UUID)
         * or 5 params (All Params) are available.
         * In case Device UUID is not present the param index will be used from the beginning (i.e. 0).
         * Validate Device UUID paramater.
        */
        if((Parameter_Count == 1) || (Parameter_Count == 5))
        {
            /* This means Device UUID is present and needs to be processed,
             * hence increment the param index for the further processing
            */
            param_index = 1;

            /* Set Device UUID */
            str_len = strlen((char *) Parameter_List[0].String_Value);

            if(str_len != (QMESH_UUID_SIZE_HWORDS * 4))
                return QCLI_STATUS_USAGE_E;

            word_len = AsciiToHexU16_BE((char *)Parameter_List[0].String_Value, str_len, device_data.uuid);

            if(word_len == 0xff)
                return QCLI_STATUS_USAGE_E;
         }

        /* Enable the debug logs */
        QmeshDebugHandleSet (&QCLI_Printf, mesh_group);

        QCLI_LOGI (mesh_group, "Device UUID :\n");
        for(word_len=0; word_len < 8; word_len++)
        {
            QCLI_LOGI (mesh_group, "uuid[%d]=0x%04x\n", word_len, device_data.uuid[word_len]);
        }

        /* Params for Provisioning Capability will only be present if 4 params (all other params except Device UUID) or
         * 5 params (Device UUID + all provisioning capability params) are available as the provisioning capability.
         * param_index indicates whether to start from the beggining or one step ahead based on whether Device UUID
         * is present (one step ahead param_index = 1) or not (beggining param_index = 0)
         * Validate other parameters for provisioning capability
        */
        if(((Parameter_Count == 4) || (Parameter_Count == 5)) &&
            (Parameter_List[param_index].Integer_Is_Valid) &&
            (Parameter_List[param_index + 1].Integer_Is_Valid))
        {
            uint8_t publicKeyOobMask = Parameter_List[param_index].Integer_Value;
            uint8_t staticOobMask = Parameter_List[param_index + 1].Integer_Value;
            uint16_t outputActionMask = Parameter_List[param_index + 2].Integer_Value ;
            uint16_t inputActionMask = Parameter_List[param_index + 3].Integer_Value;

            /*Check the specified value range for provisioning-capability info*/
            if((publicKeyOobMask > 1) || (staticOobMask > 1) ||
                (outputActionMask > 0x1F) || (inputActionMask > 0x0F))
            {
                QCLI_LOGE(mesh_group, "Provisioning capability values are not as per range!!\n");
                return QCLI_STATUS_ERROR_E;
            }
            _setProvCapData(publicKeyOobMask, staticOobMask, outputActionMask, inputActionMask);

            QCLI_LOGI (mesh_group, "Device Capability data is %x %x %x %x %x %x\n",
                device_data.dev_caps.pubkey_oob_mask, device_data.dev_caps.static_oob_mask,
                device_data.dev_caps.output_oob_size, device_data.dev_caps.output_action_mask,
                device_data.dev_caps.input_oob_size, device_data.dev_caps.input_action_mask);
        }


        /* Initialize bluetooth */
        if (!InitializeBluetooth())
        {
           QCLI_LOGE (mesh_group, "Bluetooth initialization failed!!\n");
           return QCLI_STATUS_ERROR_E;
        }

        app_context.scan_over_pb_adv = 0;
        app_context.scan_over_pb_gatt = 0;

        bt_stack_id = GetBluetoothStackId();
        /* Initialize Mesh packet scheduler */
        r = QmeshSchedInit((void *)&bt_stack_id);
        if (QMESH_RESULT_SUCCESS != r)
        {
          QCLI_LOGE (mesh_group, "QmeshSchedInit failed ret=0x%x!!\n", r);
          return QCLI_STATUS_ERROR_E;
        }

        /* Initialize Composition Data. */
        DemoCompAppKeyListInit();
        DemoCompSubsListInit();


        /* Set default data from NVM - NVM support required */
        app_context.device_provisioned = FALSE;
        res = QmeshInit(1, QmeshProcessMeshEvent);
        if(res != QMESH_RESULT_SUCCESS)
        {
           QCLI_LOGE (mesh_group, "QmeshInit failed, ret=0x%x\n", res);
           return QCLI_STATUS_ERROR_E;
        }

        /* Enable advertising bearer for stack */
        res = QmeshAddBearer(QMESH_BEARER_TYPE_LE_ADV_PROVISIONING, NULL,
                             0, &app_context.adv_bearer_handle[QMESH_BEARER_TYPE_LE_ADV_PROVISIONING]);
        if(res != QMESH_RESULT_SUCCESS)
        {
           QCLI_LOGE (mesh_group, "QmeshAddBearer failed, ret=0x%x\n", res);
           return QCLI_STATUS_ERROR_E;
        }

        res = QmeshAddBearer(QMESH_BEARER_TYPE_LE_ADV_NETWORK, NULL,
                             0, &app_context.adv_bearer_handle[QMESH_BEARER_TYPE_LE_ADV_NETWORK]);
        if(res != QMESH_RESULT_SUCCESS)
        {
           QCLI_LOGE (mesh_group, "QmeshAddBearer failed, ret=0x%x\n", res);
           return QCLI_STATUS_ERROR_E;
        }

        /*Create Timer Group*/
        if (app_timer_ghdl == QMESH_TIMER_INVALID_GROUP_HANDLE)
        {
            app_timer_ghdl = QmeshTimerCreateGroup(QMESH_APP_NUM_SOFT_TIMERS);
            if (app_timer_ghdl == QMESH_TIMER_INVALID_GROUP_HANDLE)
            {
                QCLI_LOGE (mesh_group, "Timer Group creation failed\n");
                return QCLI_STATUS_ERROR_E;
            }
            else
                QCLI_LOGE (mesh_group, "Timer Group creation done\n");
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid */
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return ret_val;
}

QCLI_Command_Status_t CreateDeviceInstance(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QmeshResult res = QCLI_STATUS_SUCCESS_E;
    uint16_t role, type;
    uint8_t deviceInstance;

    /* Check param validity */
    if((Parameter_Count == 3) && (Parameter_List) && (Parameter_List[1].Integer_Is_Valid))
    {
        deviceInstance = Parameter_List[0].Integer_Value & 0xF;
        role = (Parameter_List[1].Integer_Value == 0)?(PROVISIONER_ROLE):(NODE_ROLE);
        type = Parameter_List[2].Integer_Value;

        /* Set Device Composition Data based on Server and Client Role */
        if(type == 0) /* Client */
        {
            dev_comp = &client_device_composition;
        }
        else /* server */
        {
            dev_comp = &server_device_composition;
        }

        app_context.role = role;
#if (QMESH_PS_IFCE_ENABLED == 1)
        if (type == 1)
        {
            NVMModelInit(&server_device_composition);
            if (NVMInit (&app_context) == QMESH_RESULT_FAILURE)
            {
                QCLI_LOGE (mesh_group, "NVM Initialization failed\n");
                return QCLI_STATUS_ERROR_E;
            }
        }
#endif
        QmeshInitHealthServerData(dev_comp);

        res = QmeshCreateDeviceInstance(deviceInstance, &device_data, dev_comp);

        if(res != QMESH_RESULT_SUCCESS)
        {
           QCLI_LOGE (mesh_group, "QmeshCreateDeviceInstance : ret=0x%x\n", res);
           return QCLI_STATUS_ERROR_E;
        }


         /* Start mesh packet scheduler */
        res = QmeshSchedStart();
        if(QMESH_RESULT_SUCCESS != res)
        {
           QCLI_LOGE (mesh_group, "QmeshSchedStart failed ret=0x%x!!\n", res);
           return QCLI_STATUS_ERROR_E;
        }

        /* For Device (Node) Role */
        if (role == NODE_ROLE)
        {
            int j = 0;

            /* Set Node Role */
            res = SetRole(deviceInstance, FALSE);

            if (res != QMESH_RESULT_SUCCESS)
            {
               QCLI_LOGE (mesh_group, "SetRole failed ret=0x%x\n", res);
            }

            /* Initialize Model Common Code */
            QmeshModelInitMsgCache();
            QmeshInitDelayCache();

            QmeshInitModelCommon(QmeshHandleModelEvents,&server_device_composition);
            /* Initialize Lighting and related server models */
            QmeshGenericOnOffServerAppInit(&g_onoff_model_context);
            QmeshGenericOnOffPublicationHandler(&server_elm_light_hsl_model_data[2 ]);

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
            /* Initialize PowerLevel models */
            QmeshGenericPowerLevelServerAppInit(&g_pwr_lvl_context);
            QmeshGenericPowerLevelPublicationHandler(&server_elm_light_hsl_model_data[11]);
#endif

            for(j = 0;j<QMESH_MAX_GENERIC_LEVEL_INSTANCES;j++)
            {
                QmeshGenericLevelServerAppInit(&g_gen_lvl_context[j]);
            }

            QmeshGenericLevelUpdatePublicationPeriod(&server_elm_light_hsl_model_data[3]);
            QmeshGenericLevelUpdatePublicationPeriod(&server_elm_light_hue_model_data[0]);
            QmeshGenericLevelUpdatePublicationPeriod(&server_elm_light_saturation_model_data[0]);

            QmeshGenericPowerOnOffInit(&g_pwronoff_context);
            QmeshGenericPowerOnOffSetupInit(&g_pwronoff_context);
            QmeshGenericPowerOnOffPublicationHandler(&server_elm_light_hsl_model_data[4]);
            QmeshLightnessServerAppInit(&g_lightness_server_context);
            QmeshLightLightnessUpdatePublicationPeriod(&server_elm_light_hsl_model_data[6]);

            /* Set the association information for Light HSL */
            g_hsl_server_context.hue_context = &g_hsl_hue_server_context;
            g_hsl_server_context.sat_context = &g_hsl_sat_server_context;
            g_hsl_hue_server_context.hsl_context = (void*)&g_hsl_server_context;
            g_hsl_sat_server_context.hsl_context = (void*)&g_hsl_server_context;

            /* Set up the HSL, Hue and Saturation Models */
            QmeshLightHSLServerAppInit(&g_hsl_server_context);
            QmeshLightHslUpdatePublicationPeriod(&server_elm_light_hsl_model_data[8]);
            QmeshLightHSLHueServerAppInit(&g_hsl_hue_server_context);
            QmeshLightHslHueUpdatePublicationPeriod(&server_elm_light_hue_model_data[1]);
            QmeshLightHSLSatServerAppInit(&g_hsl_sat_server_context);
            QmeshLightHslSatUpdatePublicationPeriod(&server_elm_light_saturation_model_data[1]);
            QmeshLightHSLSetupServerAppInit();

            /* Initialise Generic Default Transition Time Server */
            QmeshGenericDTTServerAppInit(&g_gen_dtt_context);
            QmeshGenericDTTUpdatePublicationPeriod (&server_elm_light_hsl_model_data[10]);
			QmeshUpdateModelWithUnicastAddr();

            uint8_t elm_idx;
            const QMESH_ELEMENT_CONFIG_T *const elem_config = server_device_composition.elements;

            /* Pass the onpowerup boot value only when the element has a unicast address */
            for (elm_idx = 0; elm_idx < server_device_composition.num_elements; elm_idx++)
            {
                if (elem_config[elm_idx].element_data->unicast_addr != QMESH_UNASSIGNED_ADDRESS)
                {
                     QmeshUpdateBoundStates (elem_config[elm_idx].element_data->unicast_addr,
                                             QAPP_ONPOWERUP_BOOTVALUE,
                                             &g_pwronoff_context.onpowerup);
                      break; /* For Light HSL, just pass the onpowerup only to primary element */
                }
            }

#if (QMESH_PS_IFCE_ENABLED == 1)
            /* In Node Role, Check for Network Key List present in PS */
            /* Check for Proxy feature Mask */
            if (APP_IS_PROXY_SUPPORTED())
            {
                QMESH_PS_KEY_HEADER_INFO_T  keyhdr;
                if (QmeshPsKeyGetHeaderInfo (QMESH_PS_KEY_NETWORK_KEY_LIST, &keyhdr) == QMESH_RESULT_SUCCESS)
                {
                    /* Check for number of network keys entries */
                    if (keyhdr.num_entries != 0)
                    {
                        /* Start Proxy Service */
                        StartGattProxyService();
                    }
                }
            }
#endif
        }
        else if (role == PROVISIONER_ROLE)
        {
            /* Set Provisioner Role */
            res = SetRole(deviceInstance, TRUE);
            if (res != QMESH_RESULT_SUCCESS)
            {
               QCLI_LOGE (mesh_group, "SetRole failed ret=0x%x\n", res);
               return QCLI_STATUS_ERROR_E;
            }
        }
        return QCLI_STATUS_SUCCESS_E;
    }

    QCLI_LOGE(mesh_group, "Invalid number of parameters\n");
    return QCLI_STATUS_USAGE_E;
}

void GapLeEventCallback(uint32_t BluetoothStackID, qapi_BLE_GAP_LE_Event_Data_t *GAP_LE_Event_Data, uint32_t CallbackParameter)
{

    if(GAP_LE_Event_Data)
    {
        switch(GAP_LE_Event_Data->Event_Data_Type)
        {
            case QAPI_BLE_ET_LE_CONNECTION_COMPLETE_E:
            {
                qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t *data =
                    GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Complete_Event_Data;

                QCLI_LOGI (mesh_group, "APP :: Connected status=%x\n",data->Status);

                if(!data->Status)
                {
                    QCLI_LOGI (mesh_group, "APP :: AddressType=%x, Address=%x:%x:%x:%x:%x:%x\n",
                        data->Peer_Address_Type, data->Peer_Address.BD_ADDR0, data->Peer_Address.BD_ADDR1,
                        data->Peer_Address.BD_ADDR2,data->Peer_Address.BD_ADDR3,data->Peer_Address.BD_ADDR4,
                        data->Peer_Address.BD_ADDR5);
                }
                else
                {
                    QCLI_LOGE (mesh_group, "APP :: Connection NOT successful\n");
                }
            }
            break;

            case QAPI_BLE_ET_LE_DISCONNECTION_COMPLETE_E:
            {
                qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t *data =
                    GAP_LE_Event_Data->Event_Data.GAP_LE_Disconnection_Complete_Event_Data;

                QCLI_LOGI (mesh_group, "APP :: DisConnection status=%x, reason=%x\n",data->Status, data->Reason);
                if(!data->Status)
                {
                    QCLI_LOGI (mesh_group, "APP :: AddressType=%x, Address=%x:%x:%x:%x:%x:%x\n",
                        data->Peer_Address_Type, data->Peer_Address.BD_ADDR0, data->Peer_Address.BD_ADDR1,
                        data->Peer_Address.BD_ADDR2,data->Peer_Address.BD_ADDR3,data->Peer_Address.BD_ADDR4,
                        data->Peer_Address.BD_ADDR5);

                    HandleGattDisconnection(&data->Peer_Address);
                }
                else
                {
                    QCLI_LOGE (mesh_group, "Failed to disconnect\n");
                }
            }
            break;

            case QAPI_BLE_ET_LE_ADVERTISING_REPORT_E:
            {
                qapi_BLE_GAP_LE_Advertising_Report_Data_t *adv_report;
                qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t *data =
                    GAP_LE_Event_Data->Event_Data.GAP_LE_Advertising_Report_Event_Data;

                adv_report = &(data->Advertising_Data[0]);

                if((adv_report->Advertising_Report_Type == QAPI_BLE_RT_CONNECTABLE_UNDIRECTED_E) &&
                    (adv_report->Address_Type == QAPI_BLE_LAT_PUBLIC_E) &&
                    (adv_report->Advertising_Data.Number_Data_Entries == 3))
                {
                    if(provisioner_role_enabled)
                    {
                        if(app_context.scan_over_pb_gatt)
                        {
                            if (app_context.scan_for_prov_srvc)
                            {
                                /* Parse Advert Report for Provisioning Service */
                                _parseConnAdvtFrmUnprovDev(adv_report);
                            }
                            else
                            {
                                /* Parse Advert Report for Proxy Service */
                                _parseConnAdvtForProxySrvc(adv_report);
                            }
                        }
                        else
                        {
                            QCLI_Printf(mesh_group, "APP :: Not scanning for GATT connection\n");
                        }
                    }
                }
                /*TODO: if it is scan-response then find if the device is present in the table and add the details*/
            }
            break;

            default:
            {
                QCLI_LOGE (mesh_group, "APP :: Received LE event %x not handled\n", GAP_LE_Event_Data->Event_Data_Type);
            }
            break;
        }
    }
}


void GattConnEventCallback(uint32_t BluetoothStackID, qapi_BLE_GATT_Connection_Event_Data_t *GATT_Conn_Event_Data, uint32_t CallbackParameter)
{
    uint8_t  index;
    QMESHAPP_DEVICE_INFO_T *devInfo = NULL;
    QCLI_LOGI (mesh_group, "APP :: GattConnEventCallback\n");
    if(GATT_Conn_Event_Data)
    {
        switch(GATT_Conn_Event_Data->Event_Data_Type)
        {
            case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_E:
            {
                QCLI_LOGI (mesh_group, "APP :: Received Device Connected Event %x\n", GATT_Conn_Event_Data->Event_Data_Type);
                if (GATT_Conn_Event_Data->Event_Data.GATT_Device_Connection_Data)
                {
                    qapi_BLE_GATT_Device_Connection_Data_t *connData =
                            GATT_Conn_Event_Data->Event_Data.GATT_Device_Connection_Data;
                    QCLI_LOGI  (mesh_group, "APP :DEVICE CONNECTED Conn Id %x, Type %x, MTU %x\n",connData->ConnectionID, connData->ConnectionType, connData->MTU);
                    QCLI_LOGI  (mesh_group, "APP :Remote BDADDR =%x:%x:%x:%x:%x:%x\n",
                        connData->RemoteDevice.BD_ADDR0, connData->RemoteDevice.BD_ADDR1, connData->RemoteDevice.BD_ADDR2,
                        connData->RemoteDevice.BD_ADDR3,connData->RemoteDevice.BD_ADDR4, connData->RemoteDevice.BD_ADDR5);

                    if(provisioner_role_enabled)
                    {
                        /* Handling for GATT Client */
                        QCLI_LOGI (mesh_group, "APP :: Storing connected device address for Provisioning-Client\n");
                        _updateConnectionData(connData->ConnectionID, connData->MTU, &connData->RemoteDevice, TRUE);
                    }
                    else
                    {
                        /* Store Device Connection Info */
                        QCLI_LOGI (mesh_group, "APP :: Storing connected device address for bearer-server role\n");
                        _updateConnectionData(connData->ConnectionID, connData->MTU, &connData->RemoteDevice, TRUE);
                    }
                }
            }
            break;

            case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_DISCONNECTION_E:
            {
                if (GATT_Conn_Event_Data->Event_Data.GATT_Device_Disconnection_Data)
                {
                    qapi_BLE_GATT_Device_Disconnection_Data_t *discData =
                            GATT_Conn_Event_Data->Event_Data.GATT_Device_Disconnection_Data;
                    QCLI_LOGI  (mesh_group, "APP : DEVICE DISCONNECTED Conn Id %x, Type %x\n",discData->ConnectionID, discData->ConnectionType);
                    QCLI_LOGI  (mesh_group, "APP : Remote BDADDR =%x:%x:%x:%x:%x:%x\n",
                        discData->RemoteDevice.BD_ADDR0, discData->RemoteDevice.BD_ADDR1, discData->RemoteDevice.BD_ADDR2,
                        discData->RemoteDevice.BD_ADDR3,discData->RemoteDevice.BD_ADDR4, discData->RemoteDevice.BD_ADDR5);
                    /* Remove Device Connection Info */
                    HandleGattDisconnection(&discData->RemoteDevice);
                }
            }
            break;

            case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_MTU_UPDATE_E:
            {
                /* Update of Connection MTU */
                if (GATT_Conn_Event_Data->Event_Data.GATT_Device_Connection_MTU_Update_Data)
                {
                    qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *mtuUpdate =
                            GATT_Conn_Event_Data->Event_Data.GATT_Device_Connection_MTU_Update_Data;
                    QCLI_LOGI  (mesh_group, "APP : MTU Update 0x%x\n", mtuUpdate->MTU);
                    /*Update the MTU information for both Client/server roles*/
                    _updateConnectionData(mtuUpdate->ConnectionID, mtuUpdate->MTU, &mtuUpdate->RemoteDevice, TRUE);
                }
            }
            break;

            case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_CCCD_UPDATE_E:
            {
                /* CCD Write Request Event  */
                if (GATT_Conn_Event_Data->Event_Data.GATT_Connection_Service_Changed_CCCD_Update_Data)
                {
                    qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t *cccdUpdate =
                            GATT_Conn_Event_Data->Event_Data.GATT_Connection_Service_Changed_CCCD_Update_Data;
                    QCLI_LOGI  (mesh_group, "APP : ConnId %x, CCCD Update Value %x\n", cccdUpdate->ConnectionID, cccdUpdate->ClientConfigurationValue);
                    QCLI_LOGI  (mesh_group, "APP : Remote BDADDR =%02x:%02x:%02x:%02x:%02x:%02x\n",
                        cccdUpdate->RemoteDevice.BD_ADDR0, cccdUpdate->RemoteDevice.BD_ADDR1, cccdUpdate->RemoteDevice.BD_ADDR2,
                        cccdUpdate->RemoteDevice.BD_ADDR3,cccdUpdate->RemoteDevice.BD_ADDR4, cccdUpdate->RemoteDevice.BD_ADDR5);

                    devInfo = _getDeviceInfo(cccdUpdate->ConnectionID);

                    /*Add the bearer handle for the Node/server role upon enabling of notification*/
                    if(!provisioner_role_enabled)
                    {
                        if((NULL != devInfo) && app_context.prov_via_gatt)
                        {
                            QCLI_LOGI(mesh_group, "APP :: Enabling GATT bearer for Provisioning server, MTU=%d\n", devInfo->mtu);
                            app_context.prov_conn_id = cccdUpdate->ConnectionID;
                            AddGATTBearer(QMESH_GATT_BEARER_PROV_SVC_E, devInfo);
                        }
                        else if (NULL != devInfo)
                        {
                            /* NOTE:- Sometimes, Connection Complete event is not received in GapLeEventCallback() function.
                             *        So, Disabling GATT PROXY User Advert over here also.
                             */
                            QCLI_LOGI(mesh_group, "APP :: Enabling GATT bearer for Proxy server, MTU=%d\n", devInfo->mtu);
                            AddGATTBearer(QMESH_GATT_BEARER_PROXY_SVC_E, devInfo);
                        }
                        else
                        {
                            QCLI_LOGI(mesh_group, "APP :: devInfo is NULL, GATT bearer not enabled\n");
                        }
                    }
                    
                    if(NULL != devInfo)
                    {
                        QCLI_LOGI(mesh_group, "APP :: Set GATT Server Bearer Handle\n");
                        if (!setGATTConnBearerHandle(devInfo->connection_id, devInfo->bearer_handle))
                        {
                            QCLI_LOGE(mesh_group, "Failed to set GATT Server bearer handle\n");
                        }
                    }

                    
                    /* Send Secure Network Beacon */
                    if (cccdUpdate->ClientConfigurationValue )
                    {
#if (APP_PTS_MODE == 1)
                        /* For PTS GATT PROXY Test Cases , we need tod disable Secure Network Beacon
                         * As PTS test cases does not ignore these packets
                         * PTS test cases requires only one Secure Network Beacon just after enabling notification.
                         */
                        for (index = 0; index < MAX_NUM_OF_SUB_NETWORK; index++)
                        {
                            QMESH_SUBNET_KEY_IDX_INFO_T key_info = app_context.subnet_data[index].netkey_idx;
                            if (app_context.subnet_data[index].valid)
                            {
                                QmeshSetSecureNetworkBeaconState(key_info.prov_net_idx, QMESH_SECURE_NW_BEACON_STATE_START);
                                QmeshSendSecureNetworkBeacon(key_info);
                                QmeshSetSecureNetworkBeaconState(key_info.prov_net_idx, QMESH_SECURE_NW_BEACON_STATE_STOP);
                            }
                        }
#else
                        /* On connection, Proxy Server needs to send Secure Network Beacons for all known subnets. */
                        for (index = 0; index < MAX_NUM_OF_SUB_NETWORK; index++)
                        {
                            if (app_context.subnet_data[index].valid)
                            {
                                QmeshSendSecureNetworkBeacon(app_context.subnet_data[index].netkey_idx);
                            }
                        }
#endif
                    }
                }
            }
            break;

            default:
            {
                QCLI_LOGI(mesh_group, "APP :: Received GATT event %x not handled\n", GATT_Conn_Event_Data->Event_Data_Type);
            }
            break;
        }
    }
}

static bool AddGATTBearer(QMESH_GATT_BEARER_SERVICE_TYPE_T srvcType,
                          QMESHAPP_DEVICE_INFO_T* devInfo)
{
    QmeshResult res;
    bool retVal = FALSE;
    QMESH_BEARER_TYPE_T bearer_type;
    QMESH_GATT_BEARER_CNTXT_INFO_T *contxt;

    if(provisioner_role_enabled)
    {
        bearer_type = (QMESH_GATT_BEARER_PROV_SVC_E == srvcType) ? (QMESH_BEARER_TYPE_PROXY_PROVISIONING_CLIENT) :
                                                                   (QMESH_BEARER_TYPE_PROXY_NETWORK_CLIENT);
    }
    else
    {
        bearer_type = (QMESH_GATT_BEARER_PROV_SVC_E == srvcType) ? (QMESH_BEARER_TYPE_PROXY_PROVISIONING_SERVER) :
                                                                   (QMESH_BEARER_TYPE_PROXY_NETWORK_SERVER);
    }

    QCLI_LOGI(mesh_group, "AddGATTBearer : srvcType 0x%x, connectionId 0x%x, MTU 0x%x, bearer_type 0x%x\n",
                                                srvcType, devInfo->connection_id, devInfo->mtu, bearer_type);
    /*Enable GATT bearer with mesh stack*/
    contxt = SetGATTBearerHandler(srvcType, devInfo->connection_id);

    /* If using GATT for provisioning disable provisioning on Advert Bearer. */
    if (QMESH_GATT_BEARER_PROV_SVC_E == srvcType)
    {
        res = QmeshDeleteBearer(app_context.adv_bearer_handle[QMESH_BEARER_TYPE_LE_ADV_PROVISIONING]);
        app_context.adv_bearer_handle[QMESH_BEARER_TYPE_LE_ADV_PROVISIONING] = QMESH_BEARER_HANDLE_INVALID;
        QCLI_LOGI(mesh_group, "Disabling PB-ADV Bearer : %d\n", res);
    }

    /* Initialize Bearer Handle */
    res = QmeshAddBearer(bearer_type, contxt, (devInfo->mtu - ATT_HDR_OCTETS),
                         &devInfo->bearer_handle);
    if (QMESH_RESULT_SUCCESS == res)
    {
        QCLI_LOGI(mesh_group, "GATT bearer enabled with mesh stack\n");
        retVal = TRUE;
    }
    else
    {
        QCLI_LOGE(mesh_group, "Failed to enable the GATT bearer with mesh-stack=%x\n",res);
    }

    return retVal;
}

static void DeleteGATTBearer(qapi_BLE_BD_ADDR_t *dev_addr)
{
    QMESHAPP_DEVICE_INFO_T *devInfo = NULL;
    QmeshResult res=QMESH_RESULT_FAILURE;

    devInfo = _getDeviceInfoForDevAddr(dev_addr);
    QCLI_LOGI(mesh_group, "DeleteGATTBearer\n");
    if (NULL != devInfo)
    {
        /*Inform bearer of disconnection of provisioning service*/
        res = QmeshDeleteBearer(devInfo->bearer_handle);
        if(QMESH_RESULT_SUCCESS != res)
        {
            QCLI_LOGE(mesh_group, "Failed to disable the GATT bearer with mesh-stack=%x\n",res);
        }
        else
        {
            devInfo->bearer_handle = QMESH_BEARER_HANDLE_INVALID;
        }
    }
}

QCLI_Command_Status_t AddDelAdvBearers(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QMESH_RESULT_T res;
    QMESH_BEARER_TYPE_T bt;
    bool add = FALSE;

    if (1 != Parameter_Count)
    {
        QCLI_LOGE(mesh_group, "Invalid Parameters in AddDelAdvBearers");
        return QCLI_STATUS_USAGE_E;
    }

    add = (Parameter_List[0].Integer_Value) ? (TRUE) : (FALSE);

    for (bt = QMESH_BEARER_TYPE_LE_ADV_PROVISIONING; bt <= QMESH_BEARER_TYPE_LE_ADV_NETWORK; bt++)
    {
        if (add)
        {
            if (QMESH_BEARER_HANDLE_INVALID == app_context.adv_bearer_handle[bt])
            {
                res = QmeshAddBearer(bt, NULL, 0, &app_context.adv_bearer_handle[bt]);
                if (QMESH_RESULT_SUCCESS != res)
                {
                    QCLI_LOGE(mesh_group, "Failed to Add the ADV bearer : %d : Status: %x\n", bt, res);
                    return QCLI_STATUS_ERROR_E;
                }
            }
        }
        else
        {
            res = QmeshDeleteBearer(app_context.adv_bearer_handle[bt]);
            if (QMESH_RESULT_SUCCESS != res)
            {
                QCLI_LOGE(mesh_group, "Failed to disable the ADV bearer : %d : Status: %x\n", bt, res);
                return QCLI_STATUS_ERROR_E;
            }
            else
            {
                app_context.adv_bearer_handle[bt] = QMESH_BEARER_HANDLE_INVALID;
            }
        }
    }
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t AdvtForProvOverGatt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    qapi_BLE_Advertising_Data_t advert_data;
    uint32_t length=29;
    QCLI_Command_Status_t ret_val = QCLI_STATUS_ERROR_E;
    uint8_t uuid_array[] = MESH_PROVISIONING_SERVICE_UUID;

    if(provisioner_role_enabled)
    {
        QCLI_LOGE(mesh_group, "Functionality only accessible only in non-provisioner role\n");
        return ret_val;
    }
    else
    {
        /* Initialize Gatt Bearer */
        Qmesh_Gatt_Data_T  gattData;
        gattData.appGapLeEvtCallback = GapLeEventCallback;
        gattData.appGattEvtCallback = GattConnEventCallback;
        gattData.provisioner_role_enabled = provisioner_role_enabled;
        if(!InitGattBearer(QMESH_GATT_BEARER_PROV_SVC_E, NULL, &gattData))
        {
            QCLI_LOGE(mesh_group, "FAILED to registered App Callbacks\n");
            return ret_val;
        }
        /* Unregister Proxy Service */
        if(!UnregisterGattBearerService(QMESH_GATT_BEARER_PROXY_SVC_E))
        {
            QCLI_LOGE(mesh_group, "Not able to unregister proxy service\n");
        }

        /*Register the Provisioning service*/
        if (RegisterGattBearerService(QMESH_GATT_BEARER_PROV_SVC_E))
        {
            QCLI_LOGI(mesh_group, "Provisioning Service registered successfully\n");
        }
        else
        {
            QCLI_LOGE(mesh_group, "FAILED to Initialize GATT\n");
            return ret_val;
        }
    }

    /*Set the advertisement data*/
    QCLI_LOGI(mesh_group, "AdvtForProvOverGatt\n");

    advert_data.Advertising_Data[0] = 0x02;             /*Length of the AD structure*/
    advert_data.Advertising_Data[1] = 0x01;             /*AD_Type ==> */
    advert_data.Advertising_Data[2] = 0x06;             /*Flags*/
    advert_data.Advertising_Data[3] = 0x03;             /*Length of the AD structure*/
    advert_data.Advertising_Data[4] = 0x03;             /*AD_Type ==> Complete List of 16-bit Service Class UUIDs*/
    advert_data.Advertising_Data[5] = uuid_array[0];     /*Provisioning SERVICE UUID0*/
    advert_data.Advertising_Data[6] = uuid_array[1];     /*Provisioning SERVICE UUID1*/
    advert_data.Advertising_Data[7] = 0x15;             /*Length of the AD structure*/
    advert_data.Advertising_Data[8] = 0x16;             /*AD_Type ==>  «Service Data» */
    advert_data.Advertising_Data[9] = uuid_array[0];     /*Provisioning SERVICE UUID byte0*/
    advert_data.Advertising_Data[10] = uuid_array[1];     /*Provisioning SERVICE UUID byte1*/

    /* Copy Device UUID 16 Octets */
    QmeshMemCopyUnPackBE(advert_data.Advertising_Data+11, device_data.uuid, QMESH_UUID_SIZE_OCTETS);
    /* Copy OOB Data 2 Octets */
    advert_data.Advertising_Data[27] = 0; /*OOB Data byte0*/
    advert_data.Advertising_Data[28] = 0; /*OOB Data byte1*/

    QCLI_LOGI(mesh_group, "%s %d Disabling user adverts\n", __FUNCTION__, __LINE__);

    QmeshSchedUserAdvertDisable();

    QCLI_LOGI(mesh_group, "%s %d User adverts disabled\n", __FUNCTION__, __LINE__);

    if(QmeshSchedSetUserAdvertData(length, &advert_data) == QMESH_RESULT_SUCCESS)
    {
        /*Enable advertisement*/
        qapi_BLE_GAP_LE_Advertising_Parameters_t gap_le_advt_params;
        qapi_BLE_GAP_LE_Connectability_Parameters_t gap_le_conn_params;

        gap_le_advt_params.Scan_Request_Filter=QAPI_BLE_FP_NO_FILTER_E;
        gap_le_advt_params.Connect_Request_Filter=QAPI_BLE_FP_NO_FILTER_E;
        gap_le_advt_params.Advertising_Channel_Map=0x07;/*All 3 channels*/
        gap_le_advt_params.Advertising_Interval_Min=100;
        gap_le_advt_params.Advertising_Interval_Max=100;

        gap_le_conn_params.Connectability_Mode=QAPI_BLE_LCM_CONNECTABLE_E;
        gap_le_conn_params.Own_Address_Type=QAPI_BLE_LAT_PUBLIC_E;
        gap_le_conn_params.Direct_Address_Type=QAPI_BLE_LAT_PUBLIC_E;

        app_context.prov_via_gatt = TRUE;
        app_context.device_provisioned = FALSE;
        //QmeshMemSet(&app_context.remote_addr, 0, sizeof(qapi_BLE_BD_ADDR_t));

        if(QmeshSchedUserAdvertEnable(FALSE, &gap_le_advt_params,
            &gap_le_conn_params, GapLeEventCallback, 0) ==  QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGI(mesh_group, "Advertising for Provisioning over GATT\n", __FUNCTION__, __LINE__);
            /* Flag success to the caller.                                 */
            ret_val = QCLI_STATUS_SUCCESS_E;
        }
        else
        {
            QCLI_LOGI(mesh_group, "FAILED to enable advertisement\n");
        }

    }
    else
    {
        QCLI_LOGE(mesh_group, "FAILED to set the advertisement data\n");
    }

    return ret_val;
}

/*   This function handles the GATT Proxy Connectable Advert Timeout Handler. */
void GattProxyConnAdvrtTimeoutHandler(QMESH_TIMER_HANDLE_T tmr_id, void * data)
{
    QMESH_RESULT_T result = QMESH_RESULT_FAILURE;
    uint8_t  srvc_data[17] = {0};
    uint32_t advrt_data_length = 0;
    uint8_t idx, loop_cnt;
    uint32_t rand_num[2] = {0};

    if(tmr_id != gatt_proxy_conn_advrt_info.proxy_advrt_tmr_hndl)
    {
        QCLI_LOGE (mesh_group, "\n ******* TmrId (%d) != Proxy Advert Tmr (%d)\n", tmr_id, gatt_proxy_conn_advrt_info.proxy_advrt_tmr_hndl);
        return;
    }

    /* Delete the Current Timer */
    gatt_proxy_conn_advrt_info.proxy_advrt_tmr_hndl = QMESH_TIMER_INVALID_HANDLE;

    /* Disable User Advert Data Advertisement */
    QmeshSchedUserAdvertDisable();

    if (gatt_proxy_conn_advrt_info.is_proxy_advrt_enabled)
    {
        for (loop_cnt = 0; loop_cnt < MAX_NUM_OF_SUB_NETWORK; loop_cnt++)
        {
            idx = (gatt_proxy_conn_advrt_info.net_idx + loop_cnt)%MAX_NUM_OF_SUB_NETWORK;
            if (app_context.subnet_data[idx].valid)
            {
                if (app_context.subnet_data[idx].node_identity == QMESH_NODE_IDENTITY_RUNNING)
                {
                    app_context.subnet_data[idx].node_identity_cnt += (GATT_PROXY_SRVC_CONN_ADVERT_INTERVAL_IN_MS/1000);

                    if (app_context.subnet_data[idx].node_identity_cnt >= (GATT_PROXY_SRVC_CONN_ADVERT_DURATION/1000))
                    {
                        app_context.subnet_data[idx].node_identity     = QMESH_NODE_IDENTITY_STOPPED;
                        app_context.subnet_data[idx].node_identity_cnt = 0;
                    }
                }

                if (app_context.subnet_data[idx].node_identity == QMESH_NODE_IDENTITY_RUNNING)
                {
                    rand_num[0] = QmeshRandom32();
                    rand_num[1] = QmeshRandom32();

                    result = QmeshGetProxyAdvServiceData(app_context.subnet_data[idx].netkey_idx,
                                                         QMESH_PROXY_ADV_ID_TYPE_NODE,
                                                         gatt_proxy_conn_advrt_info.elm_addr,
                                                         (uint8_t *)rand_num,
                                                         srvc_data);
                    if (result == QMESH_RESULT_SUCCESS)
                    {
                        gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[11] = 0x01; /*Identification Type(Node Identity Type)*/

                        QmeshMemCpy(gatt_proxy_conn_advrt_info.advert_data.Advertising_Data+12, srvc_data+1, 16);  /*HASH (8 octet) + Random (8 octet)*/
                        QCLI_LOGI (mesh_group, "SubNetId %x Type %x HASH : [%x %x %x %x %x %x %x %x]\n", idx, srvc_data[0],
                                                            srvc_data[1], srvc_data[2], srvc_data[3], srvc_data[4],
                                                            srvc_data[5], srvc_data[6], srvc_data[7], srvc_data[8]);
                        /* Change Advert Type */
                        gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[7]=0x14;
                        advrt_data_length = 28;
                    }
                    else
                    {
                        QCLI_LOGE(mesh_group, "QmeshGetProxyAdvServiceData : %d\n", result);
                    }
                }
                else
                {
                    result = QmeshGetProxyAdvServiceData(app_context.subnet_data[idx].netkey_idx,
                                                         QMESH_PROXY_ADV_ID_TYPE_NETWORK,
                                                         gatt_proxy_conn_advrt_info.elm_addr,
                                                         NULL, srvc_data);
                    if (result == QMESH_RESULT_SUCCESS)
                    {
                        gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[11] = 0x00; /*Identification Type(Network ID Type)*/

                        /* Copy Network ID 8 Octets */
                        QmeshMemCpy(gatt_proxy_conn_advrt_info.advert_data.Advertising_Data+12, srvc_data+1, 8); /*Network ID (8 Octet)*/
                        QCLI_LOGI (mesh_group, "SubNetId %x Type %x Network ID : [%x %x %x %x %x %x %x %x]\n",
                                               idx, srvc_data[0],
                                                srvc_data[1], srvc_data[2], srvc_data[3], srvc_data[4],
                                               srvc_data[5], srvc_data[6], srvc_data[7], srvc_data[8]);

                        gatt_proxy_conn_advrt_info.advert_data.Advertising_Data[7]=0x0C;
                        advrt_data_length = 20;
                    }
                    else
                    {
                        QCLI_LOGE(mesh_group, "QmeshGetProxyAdvServiceData : %d\n", result);
                    }
                }
                gatt_proxy_conn_advrt_info.net_idx = (idx + 1)%MAX_NUM_OF_SUB_NETWORK;
                break;
            }
        }

        if (0 != advrt_data_length)
        {
            if (QmeshSchedSetUserAdvertData(advrt_data_length, &gatt_proxy_conn_advrt_info.advert_data) == QMESH_RESULT_SUCCESS)
            {
                /* Enable Advertisement */
                if (QmeshSchedUserAdvertEnable(FALSE, &gatt_proxy_conn_advrt_info.advert_params,
                                 &gatt_proxy_conn_advrt_info.conn_params, GapLeEventCallback, 0) != QMESH_RESULT_SUCCESS)
                {
                    QCLI_LOGE (mesh_group, "FAILED to enable advertisement\n");
                }
            }
            else
            {
                QCLI_LOGE (mesh_group, "FAILED to set User Advert Data\n");
            }

            if (result == QMESH_RESULT_SUCCESS)
            {
                 CreateGattProxyAdvertTimer();
            }
        }
    }
}


static QmeshResult CreateGattProxyAdvertTimer(void)
{
    /* Start a timer to send Device ID messages periodically to get
     * associated to a network
     */
    gatt_proxy_conn_advrt_info.proxy_advrt_tmr_hndl = QmeshTimerCreate(&app_timer_ghdl,
                                             GattProxyConnAdvrtTimeoutHandler,
                                             NULL,
                                             GATT_PROXY_SRVC_CONN_ADVERT_INTERVAL_IN_MS);
    if(gatt_proxy_conn_advrt_info.proxy_advrt_tmr_hndl == QMESH_TIMER_INVALID_HANDLE)
    {
        QCLI_LOGE (mesh_group, "CreateGattProxyAdvertTimer: Failed to create advert timer!!");
        return QMESH_RESULT_FAILURE;
    }

    return QMESH_RESULT_SUCCESS;
}

static QmeshResult StartGattProxyService(void)
{
    QmeshResult result = QMESH_RESULT_FAILURE;
    Qmesh_Gatt_Data_T  gattData;
    QCLI_LOGI (mesh_group, "%s\n",__FUNCTION__);
    /* Enable GATT Proxy Service*/

    gattData.appGapLeEvtCallback = GapLeEventCallback;
    gattData.appGattEvtCallback = GattConnEventCallback;
    gattData.provisioner_role_enabled = provisioner_role_enabled;

    if(!InitGattBearer(QMESH_GATT_BEARER_PROXY_SVC_E, NULL, &gattData))
    {
        QCLI_LOGE (mesh_group, "FAILED to initialize GATT Bearer\n");
        return result;
    }
    QCLI_LOGI (mesh_group, "Gatt Bearer Initialized\n");

    /* Unregister Provisioning Service */
    UnregisterGattBearerService(QMESH_GATT_BEARER_PROV_SVC_E);

    /* Register Proxy Service */
    if(RegisterGattBearerService(QMESH_GATT_BEARER_PROXY_SVC_E))
    {
        QCLI_LOGI (mesh_group, "Successfully registered GATT Proxy service\n");
        /* Set Flags */
        app_context.prov_via_gatt = FALSE;
        app_context.is_proxy_srvc_enabled = TRUE;
        /* Set GATT Proxy Connectable Advert */
        SetGattProxyConnAdvertParams();
        gatt_proxy_conn_advrt_info.is_proxy_advrt_enabled = TRUE;
        /* Primary Element Address */
        gatt_proxy_conn_advrt_info.elm_addr = dev_comp->elements[0].element_data->unicast_addr;
        /* Handle GATT Proxy Connectable Advert in Timeout Handler */
        result = CreateGattProxyAdvertTimer();
        if (QMESH_RESULT_SUCCESS == result)
        {
            QCLI_LOGI (mesh_group, "%s : Proxy Advert timer created %d\n", __FUNCTION__, gatt_proxy_conn_advrt_info.proxy_advrt_tmr_hndl);
            gatt_proxy_conn_advrt_info.proxy_state = QMESH_GATT_PROXY_STATE_RUNNING;
        }
        else
        {
            /* Reset Proxy Connection Advert In Progress flag */
            gatt_proxy_conn_advrt_info.proxy_state = QMESH_GATT_PROXY_STATE_DISABLED;
            QCLI_LOGE (mesh_group, "%s : Failed to create Proxy Advert timer!!\n", __FUNCTION__);
        }
    }
    return result;
}

static QmeshResult StopGattProxyService(void)
{
    QmeshResult result = QMESH_RESULT_FAILURE;
    QCLI_LOGI (mesh_group, "%s\n",__FUNCTION__);

    /* Disconnect All GATT Connections */
    if (IsGATTProxyConnected())
    {
        uint8_t idx;
        for (idx = 0; idx < MAX_NUM_OF_GATT_CONN; idx++)
        {
            if (app_context.deviceInfo[idx].connection_id > 0)
            {
                InitiateGattDisconnection(app_context.deviceInfo[idx].remote_addr);
                QCLI_LOGI(mesh_group, "APP : Remote BDADDR =%02x:%02x:%02x:%02x:%02x:%02x\n",
                            app_context.deviceInfo[idx].remote_addr.BD_ADDR0,
                            app_context.deviceInfo[idx].remote_addr.BD_ADDR1,
                            app_context.deviceInfo[idx].remote_addr.BD_ADDR2,
                            app_context.deviceInfo[idx].remote_addr.BD_ADDR3,
                            app_context.deviceInfo[idx].remote_addr.BD_ADDR4,
                            app_context.deviceInfo[idx].remote_addr.BD_ADDR5);
            }
        }
    }

    gatt_proxy_conn_advrt_info.proxy_state = QMESH_GATT_PROXY_STATE_DISABLED;
    gatt_proxy_conn_advrt_info.is_proxy_advrt_enabled = FALSE;

    return result;
}


QCLI_Command_Status_t EnableGattProxyService(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_ERROR_E;

    if(provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Node Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    QCLI_LOGI (mesh_group, "EnableGattProxyService\n");

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 1) && (Parameter_List))
    {
        /* Check for Enable/Disable*/
        if (Parameter_List[0].Integer_Value)
        {
            StartGattProxyService();
        }
        else
        {
            StopGattProxyService();
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "EnableGattProxyService invalid input params\n");
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}


QCLI_Command_Status_t SendUnProvDeviceBeacon(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val;
    QMESH_RESULT_T        res;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    if (0 == Parameter_Count)
    {
        if (app_state_provision_started == prov_state)
        {
            QCLI_LOGE(mesh_group, "Provisioing already in progress. Can't Start new one.");
            return QCLI_STATUS_ERROR_E;
        }

        if (app_context.device_provisioned == TRUE)
        {
            QCLI_LOGE(mesh_group, "Device already provisioned.");
            return QCLI_STATUS_ERROR_E;
        }

        /* Enable LE Advertisement Provisioning Bearer for provisioning. */
        if (QMESH_BEARER_HANDLE_INVALID == app_context.adv_bearer_handle[QMESH_BEARER_TYPE_LE_ADV_PROVISIONING])
        {
            res = QmeshAddBearer(QMESH_BEARER_TYPE_LE_ADV_PROVISIONING, NULL, 0,
                                 &app_context.adv_bearer_handle[QMESH_BEARER_TYPE_LE_ADV_PROVISIONING]);
            if (QMESH_RESULT_SUCCESS != res)
            {
                QCLI_LOGE(mesh_group, "Error: Failed to enable to PB-ADV");
                return QCLI_STATUS_ERROR_E;
            }
        }

        /* Send UPD Beacons. */
        CreateBeaconHandlerTimer();

        /* Flag success to the caller. */
        ret_val = QCLI_STATUS_SUCCESS_E;
    }
    else
    {
        QCLI_LOGE (mesh_group, "%s:%d: Invalid inputs\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

QCLI_Command_Status_t SetOobData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.
    */
    if((Parameter_Count == 1) && (Parameter_List))
    {
        uint8_t val, index = 0;
        uint8_t auth[16]={0};

        /* 16 octets Auth data */
        val = AsciiToHex((char *)Parameter_List[0].String_Value, strlen((char *) Parameter_List[0].String_Value), auth);

        if(val == 0xff)
            return QCLI_STATUS_USAGE_E;

        QCLI_LOGI (mesh_group, "Set OOB Data -- Auth value entered is \n");
        for(;index < 16; index++)
        {
            QCLI_LOGI (mesh_group, " %x \n", auth[index]);
        }
        QCLI_LOGI (mesh_group, "\n");

        res = QmeshSetOobData(auth);

        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "Set OOB Data failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }
    return(ret_val);
}

static bool _checkIfDevPresent(qapi_BLE_BD_ADDR_t *addr)
{
    uint8_t indx=0;
    bool result=FALSE;

    while(indx < gatt_unprov_dev_index)
    {
        if(memcmp(&gatt_unprov_dev_found_list[indx].device_addr, addr, sizeof(qapi_BLE_BD_ADDR_t)) == 0)
        {
            QCLI_LOGI(mesh_group, "APP : Device found in the list\n");
            result = TRUE;
            break;
        }
        indx++;
    }
    return result;
}


static void _addToUnProvDevList(qapi_BLE_GAP_LE_Advertising_Report_Data_t *adv_report, uint8_t indx)
{
    uint8_t *uuid_ptr;

    QCLI_LOGI (mesh_group, " DeviceIndex %d Address 0x%2x%2x%2x%2x%2x%2x\n",gatt_unprov_dev_index,
        adv_report->BD_ADDR.BD_ADDR5,adv_report->BD_ADDR.BD_ADDR4,
        adv_report->BD_ADDR.BD_ADDR3,adv_report->BD_ADDR.BD_ADDR2,
        adv_report->BD_ADDR.BD_ADDR1,adv_report->BD_ADDR.BD_ADDR0);

    memcpy(&gatt_unprov_dev_found_list[gatt_unprov_dev_index].device_addr,
        &adv_report->BD_ADDR, sizeof(qapi_BLE_BD_ADDR_t));

    uuid_ptr  = &adv_report->Advertising_Data.Data_Entries[indx].AD_Data_Buffer[3];
    QCLI_LOGI (mesh_group, "UUID %x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x\n",
        uuid_ptr[0],uuid_ptr[1],uuid_ptr[2], uuid_ptr[3], uuid_ptr[4],
        uuid_ptr[5],uuid_ptr[6],uuid_ptr[7], uuid_ptr[8], uuid_ptr[9],
        uuid_ptr[10],uuid_ptr[11],uuid_ptr[12], uuid_ptr[13], uuid_ptr[14],uuid_ptr[15]);

    memcpy(gatt_unprov_dev_found_list[gatt_unprov_dev_index].uuid,
        &adv_report->Advertising_Data.Data_Entries[indx+1].AD_Data_Buffer[2],
        QMESH_UUID_SIZE_OCTETS);

    gatt_unprov_dev_index++;
}

static bool _checkIfProxyDevPresent(QMESHAPP_PROXY_DEVICE_FOUND_LIST_T *dev_info)
{
    uint8_t indx = 0;
    bool result=FALSE;

    while(indx < gatt_proxy_dev_index)
    {
        /* Check for coomplete structure i.e. Device Address, Advert Type
           and Network ID or Node Identity */
        if(memcmp(&gatt_proxy_dev_found_list[indx], dev_info, sizeof(QMESHAPP_PROXY_DEVICE_FOUND_LIST_T)) == 0)
        {
            QCLI_LOGI(mesh_group, "APP : Proxy Device present in the list at index=%d\n", indx);
            result = TRUE;
            break;
        }
        indx++;
    }

    return result;
}

static void _addToProxyDevList(QMESHAPP_PROXY_DEVICE_FOUND_LIST_T *dev_info)
{
    uint8_t index;
    /* Store Proxy Device info */
    memcpy(&gatt_proxy_dev_found_list[gatt_proxy_dev_index],
        dev_info, sizeof(QMESHAPP_PROXY_DEVICE_FOUND_LIST_T));

    QCLI_LOGI (mesh_group, "APP : Proxy Device Index %d : BD Addr 0x%02x%02x%02x%02x%02x%02x\n",gatt_proxy_dev_index,
        dev_info->device_addr.BD_ADDR5,dev_info->device_addr.BD_ADDR4,
        dev_info->device_addr.BD_ADDR3,dev_info->device_addr.BD_ADDR2,
        dev_info->device_addr.BD_ADDR1,dev_info->device_addr.BD_ADDR0);
    QCLI_LOGI (mesh_group, "APP : Advrt Type %d\n",dev_info->advrt_type);
    if (dev_info->advrt_type == QMESH_PROXY_ADV_ID_TYPE_NETWORK)
    {
        QCLI_LOGI (mesh_group, "APP : Network ID = [ ");
        for (index = 0; index < 8; index++)
        {
            QCLI_LOGI (mesh_group, "%02x ",dev_info->networkID[index]);
        }
        QCLI_LOGI (mesh_group, "]\n");
    }
    else
    {
        QCLI_LOGI (mesh_group, "APP : Hash = ");
        for (index = 0; index < 8; index++)
        {
            QCLI_LOGI (mesh_group, " %02x",dev_info->hash[index]);
        }
        QCLI_LOGI (mesh_group, "]\nAPP : Random = [ ");
        for (index = 0; index < 8; index++)
        {
            QCLI_LOGI (mesh_group, "%02x ",dev_info->random[index]);
        }
        QCLI_LOGI (mesh_group, "]\n");
    }
    /* Update Index */
    gatt_proxy_dev_index++;
}

static void _parseConnAdvtFrmUnprovDev(qapi_BLE_GAP_LE_Advertising_Report_Data_t *adv_report)
{
    uint8_t uuid_array[] = MESH_PROVISIONING_SERVICE_UUID;
    uint8_t itr=0;

    while(itr < adv_report->Advertising_Data.Number_Data_Entries)
    {
        QCLI_LOGI (mesh_group, "APP :: adv-data-length=%d,adv-data-type=%d, UUID=0x%2x%2x\n",
                                adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Length,
                                adv_report->Advertising_Data.Data_Entries[itr].AD_Type,
                                adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[0],
                                adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[1]);
        if((adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Length == 2) &&
            (adv_report->Advertising_Data.Data_Entries[itr].AD_Type == 3) &&
            (adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[0] == uuid_array[0]) &&
            (adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[1] == uuid_array[1]))
        {
            if((gatt_unprov_dev_index < UNPROVISIONED_DEVICE_FOUND_LIST) &&
                (!_checkIfDevPresent(&adv_report->BD_ADDR)))
            {
                _addToUnProvDevList(adv_report, itr);
                break;
            }
        }
        itr++;
    }
}

static void _parseConnAdvtForProxySrvc(qapi_BLE_GAP_LE_Advertising_Report_Data_t *adv_report)
{
    uint8_t uuid_array[] = MESH_PROXY_SERVICE_UUID;
    uint8_t itr=0;
    bool has_proxy_srvc = FALSE;
    QMESHAPP_PROXY_DEVICE_FOUND_LIST_T   proxy_dev_info;

    while(itr < adv_report->Advertising_Data.Number_Data_Entries)
    {
        /* Parsing for Network ID or Node Identity */
        QCLI_LOGI (mesh_group, "APP :: adv-data-length=%d, adv-data-type=%d, ",
                                adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Length,
                                adv_report->Advertising_Data.Data_Entries[itr].AD_Type);
        if (adv_report->Advertising_Data.Data_Entries[itr].AD_Type == 0x01)
        {
            QCLI_LOGI (mesh_group, "flags=0x%x\n",adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[0]);
        }
        else if (adv_report->Advertising_Data.Data_Entries[itr].AD_Type == 0x03)
        {
            QCLI_LOGI (mesh_group, "UUID=0x%02x%02x\n",adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[0],
                        adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[1]);
        }
        else if (adv_report->Advertising_Data.Data_Entries[itr].AD_Type == 0x16)
        {
            uint8_t index;
            QCLI_LOGI (mesh_group, "DATA= [ ");
            for (index=0; index < adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Length; index++)
            {
                QCLI_LOGI (mesh_group, "%02x ",adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[index]);
            }
            QCLI_LOGI (mesh_group, "]\n");
            /* If proxy service present, parse it for Network ID or Hash-Random */
            if ((adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[0] == uuid_array[0]) &&
                (adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[1] == uuid_array[1]))
            {
                has_proxy_srvc = TRUE;
                memset(&proxy_dev_info, 0x00, sizeof(QMESHAPP_PROXY_DEVICE_FOUND_LIST_T));

                proxy_dev_info.advrt_type = adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[2];
                if (proxy_dev_info.advrt_type == QMESH_PROXY_ADV_ID_TYPE_NETWORK)
                {
                    /* Store Network ID */
                    memcpy(proxy_dev_info.networkID, &adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[3], 8);
                }
                else if (proxy_dev_info.advrt_type == QMESH_PROXY_ADV_ID_TYPE_NODE)
                {
                    /* Store Node Hash */
                    memcpy(proxy_dev_info.hash, &adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[3], 8);
                    /* Store Node Random */
                    memcpy(proxy_dev_info.random, &adv_report->Advertising_Data.Data_Entries[itr].AD_Data_Buffer[11], 8);
                }
                memcpy(&proxy_dev_info.device_addr,
                        &adv_report->BD_ADDR, sizeof(qapi_BLE_BD_ADDR_t));
            }
        }
        itr++;
    }
    if(has_proxy_srvc)
    {
        if((gatt_proxy_dev_index < PROXY_DEVICE_FOUND_LIST) &&
            (!_checkIfProxyDevPresent(&proxy_dev_info)))
        {
            _addToProxyDevList(&proxy_dev_info);
        }
    }
}

static bool _startDeviceDiscoveryForGATT(uint8_t scanForProv)
{
    QMESH_RESULT_T res;
    QCLI_LOGI(mesh_group, "_startDeviceDiscoveryForGATT\n");
    app_context.scan_over_pb_gatt = TRUE;
    /* Scanning for connectable adverts for device with Provisioning service*/
    if(scanForProv == 0)
    {
        QCLI_LOGI(mesh_group, "Scan for provisioning via GATT\n");
        memset(gatt_unprov_dev_found_list, 0, sizeof(QMESHAPP_UNPROV_DEVICE_FOUND_LIST_t)*UNPROVISIONED_DEVICE_FOUND_LIST);
        gatt_unprov_dev_index=0;
        app_context.scan_for_prov_srvc = TRUE;
    }
    else
    {
        memset(gatt_proxy_dev_found_list, 0, sizeof(QMESHAPP_PROXY_DEVICE_FOUND_LIST_T)*PROXY_DEVICE_FOUND_LIST);
        gatt_proxy_dev_index=0;
        app_context.scan_for_prov_srvc = FALSE;
    }
    QCLI_LOGI(mesh_group, "QmeshSchedPerformUserScan\n");
    res = QmeshSchedPerformUserScan(QAPI_BLE_ST_ACTIVE_E,
                            LE_SCAN_INTERVAL,
                            LE_SCAN_WINDOW,
                            QAPI_BLE_LAT_PUBLIC_E,
                            QAPI_BLE_FP_NO_FILTER_E,
                            TRUE,
                            GapLeEventCallback,
                            0);
    if(res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "StartDeviceDiscoveryGatt failed, ret=0x%x\n", res);
        return FALSE;
    }
    QCLI_LOGI(mesh_group, "QmeshTimerCreate\n");
    device_disc_tid = QmeshTimerCreate(&app_timer_ghdl,
                                             deviceDiscoveryTimeoutHandler,
                                             NULL,
                                             UNPROV_CONN_ADVRT_DISCOVERY_DUARTION);
    if (device_disc_tid == QMESH_TIMER_INVALID_HANDLE)
    {
        QCLI_LOGE(mesh_group, "%d: Failed to device discovery timer!",__FUNCTION__);
        return FALSE;
    }

    return TRUE;
}


static void deviceDiscoveryTimeoutHandler(QMESH_TIMER_HANDLE_T tid, void* data)
{
    QMESH_RESULT_T res;
    QCLI_LOGI(mesh_group, "deviceDiscoveryTimeoutHandler\n");
    if(tid == device_disc_tid)
    {
        QCLI_LOGI(mesh_group, "Device discovery timer expired, hence stopping the discovery\n");
        device_disc_tid = QMESH_TIMER_INVALID_HANDLE;
        res = QmeshSchedCancelUserScan();
        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE(mesh_group, "StopDeviceDiscoveryGatt failed, ret=0x%x\n", res);
            return;
        }
        app_context.scan_over_pb_gatt = FALSE;
        app_context.scan_for_prov_srvc = FALSE;
    }
}


/*Used by the Proxy client to add the bearer with mesh
  *post enabling notification
  */
static void EnableProxyServiceWithStack(void *ptr)
{
    QMESH_GATT_CLIENT_DATA_T *gattClientData = (QMESH_GATT_CLIENT_DATA_T*)ptr;
    QMESHAPP_DEVICE_INFO_T* devInfo = NULL;
    bool res;

    QCLI_LOGI(mesh_group, "APP : Enabling Proxy service with stack...\n");
    devInfo = _getDeviceInfo(gattClientData->connectionId);
    if (devInfo == NULL)
    {
        QCLI_LOGE(mesh_group, "APP : No Device Information Present\n");
        return;
    }

    res = AddGATTBearer(QMESH_GATT_BEARER_PROXY_SVC_E, devInfo);
    if(!res)
    {
        QCLI_LOGE (mesh_group, "AddGATTBearer failed, res=0x%x\n", res);
        InitiateGattDisconnection(devInfo->remote_addr);
        return;
    }

    devInfo->stack_bearer_added = TRUE;
    /* Returning bearer handle */
    gattClientData->bearer_handle = devInfo->bearer_handle;

}

static void InitiateProvisioning(void *ptr)
{
    QMESH_GATT_CLIENT_DATA_T *gattClientData = (QMESH_GATT_CLIENT_DATA_T*)ptr;
    QMESHAPP_DEVICE_INFO_T * devInfo = NULL;
    QMESH_RESULT_T result;
    bool res;

    QCLI_LOGI(mesh_group, "APP : Initiating provisioning...\n");

    devInfo = _getDeviceInfo(gattClientData->connectionId);
    if (devInfo == NULL)
    {
        QCLI_LOGE(mesh_group, "APP : No Device Information Present\n");
        return;
    }

    res = AddGATTBearer(QMESH_GATT_BEARER_PROV_SVC_E, devInfo);
    if(!res)
    {
        QCLI_LOGE (mesh_group, "AddGATTBearer failed, res=0x%x\n", res);
        InitiateGattDisconnection(devInfo->remote_addr);
        return;
    }

    devInfo->stack_bearer_added = TRUE;
    /* Returning Bearer Handle */
    gattClientData->bearer_handle = devInfo->bearer_handle;

    result = QmeshProvisionDevice(gatt_dev_prov_info.net_key,
                                    gatt_dev_prov_info.element_addr,
                                    (uint16_t *)gatt_unprov_dev_found_list[gatt_unprov_dev_index].uuid,
                                    gatt_dev_prov_info.attention_timer);

    if(result != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE(mesh_group, "APP : Failed to initiate provisioning...%x\n",result);
        InitiateGattDisconnection(devInfo->remote_addr);
    }
    else
    {
        app_context.prov_via_gatt = TRUE;
        QCLI_LOGI(mesh_group, "APP : Provisioning initiated successfully\n");
    }
}

QCLI_Command_Status_t DisplayUnProvDevList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    uint8_t indx=0;
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    QCLI_Printf(mesh_group, "DisplayUnProvDevList\n");

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 1) && (Parameter_List) &&
        (Parameter_List[0].Integer_Is_Valid))
    {
        if(Parameter_List[0].Integer_Value == 0)
        {
            QCLI_Printf(mesh_group, "Provisionable Devices over PB-ADV\n");
            /* Provisionable Devices over ADV */
            for(indx = 0; indx < QMESH_MAX_NUM_OF_PROVISIONABLE_DEVICES; indx++)
            {
                uint16_t *uuid = (uint16_t*)&app_context.unprovisioned_devices[indx];
                QCLI_Printf(mesh_group, "Device-Index %d ", indx);
                QCLI_Printf(mesh_group, "UUID %04x%04x%04x%04x%04x%04x%04x%04x\n",
                                        uuid[0], uuid[1], uuid[2], uuid[3],
                                        uuid[4], uuid[5], uuid[6], uuid[7]);
            }
        }
        else
        {
            QCLI_Printf(mesh_group, "Provisionable Devices over PB-GATT\n");
            /* Provisionable Devices over GATT */
            while (indx < gatt_unprov_dev_index)
            {
                QCLI_Printf(mesh_group, "Device-Index %d Address 0x%02x%02x%02x%02x%02x%02x ", indx,
                    gatt_unprov_dev_found_list[indx].device_addr.BD_ADDR5,gatt_unprov_dev_found_list[indx].device_addr.BD_ADDR4,
                    gatt_unprov_dev_found_list[indx].device_addr.BD_ADDR3,gatt_unprov_dev_found_list[indx].device_addr.BD_ADDR2,
                    gatt_unprov_dev_found_list[indx].device_addr.BD_ADDR1,gatt_unprov_dev_found_list[indx].device_addr.BD_ADDR0);

                QCLI_Printf(mesh_group, "UUID %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
                gatt_unprov_dev_found_list[indx].uuid[0],gatt_unprov_dev_found_list[indx].uuid[1],gatt_unprov_dev_found_list[indx].uuid[2],
                gatt_unprov_dev_found_list[indx].uuid[3],gatt_unprov_dev_found_list[indx].uuid[4],gatt_unprov_dev_found_list[indx].uuid[5],
                gatt_unprov_dev_found_list[indx].uuid[6],gatt_unprov_dev_found_list[indx].uuid[7],gatt_unprov_dev_found_list[indx].uuid[8],
                gatt_unprov_dev_found_list[indx].uuid[9],gatt_unprov_dev_found_list[indx].uuid[10],gatt_unprov_dev_found_list[indx].uuid[11],
                gatt_unprov_dev_found_list[indx].uuid[12],gatt_unprov_dev_found_list[indx].uuid[13],gatt_unprov_dev_found_list[indx].uuid[14],
                gatt_unprov_dev_found_list[indx].uuid[15]);
                indx++;
            }
        }
    }
    else
    {
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

QCLI_Command_Status_t ProvisionDeviceOverGatt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    APP_GATT_Bearer_Callback func_ptr = NULL;
    Qmesh_Gatt_Data_T  gattData;
    bool res;

    QCLI_LOGI (mesh_group, "ProvisionDeviceOverGatt\n");

    if ((Parameter_List[1].Integer_Is_Valid) &&
        (Parameter_List[2].Integer_Is_Valid) &&
        (Parameter_List[3].Integer_Is_Valid) &&
        (Parameter_List[4].Integer_Is_Valid) &&
        (Parameter_List[5].Integer_Is_Valid))
    {

        gatt_unprov_dev_index = Parameter_List[3].Integer_Value;
        if(gatt_unprov_dev_index >= UNPROVISIONED_DEVICE_FOUND_LIST)
        {
            QCLI_LOGE (mesh_group, "  Invalid device index\n");
            return QCLI_STATUS_ERROR_E;
        }

        /* Check for Valid Global Network Key Index */
        if ((Parameter_List[2].Integer_Value & 0xF000) != 0)
            return QCLI_STATUS_USAGE_E;

        gatt_dev_prov_info.net_key.prov_net_idx = Parameter_List[1].Integer_Value & 0xF;
        gatt_dev_prov_info.net_key.key_idx = Parameter_List[2].Integer_Value;
        gatt_dev_prov_info.element_addr = (uint16_t)Parameter_List[4].Integer_Value;
        gatt_dev_prov_info.attention_timer = Parameter_List[5].Integer_Value;

        /* Initialize Gatt Bearer */
        if(provisioner_role_enabled)
        {
            func_ptr = InitiateProvisioning;
        }

        gattData.appGapLeEvtCallback = GapLeEventCallback;
        gattData.appGattEvtCallback = GattConnEventCallback;
        gattData.provisioner_role_enabled = provisioner_role_enabled;

        if(!InitGattBearer(QMESH_GATT_BEARER_PROV_SVC_E, func_ptr, &gattData))
        {
            QCLI_LOGE(mesh_group, "FAILED to registered App Callbacks\n");
            return QCLI_STATUS_ERROR_E;
        }

        res = InitiateGattConnection(QAPI_BLE_LAT_PUBLIC_E,
                                &gatt_unprov_dev_found_list[gatt_unprov_dev_index].device_addr,
                                QMESH_GATT_BEARER_PROV_SVC_E);

        if(!res)
        {
            QCLI_LOGE (mesh_group, "CreateGattConnection failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }
    return(ret_val);
}

QCLI_Command_Status_t AddNetwork(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QmeshResult                   res;
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 3) && (Parameter_List))
    {
        QMESH_SUBNET_KEY_IDX_INFO_T net_key_index;  /* Global Net Key Index.*/
        QMESH_NETWORK_KEY_T key = {0};

        /* Check for Valid Global Network Key Index */
        if ((Parameter_List[1].Integer_Value & 0xF000) != 0)
            return QCLI_STATUS_USAGE_E;

        net_key_index.prov_net_idx = Parameter_List[0].Integer_Value & 0xF;
        net_key_index.key_idx = Parameter_List[1].Integer_Value;

        QCLI_LOGI (mesh_group, "  AddNetwork NetKey entered=%s\n", Parameter_List[2].String_Value);

        /* Retrieve key from param */
        uint8_t str_len = strlen((char *) Parameter_List[2].String_Value);
        uint8_t word_len = 0;

        if(str_len != (QMESH_NETWORK_KEY_SIZE_HWORDS * 4))
            return QCLI_STATUS_USAGE_E;

        word_len = AsciiToHex((char *)Parameter_List[2].String_Value, str_len, (uint8_t*)key);

        if(word_len == 0xFF)
            return QCLI_STATUS_USAGE_E;

        res = QmeshAddNetwork(net_key_index, key);
        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "  QmeshAddNetwork failed res=%d\n", res);
            return  QCLI_STATUS_ERROR_E;
        }
        else
        {
            QCLI_LOGI (mesh_group, "Network Added with Prov Net Index : 0x%x,  Key Index : 0x%x \n",
                                    net_key_index.prov_net_idx, net_key_index.key_idx);
        }
    }
    else
    {
        QCLI_LOGE (mesh_group, "%s:%d: Invalid input params\n", __FUNCTION__, __LINE__);
        /* One or more of the necessary parameters is/are invalid.     */
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

QCLI_Command_Status_t AddApplicationKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QmeshResult                   res;
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    if((Parameter_Count == 4) && (Parameter_List) && (Parameter_List[1].Integer_Is_Valid))
    {
        QMESH_APPLICATION_KEY_T appKey = {0};
        QMESH_SUBNET_KEY_IDX_INFO_T net_key_info;
        uint16_t app_key_index;
        /* Retrieve app key from param */
        uint8_t str_len = strlen((char *) Parameter_List[3].String_Value);
        uint8_t word_len = 0;

        /* Check for valid Network Key Index */
        if ((Parameter_List[1].Integer_Value & 0xF000) != 0)
            return QCLI_STATUS_USAGE_E;

        /* Check for valid App Key Index */
        if ((Parameter_List[2].Integer_Value & 0xF000) != 0)
            return QCLI_STATUS_USAGE_E;

        if(str_len != (QMESH_APP_KEY_SIZE_HWORDS * 4))
            return QCLI_STATUS_USAGE_E;

        net_key_info.prov_net_idx = Parameter_List[0].Integer_Value & 0xF;
        net_key_info.key_idx = Parameter_List[1].Integer_Value;

        app_key_index = Parameter_List[2].Integer_Value;

        QCLI_LOGI (mesh_group, "  AddApplicationKey AppKey entered=%s\n", Parameter_List[3].String_Value);

        word_len = AsciiToHex((char *)Parameter_List[3].String_Value, str_len, (uint8_t*)appKey);

        if(word_len == 0xFF)
            return QCLI_STATUS_USAGE_E;

        res = QmeshAddAppKey(net_key_info, app_key_index, appKey);
        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "  QmeshAddAppKey failed res=%d\n", res);
            ret_val = QCLI_STATUS_ERROR_E;
        }
        else
        {
            QCLI_LOGI (mesh_group, "Application Key Added for Prov Net Index : 0x%x,  Key Index : 0x%x, App Key Index : 0x%x \n",
                                    net_key_info.prov_net_idx, net_key_info.key_idx,
                                    app_key_index);
        }
    }
    else
    {
        QCLI_LOGE (mesh_group, "%s:%d: Invalid input params\n", __FUNCTION__, __LINE__);
        /* One or more of the necessary parameters is/are invalid.     */
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

QCLI_Command_Status_t RemoveNetwork(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QmeshResult                   res;
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    if((Parameter_Count == 2) && (Parameter_List) && (Parameter_List[1].Integer_Is_Valid))
    {
        QMESH_SUBNET_KEY_IDX_INFO_T net_key_info = {.prov_net_idx = 0, .key_idx = 0};  /* Global Net Key Index.*/

        /* Check for valid Network Key Index */
        if((Parameter_List[1].Integer_Value & 0xF000) != 0)
            return QCLI_STATUS_USAGE_E;

        net_key_info.prov_net_idx = Parameter_List[0].Integer_Value & 0xF;
        net_key_info.key_idx = Parameter_List[1].Integer_Value;

        res = QmeshRemoveNetKey(net_key_info);
        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "  QmeshRemoveNetKey failed res=%d\n", res);
            return  QCLI_STATUS_ERROR_E;
        }
        else
        {
            QCLI_LOGI (mesh_group, "Network Key 0x%x removed\n", net_key_info.key_idx);
        }
    }
    else
    {
        QCLI_LOGE (mesh_group, "%s:%d: Invalid input params\n", __FUNCTION__, __LINE__);
        /* One or more of the necessary parameters is/are invalid.     */
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

QCLI_Command_Status_t StartDeviceDiscovery(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult           res;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    /*Check if scanning in progress on any of the bearers*/
    if(app_context.scan_over_pb_adv || app_context.scan_over_pb_gatt)
    {
        QCLI_LOGE (mesh_group, "Device discovery already in progress\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List) &&
        (Parameter_List[0].Integer_Is_Valid) &&
        (Parameter_List[1].Integer_Is_Valid) &&
        (Parameter_List[0].Integer_Value <= 1) &&
        (Parameter_List[1].Integer_Value <= 1))
    {
        QCLI_LOGI (mesh_group, "StartDeviceDiscovery\n");
        if(Parameter_List[0].Integer_Value == 0)
        {
            app_context.scan_over_pb_adv = TRUE;
            memset(app_context.unprovisioned_devices, 0, sizeof(app_context.unprovisioned_devices));
            app_context.dev_idx = 0;

            res = QmeshStartDeviceDiscovery(NULL, 0, FALSE);
            if(res != QMESH_RESULT_SUCCESS)
            {
                QCLI_LOGE (mesh_group, "StartDeviceDiscovery failed, ret=0x%x\n", res);
                return QCLI_STATUS_ERROR_E;
            }
        }
        else
        {
            _startDeviceDiscoveryForGATT(Parameter_List[1].Integer_Value);
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

 QCLI_Command_Status_t StopDeviceDiscovery(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;

    QCLI_LOGI (mesh_group, "StopDeviceDiscovery role=%x, pb-gatt=%x, pb-adv=%x\n",
        provisioner_role_enabled, app_context.scan_over_pb_gatt, app_context.scan_over_pb_adv);
    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    if((!app_context.scan_over_pb_adv) && (!app_context.scan_over_pb_gatt))
    {
        QCLI_LOGE (mesh_group, "Device discovery has not started\n");
        return QCLI_STATUS_ERROR_E;
    }

    if(app_context.scan_over_pb_adv)
    {
        res = QmeshStopDeviceDiscovery();
        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "QmeshStopDeviceDiscovery failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }
        app_context.scan_over_pb_adv = FALSE;
    }
    else /*(app_context.scan_over_pb_gatt)*/
    {
        if(device_disc_tid != QMESH_TIMER_INVALID_HANDLE)
        {
            QmeshTimerDelete (&app_timer_ghdl, &device_disc_tid);
            device_disc_tid = QMESH_TIMER_INVALID_HANDLE;
            res = QmeshSchedCancelUserScan();
            if(res != QMESH_RESULT_SUCCESS)
            {
                QCLI_LOGE (mesh_group, "StopDeviceDiscoveryGatt failed, ret=0x%x\n", res);
                return QCLI_STATUS_ERROR_E;
            }
            app_context.scan_over_pb_gatt = FALSE;
            app_context.scan_for_prov_srvc = FALSE;
        }
        else
        {
            QCLI_LOGE(mesh_group, "Device discovery has already been stopped\n");
        }
    }

    return(ret_val);
}


QCLI_Command_Status_t StartProvisionDevice(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 6) && (Parameter_List))
    {
        if(Parameter_List[0].Integer_Value == 0)
        {
            QmeshResult res;
            const uint16_t *uuid;
            QMESH_SUBNET_KEY_IDX_INFO_T net_key;
            uint8_t val;
            QCLI_LOGI (mesh_group, " StartProvisionDevice: Provisioning over PB-ADV bearer\n");

            /* Check for Valid Global Network Key Index */
            if ((Parameter_List[2].Integer_Value & 0xF000) != 0)
                return QCLI_STATUS_USAGE_E;

            if(Parameter_List[3].Integer_Value >= QMESH_MAX_NUM_OF_PROVISIONABLE_DEVICES)
            {
                QCLI_LOGE (mesh_group, "  Invalid device index\n");
                return QCLI_STATUS_ERROR_E;
            }

            uuid =  app_context.unprovisioned_devices[Parameter_List[3].Integer_Value];

            QCLI_LOGI (mesh_group, "  Provisioning Device uuid:");
            for(val = 0; val < QMESH_UUID_SIZE_HWORDS; val++)
            {
                QCLI_LOGI (mesh_group, " 0x%04x", uuid[val]);
            }
            net_key.prov_net_idx = Parameter_List[1].Integer_Value & 0xF;
            net_key.key_idx = Parameter_List[2].Integer_Value;

            if (QMESH_BEARER_HANDLE_INVALID == app_context.adv_bearer_handle[QMESH_BEARER_TYPE_LE_ADV_PROVISIONING])
            {
                res = QmeshAddBearer(QMESH_BEARER_TYPE_LE_ADV_PROVISIONING,  NULL, 0,
                                     &app_context.adv_bearer_handle[QMESH_BEARER_TYPE_LE_ADV_PROVISIONING]);
                if (QMESH_RESULT_SUCCESS != res)
                {
                    QCLI_LOGE(mesh_group, "Failed to Add Advert Provisioning Bearer : %d\n", res);
                    return QCLI_STATUS_ERROR_E;
                }
            }

            /* Initiate Provisioning for Provisioning over PB-ADV */
            res = QmeshProvisionDevice(net_key, Parameter_List[4].Integer_Value, uuid, Parameter_List[5].Integer_Value);

            if(res != QMESH_RESULT_SUCCESS)
            {
                QCLI_LOGE (mesh_group, "  StartProvisionDevice failed, ret=0x%x\n", res);
                return QCLI_STATUS_ERROR_E;
            }

            /* Flag success to the caller.                                 */
            ret_val = QCLI_STATUS_SUCCESS_E;
            prov_state = app_state_provision_started;
        }
        else
        {
            ret_val = ProvisionDeviceOverGatt(Parameter_Count, Parameter_List);
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }
    return(ret_val);
}

QCLI_Command_Status_t StopProvision(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    QCLI_LOGI (mesh_group, " StopProvision\n");

    res = QmeshStopProvision();
    if(res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "StopProvision failed, ret=0x%x\n", res);
        return QCLI_STATUS_ERROR_E;
    }
    /* Flag success to the caller.                                 */
    ret_val = QCLI_STATUS_SUCCESS_E;
    prov_state = app_state_not_provisioned;

    return(ret_val);
}


  QCLI_Command_Status_t UpdateKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res = QCLI_STATUS_SUCCESS_E;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    QCLI_LOGI (mesh_group, " UpdateKey\n");

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count >= 4) && (Parameter_List))
    {
        QMESH_SUBNET_KEY_IDX_INFO_T key_info;
        QMESH_NETWORK_KEY_T key; //16 octets
        uint8_t len = strlen((char *) Parameter_List[3].String_Value);
        uint8_t val = 0;
        uint8_t key_type = Parameter_List[1].Integer_Value;

        if (key_type == 1)
        {
            if (Parameter_Count != 5)
                return QCLI_STATUS_USAGE_E;

            /* Check for valid App Key Index */
            if ((Parameter_List[4].Integer_Value & 0xF000) != 0)
                return QCLI_STATUS_USAGE_E;
        }

        if(len != (QMESH_NETWORK_KEY_SIZE_HWORDS * 4))
            return QCLI_STATUS_USAGE_E;

        /* Check for valid Network Key Index */
        if ((Parameter_List[2].Integer_Value & 0xF000) != 0)
            return QCLI_STATUS_USAGE_E;

        QCLI_LOGI (mesh_group, "  UpdateKey Key entered=%s\n", Parameter_List[3].String_Value);

        val = AsciiToHex((char *)Parameter_List[3].String_Value, len, (uint8_t*)key);

        if(val == 0xFF)
            return QCLI_STATUS_USAGE_E;

        key_info.prov_net_idx = Parameter_List[0].Integer_Value & 0xF;
        key_info.key_idx = Parameter_List[2].Integer_Value;

        if(key_type == 1)
        {
            res = QmeshUpdateAppKey(key_info, Parameter_List[4].Integer_Value, key);
        }
        else
        {
            res = QmeshUpdateNetKey(key_info, key);
        }

        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "UpdateKey failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

 QCLI_Command_Status_t UpdateKeyRefreshPhase(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res = QCLI_STATUS_SUCCESS_E;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 3) && (Parameter_List))
    {
        QMESH_SUBNET_KEY_IDX_INFO_T net_key_info;

        /* Check for valid Network Key Index */
        if ((Parameter_List[1].Integer_Value & 0xF000) != 0)
            return QCLI_STATUS_USAGE_E;

        QCLI_LOGI (mesh_group, " UpdateKeyRefreshPhase()\n");
        net_key_info.prov_net_idx = Parameter_List[0].Integer_Value & 0xF;
        net_key_info.key_idx = Parameter_List[1].Integer_Value;

        res = QmeshUpdateKeyRefreshPhase(net_key_info, Parameter_List[2].Integer_Value);
        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "UpdateKeyRefreshPhase failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

 QCLI_Command_Status_t UpdateModelAppBindList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;

    if(FALSE == provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 6) && (Parameter_List))
    {
        QMESH_MODEL_APPKEY_BIND_INFO_T bind_info;

        /* Check for valid App Key Index */
        if ((Parameter_List[2].Integer_Value & 0xF000) != 0)
            return QCLI_STATUS_USAGE_E;

        QCLI_LOGI (mesh_group, " UpdateModelAppBindList\n");

        bind_info.elm_addr = Parameter_List[1].Integer_Value;
        bind_info.app_key_idx = Parameter_List[2].Integer_Value & 0xFFF;
        bind_info.model_id = Parameter_List[3].Integer_Value;
        bind_info.model_type = Parameter_List[4].Integer_Value;

        res = QmeshUpdateModelAppBind(Parameter_List[0].Integer_Value & 0xF,
                                      &bind_info,
                                      Parameter_List[5].Integer_Value);

        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "UpdateModelAppBindList failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

 QCLI_Command_Status_t SelectPeerCapability(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 4) && (Parameter_List))
    {
        QMESH_PROV_SELECTED_CAPS_T cap;

        cap.pub_key_method = Parameter_List[0].Integer_Value;
        cap.auth_method = Parameter_List[1].Integer_Value;
        if(cap.auth_method == QMESH_AUTH_METHOD_OOB_OUTPUT)
        {
            cap.action.input_oob_action = (QMESH_INPUT_OOB_ACTION_T) Parameter_List[2].Integer_Value;
        }
        else if(cap.auth_method == QMESH_AUTH_METHOD_OOB_INPUT)
        {
            cap.action.output_oob_action = (QMESH_OUTPUT_OOB_ACTION_T) Parameter_List[2].Integer_Value;
        }
        else
        {
            cap.action.output_oob_action = QMESH_OUTPUT_OOB_ACTION_BLINK;
        }
        cap.auth_data_size = Parameter_List[3].Integer_Value;

        QCLI_LOGI (mesh_group, " SelectPeerCapability\n");

        res = QmeshSelectCapability(&cap);

        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "SelectPeerCapability failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}


 QCLI_Command_Status_t IncrementNetworkIVIndex(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;

    if(FALSE == provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    QCLI_LOGI (mesh_group, "IncrementNetworkIVIndex\n");

    if ((Parameter_Count == 1) && (Parameter_List))
    {
        uint8_t prov_net_idx = Parameter_List[0].Integer_Value & 0xF;

        res = QmeshIncrementIV(prov_net_idx);
        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "IncrementNetworkIVIndex for Network %d failed, ret=0x%x\n", prov_net_idx, res);
            ret_val=  QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}


 QCLI_Command_Status_t DisplayDiscoveredProxyDevices(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    uint8_t index = 0;
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QMESHAPP_PROXY_DEVICE_FOUND_LIST_T *dev_info;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    QCLI_Printf(mesh_group, "APP :: DisplayDiscoveredProxyDevices, gatt_proxy_dev_index=%d\n", gatt_proxy_dev_index);

    while (index < gatt_proxy_dev_index)
    {
        uint8_t itr;
        dev_info = (QMESHAPP_PROXY_DEVICE_FOUND_LIST_T*)&gatt_proxy_dev_found_list[index];

        QCLI_Printf(mesh_group, "APP : Proxy Device Index %d, Advrt Type %d, BD Addr 0x%02x%02x%02x%02x%02x%02x\n",index,
            dev_info->advrt_type, dev_info->device_addr.BD_ADDR5,dev_info->device_addr.BD_ADDR4,
            dev_info->device_addr.BD_ADDR3,dev_info->device_addr.BD_ADDR2,
            dev_info->device_addr.BD_ADDR1,dev_info->device_addr.BD_ADDR0);
        if (dev_info->advrt_type == QMESH_PROXY_ADV_ID_TYPE_NETWORK)
        {
            QCLI_Printf(mesh_group, "APP : Network ID = [ ");
            for (itr = 0; itr < 8; itr++)
            {
                QCLI_Printf(mesh_group, "%02x ",dev_info->networkID[itr]);
            }
            QCLI_Printf(mesh_group, "]\n");
        }
        else
        {
            QCLI_Printf(mesh_group, "APP : Hash = ");
            for (itr = 0; itr < 8; itr++)
            {
                QCLI_Printf(mesh_group, " %02x",dev_info->hash[itr]);
            }
            QCLI_Printf(mesh_group, "]\nAPP : Random = [ ");
            for (itr = 0; itr < 8; itr++)
            {
                QCLI_Printf(mesh_group, "%02x ",dev_info->random[itr]);
            }
            QCLI_Printf(mesh_group, "]\n");
        }
        index++;
    }

    return(ret_val);
}

 QCLI_Command_Status_t FindNodeFromProxyDevList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    if (Parameter_Count == 3)
    {
        QMESHAPP_PROXY_DEVICE_FOUND_LIST_T *dev_info;
        QMESH_SUBNET_KEY_IDX_INFO_T key_info;
        uint8_t index = 0;
        uint8_t srvc_data[17] = {0};
        uint16_t subnet_id = Parameter_List[1].Integer_Value;
        uint32_t elm_addr = Parameter_List[2].Integer_Value;
        bool dev_found = FALSE;

        /* Check for Valid Global Network Key Index */
        if ((Parameter_List[1].Integer_Value & 0xF000) != 0)
            return QCLI_STATUS_USAGE_E;

        QCLI_LOGI (mesh_group, "FindNodeFromProxyDevList\n");

        while (index < gatt_proxy_dev_index)
        {
            dev_info = (QMESHAPP_PROXY_DEVICE_FOUND_LIST_T*)&gatt_proxy_dev_found_list[index];

            QCLI_LOGI (mesh_group, "APP : Proxy Device Index %d, Advrt Type %d, BD Addr 0x%02x%02x%02x%02x%02x%02x\n",
                index, dev_info->advrt_type,
                dev_info->device_addr.BD_ADDR5,dev_info->device_addr.BD_ADDR4,
                dev_info->device_addr.BD_ADDR3,dev_info->device_addr.BD_ADDR2,
                dev_info->device_addr.BD_ADDR1,dev_info->device_addr.BD_ADDR0);
            if (dev_info->advrt_type == QMESH_PROXY_ADV_ID_TYPE_NODE)
            {
                QCLI_LOGI (mesh_group, "APP : Hash = ");
                for (index = 0; index < 8; index++)
                {
                    QCLI_LOGI (mesh_group, " %02x",dev_info->hash[index]);
                }
                QCLI_LOGI (mesh_group, "]\nAPP : Random = [ ");
                for (index = 0; index < 8; index++)
                {
                    QCLI_LOGI (mesh_group, "%02x ",dev_info->random[index]);
                }
                QCLI_LOGI (mesh_group, "]\n");
                /* Get Device Hash */
                key_info.prov_net_idx = Parameter_List[0].Integer_Value & 0xF;
                key_info.key_idx = subnet_id;
                res = QmeshGetProxyAdvServiceData(key_info,
                                                  QMESH_PROXY_ADV_ID_TYPE_NODE,
                                                  elm_addr,
                                                  dev_info->random,
                                                  srvc_data);
                if (res == QMESH_RESULT_SUCCESS)
                {
                    /* Comapre HASH */
                    if (memcmp(srvc_data, dev_info->hash, 8) == 0)
                    {
                        QCLI_LOGI (mesh_group, "APP : Device Found\n");
                        dev_found = TRUE;
                        break;
                    }
                }
            }
            index++;
        }

        if (!dev_found)
        {
            QCLI_LOGI (mesh_group, "APP : Device not present in discovered proxy device list\n");
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

 QCLI_Command_Status_t EstablishGattProxyConnection(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    APP_GATT_Bearer_Callback func_ptr = NULL;
    Qmesh_Gatt_Data_T  gattData;
    bool res;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    QCLI_LOGI (mesh_group, "EstablishGattProxyConnection\n");

    if (Parameter_Count == 1)
    {
        uint8_t index = Parameter_List[0].Integer_Value;
        if(index >= PROXY_DEVICE_FOUND_LIST)
        {
            QCLI_LOGE (mesh_group, " Invalid proxy device index\n");
            return QCLI_STATUS_ERROR_E;
        }

        /* Initialize Gatt Bearer */
        func_ptr = EnableProxyServiceWithStack;

        gattData.appGapLeEvtCallback = GapLeEventCallback;
        gattData.appGattEvtCallback = GattConnEventCallback;
        gattData.provisioner_role_enabled = provisioner_role_enabled;

        if(!InitGattBearer(QMESH_GATT_BEARER_PROXY_SVC_E, func_ptr, &gattData))
        {
            QCLI_LOGE(mesh_group, "FAILED to registered App Callbacks\n");
            return QCLI_STATUS_ERROR_E;
        }
        /* Store GATT Proxy info */
        res = InitiateGattConnection(QAPI_BLE_LAT_PUBLIC_E,
                                &gatt_proxy_dev_found_list[index].device_addr,
                                QMESH_GATT_BEARER_PROXY_SVC_E);
        if(!res)
        {
            QCLI_LOGE (mesh_group, "CreateGattConnection failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }
        /* Store device index */
        gatt_proxy_client_context.dev_index = index;
        /* Store BD Address */
        memcpy(&gatt_proxy_client_context.remote_addr,
                &gatt_proxy_dev_found_list[index].device_addr, sizeof(qapi_BLE_BD_ADDR_t));
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }
    return(ret_val);
}

 QCLI_Command_Status_t SendGattProxySetFilterType(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    QCLI_LOGI (mesh_group, "SendGattProxySetFilterType\n");

    if (Parameter_Count == 4)
    {
        QMESH_SUBNET_KEY_IDX_INFO_T  key_info;
        QMESH_PROXY_FILTER_TYPE_T filter_type;
        uint8_t idx;

        /* Check for valid Network Key Index */
        if ((Parameter_List[1].Integer_Value & 0xF000) != 0)
            return QCLI_STATUS_USAGE_E;

        key_info.prov_net_idx = Parameter_List[0].Integer_Value & 0xF;
        key_info.key_idx = Parameter_List[1].Integer_Value;

        filter_type = Parameter_List[2].Integer_Value == 0 ?
              QMESH_PROXY_FILTER_TYPE_WHITELIST : QMESH_PROXY_FILTER_TYPE_BLACKLIST;

        idx = Parameter_List[3].Integer_Value;
        if (MAX_NUM_OF_GATT_CONN <= idx)
        {
            QCLI_LOGE (mesh_group, "Invalid Device Index\n");
            return QCLI_STATUS_USAGE_E;
        }

        res = QmeshProxySetFilterType(app_context.deviceInfo[idx].bearer_handle,
                                    filter_type, key_info);
        if(res)
        {
            QCLI_LOGE (mesh_group, "QmeshProxySetFilterType failed, ret=0x%x\n", res);
            ret_val = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

 QCLI_Command_Status_t SendGattProxyAddRemoveAddr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;


    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    QCLI_LOGI (mesh_group, "SendGattProxyAddRemoveAddr\n");
    if (Parameter_Count == 5)
    {
        QMESH_SUBNET_KEY_IDX_INFO_T  key_info;
        uint8_t str_len = strlen((char *) Parameter_List[3].String_Value);
        uint8_t word_len = 0;
        uint8_t index = 0;
        uint8_t *addr_buff;
        bool add_dev;
        uint8_t num_of_addr;
        uint8_t idx;

        if((str_len % 4) != 0)
            return QCLI_STATUS_USAGE_E;

        /* Check for valid Network Key Index */
        if ((Parameter_List[1].Integer_Value & 0xF000) != 0)
            return QCLI_STATUS_USAGE_E;

        addr_buff = (uint8_t*) malloc(str_len);
        if (addr_buff == NULL)
        {
            QCLI_LOGE (mesh_group, "Malloc failed\n");
            return QCLI_STATUS_ERROR_E;
        }

        key_info.prov_net_idx = Parameter_List[0].Integer_Value & 0xF;
        key_info.key_idx = Parameter_List[1].Integer_Value;

        word_len = AsciiToHex((char *)Parameter_List[3].String_Value, str_len, addr_buff);
        if (word_len == 0xFF)
        {
            free(addr_buff);
            return QCLI_STATUS_USAGE_E;
        }

        num_of_addr = str_len / 4;
        QCLI_LOGI (mesh_group,"Dev Addr : ");
        for(index = 0; index < (num_of_addr * 2); index++)
        {
            QCLI_LOGI (mesh_group, " %02x", addr_buff[index]);
        }

        add_dev = Parameter_List[2].Integer_Value == 0 ? TRUE : FALSE;
        idx = Parameter_List[4].Integer_Value;
        if (MAX_NUM_OF_GATT_CONN <= idx)
        {
            QCLI_LOGE (mesh_group, "Invalid Device Index\n");
            free(addr_buff);
            return QCLI_STATUS_USAGE_E;
        }

        res = QmeshProxyFilterAddRemoveAddr(app_context.deviceInfo[idx].bearer_handle,
                                    key_info, add_dev, (uint16_t *)addr_buff, num_of_addr);
        if(res)
        {
            QCLI_LOGE (mesh_group, "SendGattProxyAddRemoveAddr failed, ret=0x%x\n", res);
            ret_val = QCLI_STATUS_ERROR_E;
        }
        if (addr_buff)
            free(addr_buff);
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

 QCLI_Command_Status_t ListConnectedGattDevices(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    uint8_t idx;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    for (idx = 0; idx < MAX_NUM_OF_GATT_CONN; idx++)
    {
        if (TRUE == app_context.deviceInfo[idx].is_connected)
        {
            QCLI_LOGE (mesh_group, "Connection Index: %d | Connection Handle: %d | Device address: 0x%02x%02x%02x%02x%02x%02x\n",
                       idx, app_context.deviceInfo[idx].connection_id,
                       app_context.deviceInfo[idx].remote_addr.BD_ADDR5,
                       app_context.deviceInfo[idx].remote_addr.BD_ADDR4,
                       app_context.deviceInfo[idx].remote_addr.BD_ADDR3,
                       app_context.deviceInfo[idx].remote_addr.BD_ADDR2,
                       app_context.deviceInfo[idx].remote_addr.BD_ADDR1,
                       app_context.deviceInfo[idx].remote_addr.BD_ADDR0);
        }
    }

    return QCLI_STATUS_SUCCESS_E;
}

 QCLI_Command_Status_t DisconnectGattProxyConnection(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    QCLI_LOGI (mesh_group, "DisconnectGattProxyConnection\n");

    if (Parameter_Count == 1)
    {
        InitiateGattDisconnection(app_context.deviceInfo[Parameter_List[0].Integer_Value].remote_addr);
    }
    else
    {
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

extern QCLI_Command_Status_t ClearReplayProtectionEntries(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QMESH_RESULT_T res;

    if(!provisioner_role_enabled)
    {
        QCLI_LOGE (mesh_group, "Accessible only with Provisioner Role\n");
        return QCLI_STATUS_ERROR_E;
    }

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    QCLI_LOGI (mesh_group, "ClearReplayProtectionEntries\n");

    if (Parameter_Count == 3)
    {
        res = QmeshClearRPLEntry(Parameter_List[0].Integer_Value & 0xF,
                                 Parameter_List[1].Integer_Value,
                                 Parameter_List[2].Integer_Value);
        if(res)
        {
            QCLI_LOGE (mesh_group, "ClearReplayProtectionEntries failed, ret=0x%x\n", res);
            ret_val = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

 QCLI_Command_Status_t SendSecureNwBeacon(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;
    QMESH_SUBNET_KEY_IDX_INFO_T net_key_info;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List))
    {
        /* Check for Valid Global Network Key Index */
        if ((Parameter_List[1].Integer_Value & 0xF000) != 0)
            return QCLI_STATUS_USAGE_E;

        QCLI_LOGI (mesh_group, " SendSecureNwBeacon\n");

        net_key_info.prov_net_idx = 0xF & Parameter_List[0].Integer_Value;
        net_key_info.key_idx = 0xFFF & Parameter_List[1].Integer_Value;

        res = QmeshSendSecureNetworkBeacon(net_key_info);

        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "SendSecureNwBeacon failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

 QCLI_Command_Status_t SendMessage(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 10) && (Parameter_List))
    {
        QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
        QMESH_ACCESS_PAYLOAD_INFO_T data_info;
        QCLI_LOGI (mesh_group, "SendSIGMessage\n");
        uint8_t *data = NULL;
        uint16_t len = strlen((char *) Parameter_List[7].String_Value);
        uint16_t indx, val;

        //check if param buffer is sufficient to recieve all the inputs required for this API - max data vs console buffer available.

        app_key_info.key_type = Parameter_List[1].Integer_Value;

        app_key_info.key_info.prov_net_idx = Parameter_List[0].Integer_Value & 0xF;
        app_key_info.key_info.key_idx = Parameter_List[2].Integer_Value;

        data_info.dst_addr = Parameter_List[3].Integer_Value;
        data_info.src_addr = Parameter_List[4].Integer_Value;
        data_info.ttl = Parameter_List[5].Integer_Value;
        data_info.payload_len = Parameter_List[6].Integer_Value/2;

        // Retrieve payload data from param
        if((len > (QMESH_MSG_SIZE_MAX * 2)) || (len != Parameter_List[6].Integer_Value))
        {
            return QCLI_STATUS_USAGE_E;
        }

        data = (uint8_t*)malloc(len/2);

        if(data == NULL)
        {
            QCLI_LOGE (mesh_group, "Malloc failed for %d bytes \n", len/2);
            return QCLI_STATUS_ERROR_E;
        }

        memset(data, 0, len/2);

        val = AsciiToHex((char *)Parameter_List[7].String_Value, len, data);

        if(val == 0xFF)
        {
            free(data);
            return QCLI_STATUS_USAGE_E;
        }

        QCLI_LOGI (mesh_group, "Data:");
        for(indx = 0; indx < (len/2); indx++)
        {
            QCLI_LOGI (mesh_group, " %02x", data[indx]);
        }
        QCLI_LOGI (mesh_group, "\n");

        data_info.payload = data;
        data_info.trans_mic_size = Parameter_List[8].Integer_Value;
        data_info.trans_ack_required = Parameter_List[9].Integer_Value;

        res = QmeshSendMessage(&app_key_info, &data_info);

        free(data);

        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "SendSIGMessage failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}


QCLI_Command_Status_t SetNetworkIVIndex(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List))
    {
        uint32_t iv_index;
        QCLI_LOGI (mesh_group, " SetNetworkIVIndex\n");

        //Retrieve iv_index from param
        {
            uint8_t len = strlen((char *) Parameter_List[1].String_Value);
            uint8_t val;

            if(len != 4)
                return QCLI_STATUS_USAGE_E;

            val = AsciiToHex((char *)Parameter_List[1].String_Value, len, (uint8_t*)&iv_index);

            if(val == 0xff)
                return QCLI_STATUS_USAGE_E;

            QCLI_LOGI (mesh_group, "iv_index=0x%x\n", iv_index);
        }

        res = QmeshSetIV(Parameter_List[0].Integer_Value & 0xF, iv_index);

        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "SetNetworkIVIndex failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }

        /* Flag success to the caller.                                 */
        ret_val = QCLI_STATUS_SUCCESS_E;
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

   return(ret_val);
}

QCLI_Command_Status_t AddPeerDeviceKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 4) && (Parameter_List))
    {
        QMESH_DEVICE_KEY_T devKey;//16 octets
        QMESH_PEER_DEVICE_KEY_LIST *peer_dev_info;

        QCLI_LOGI (mesh_group, "AddPeerDeviceKey\n");

        //populate devKey from param
        {
            uint8_t len = strlen((char *) Parameter_List[3].String_Value);
            uint8_t val;

            if(len != (QMESH_SECURITY_KEY_HWORDS * 4))
                return QCLI_STATUS_USAGE_E;

            QCLI_LOGI (mesh_group, "  AddPeerDeviceKey Key entered=%s\n", Parameter_List[3].String_Value);
            val = AsciiToHex((char *)Parameter_List[3].String_Value, len, (uint8_t*)devKey);

            if(val == 0xff)
                return QCLI_STATUS_USAGE_E;
        }

        peer_dev_info = (QMESH_PEER_DEVICE_KEY_LIST*)malloc(sizeof(QMESH_PEER_DEVICE_KEY_LIST));
        if (peer_dev_info != NULL)
        {
            peer_dev_info->prov_net_id = Parameter_List[0].Integer_Value & 0xF;
            peer_dev_info->elem_addr = Parameter_List[1].Integer_Value;
            peer_dev_info->num_elems = Parameter_List[2].Integer_Value;
            memcpy(peer_dev_info->device_key, devKey, QMESH_SECURITY_KEY_HWORDS * sizeof(uint16_t));
            peer_dev_info->next = app_context.peer_device_key_list;
            app_context.peer_device_key_list = peer_dev_info;

            /* Flag success to the caller.  */
            return QCLI_STATUS_SUCCESS_E;
        }
        else
        {
            QCLI_LOGE (mesh_group, "AddPeerDeviceKey failed");
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        return QCLI_STATUS_USAGE_E;
    }
}

QCLI_Command_Status_t DeviceReset(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QMESH_RESULT_T res;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 1) && (Parameter_List))
    {
        uint8_t prov_net_idx = Parameter_List[0].Integer_Value & 0xF;

        QCLI_LOGI (mesh_group, "DeviceReset\n");

        res = QmeshNodeReset(prov_net_idx);

        if(QMESH_RESULT_SUCCESS != res)
        {
            QCLI_LOGE (mesh_group, "DeviceReset failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }

        /* Flag success to the caller.  */
        return QCLI_STATUS_SUCCESS_E;
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        return QCLI_STATUS_USAGE_E;
    }
}

#if (APP_PTS_MODE == 1)
QCLI_Command_Status_t AddSelfDeviceKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 3) && (Parameter_List))
    {
        QMESH_SUBNET_KEY_IDX_INFO_T net_key_info;
        QMESH_DEVICE_KEY_T devKey;//16 octets
        QCLI_LOGI (mesh_group, "AddSelfDeviceKey\n");

        //populate devKey from param
        {
            uint8_t len = strlen((char *) Parameter_List[2].String_Value);
            uint8_t val;

            if(len != (QMESH_APP_KEY_SIZE_HWORDS * 4))
                return QCLI_STATUS_USAGE_E;

            QCLI_LOGI (mesh_group, "  AddSelfDeviceKey Key entered=%s\n", Parameter_List[2].String_Value);
            val = AsciiToHex((char *)Parameter_List[2].String_Value, len, (uint8_t*)devKey);

            if(val == 0xff)
                return QCLI_STATUS_USAGE_E;
        }

        net_key_info.prov_net_idx = Parameter_List[0].Integer_Value & 0xF;
        net_key_info.key_idx = Parameter_List[1].Integer_Value & 0xFFF;
        res = QmeshAddDevKey(net_key_info, devKey);

        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "AddSelfDeviceKey failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }

        /* Flag success to the caller.                                 */
        ret_val = QCLI_STATUS_SUCCESS_E;
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "%s:%d: Invalid params\n", __FUNCTION__, __LINE__);
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

QCLI_Command_Status_t EnableIVTest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_LOGI (mesh_group, "EnableIVTest: Parameter_Count=%d, \n", Parameter_Count);

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if ((Parameter_Count == 1) && (Parameter_List))
    {
        QmeshEnableIVTestMode((bool)Parameter_List[0].Integer_Value);
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, " One or more parameters is/are invalid\n");
        return QCLI_STATUS_USAGE_E;
    }

    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t SendIVTestSignal(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QMESH_RESULT_T result;
    QCLI_Command_Status_t ret_val = QCLI_STATUS_USAGE_E;

    QCLI_LOGI (mesh_group, "SendIVTestSignal: Parameter_Count=%d, \n", Parameter_Count);

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if ((Parameter_Count == 2) && (Parameter_List))
    {
        result = QmeshSendIVTestModeSignal((uint8_t)(Parameter_List[0].Integer_Value & 0xF),
                                       (QMESH_IV_UPDATE_STATE_T)Parameter_List[1].Integer_Value);
        if (QMESH_RESULT_SUCCESS != result)
        {
            QCLI_LOGE (mesh_group, "QmeshEnableIVTestMode : %d\n", result);
        }
        else
        {
            ret_val = QCLI_STATUS_SUCCESS_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, " One or more parameters is/are invalid\n");
    }

    return ret_val;
}


/*----------------------------------------------------------------------------*
 * SetStackPTSmode
 */
/*! \brief Enable or Disable PTS mode in stack
 *
 *  \returns QCLI_Command_Status_t. Refer to \ref QCLI_Command_Status_t.
 */
/*----------------------------------------------------------------------------*/
QCLI_Command_Status_t SetStackPTSmode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_LOGI (mesh_group, "SetStackPTSmode: Parameter_Count=%d, \n", Parameter_Count);

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 1) && (Parameter_List))
    {
        QmeshSetPTSMode((bool)Parameter_List[0].Integer_Value);
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, " One or more parameters is/are invalid\n");
    }

    return QCLI_STATUS_SUCCESS_E;
}

/*----------------------------------------------------------------------------*
 * SetSecureNetworkBeaconState
 */
/*! \brief API to change the secure network beacon broadcast state.
 *   This API is typically needed for PTS to start or stop sending secure network beacons.
 *   NOTE: This API needs to be called only after \ref QmeshSendSecureNetworkBeacon() is called.
 *
 *  \returns QCLI_Command_Status_t. Refer to \ref QCLI_Command_Status_t.
 */
/*----------------------------------------------------------------------------*/
 QCLI_Command_Status_t SetSecureNetworkBeaconState(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QmeshResult res = QMESH_RESULT_SUCCESS;
    QCLI_Command_Status_t ret_val;
    uint8_t prov_net_id;

    QCLI_LOGI (mesh_group, "SetSecureNetworkBeaconState: Parameter_Count=%d, \n", Parameter_Count);

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List))
    {
        prov_net_id = Parameter_List[0].Integer_Value & 0xF;
        res = QmeshSetSecureNetworkBeaconState(prov_net_id, Parameter_List[1].Integer_Value);
        if (res == QMESH_RESULT_SUCCESS)
        {
            /* Flag success to the caller.                                 */
            ret_val = QCLI_STATUS_SUCCESS_E;
        }
        else
        {
            QCLI_LOGE (mesh_group, "Set Secure Netowrk Beacon State failed  reason = 0x%x\n", res);
            ret_val = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, " One or more parameters is/are invalid\n");
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

/*----------------------------------------------------------------------------*
 * SetSecureNetworkBeaconPeriod
 */
/*! \brief API to change the secure network beacon broadcast period.
 *   NOTE: This API needs to be called only after \ref QmeshSendSecureNetworkBeacon() is called.
 *
 *  \returns QCLI_Command_Status_t. Refer to \ref QCLI_Command_Status_t.
 */
/*----------------------------------------------------------------------------*/
 QCLI_Command_Status_t SetSecureNetworkBeaconPeriod(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QMESH_SUBNET_KEY_IDX_INFO_T net_key_info;
    QMESH_RESULT_T res;

    QCLI_LOGI (mesh_group, "SetSecureNetworkBeaconPeriod: Parameter_Count=%d, \n", Parameter_Count);

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    if((Parameter_Count == 3) && (Parameter_List))
    {
        net_key_info.prov_net_idx = 0xF & Parameter_List[0].Integer_Value;
        net_key_info.key_idx = 0xFFF & Parameter_List[1].Integer_Value;

        res = QmeshSetSecureNetworkBeaconPeriod(net_key_info, Parameter_List[2].Integer_Value);
        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "SetSecureNetworkBeaconPeriod failed, ret=0x%x\n", res);
            ret_val = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, " One or more parameters is/are invalid\n");
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return(ret_val);
}

extern QCLI_Command_Status_t SetMaxSubnets(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QMESH_RESULT_T res;
    uint8_t prov_net_id;
    uint16_t max_sub_net;

    QCLI_LOGI (mesh_group, "SetMaxSubnets: Parameter_Count=%d, \n", Parameter_Count);
    PrintQCLIParameters(Parameter_Count, Parameter_List);

    if((Parameter_Count == 2) && (Parameter_List))
    {
        prov_net_id = 0xF & Parameter_List[0].Integer_Value;
        max_sub_net = Parameter_List[1].Integer_Value;

        res = QmeshSetMaxSubnet(prov_net_id, max_sub_net);
        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "SetMaxSubnets failed, ret=0x%x\n", res);
            ret_val = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, " One or more parameters is/are invalid\n");
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return ret_val;
}

#endif

extern QCLI_Command_Status_t UserNodeIdentity(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    uint8_t idx;

    if (APP_IS_PROXY_SUPPORTED())
    {
        for (idx = 0; idx < MAX_NUM_OF_SUB_NETWORK; idx++)
        {
            if (app_context.subnet_data[idx].valid)
            {
                app_context.subnet_data[idx].node_identity = QMESH_NODE_IDENTITY_RUNNING;
                app_context.subnet_data[idx].node_identity_cnt = 0;
            }
        }
    }
    else
    {
        QCLI_LOGE (mesh_group, "Proxy Not Supported in Features\n");
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return ret_val;
}

/* Send User LE Advert */
 QCLI_Command_Status_t SendUserLEAdvert (uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_USAGE_E;

    PrintQCLIParameters (Parameter_Count, Parameter_List);

    if((Parameter_Count == 4) && (Parameter_List))
    {
        uint8_t adv_len = Parameter_List[1].Integer_Value;
        uint8_t len = strlen((char *) Parameter_List[0].String_Value);
        uint8_t val;

        if (len > 64 && (len != adv_len))
            return QCLI_STATUS_USAGE_E;

        uint8_t * data = (uint8_t *)malloc(len);
        if(data == NULL)
        {
            QCLI_Printf (mesh_group, "Malloc failed for %d bytes \n", len);
            return QCLI_STATUS_ERROR_E;
        }

        val = AsciiToHex ((char *)Parameter_List[0].String_Value, len, data);
        if(val == 0xFF)
        {
            free (data);
            return QCLI_STATUS_USAGE_E;
        }

        QCLI_Printf (mesh_group, "advert payload:");
        QCLI_ARR_LOGI (mesh_group, data, len);

        /* Copy the data to an advertising buffer and send */
        qapi_BLE_Advertising_Data_t advert_data;
        memcpy (advert_data.Advertising_Data, data, len);
        if(QmeshSchedSetUserAdvertData (len, &advert_data) == QMESH_RESULT_SUCCESS)
        {
            /* Set default advert parameters for a non-connectable advert */
            qapi_BLE_GAP_LE_Advertising_Parameters_t gapLeAdvtParams;
            qapi_BLE_GAP_LE_Connectability_Parameters_t gapLeConnParams;
            gapLeAdvtParams.Scan_Request_Filter = QAPI_BLE_FP_NO_FILTER_E;
            gapLeAdvtParams.Connect_Request_Filter = QAPI_BLE_FP_NO_FILTER_E;
            gapLeAdvtParams.Advertising_Channel_Map = 0x07;/*All 3 channels*/
            gapLeAdvtParams.Advertising_Interval_Min = Parameter_List[2].Integer_Value;
            gapLeAdvtParams.Advertising_Interval_Max = Parameter_List[3].Integer_Value;
            gapLeConnParams.Connectability_Mode = QAPI_BLE_LCM_NON_CONNECTABLE_E;
            gapLeConnParams.Own_Address_Type = QAPI_BLE_LAT_PUBLIC_E;
            gapLeConnParams.Direct_Address_Type = QAPI_BLE_LAT_PUBLIC_E;

            /* Enable advert */
            if (QmeshSchedUserAdvertEnable (FALSE,
                                            &gapLeAdvtParams,
                                            &gapLeConnParams,
                                            GapLeEventCallback,
                                            0) == QMESH_RESULT_SUCCESS)
            {
                ret_val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
                QCLI_Printf (mesh_group, "FAILED to enable advertisement\n");
                ret_val = QCLI_STATUS_ERROR_E;
            }
        }

        free (data);
    }

    return (ret_val);
}

 QCLI_Command_Status_t StopUserLEAdvert (uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_ERROR_E;
    PrintQCLIParameters (Parameter_Count, Parameter_List);
    QmeshSchedUserAdvertDisable ();
    ret_val = QCLI_STATUS_SUCCESS_E;
    return (ret_val);
}

 QCLI_Command_Status_t ClearRpl(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res = QMESH_RESULT_SUCCESS;
    uint8_t prov_net_id;

    QCLI_LOGI (mesh_group, "\r\n\r\nClearRpl");

    /* Make sure that all of the parameters required for this function
     * appear to be at least semi-valid.
     */
    if((Parameter_Count == 1) && (Parameter_List))
    {
        prov_net_id = Parameter_List[0].Integer_Value & 0xF;
        res = QmeshClearRPL(prov_net_id);
        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "ClearReplayProtectionList failed, ret=0x%x\n", res);
            ret_val = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, " One or more parameters is/are invalid\n");
        ret_val = QCLI_STATUS_USAGE_E;
    }

    return ret_val;
}

 QCLI_Command_Status_t DisplayCompData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    const QMESH_ELEMENT_CONFIG_T *p_ele, *p_end_ele;

    QCLI_LOGE (mesh_group, "\r\nComposition Data");
    QCLI_LOGE (mesh_group, "\r\n cid: %04x", dev_comp->cid);
    QCLI_LOGE (mesh_group, "\r\n pid: %04x", dev_comp->pid);
    QCLI_LOGE (mesh_group, "\r\n vid: %04x", dev_comp->vid);
    QCLI_LOGE (mesh_group, "\r\n crpl: %04x", dev_comp->crpl);
    QCLI_LOGE (mesh_group, "\r\n fmask: %04x", dev_comp->feature_mask);
    QCLI_LOGE (mesh_group, "\r\n ele_num: %04x", dev_comp->num_elements);
    for(p_ele = &dev_comp->elements[0], p_end_ele = p_ele + dev_comp->num_elements;
        p_ele < p_end_ele;
        p_ele++)
    {
        QMESH_ELEMENT_DATA_T *ele_data = p_ele->element_data;
        QMESH_MODEL_DATA_T *p_model, *p_end_model;

        QCLI_LOGE (mesh_group, "\r\n Element #%u", (p_ele - &dev_comp->elements[0] + 1));
        QCLI_LOGE (mesh_group, "\r\n  loc: %04x", p_ele->loc);
        QCLI_LOGE (mesh_group, "\r\n  sigmodels:"); DisplayU16Array(p_ele->btsig_model_ids, p_ele->num_btsig_models);
        QCLI_LOGE (mesh_group, "\r\n  vemmodels:"); DisplayU32Array(p_ele->vendor_model_ids, p_ele->num_vendor_models);
        QCLI_LOGE (mesh_group, "\r\n  seq_num: %06x", ele_data->seq_num);
        QCLI_LOGE (mesh_group, "\r\n  unicast_addr: %04x", ele_data->unicast_addr);

        for(p_model = ele_data->model_data, p_end_model = p_model + p_ele->num_btsig_models + p_ele->num_vendor_models;
            p_model < p_end_model;
            p_model++)
        {
            QCLI_LOGE (mesh_group, "\r\n  Model #%u", (p_model - ele_data->model_data + 1));
            if(p_model->publish_state != NULL)
            {
                QCLI_LOGE (mesh_group, "\r\n   app_key_idx: %04x", p_model->publish_state->app_key_idx);
                QCLI_LOGE (mesh_group, "\r\n   publish_addr: %04x", p_model->publish_state->publish_addr);
            }

            if(p_model->subs_list != NULL)
            {
                QCLI_LOGE (mesh_group, "\r\n   subs_list"); DisplayU16Array(p_model->subs_list, 6);
            }

            if(p_model->app_key_list != NULL)
            {
                QCLI_LOGE (mesh_group, "\r\n   app_key_list"); DisplayU16Array(p_model->app_key_list, 6);
            }
        }
        QCLI_LOGE (mesh_group, "\r\n");
    }

    return QCLI_STATUS_SUCCESS_E;
}


/*! \brief Max number of LPNs supported at a time.
*/
#define MAX_NUM_OF_LPN_SUPPORTED                 (3)
/*! \brief Maximum allowed Subscription List Size per LPN.
*/
#define MAX_SUB_LIST_SIZE_PER_LPN                (8)
/*! \brief Friend Cache Size.per LPN
*/
#define FRIEND_CACHE_SIZE_PER_LPN                (16)

uint16_t lpn_sub_list[MAX_NUM_OF_LPN_SUPPORTED][MAX_SUB_LIST_SIZE_PER_LPN] = {{0}};
QMESH_LPN_INFO_T lpn_info_list[MAX_NUM_OF_LPN_SUPPORTED] = {{0}};
QMESH_FRND_CACHE_SLOT_T frnd_cache[MAX_NUM_OF_LPN_SUPPORTED][FRIEND_CACHE_SIZE_PER_LPN];

extern void QmeshEnableFriendFeature(bool enable)
{
    uint8_t recv_window = 0xFF;
    uint8_t num_lpn = MAX_NUM_OF_LPN_SUPPORTED;
    uint8_t idx;

    for(idx = 0; idx < 1; idx++)
    {
        QMESH_NETWORK_TRANSMIT_STATE_T tr_state;
        tr_state.transmit_count = 3;
        tr_state.transmit_interval = 0;
        QmeshSetNetworkTransmitState(idx, tr_state);
    }

    QmeshMemSet(lpn_info_list, 0x00, MAX_NUM_OF_LPN_SUPPORTED*sizeof(QMESH_LPN_INFO_T));
    QmeshMemSet(frnd_cache, 0x00, MAX_NUM_OF_LPN_SUPPORTED*FRIEND_CACHE_SIZE_PER_LPN*sizeof(QMESH_FRND_CACHE_SLOT_T));

    QmeshMemSet(lpn_sub_list, 0x00, MAX_NUM_OF_LPN_SUPPORTED*MAX_SUB_LIST_SIZE_PER_LPN*sizeof(uint16_t));

    for(idx = 0; idx < MAX_NUM_OF_LPN_SUPPORTED; idx++)
    {
        lpn_info_list[idx].frnd_valid = FALSE;

        lpn_info_list[idx].sub_list_size = MAX_SUB_LIST_SIZE_PER_LPN;
        lpn_info_list[idx].sub_list = &lpn_sub_list[idx][0];

        lpn_info_list[idx].len_frnd_cache = FRIEND_CACHE_SIZE_PER_LPN;
        lpn_info_list[idx].frnd_cache = &frnd_cache[idx][0];
    }

    QCLI_LOGI (mesh_group, "\nCalling QmeshEnableFriendMode\n");
    QmeshEnableFriendMode(enable, recv_window, num_lpn, lpn_info_list);
}
/* ------------------------------------------------------------------------------------------------------------ */
extern QCLI_Command_Status_t EnableFriendMode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    uint8_t enable = 0;
    PrintQCLIParameters (Parameter_Count, Parameter_List);

    /* Check the parameter count. There should be only 1 parameter
     * 1. Feature enable(1: Enable, 0: Disable)
    */
    if((Parameter_Count == 1) && (Parameter_List) &&
        ((Parameter_List[0].Integer_Value == 1) || (Parameter_List[0].Integer_Value == 0)))
    {
        enable = Parameter_List[0].Integer_Value;

        QCLI_LOGI (mesh_group, "\nFriend Enable: %d \n", enable);
        QmeshEnableFriendFeature(enable);
    }
    else
    {
        QCLI_LOGI (mesh_group, "Error");
        return QCLI_STATUS_USAGE_E;
    }
    return ret_val;
}

extern QCLI_Command_Status_t StartIVRecovery(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QMESH_RESULT_T result;
    QCLI_Command_Status_t ret_val = QCLI_STATUS_USAGE_E;

    QCLI_LOGI (mesh_group, "StartIVRecovery: Parameter_Count=%d, \n", Parameter_Count);

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if ((Parameter_Count == 2) && (Parameter_List))
    {
        result = QmeshSetIVRecoveryState((uint8_t)Parameter_List[0].Integer_Value,
                                         (bool)Parameter_List[1].Integer_Value);
        if (QMESH_RESULT_SUCCESS != result)
        {
            QCLI_LOGE (mesh_group, "QmeshSetIVRecoveryState : %d\n", result);
        }
        else
        {
            ret_val = QCLI_STATUS_SUCCESS_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, " One or more parameters is/are invalid\n");
    }

    return ret_val;
}


extern QCLI_Command_Status_t SaveStateAndReset(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
#if (QMESH_PS_IFCE_ENABLED == 1)
    /*Save only the keys which might not have been sync'd with NVM*/
    if ((app_context.device_provisioned) && (app_context.role == NODE_ROLE))
    {
        StopElementSequenceNoTimer ();
        StoreElementSequenceNo ();
        QCLI_LOGI (mesh_group, "Sequence number written to NVM\n");
    }
#endif
    QCLI_LOGI (mesh_group, "Performing Cold Reset.....\n");
    qapi_System_Reset ();
    return QCLI_STATUS_SUCCESS_E;
}

extern QCLI_Command_Status_t PrintMemHeapUsage(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    uint32_t total, free;

    if (QAPI_OK != qapi_Heap_Status(&total, &free))
    {
        QCLI_LOGE (mesh_group, "Error getting heap status\n");
        return QCLI_STATUS_ERROR_E;
    }

    QCLI_LOGE (mesh_group, "\n======================================");
    QCLI_LOGE (mesh_group, "\n           total       used       free\n");
    QCLI_LOGE (mesh_group, "Heap:   %8d   %8d   %8d\n", total, (total - free), free);
    QCLI_LOGE (mesh_group, "======================================\n");

    return QCLI_STATUS_SUCCESS_E;
}


