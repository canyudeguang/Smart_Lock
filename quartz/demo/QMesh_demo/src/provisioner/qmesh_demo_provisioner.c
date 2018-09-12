/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*
  * This file contains functions and definitions for Provisioning
 */
/*****************************************************************************/
#include "qmesh_sched.h"
#include "qmesh_model_common.h"
#include "qmesh_demo_composition.h"
#include "qmesh_demo_macros.h"
#include "qmesh_demo_nvm_utilities.h"
#include "qmesh_demo_platform.h"
#include "qmesh_demo_utilities.h"

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

/******************************************************************************
 *  Private Data
 *****************************************************************************/
#define QMESH_HEALTH_PERIOD_DIVISOR_INVALID         (0x10u)
#define QMESH_HEALTH_PUB_PERIOD_DISABLED            (0x00000000u)
#define QMESH_HEALTH_PUB_TIMER_INVALID_HANDLE       (0)
/***********   Network configuration details   ******************/
/* Network key - 128 bits */
static const uint16_t net_key[QMESH_NETWORK_KEY_SIZE_HWORDS] = 
                    {0x7dd7, 0x364c, 0xd842, 0xad18, 0xc17c, 0x2b82, 0x0c84, 0xc3e0};
/* Application key - 128 bits */
static uint16_t app_key[QMESH_APP_KEY_SIZE_HWORDS] = 
                    {0x6396, 0x4771, 0x734f, 0xbd76, 0xe3b4, 0x0519, 0xd1d9, 0x4a48};
/* Global Network key index */
static QMESH_SUBNET_KEY_IDX_INFO_T global_nw_key_index = {.prov_net_idx = 0, .key_idx = 0};
/* Global Application key index */
static uint16_t global_app_key_index = 0;
/* Remote Device Element Count */
static uint8_t remote_num_elms = 0;
/* OOB public key used during provisioining. The stack generates and copies here for application reference */
static uint16_t      dev_pub_key[32];
/* Timer handle */
static QMESH_TIMER_HANDLE_T    app_key_tid;
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
     0x0A0C, 0x000B, 0x0E0A, 0x0505},   /* uuid */
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
static QmeshResult AddRemoteApplicationKey(void);
static QmeshResult BindModelApplicationKey(void);
static QmeshResult AddNetwork(void);
static QmeshResult AddApplicationKey(void);

/******************************************************************************
 * Private Function Implementations
 *****************************************************************************/

/*----------------------------------------------------------------------------*
 * AddApplicationKeyTimeoutHandler
 */
/*! \brief This function sends the Add Application Key config message to the remote device.
 *             QMESH_CONFIG_APP_KEY_STATUS_EVENT will be received with the status via 
 *             ProcessMeshEvent().
 *
 *  \param tid      timer ID
 *  \param data   NULL
 *
 *  \returns none.
 */
/*---------------------------------------------------------------------------*/
static void AddApplicationKeyTimeoutHandler(QMESH_TIMER_HANDLE_T tid, void* data)
{
    if(tid == app_key_tid)
    {
        /* Delete the current timer */
        QmeshTimerDelete (&app_timer_ghdl, &tid);
        app_key_tid = QMESH_TIMER_INVALID_HANDLE;

        if(app_cntxt->app_state == app_state_provisioned)
        {
            /* Add Application key to the remote device */
            if(AddRemoteApplicationKey() == QMESH_RESULT_SUCCESS)
            {
                app_cntxt->app_state = app_state_application_key_add;
            }
            else
            {
                QCLI_LOGE (mesh_group, "%s: AddRemoteApplicationKey failed\n", __FUNCTION__);
            }
        }
        else
        {
            /* Application not in valid state */
            QCLI_LOGE (mesh_group, "%s: Could not add application key - Application in improper state",
                                    __FUNCTION__);
        }
    }
}


/*----------------------------------------------------------------------------*
 * CreateAddAppKeyTimer
 */
/*! \brief Creates timer of 2 seconds to send Add Application key config message.
 *
 *  \param none
 *
 *  \returns bool   TRUE - if timer is created, FALSE - otherwise
 */
