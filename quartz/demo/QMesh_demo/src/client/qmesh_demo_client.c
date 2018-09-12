/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*
  * This file contains functions and definitions for Client node.
 */
/*****************************************************************************/
#include "qmesh_sched.h"
#include "qmesh_model_common.h"
#include "qmesh_demo_composition.h"
#include "qmesh_demo_macros.h"
#include "qmesh_demo_nvm_utilities.h"
#include "qmesh_demo_platform.h"
#include "qmesh_demo_utilities.h"

#include "model_client_common.h"
#include "generic_onoff_client.h"

#include "qmesh_cache_mgmt.h"
#include "qmesh_generic_onoff_handler.h"
/******************************************************************************
 *  Global Data
 *****************************************************************************/
/* Pointer to hold the device composition */
const QMESH_DEVICE_COMPOSITION_DATA_T* dev_comp;
/* Default TTL value used to send model messages. Referred by client models as well */
int8_t TTL = QCLI_DFLT_TTL;
/* Health model state data */
extern QMESH_HEALTH_SERVER_STATE_T   g_health_server_state;
/* Health model data */
extern QMESH_HEALTH_MODEL_DATA_T current_fault;
/* Timer handle for sending upprovisioned beacons */
extern QMESH_TIMER_GROUP_HANDLE_T app_timer_ghdl;

/******************************************************************************
 *  Private Data
 *****************************************************************************/
#define QMESH_HEALTH_PERIOD_DIVISOR_INVALID         (0x10u)
#define QMESH_HEALTH_PUB_PERIOD_DISABLED            (0x00000000u)
#define QMESH_HEALTH_PUB_TIMER_INVALID_HANDLE       (0)
static QMESH_TIMER_HANDLE_T    unprov_dev_beacon_tid;
/***********   Network configuration details   ******************/
/* OOB public key used during provisioining. The stack generates and copies here for application reference */
static uint16_t      dev_pub_key[32];
/* Global Network key index */
static QMESH_SUBNET_KEY_IDX_INFO_T global_nw_key_index = {.prov_net_idx = 0, .key_idx = 0};
/* Global Application key index */
//static uint16_t global_app_key_index = 0;
/* Pointer to Application context */
static QMESHAPP_CONTEXT_T *app_cntxt;

/* Various values of QMESH_PROVISION_STATUS_EVENT sent during provisioning */
static const const char *prov_status[QMESH_PROV_STATUS_MAX + 1] =
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

/* Local Device Identifier Data */
static QMESH_DEVICE_IDENTIFIER_DATA_T device_data =
{
    {0x691B, 0xDC08, 0x1021, 0x0B0E,
     0x0A0C, 0x000B, 0x0E0A, 0x0606},   /* uuid */
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

/******************************************************************************
 * Private Function Prototypes
 *****************************************************************************/
static void ProcessMeshEvent (QMESH_APP_EVENT_DATA_T event_data_cb);
static bool CreateBeaconHandlerTimer(void);
static void UnProvDeviceBeaconAdvertTimeoutHandler(QMESH_TIMER_HANDLE_T tid, void* data);
static void AddSubnetKey(QMESH_SUBNET_KEY_IDX_INFO_T key_info);
static void HandleAppKeyBindEvent(QMESH_APP_EVENT_DATA_T event_data);

/******************************************************************************
 * Private Function Implementations
 *****************************************************************************/

/*----------------------------------------------------------------------------*
 * HandleProvisionCompleteEvent
 */
/*! \brief Handles QMESH_PROV_COMPLETE_DATA event received from the mesh stack in 
 *             during provisioning. The provisioned device details are stored the peer device list.
 *             model messages. The User can configure the remote node after this event like Adding 
 *             Application Key and binding supported models using Config model messages.
 *
 *  \param event_data  Qmesh application event.
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
static void HandleProvisionCompleteEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_PROV_COMPLETE_DATA_T *prov_info;

    prov_info = (QMESH_PROV_COMPLETE_DATA_T *) event_data.param;

    /* Check if provisioning is successfull */
    if(QMESH_RESULT_SUCCESS == event_data.status)
    {
        /* Print the device key */
        if (NULL != prov_info->device_key)
        {
            QCLI_LOGI (mesh_group, "\r\nDevice key is ");
            DisplayU16Array(prov_info->device_key, 8);
            QCLI_LOGI (mesh_group, "\n");
        }

        QCLI_LOGE (mesh_group, "Provisioning completed, element address assigned=0x%x\n", prov_info->elm_addr);

        /* Set the application state to Provisioned */
        app_cntxt->app_state = app_state_provisioned;

        /* Store the provisioned network information */
        global_nw_key_index.prov_net_idx = prov_info->key_info.prov_net_idx;
        global_nw_key_index.key_idx = prov_info->key_info.key_idx;

#if (QMESH_PS_IFCE_ENABLED == 1)
        NVMWrite_ProvState(true);
        StoreElementSequenceNo ();
        NVMWrite_PrimaryElementAddress(prov_info->elm_addr);
#endif
        QmeshUpdateModelWithUnicastAddr();
        AddSubnetKey(prov_info->key_info);

        /* Provisioning is successful. Remote side should send "Config AppKey Add message" to 
             add an Application key followd by "Config Model App Bind" to bind supported model
             (Generic OnOff Client model) to the Application key. 
             QMESH_CONFIG_APP_BIND_UNBIND_EVENT event is will 
             received via ProcessMeshEvent once the model is bound to the application key.
        */
    }
    else
    {
        QCLI_LOGE(mesh_group, "Provisioning failed with res: 0x%x\n", event_data.status);
        app_cntxt->app_state = app_state_initialized;
    }
}

