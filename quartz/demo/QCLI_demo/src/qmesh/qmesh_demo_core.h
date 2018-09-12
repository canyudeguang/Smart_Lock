/******************************************************************************
 *  Copyright 2017-2018 Qualcomm Technologies International, Ltd.
 *  All Rights Reserved.
 *  Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
#ifndef __QMESH_DEMO_CORE_H__
#define __QMESH_DEMO_CORE_H__

#include "qapi.h"
#include "qcli_api.h"
#include "qmesh_demo_composition.h"
#include "qmesh_types.h"
#include "qmesh_data_types.h"
#include "qapi_ble_gap.h"
#include "qmesh.h"
#include "qmesh_hal_ifce.h"
#include "qmesh_ble_gap.h"
#include "qmesh_ble_gatt.h"
#include "qmesh_ble_gatt_client.h"
#include "qmesh_ble_gatt_server.h"
#include "qmesh_sched.h"
#include "qmesh_qca402x_sched.h"
#include "qmesh_config_server.h"
#include "qmesh_config_client.h"
#include "qmesh_health_server.h"
#include "qmesh_health_client.h"
#include "qmesh_demo_debug.h"
#include "qmesh_demo_config.h"
#include "model_client_event_handler.h"
#include "qmesh_pts_test.h"
#include "qmesh_model_common.h"
#include "qmesh_model_config.h"
#include "qmesh_generic_onoff_handler.h"
#include "qmesh_generic_level_handler.h"
#include "qmesh_generic_poweronoff_handler.h"
#include "qmesh_generic_poweronoff_setup_handler.h"
#include "qmesh_light_hsl_handler.h"
#include "qmesh_light_hsl_hue_handler.h"
#include "qmesh_light_hsl_saturation_handler.h"
#include "qmesh_light_hsl_setup_handler.h"
#include "qmesh_light_lightness_handler.h"
#include "qmesh_light_lightness_setup_handler.h"
#include "qmesh_friendship.h"
/* Mesh device association state */
typedef enum
{
    app_state_not_provisioned = 0,  /* Application Initial State */
    app_state_provision_started,    /* Application state association started */
    app_state_provisioned,          /* Application state associated */
} app_association_state;

/* Key refresh states */
typedef enum
{
    app_key_refresh_state_init,
    app_key_refresh_state_idle,
    app_key_refresh_get,
    app_key_refresh_get_status_1,
    app_key_refresh_net_key_update,
    app_key_refresh_net_key_update_status_1,
    app_key_refresh_app_key_update,
    app_key_refresh_app_key_update_status_1,
    app_key_refresh_app_phase_set,
    app_key_refresh_app_phase_set_status_1,
    app_key_refresh_app_phase_set_1,
    app_key_refresh_app_end,
} app_key_refresh_state;

/* GATT Proxy Service Connection Info */
typedef struct
{
    QMESH_KEY_IDX_INFO_T        netkey_idx;
    QMESH_NODE_IDENTITY_T       node_identity;
    uint16_t                    node_identity_cnt;
    bool                        valid;
} APP_SUBNET_DATA_T;

/* GATT Proxy Client Connection Info */
typedef struct
{
    bool                   is_connected;
    uint8_t                dev_index;      /* device index */
    uint8_t                net_id;         /* network identifier */
    uint32_t               conn_id;        /* connection identifier */
    uint32_t               srvc_id;        /* service identifier */
    uint8_t                mtu;            /* mtu size */
    qapi_BLE_BD_ADDR_t     remote_addr;    /* remote device address */
} QMESHAPP_GATT_PROXY_CLIENT_INFO_T;

/* GATT Proxy Connectable Advert Info */
typedef struct
{
    QMESH_GATT_PROXY_STATE_T     proxy_state;
    uint8_t                      net_idx;
    uint16_t                     elm_addr;
    bool                         is_proxy_advrt_enabled;
    QMESH_TIMER_HANDLE_T         proxy_advrt_tmr_hndl;
    qapi_BLE_Advertising_Data_t  advert_data;
    qapi_BLE_GAP_LE_Advertising_Parameters_t     advert_params;
    qapi_BLE_GAP_LE_Connectability_Parameters_t  conn_params;
} QMESHAPP_GATT_PROXY_CONN_ADVRT_INFO_t;

/* GATT Unprovisioned device list */
typedef struct
{
    qapi_BLE_BD_ADDR_t          device_addr;
    uint8_t                     uuid[QMESH_UUID_SIZE_OCTETS];
} QMESHAPP_UNPROV_DEVICE_FOUND_LIST_t;

typedef struct
{
    QMESH_SUBNET_KEY_IDX_INFO_T net_key;
    uint8_t                     attention_timer;
    uint16_t                    element_addr;
} QMESHAPP_DEVICE_PROVISIONING_INFO_t;

/* GATT Proxy Device List */
typedef struct
{
    qapi_BLE_BD_ADDR_t          device_addr;
    uint8_t                     uuid[QMESH_UUID_SIZE_OCTETS];
    QMESH_PROXY_ADV_ID_TYPE_T   advrt_type;
    uint8_t                     networkID[8];
    uint8_t                     hash[8];
    uint8_t                     random[8];
} QMESHAPP_PROXY_DEVICE_FOUND_LIST_T;