/*---------------------------------------------------------------------------*/
static bool CreateAddAppKeyTimer(void)
{
    /* Start a timer to send Add App Key config message after a delay of 2 seconds specified in
         QMESH_DEMO_APP_KEY_ADD_DELAY.
    */
    app_key_tid = QmeshTimerCreate(&app_timer_ghdl,
                                             AddApplicationKeyTimeoutHandler,
                                             NULL,
                                             QMESH_DEMO_APP_KEY_ADD_DELAY);

    if (app_key_tid == QMESH_TIMER_INVALID_HANDLE)
    {
        QCLI_LOGE (mesh_group, "%d: Failed to create app key timer!", __FUNCTION__);
        return FALSE;
    }
    return TRUE;
}

/*----------------------------------------------------------------------------*
 * HandleUnprovisionedDeviceEvent
 */
/*! \brief Handles QMESH_UNPROVISIONED_DEVICE_EVENT event received from the mesh stack.
 *             Stores the UUIDs discovered in the UUID list.
 *
 *  \param event_data  Qmesh application event.
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
static void HandleUnprovisionedDeviceEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    uint16_t *rcvd_uuid_packed = (uint16_t *) event_data.param;
    uint8_t i;

    /* Discard duplicates */
    for(i = 0; i < QMESH_MAX_NUM_OF_PROVISIONABLE_DEVICES; i++)
    {
        if(memcmp(app_cntxt->unprovisioned_devices[i], rcvd_uuid_packed, sizeof(QMESH_UUID_T)) == 0)
        {
            return;
        }
    }

    QCLI_LOGE (mesh_group, "\r\nFound device with UUID: ");
    DisplayU16Array(rcvd_uuid_packed, QMESH_UUID_SIZE_HWORDS);
    QCLI_LOGE (mesh_group, " stored at index %u:", app_cntxt->unprov_device_index);
    QCLI_LOGE (mesh_group, "\r\n");

    /* Stores the UUIDs received */
    memcpy(app_cntxt->unprovisioned_devices[app_cntxt->unprov_device_index], rcvd_uuid_packed, sizeof(QMESH_UUID_T));
    app_cntxt->unprov_device_index = (app_cntxt->unprov_device_index + 1) % QMESH_MAX_NUM_OF_PROVISIONABLE_DEVICES;
}

/*----------------------------------------------------------------------------*
 * HandleGetDeviceKeyEvent
 */
/*! \brief Handles QMESH_PROVISION_GET_DEVICE_KEY_EVENT event received from the mesh stack.
 *             This function returns the device key for the element address specified in 
 *             eventDataCallback.param.
 *
 *  \param event_data  Qmesh application event.
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
static void HandleGetDeviceKeyEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_DEVICE_KEY_INFO_T         *device_key_info;
    QMESHAPP_PEER_DEVICE_KEY_LIST     *peer_device_list;

    device_key_info = (QMESH_DEVICE_KEY_INFO_T *)event_data.param;
    peer_device_list = app_cntxt->peer_device_key_list;

    while(peer_device_list)
    {
        /* Device key can be used by models other than the Config Model. For intance
         * Time Server Model uses device key & can be present on any element. So the
         * requested element addresses should be checked against all the element address
         * for the specified provisioned network.
         */
        if((peer_device_list->prov_net_id == device_key_info->prov_net_id) &&
           ((device_key_info->elm_addr == peer_device_list->elem_addr) &&
            (device_key_info->elm_addr < peer_device_list->elem_addr + peer_device_list->num_elems)))
        {
            device_key_info->device_key = peer_device_list->device_key;
            break;
        }
        peer_device_list = peer_device_list->next;
    }
}

/*----------------------------------------------------------------------------*
 * HandleAppKeyStatusEvent
 */