/*----------------------------------------------------------------------------*
 * ProcessMeshEvent
 */
/*! \brief Initializes Health server model state data.
 *
 *  \param dev_comp Pointe to the composition data.
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
static void QmeshInitHealthServerData(const QMESH_DEVICE_COMPOSITION_DATA_T *dev_comp)
{
    /* Following example is shown considering health server is registed only in primary element */
    g_health_server_state.company_id = dev_comp->cid;
    g_health_server_state.pub_info = &client_publish_state [1];
    g_health_server_state.fault = (QMESH_HEALTH_MODEL_DATA_T *) &current_fault;
    g_health_server_state.fast_period_divisor = QMESH_HEALTH_PERIOD_DIVISOR_INVALID;
    g_health_server_state.pub_interval = QMESH_HEALTH_PUB_PERIOD_DISABLED;
    g_health_server_state.pub_tid = QMESH_HEALTH_PUB_TIMER_INVALID_HANDLE;
}

/*----------------------------------------------------------------------------*
 * AddSubnetKey
 */
/*! \brief Stores the Subnetwork key details in the application context. Also updates NVM if NVM 
 *             is supported.
 *
 *  \param key_info Qmesh application event.
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
static void AddSubnetKey(QMESH_SUBNET_KEY_IDX_INFO_T key_info)
{
    uint32_t idx;
    uint32_t free_slot = MAX_NUM_OF_SUB_NETWORK;

    for (idx = 0; idx < MAX_NUM_OF_SUB_NETWORK; idx++)
    {
        if (!app_cntxt->subnet_data[idx].valid)
        {
            if (MAX_NUM_OF_SUB_NETWORK == free_slot)
            {
                free_slot = idx;
            }
        }
        else if ((app_cntxt->subnet_data[idx].netkey_idx.prov_net_idx == key_info.prov_net_idx) &&
                 (app_cntxt->subnet_data[idx].netkey_idx.key_idx == key_info.key_idx))
        {
            free_slot = MAX_NUM_OF_SUB_NETWORK;
            break;
        }
    }

    if (MAX_NUM_OF_SUB_NETWORK != free_slot)
    {
        app_cntxt->subnet_data[free_slot].valid = TRUE;
        app_cntxt->subnet_data[free_slot].node_identity = QMESH_NODE_IDENTITY_NOT_SUPPORTED;
        app_cntxt->subnet_data[free_slot].netkey_idx = key_info;

#if (QMESH_PS_IFCE_ENABLED == 1)
        NVMWrite_SubNetIdentityUpdate(&app_cntxt->subnet_data[idx], false);
#endif

        QCLI_LOGE(mesh_group, "\nAdd Subnet : 0x%01x : 0x%03x\n", key_info.prov_net_idx, key_info.key_idx);
    }
}

/*----------------------------------------------------------------------------*
 * HandleAppKeyBindEvent
 */