extern QMESH_GENERIC_LEVEL_CONTEXT_T        g_gen_lvl_context[3];
extern QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T  g_onoff_model_context;
extern QMESH_GEN_POWERONOFF_DATA_T          g_pwronoff_context;
extern QMESH_LIGHTNESS_MODEL_CONTEXT_T      g_lightness_server_context;
extern QMESH_LIGHT_HSL_MODEL_CONTEXT_T      g_hsl_server_context;
extern QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T  g_hsl_hue_server_context;
extern QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T  g_hsl_sat_server_context;
extern QMESH_GENERIC_DTT_CONTEXT_T g_gen_dtt_context;
#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
extern QMESH_GENERIC_POWERLEVEL_CONTEXT_T g_pwr_lvl_context;
#endif
/******************************************************************************
 *Provisioning Data
 *****************************************************************************/
/*! \brief QMesh Provisioned Device List */
typedef struct QMESH_PEER_DEVICE_KEY_LIST
{
    uint16_t   prov_net_id;
    uint16_t   device_key[QMESH_SECURITY_KEY_HWORDS];
    uint8_t    num_elems;
    uint16_t   elem_addr;
    struct QMESH_PEER_DEVICE_KEY_LIST *next;
} QMESH_PEER_DEVICE_KEY_LIST;


/*! \brief QMesh Application Device Info */
typedef struct
{
    bool                            is_connected;
    uint32_t                        connection_id;
    uint16_t                        mtu;
    qapi_BLE_BD_ADDR_t              remote_addr;
    qapi_BLE_GAP_LE_Address_Type_t  remote_addr_type;
    bool                            stack_bearer_added; /* Flag to indiacte that bearer is added to the stack*/
    QMESH_BEARER_HANDLE_T           bearer_handle;
} QMESHAPP_DEVICE_INFO_T;


typedef struct
{
    uint8_t                         role; /* Role */
    /* Advertisment Bearer */
    QMESH_BEARER_HANDLE_T           adv_bearer_handle[MAX_NUM_OF_ADV_BEARER];

    /* Data in node role - Start */
    bool                            device_provisioned;
    uint16_t                        oob_info;
    bool                            prov_via_gatt;
    uint32_t                        prov_conn_id;  /* Connection ID for Provisioning Server/Client. */
    bool                            is_proxy_srvc_enabled;
    bool                            handle_oob_act_evt;

    /*Connected device info*/
    QMESHAPP_DEVICE_INFO_T          deviceInfo[MAX_NUM_OF_GATT_CONN];

    APP_SUBNET_DATA_T               subnet_data[MAX_NUM_OF_SUB_NETWORK];

    QMESH_PEER_DEVICE_KEY_LIST      *peer_device_key_list;

    bool                            scan_over_pb_adv;
    bool                            scan_over_pb_gatt;
    bool                            scan_for_prov_srvc;
    uint8                           dev_idx;
    QMESH_DEVICE_UUID_T             unprovisioned_devices[QMESH_MAX_NUM_OF_PROVISIONABLE_DEVICES];

} QMESHAPP_CONTEXT_T;


extern QCLI_Command_Status_t InitializeMesh(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t CreateDeviceInstance(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t ShutdownBluetooth(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t EnableMeshBearer(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SetProvisioningData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SendUnProvDeviceBeacon(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t ConfigureRole(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SetOobData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t AddNetwork(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t AddApplicationKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t RemoveNetwork(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t StartDeviceDiscovery(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t StopDeviceDiscovery(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t DisplayUnProvDevList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t ProvisionDeviceOverGatt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t StartProvisionDevice(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t StopProvision(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SetDeviceList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t UpdateKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t UpdateKeyRefreshPhase(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t UpdateModelAppBindList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SelectPeerCapability(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t IncrementNetworkIVIndex(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t DisplayDiscoveredProxyDevices(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t FindNodeFromProxyDevList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t EstablishGattProxyConnection(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SendGattProxySetFilterType(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SendGattProxyAddRemoveAddr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t ListConnectedGattDevices(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t DisconnectGattProxyConnection(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SendSecureNwBeacon(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SendMessage(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SetNetworkIVIndex(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t AddSelfDeviceKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t AddPeerDeviceKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t DeviceReset(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t EnableFriendMode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t DiscoverFriendNodes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t EstablishFriendship(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t FriendWakeupStack(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t FriendGetData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t AdvtForProvOverGatt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SendUserLEAdvert (uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t StopUserLEAdvert (uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t ClearRpl(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SetNetworkIVState(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SetSecureNetworkBeaconState(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SetSecureNetworkBeaconPeriod(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t EnableGattProxyService(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SetStackPTSmode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t DisplayCompData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SendConfigClientMsg(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SetMaxSubnets(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t UserNodeIdentity(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t AddDelAdvBearers(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t ClearReplayProtectionEntries(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t StartIVRecovery(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t EnableIVTest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SendIVTestSignal(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t SaveStateAndReset(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Command_Status_t PrintMemHeapUsage(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

#endif /* __QMESH_DEMO_CORE_H__ */