/*! \brief Handles QMESH_CONFIG_APP_KEY_STATUS_EVENT event received from the mesh stack in 
 *             response to the Config App Key Add message.
 *             If the message status is successful, Config Model App Bind message is sent to the remote
 *             device.
 *
 *  \param event_data  Qmesh application event.
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
static void HandleAppKeyStatusEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;

    if(cli_evt_data->event_info.app_key_status.status == QMESH_RESULT_SUCCESS)
    {
        /* Send model app bind key message */
            /* If Application key command */
        if((event_data.status == QMESH_RESULT_SUCCESS) &&
            (app_cntxt->app_state == app_state_application_key_add))
        {
            QCLI_LOGI (mesh_group, "\r\nAdd Application key is successful\n");
            QCLI_LOGI (mesh_group, "\r\nSending Model App Bind Key message...\n");

            if(BindModelApplicationKey() == QMESH_RESULT_SUCCESS)
            {
                app_cntxt->app_state = app_state_model_key_bind;
                return;
                /* After this, QMESH_CONFIG_MODEL_APP_STATUS_EVENT will be received via ProcessMeshEvent callback.
                 If status=QMESH_RESULT_SUCCESS, then User can provision another device or start sending 
                 the model messages just configured */
            }
            else
            {
                QCLI_LOGI (mesh_group, "\r\nFailed to send Model App Bind Key message\n");
            }
        }
        else
        {
            QCLI_LOGI (mesh_group, "\r\nQMESH_CONFIG_APP_KEY_STATUS_EVENT received in invalid state\n");
        }
    }
    else
    {
        QCLI_LOGI (mesh_group, "\r\nApplication Key Add failed with status =0x%x\n",
            cli_evt_data->event_info.app_key_status.status);
    }
    app_cntxt->app_state = app_state_initialized;
}

/*----------------------------------------------------------------------------*
 * HandleConfigModelAppStatusEvent
 */
/*! \brief Handles QMESH_CONFIG_MODEL_APP_STATUS_EVENT event received from the mesh stack in 
 *             response to the Config Model App Bind message.
 *             If the message status is successful then the remote device can send and receive the configured
 *             model messages.
 *
 *  \param event_data  Qmesh application event.
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
static void HandleConfigModelAppStatusEvent (QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_CONFIG_CLIENT_EVT_DATA_T *cli_evt_data;
    cli_evt_data = ( QMESH_CONFIG_CLIENT_EVT_DATA_T *)event_data.param;

    if(cli_evt_data->event_info.mdl_app_status.status == QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGI (mesh_group, "\r\nModel App Bind successful\n");
        /* Now the User can send supported model messages to the remote device */
    }
    else
    {
        QCLI_LOGI (mesh_group, "\r\nFailed to bind the model to application key,  status=0x%x\n",
               cli_evt_data->event_info.mdl_app_status.status);
    }
    app_cntxt->app_state = app_state_initialized;
}

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
        QMESHAPP_PEER_DEVICE_KEY_LIST *prov_dev_info;

        app_cntxt->app_state = app_state_provisioned;

        prov_dev_info = (QMESHAPP_PEER_DEVICE_KEY_LIST*)QmeshMalloc(QMESH_MM_SECTION_APP,
                                   sizeof(QMESHAPP_PEER_DEVICE_KEY_LIST));

        /* Print the device key */
        if (NULL != prov_info->device_key)
        {
            QCLI_LOGI (mesh_group, "\r\nDevice key is ");
            DisplayU16Array(prov_info->device_key, 8);
            QCLI_LOGI (mesh_group, "\n");
        }

        QCLI_LOGI (mesh_group, "Provisioning Complete, Address assigned=0x%x\n", prov_info->elm_addr);

        /* Store the provisioned device details */
        if (prov_dev_info != NULL)
        {
            prov_dev_info->prov_net_id = prov_info->key_info.prov_net_idx;
            prov_dev_info->elem_addr = prov_info->elm_addr;
            prov_dev_info->num_elems = remote_num_elms;
            memcpy(prov_dev_info->device_key, prov_info->device_key, QMESH_SECURITY_KEY_HWORDS * sizeof(uint16_t));
            prov_dev_info->next = app_cntxt->peer_device_key_list;
            app_cntxt->peer_device_key_list = prov_dev_info;
        }
        else
        {
            QCLI_LOGI (mesh_group, "\r\n Memory allocation failed!!");
        }

        /* Provisioning is successful. Now Add Application key and bind required models to the 
            Application key in the remote device using Config model messages.
            We shall send Add Application Key message after a delay so that remote side gets just enough 
            time to update NVM(if it supports) after provisioning.
        */
        CreateAddAppKeyTimer();
    }
    else
    {
        QCLI_LOGI(mesh_group, "Provisioning failed with res=0x%x\n", event_data.status);
        app_cntxt->app_state = app_state_initialized;
    }
}