/*! \brief Handles QMESH_CONFIG_APP_BIND_UNBIND_EVENT event received from stack when a model
 *             is either bound/unbound to an AppKey. Also updates NVM if NVM is supported.
 *
 *  \param event_data Subnetwork key.
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
static void HandleAppKeyBindEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_APP_BIND_EVENT_T *bind_event;
    const QMESH_ELEMENT_CONFIG_T const *p_ele;
#if (QMESH_PS_IFCE_ENABLED == 1)
    uint8_t elementid, modelid;
    const QMESH_MODEL_DATA_T const *p_model;
#endif

    bind_event = (QMESH_CONFIG_APP_BIND_EVENT_T *)event_data.param;
    p_ele = bind_event->ele_model_info.p_ele;
    QCLI_LOGI (mesh_group, "\r\nBind state: %d", bind_event->bind_state);
    QCLI_LOGI (mesh_group, "\r\nElement Address: %d", p_ele->element_data->unicast_addr);
    QCLI_LOGI (mesh_group, "\r\nApp Key Index: %d", bind_event->app_key_idx);
    QCLI_LOGI (mesh_group, "\r\nModel ID: %d", bind_event->ele_model_info.model_id);
    QCLI_LOGI (mesh_group, "\r\nModel Type: %d", bind_event->ele_model_info.model_type);
#if (QMESH_PS_IFCE_ENABLED == 1)
    p_model = bind_event->ele_model_info.p_model;
    elementid =  p_ele - &dev_comp->elements[QMESH_PRIMARY_ELEMENT_INDEX];
    modelid = p_model - &dev_comp->elements[elementid].element_data->model_data[0];
    if (QMESH_CONFIG_APP_BIND == bind_event->bind_state)
        NVMWrite_ModelAppKeyAdd (elementid, modelid, bind_event->app_key_idx);
    else
        NVMWrite_ModelAppKeyDelete (elementid, modelid, bind_event->app_key_idx);
#endif

    QCLI_LOGE (mesh_group, "\r\nNode ready to send Generic OnOff messages \n");

    /*
        Now this device can send Generic OnOff Client message to the Generic OnOff server node.
        Use GenericOnOffSet or GenericOnOffGet commands to send the message.

        Note: Generic OnOff server node should have been provisioned to send the OnOff client
        messages
    */
}


/*----------------------------------------------------------------------------*
 * ProcessMeshEvent
 */
/*! \brief This is the Application callback function registerd vis QmeshInit() API. This function is called by
 *             the Qmesh stack to send asynchronous events to the Application.
 *
 *             For the complete list of event refer QMESH_EVENT_T.
 *
 *  \param event_data_cb Qmesh application event.
 *
 *  \returns QmeshResult.
 */
/*---------------------------------------------------------------------------*/
static void ProcessMeshEvent (QMESH_APP_EVENT_DATA_T event_data_cb)
{
    QMESH_EVENT_T operation_event = event_data_cb.event;

    switch(operation_event)
    {
        case QMESH_PROVISION_INVITE_EVENT:
        {
            QMESH_PROV_INVITE_DATA_T *prov_inv_info = (QMESH_PROV_INVITE_DATA_T *)event_data_cb.param;
            QCLI_LOGE (mesh_group, "\r\nProvisioning started...\n",
                                    prov_inv_info->att_duration);
            /* Blink LED or other forms supported on the platform to indicate provisioning has started */
        }
        break;
        case QMESH_CONFIG_NETKEY_ADD_EVENT:
        {
            if (event_data_cb.status == QMESH_RESULT_SUCCESS)
            {
                AddSubnetKey(*(QMESH_SUBNET_KEY_IDX_INFO_T *)event_data_cb.param);
            }
        }
        break;
        case QMESH_CONFIG_APP_BIND_UNBIND_EVENT:
        {
            QCLI_LOGE (mesh_group, "\r\nModel App Key bind successfull\n");
            HandleAppKeyBindEvent(event_data_cb);
        }
        break;
        case QMESH_PROVISION_STATUS_EVENT:
        {
            QMESH_PROV_STATUS_T st = *(QMESH_PROV_STATUS_T *) event_data_cb.param;
            QCLI_LOGE (mesh_group, "\r\nProvisioning : %s (%d)",
                       prov_status[st >= QMESH_PROV_STATUS_MAX ? QMESH_PROV_STATUS_MAX : st ], st);
        }
        break;
        case QMESH_PROVISION_COMPLETE_EVENT:
        {
            HandleProvisionCompleteEvent (event_data_cb);
            /* Stop blinking LED or other forms supported on the platform to indicate provisioning is complete */
        }
        break;
        case QMESH_PROV_LINK_OPEN_EVENT:
        {
            app_cntxt->app_state = app_state_provision_started;
        }
        break;
        default:
            QCLI_LOGE (mesh_group, "Received unhandled event=0x%x\n", operation_event);
            break;
    }
}