/*----------------------------------------------------------------------------*
 * HandleDeviceCapabilityEvent
 */
/*! \brief Handles QMESH_DEVICE_CAPABILITY_EVENT event received from the mesh stack in 
 *             during provisioning.
 *
 *  \param event_data  Qmesh application event.
 *
 *  \returns QmeshResult.
 */
/*---------------------------------------------------------------------------*/
static QmeshResult HandleDeviceCapabilityEvent(QMESH_APP_EVENT_DATA_T event_data)
{
    QMESH_PROV_SELECTED_CAPS_T cap;
    QMESH_REMOTE_NODE_CAPS_DATA_T *dev_cap_info;
    QmeshResult res;

    dev_cap_info = (QMESH_REMOTE_NODE_CAPS_DATA_T *) event_data.param;

    remote_num_elms = dev_cap_info->num_elements;
    cap.pub_key_method = QMESH_PUBLIC_KEY_NO_OOB;
    cap.auth_method = QMESH_AUTH_METHOD_OOB_NONE;
    cap.action.output_oob_action = QMESH_OUTPUT_OOB_ACTION_BLINK;
    cap.auth_data_size = 0;

    res = QmeshSelectCapability(&cap);

    if(res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "SelectPeerCapability failed, ret=0x%x\n", res);
    }

    return res;
}

/*----------------------------------------------------------------------------*
 * BindModelApplicationKey
 */
/*! \brief This function sends "Config Model App Bind" message to bind the application key previously 
*              added to the model on the remote device.
 *             For the Client node, Generic OnOff Client model will be bound to the Application key
 *             For the Server node, Generic OnOff Server model will be bound to the Application key
 *
 *      Please refer "Section 4.3.2.46" in Mesh Profile v1.0 for details for Config Model App Bind message.
 *
 *  \param None.
 *
 *  \returns QmeshResult.
 */
/*---------------------------------------------------------------------------*/
static QmeshResult BindModelApplicationKey(void)
{
    QCLI_Command_Status_t res = QMESH_RESULT_SUCCESS;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;
    QMESH_RESULT_T result;
    uint16_t remote_addr;
    uint32_t model_id;

    /* For client device configure Generic OnOff Client model */
    if(app_cntxt->provisioning_client)
    {
        QCLI_LOGE (mesh_group, "\nBinding Model on Client node\n");
        remote_addr = QMESH_DEMO_CLIENT_ELEMENT_ADDRESS;
        model_id = QMESH_GENERIC_ONOFF_CLIENT_MODEL_ID;
    }
    /* For server device configure Generic OnOff Server model */
    else
    {
        QCLI_LOGE (mesh_group, "\nBinding Model on Server node\n");
        remote_addr = QMESH_DEMO_SERVER_ELEMENT_ADDRESS;
        model_id = QMESH_GENERIC_ONOFF_SERVER_MODEL_ID;
    }

    /* Update Key Info */
    msg_info.key_info.prov_net_idx = (global_nw_key_index.prov_net_idx & PROVISIONED_NET_ID_MASK) >> 12;
    msg_info.key_info.key_idx = (global_nw_key_index.key_idx & KEY_ID_MASK);
    /* Update Src and Dst Address */
    msg_info.src_addr = LOCAL_UNICAST_ADDRESS;
    msg_info.dst_addr = remote_addr;
    /* Update TTL */
    msg_info.ttl = QCLI_DFLT_TTL ;

    /* Update Command Data */
    cmd_data.app_key_bind.ele_info.ele_addr = remote_addr;
    cmd_data.app_key_bind.ele_info.model_type = QMESH_MODEL_TYPE_SIG; //model type
    cmd_data.app_key_bind.ele_info.model_id = model_id; // model id
    cmd_data.app_key_bind.app_key_idx = global_app_key_index; //appkey indx

    result = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_APP_BIND,
                                      &cmd_data);

    if (result != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "\n QmeshConfigSendClientCmd ConfigBindAppKey failed res=%d\n", result);
        res = QMESH_RESULT_FAILURE;
    }
    return res;
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
 /* Health server data Initialiation procedure */
static void QmeshInitHealthServerData(const QMESH_DEVICE_COMPOSITION_DATA_T *dev_comp)
{
    /* Following example is shown considering health server is registed only in primary element */
    g_health_server_state.company_id = dev_comp->cid;
    g_health_server_state.pub_info = &provisioner_publish_state [1];
    g_health_server_state.fault = (QMESH_HEALTH_MODEL_DATA_T *) &current_fault;
    g_health_server_state.fast_period_divisor = QMESH_HEALTH_PERIOD_DIVISOR_INVALID;
    g_health_server_state.pub_interval = QMESH_HEALTH_PUB_PERIOD_DISABLED;
    g_health_server_state.pub_tid = QMESH_HEALTH_PUB_TIMER_INVALID_HANDLE;
}

/*----------------------------------------------------------------------------*
 * AddRemoteApplicationKey
 */
/*! \brief Sends Config model add application key message to add an Application key to the 
 *             remote device.
 *             Please refer "Section 4.3.2.37" in Mesh Profile v1.0 for details for Config AppKey Add message.
 *
 *  \param None.
 *
 *  \returns QmeshResult.
 */
/*---------------------------------------------------------------------------*/
static QmeshResult AddRemoteApplicationKey(void)
{
    QmeshResult res = QMESH_RESULT_SUCCESS;
    QMESH_CONFIG_CLIENT_CMD_DATA_T cmd_data;
    QMESH_CONFIG_CLIENT_MSG_INFO_T msg_info;

    uint16_t key[QMESH_APP_KEY_SIZE_HWORDS] = 
                    {0x9663, 0x7147, 0x4f73, 0x76bd, 0xb4e3, 0x1905, 0xd9d1, 0x484a};

    /* Update Key Info */
    msg_info.key_info.prov_net_idx = (global_nw_key_index.prov_net_idx & PROVISIONED_NET_ID_MASK) >> 12;
    msg_info.key_info.key_idx = (global_nw_key_index.key_idx & KEY_ID_MASK);
    /* Update Src and Dst Address */
    msg_info.src_addr = LOCAL_UNICAST_ADDRESS;

    /* Get the remote address based on sever or client device type chosen during provisioning */
    if(app_cntxt->provisioning_client)
    {
        QCLI_LOGE (mesh_group, "\nAdding Application Key on Client node\n");
        msg_info.dst_addr = QMESH_DEMO_CLIENT_ELEMENT_ADDRESS;
        app_cntxt->provisioning_client = TRUE;
    }
    else
    {
        QCLI_LOGE (mesh_group, "\nAdding Application Key on Server node\n");
        msg_info.dst_addr = QMESH_DEMO_SERVER_ELEMENT_ADDRESS;
        app_cntxt->provisioning_client = FALSE;
    }

    /* Update TTL */
    msg_info.ttl = QCLI_DFLT_TTL ;

    /* Update Command Data */
    cmd_data.app_key_add.app_key_idx = global_app_key_index;
    cmd_data.app_key_add.net_key_idx = global_nw_key_index.key_idx;
    QmeshMemCpy(cmd_data.app_key_add.app_key, (uint8_t*)key, QMESH_APP_KEY_SIZE_OCTETS);

    res = QmeshConfigSendClientCmd(&msg_info, QMESH_CONFIG_CLI_APP_KEY_ADD,
                                      &cmd_data);

    if (res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "\nConfigAddAppKey failed res=%d\n", res);
        res = QMESH_RESULT_FAILURE;
    }
    return res;
}