/*----------------------------------------------------------------------------*
 * UnProvDeviceBeaconAdvertTimeoutHandler
 */
/*! \brief This function sends the unprovisioned Beacon advertisement and creates a timer again to send
 *             the beacon until the provisioning is started or cancelled.
 *
 *  \param tid      timer ID
 *  \param data   NULL
 *
 *  \returns none.
 */
/*---------------------------------------------------------------------------*/
static void UnProvDeviceBeaconAdvertTimeoutHandler(QMESH_TIMER_HANDLE_T tid, void* data)
{
    QmeshResult result;
    if(tid == unprov_dev_beacon_tid)
    {
        /* Delete the current timer */
        QmeshTimerDelete (&app_timer_ghdl, &tid);
        unprov_dev_beacon_tid = QMESH_TIMER_INVALID_HANDLE;

        if(app_cntxt->app_state == app_state_discoverable)
        {
            /* Send unprovisioned beacon */
            result = QmeshSendUnProvDeviceBeacon(DEVICE_INSTANCE, FALSE);
            if(result == QMESH_RESULT_SUCCESS)
            {
                /* Restart timer to send unprovisioned beacon again */
                CreateBeaconHandlerTimer();
            }
            else
            {
                QCLI_LOGE (mesh_group, "QmeshSendUnProvDeviceBeacon failed res=0x%x\n", result);
            }
        }
    }
}

/*----------------------------------------------------------------------------*
 * CreateBeaconHandlerTimer
 */
/*! \brief Creates beacon timer of 2 seconds plus a random delay to send unprovisioned beacons.
 *
 *  \param none
 *
 *  \returns bool   TRUE - if timer is created, FALSE - otherwise
 */
/*---------------------------------------------------------------------------*/
static bool CreateBeaconHandlerTimer(void)
{
    /* Generate a random delay between 0 to 511 ms */
    uint32_t random_delay = ((uint32_t)(QmeshRandom16() & 0x0FFF));

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
        return FALSE;
    }
    return TRUE;
}

/******************************************************************************
 *  Public Functions called from the QCLI Command Line Interface
 *****************************************************************************/

/*----------------------------------------------------------------------------*
 * InitializeMesh
 */