/*----------------------------------------------------------------------------*
 * AddNetwork
 */
/*! \brief Creates a network with Network key set in 'net_key'. 
 *             AddNetwork must be called in provisioner role only.
 *
 *  \param None.
 *
 *  \returns QmeshResult.
 */
/*---------------------------------------------------------------------------*/
static QmeshResult AddNetwork(void)
{
    QmeshResult res;

    res = QmeshAddNetwork(global_nw_key_index, net_key);
    if(res == QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGI (mesh_group, "Network Added with Prov Net Index : 0x%x,  Key Index : 0x%x \n",
                                global_nw_key_index.prov_net_idx, global_nw_key_index.key_idx);
    }
    return res;
}

/*----------------------------------------------------------------------------*
 * AddApplicationKey
 */
/*! \brief Adds an Application key with the value in 'app_key' to the network pointed by 
 *             'global_nw_key_index'.
 *             AddNetwork must be called in provisioner role only.
 *
 *  \param None.
 *
 *  \returns QmeshResult.
 */
/*---------------------------------------------------------------------------*/
static QmeshResult AddApplicationKey(void)
{
    QmeshResult                   res;

    res = QmeshAddAppKey(global_nw_key_index, global_app_key_index, app_key);

    if(res == QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGI (mesh_group, "Application Key Added for Prov Net Index : 0x%x,  Key Index : 0x%x, App Key Index : 0x%x \n",
                                global_nw_key_index.prov_net_idx, global_nw_key_index.key_idx,
                                global_app_key_index);
    }
    else
    {
            QCLI_LOGE (mesh_group, "AddApplicationKey failed\n");
    }
    return res;
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
        case QMESH_UNPROVISIONED_DEVICE_EVENT:
        {
            HandleUnprovisionedDeviceEvent(event_data_cb);
            /* Blink LED or other forms supported on the platform to indicate provisioning has started */
        }
        break;

        case QMESH_DEVICE_CAPABILITY_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_DEVICE_CAPABILITY_EVENT\n");
            QmeshResult res;

            res = HandleDeviceCapabilityEvent(event_data_cb);

            if(res != QMESH_RESULT_SUCCESS)
            {
                QCLI_LOGE (mesh_group, "SelectPeerCapability failed, ret=0x%x\n", res);
            }

            /* Events QMESH_PROVISION_STATUS/QMESH_PROVISION_COMPLETE will be received after this
                 indicating SUCCESS or FAILURE.
                 See QMESH_PROVISION_STATUS/QMESH_PROVISION_COMPLETE_EVENT event handling */
        }
        break;
        case QMESH_CONFIG_APP_KEY_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_APP_KEY_STATUS_EVENT\n");
            HandleAppKeyStatusEvent(event_data_cb);
        }
        break;
        case QMESH_CONFIG_MODEL_APP_STATUS_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_CONFIG_MODEL_APP_STATUS_EVENT\n");
            HandleConfigModelAppStatusEvent(event_data_cb);
        }
        break;
        case QMESH_APP_PAYLOAD_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\n Received QMESH_APP_PAYLOAD_EVENT: 0x%04x\n", event_data_cb.status);
        }
        break;
        case QMESH_PROVISION_STATUS_EVENT:
        {
            QMESH_PROV_STATUS_T st = *(QMESH_PROV_STATUS_T *) event_data_cb.param;
            QCLI_LOGI (mesh_group, "\r\nProvisioning : %s (%d)",
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
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_OPERATION_LINK_OPEN");
            app_cntxt->app_state = app_state_provision_started;
        }
        break;
        case QMESH_PROV_LINK_CLOSE_EVENT:
        {
            QCLI_LOGI (mesh_group, "\r\nReceived QMESH_PROV_LINK_CLOSE_EVENT");
            app_cntxt->app_state = app_state_initialized;
        }
        break;
        case QMESH_GET_DEVICE_KEY_EVENT:
        {
            HandleGetDeviceKeyEvent(event_data_cb);
        }
        break;
        default:
            QCLI_LOGI (mesh_group, "Received unhandled event=0x%x\n", operation_event);
            break;
    }
}