/*! \brief Initializes BT controller, UART for logging the output and initializes the Qmesh stack in the node
 *             role.
 *
 *  \param app_context  Pointer to the Application context.
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
QmeshResult InitializeMesh(QMESHAPP_CONTEXT_T *app_context)
{
    QmeshResult           res=QMESH_RESULT_SUCCESS;
    static int32_t bt_stack_id;

    /* Store the application context */
    app_cntxt = app_context;

    /* Check if Application already initialized, if yes, do nothing */
    if(app_cntxt->app_state != app_state_idle)
    {
       QCLI_LOGE (mesh_group, "Stack already initialized!!\n");
       return res;
    }

    /* Initialize BT stack */
    bt_stack_id = InitializePlatform();

    if(bt_stack_id < 0)
    {
       QCLI_LOGE (mesh_group, "Stack  initialization failed!!\n");
       return QMESH_RESULT_FAILURE;
    }

    /* Initialize Mesh packet scheduler.*/
    res = QmeshSchedInit((void *)&bt_stack_id);
    if(res != QMESH_RESULT_SUCCESS)
    {
       QCLI_LOGE (mesh_group, "QmeshSchedStart failed ret=0x%x!!\n", res);
       return res;
    }

    /* Initialize Composition Data. */
    DemoCompAppKeyListInit();
    DemoCompSubsListInit();

    /* Start mesh scheduler to schedule LE scan and advertisment */
    res = QmeshSchedStart();
    if(res != QMESH_RESULT_SUCCESS)
    {
       QCLI_LOGE (mesh_group, "QmeshSchedStart failed ret=0x%x!!\n", res);
       return res;
    }

    /* Initialize the Qmesh stack. ProcessMeshEvent will be called for all the asynchronous events from
         the Qmesh stack */
    res = QmeshInit(QMESH_APP_MAX_NETWORKS, ProcessMeshEvent);
    if(res != QMESH_RESULT_SUCCESS)
    {
       QCLI_LOGE (mesh_group, "QmeshInit failed, ret=0x%x\n", res);
       return res;
    }

    /* Enable PB-ADV (BT LE advertising bearer) for the stack */
    res = QmeshAddBearer(QMESH_BEARER_TYPE_LE_ADV_PROVISIONING, NULL, 
                    0, &(app_cntxt->bearer_handle[QMESH_BEARER_TYPE_LE_ADV_PROVISIONING]));
    if(res != QMESH_RESULT_SUCCESS)
    {
       QCLI_LOGE (mesh_group, "QmeshAddBearer failed, ret=0x%x\n", res);
       return res;
    }

    res = QmeshAddBearer(QMESH_BEARER_TYPE_LE_ADV_NETWORK, NULL,
                         0, &app_cntxt->bearer_handle[QMESH_BEARER_TYPE_LE_ADV_NETWORK]);
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
    }

    /* Point to the device composition of the local device */
    dev_comp = &server_device_composition;

    /* Initialize any persistent storage module if available on the platform */
#if (QMESH_PS_IFCE_ENABLED == 1)
    if (NVMInit (app_cntxt) == QMESH_RESULT_FAILURE)
    {
        QCLI_LOGE (mesh_group, "NVM Initialization failed\n");
        return res;
    }
#endif

    /* Set the Heath server model data */
    QmeshInitHealthServerData(dev_comp);

    /* Create a new device instance */
    res = QmeshCreateDeviceInstance(DEVICE_INSTANCE, &device_data, dev_comp);
    if(res != QMESH_RESULT_SUCCESS)
    {
       QCLI_LOGE (mesh_group, "QmeshRegisterAppCallback  Failed ret=0x%x\n", res);
       return res;
    }

    /* Set Node Role */
    res = QmeshSetProvisioningRole(DEVICE_INSTANCE, FALSE);

    if(res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "QmeshSetProvisioningRole failed (0x%x)\n", res);
        return res;
    }

    /* Now, Start discoverable so that provisioner can scan for this device to provision */
    return res;
}

/*----------------------------------------------------------------------------*
 * StartDiscoverable
 */
/*! \brief Starts sending unprovisioned device beacon continously to get associated to a network.
 *             Beconing will be stopped once provisioning is initiated from the remote side or cancelled via
 *             StopDiscoverable command from the Application interface.
 *             The UUID sent in the beacon is set in 'device_data'.
 *
 *  \param None
 *
 *  \returns QmeshResult.
 */
/*---------------------------------------------------------------------------*/
QmeshResult StartDiscoverable(void)
{
    QmeshResult           res=QMESH_RESULT_SUCCESS;

    /* Send unprovisioned beacon */
    if(!CreateBeaconHandlerTimer())
    {
        QCLI_LOGE (mesh_group, "QmeshSendUnProvDeviceBeacon failed, ret=0x%x\n", res);
        return QMESH_RESULT_FAILURE;
    }
    return res;
}

/*----------------------------------------------------------------------------*
 * StopDiscoverable
 */
/*! \brief  Stops sending unprovisioned beacons or stops any ongoing provisioning procedure if started.
 *
 *  \param None.
 *
 *  \returns QmeshResult.
 */
/*---------------------------------------------------------------------------*/
QmeshResult StopDiscoverable(void)
{
    QmeshResult  res=QMESH_RESULT_SUCCESS;

    /* Stop device discovery */
    if(app_cntxt->app_state == app_state_discoverable)
    {
        /* Stop beaconing timer */
        QmeshTimerDelete (&app_timer_ghdl, &unprov_dev_beacon_tid);
        unprov_dev_beacon_tid = QMESH_TIMER_INVALID_HANDLE;
    }
    /* If provisioning is inprogress then stop provisioning procedure */
    else if(app_cntxt->app_state == app_state_provision_started) 
    {
        res = QmeshStopProvision();
        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "QmeshStopProvision failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }        
    }
    return res;
}

/*----------------------------------------------------------------------------*
 * GenericOnOffModelGet
 */
/*! \brief   Sends Generic OnOff Get model message to the server with element address set in 
 *              QMESH_DEMO_SERVER_ELEMENT_ADDRESS.
 *              QMESH_GENERIC_ONOFF_CLIENT_STATUS will be received by 
 *              QmeshGenericOnOffClientHandler() with the OnOff status message.
 *
 *  \param none
 *
 *  \returns QmeshResult.
 */
/*---------------------------------------------------------------------------*/
QmeshResult GenericOnOffModelGet(void)
{
    QmeshResult res;    
    QMESH_MSG_HEADER_T header;

    QmeshMemSet(&header, 0x00, sizeof(QMESH_MSG_HEADER_T));

    /* AppKeyIdx */
    header.keyInfo.prov_net_idx = (global_nw_key_index.prov_net_idx & PROVISIONED_NET_ID_MASK) >> 12;
    header.keyInfo.key_idx = (global_nw_key_index.key_idx & KEY_ID_MASK);
    /* dstAddr */
    header.dstAddr = QMESH_DEMO_SERVER_ELEMENT_ADDRESS;
    /* elmtId */
    header.srcAddr = QMESH_DEMO_CLIENT_ELEMENT_ADDRESS;
    header.reliable = TRUE;
    header.use_frnd_keys = FALSE;
    /* ttl */
    header.ttl = TTL;
  
    /* call model API */
    res = GenericOnoffClientGet(&header);

    if(res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "  GenericOnoffClientGet failed, ret=0x%x\n", res);
    }
    return res;
}

/*----------------------------------------------------------------------------*
 * GenericOnOffModelSet
 */
/*! \brief  Sends Generic OnOff Set model message to the server with element address set in 
 *              QMESH_DEMO_SERVER_ELEMENT_ADDRESS.
 *              QMESH_GENERIC_ONOFF_CLIENT_STATUS will be received by 
 *              QmeshGenericOnOffClientHandler() with the OnOff status message.
 *
 *  \param onoff                        OnOff state value
 *  \param tid                            Transaction Identifier of the message
 *  \param transitionTime           Transition time
 *  \param delay                        Delay time
 *  \param validTransitionTime    TRUE if transitionTime and Delay parameters are present else FALSE
 *
 *  \returns QmeshResult.
 */
/*---------------------------------------------------------------------------*/
QmeshResult GenericOnOffModelSet(uint8_t onoff,
    uint8_t tid,
    uint8_t transitionTime,
    uint8_t delay,
    bool validTransitionTime)
{
    QmeshResult res;
    QMESH_MSG_HEADER_T header;
    GENERIC_ONOFF_CLIENT_SET_T data;

    QmeshMemSet(&header, 0x00, sizeof(QMESH_MSG_HEADER_T));
    QmeshMemSet(&data, 0x00, sizeof(GENERIC_ONOFF_CLIENT_SET_T));

    /* AppKeyIdx */
    header.keyInfo.prov_net_idx = (global_nw_key_index.prov_net_idx & PROVISIONED_NET_ID_MASK) >> 12;
    header.keyInfo.key_idx = (global_nw_key_index.key_idx & KEY_ID_MASK);
    /* dstAddr */
    header.dstAddr = QMESH_DEMO_SERVER_ELEMENT_ADDRESS;
    /* elmtId */
    header.srcAddr = QMESH_DEMO_CLIENT_ELEMENT_ADDRESS;
    header.reliable = TRUE; 
    header.use_frnd_keys = FALSE;
    /* ttl */
    header.ttl = TTL;

    /* onOff */
    data.onOff = onoff;
    /* tid */
    data.tid = tid;

    /* optional params */
    if(validTransitionTime)
    {
        /* transitionTime */
        data.transitionTime = transitionTime;
        /* delay */
        data.delay = delay;
        /* update transition time validity flag */
        data.validTransitionTime = TRUE;
    }    

    /* Send Generic OnOff set model client API */
    res = GenericOnoffClientSet(&header, &data);

    if(res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "  GenericOnoffClientSet failed, ret=0x%x\n", res);
    }

    return res;
}