/******************************************************************************
 *  Public Functions called from the QCLI Command Line Interface
 *****************************************************************************/

/*----------------------------------------------------------------------------*
 * InitializeMesh
 */
/*! \brief Initializes BT controller, UART for logging the output and initializes the Qmesh stack.
 *             Adds the network and application key into the local stack.
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
    res = QmeshSchedInit(&bt_stack_id);
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

    /* Set Provisioner Role */
    res = QmeshSetProvisioningRole(DEVICE_INSTANCE, TRUE);

    if(res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "QmeshSetProvisioningRole failed (0x%x)\n", res);
        return res;
    }

    /* Create a network with network key mentioned in net_key */
    res = AddNetwork();

    if(res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "QmeshSetProvisioningRole failed (0x%x)\n", res);
        return res;
    }

    /* Add an Application Key mentioned in app_key */
    res = AddApplicationKey();

    if(res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "QmeshSetProvisioningRole failed (0x%x)\n", res);
        return res;
    }

    /* We have created a network and an application key is added to the network */

    /* Reset the unprovisioned device list */
    memset(app_cntxt->unprovisioned_devices, 0, sizeof(app_cntxt->unprovisioned_devices));
    app_cntxt->unprov_device_index = 0;

    /* Now, Start device discovery to scan for devices to provision */
    /* Devices discovered are sent as QMESH_UNPROVISIONED_DEVICE_EVENT event and the callback
    ProcessMeshEvent will be called with device UUID */
    /* User can choose the UUID and call QmeshProvisionDevice() to start provisioning the device */
    return res;
}

/*----------------------------------------------------------------------------*
 * StartDeviceDiscovery
 */
/*! \brief Starts device discovery of the unprovisioned devices. Optionally, filters the beacons as per
 *             the parameters.
 *             Upon successfull return, QMESH_UNPROVISIONED_DEVICE_EVENT events will be sent via 
 *             ProcessMeshEvent() callback registered in QmeshInit() API 
 *
 *  \param uuid_list            Array of UUIDs to be filtered. NULL if no filtering is needed.
 *  \param uuid_list_len      Number of UUIDs in the UUID list.
 *  \param ignore_dup       TRUE - Ignores duplicated beacons, 
 *                                      FALSE -All the beacons would be sent to the Application
 *
 *  \returns QmeshResult.
 */
/*---------------------------------------------------------------------------*/
QmeshResult StartDeviceDiscovery(QMESH_DEVICE_UUID_T *const uuid_list,
                                                uint16_t uuid_list_len, bool ignore_dup)
{
    /* Initiate device discovery */
    return QmeshStartDeviceDiscovery(uuid_list, uuid_list_len, ignore_dup);
}

/*----------------------------------------------------------------------------*
 * StartProvisioning
 */
/*! \brief   Starts provisioning the device with UUID value set in 'uuid' to the network pointed
 *               to by 'net_key' and assigns the Primary element address with value in 'pri_elm_addr'.
 *               ProcessMeshEvent() callback will be called with associated events during provisioning.
 *               See QmeshProvisionDevice() documentation for more details 
 *
 *  \param net_key            Provisioned network and subnetwork information.
 *  \param pri_elm_addr     Address to be assigned to the primary element.
 *  \param uuid                  128-bit UUID of the device.
 *
 *  \returns QmeshResult.
 */
/*---------------------------------------------------------------------------*/
QmeshResult StartProvisioning(QMESH_KEY_IDX_INFO_T net_key, uint16_t pri_elm_addr,
    QMESH_DEVICE_UUID_T *uuid)
{
    QmeshResult res;

    /* Initiate Provisioning for Provisioning over PB-ADV bearer */
    res = QmeshProvisionDevice(net_key, pri_elm_addr, *uuid, QMESH_APP_ATTENTION_DURATION);

    if(res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "  QmeshProvisionDevice failed, ret=0x%x\n", res);
    }
    return res;
}
