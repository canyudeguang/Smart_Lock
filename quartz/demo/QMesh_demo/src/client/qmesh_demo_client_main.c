/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*
  * This file contains functions and definitions for Client node Application interfacing QCLI framework
 */
/*****************************************************************************/
#include "qmesh_demo_client.h"
#include "qmesh_demo_client_main.h"

#include "qmesh_ble_gap.h"
#include "qapi_ble_hcitypes.h"

/******************************************************************************
 *  Global Data
 *****************************************************************************/
/* Group Handle for the main Qmesh commands registered with QCLI */
QCLI_Group_Handle_t                             mesh_group;
QCLI_Group_Handle_t                             mesh_model_group;
QMESHAPP_CONTEXT_T                          app_context = {0};
/******************************************************************************
 *  Private Data
 *****************************************************************************/
static qapi_BLE_HCI_DriverInformation_t HCI_DriverInformation;
#define     QMESH_DEMO_CLIENT_ONOFF_TRANSITION_TIME           (0)
#define     QMESH_DEMO_CLIENT_ONOFF_DELAY                                (0)

/*----------------------------------------------------------------------------*
 * AppStartDeviceDiscovery
 */
/*! \brief  Initiates sending unprovisioned beacons for the provisioner to discovery this device.
 *
 *  \param None.
 *
 *  \returns QCLI_Command_Status_t
 */
/*---------------------------------------------------------------------------*/
QCLI_Command_Status_t AppStartDiscoverable(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult           res;

    /* Check if stack is initialized */
    if(!app_context.stack_initialized)
    {
        /* Initialize the platform and QMesh stack only once */
        res = InitializeMesh(&app_context);

        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "InitializeMesh failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }
        else
        {
            /* Move the application state to initialized.*/
            app_context.app_state = app_state_initialized;
            app_context.stack_initialized = TRUE;
        }
    }

    /* Check if we are already discoverable */
    if(app_context.app_state == app_state_discoverable)
    {
        QCLI_LOGE (mesh_group, "Device already discoverable\n");
        return QCLI_STATUS_SUCCESS_E;
    }

    /* Start mesh device discovery */
    res = StartDiscoverable();

    if(res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "StartDiscoverable failed, ret=0x%x\n", res);
        return QCLI_STATUS_ERROR_E;
    }
    else
    {
        /* Move the application state discoverable state */
        app_context.app_state = app_state_discoverable;
        QCLI_LOGI (mesh_group, "Device is discoverable...\n");
    }

    return(ret_val);
}

/*----------------------------------------------------------------------------*
 * AppStopDeviceDiscovery
 */
/*! \brief  Stops sending unprovisioned beacons or stops ongoing provisioning procedure if started.
 *
 *  \param None.
 *
 *  \returns QCLI_Command_Status_t.
 */
/*---------------------------------------------------------------------------*/
QCLI_Command_Status_t AppStopDiscoverable(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult res;

    if((app_context.app_state != app_state_discoverable) ||
        (app_context.app_state != app_state_provision_started))
    {
        QCLI_LOGE (mesh_group, "Not in discoverable state\n");
        return QCLI_STATUS_ERROR_E;
    }

    /* Stop mesh device discovery */
    res = StopDiscoverable();
    if(res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "QmeshStopDeviceDiscovery failed, ret=0x%x\n", res);
        return QCLI_STATUS_ERROR_E;
    }

    app_context.app_state = app_state_initialized;
    return(ret_val);
}

/*----------------------------------------------------------------------------*
 * AppGenericOnOffSet
 */
/*! \brief  Sends Generic OnOff Set message to the Generic OnOff server node with address
 *              QMESH_DEMO_SERVER_ELEMENT_ADDRESS. Transition Time and Delay parameters
 *              are not set in the OnOff set message for simplicity.
 *
 *
 *    Note that this API should be called once the Server node is provisioned with element address
 *    QMESH_DEMO_SERVER_ELEMENT_ADDRESS and Generic OnOff server model is bound to the
 *    same network. Use Provisioner application to provision Server node.
 *
 *
 *  \param 
 *   Param 1 - OnOff state. 0 = Off, 1 = On
 *   Param 2 - Transaction Identifier of the message.
 *
 *  \returns QCLI_Command_Status_t.
 */
/*---------------------------------------------------------------------------*/
QCLI_Command_Status_t AppGenericOnOffSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List) &&
        (Parameter_List[0].Integer_Is_Valid) && (Parameter_List[1].Integer_Is_Valid) &&
         app_context.app_state == app_state_provisioned)
    {
        QmeshResult res;

        res = GenericOnOffModelSet(Parameter_List[0].Integer_Value,
                                                        Parameter_List[1].Integer_Value,
                                                        QMESH_DEMO_CLIENT_ONOFF_TRANSITION_TIME,
                                                        QMESH_DEMO_CLIENT_ONOFF_DELAY,
                                                        FALSE);

        if(res == QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "Generic OnOff Set message sent to remote node\n");
        }            
        else
        {
            QCLI_LOGE (mesh_group, "Failed to send Generic OnOff Set message with error=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        /* One or more of the necessary parameters is/are invalid.     */
        QCLI_LOGE (mesh_group, "Invalid params or applicaiton in invalid state \n");
        ret_val = QCLI_STATUS_USAGE_E;
    }
    return ret_val;
}

/*----------------------------------------------------------------------------*
 * AppGenericOnOffGet
 */
/*! \brief  Sends Generic OnOff Get message to the Generic OnOff server node with address
 *              QMESH_DEMO_SERVER_ELEMENT_ADDRESS.
 *              QmeshGenericOnOffClientHandler() will be called with opcode 
 *              QMESH_GENERIC_ONOFF_CLIENT_STATUS once the server node responds.
 *
 *  \param none.
 *
 *  \returns QCLI_Command_Status_t.
 */
/*---------------------------------------------------------------------------*/
QCLI_Command_Status_t AppGenericOnOffGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QmeshResult res;

    if(app_context.app_state != app_state_provisioned)
    {
        QCLI_LOGE (mesh_group, "Device not provisioned. Provision the device first!\n");
        return QCLI_STATUS_ERROR_E;
    }

    /* Send OnOff Get message */
    res = GenericOnOffModelGet();

    if(res == QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "Generic OnOff Get message sent to remote node\n");
    }            
    else
    {
        QCLI_LOGE (mesh_group, "Failed to send Generic OnOff Get message with error=0x%x\n", res);
        return QCLI_STATUS_ERROR_E;
    }

    /* Flag success to the caller */
    return QCLI_STATUS_SUCCESS_E;
}

/*----------------------------------------------------------------------------*
 * AppCommandList
 */
/*! \brief  Helper function to list all the application commands for the Client application.
 *
 *  \param None
 *
 *  \returns QCLI_Command_Status_t.
 */
/*---------------------------------------------------------------------------*/
/* Helper function to list all the application commands */
 QCLI_Command_Status_t AppCommandList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_LOGE (mesh_group, "\n4. StartDiscoverable - Starts sending unprovisioned beacons discovery. Parameters:none\n");
    QCLI_LOGE (mesh_group, "\n5. StopDiscoverable - Stops sending unprovisioned beacons or provisioning if started. Parameters:none\n");
    QCLI_LOGE (mesh_group, "\n6. SendGenericOnOffSet - Send Generic OnOff Set message to server node. Parameters: [OnOffState (0=Off, 1=On)]\n");
    QCLI_LOGE (mesh_group, "\n7. SendGenericOnOffGet - Send Generic OnOff Get message to server node.. Parameters:none\n");

    return QCLI_STATUS_SUCCESS_E;
}

/*****************************************************************************
 * QCLI Command Group Definitions for the Client node Application
 *****************************************************************************/
const QCLI_Command_t qmesh_cmd_list[] =
{
   /* cmd_function       start_thread      cmd_string                 usage_string description */
   { AppStartDiscoverable,   false, "StartDiscoverable",      "",  "Starts sending unprovisioned beacons"},
   { AppStopDiscoverable,   false, "StopDiscoverable",       "",  "Starts sending unprovisioned beacons"},
   { AppGenericOnOffSet,    false, "SendGenericOnOffSet",      "[OnOffState (0=Off, 1=On)] [2 bytes TID]",  "Send Generic OnOff Set message"},
   { AppGenericOnOffGet,   false, "SendGenericOnOffGet",             "",  "Send Generic OnOff Get message"},
   { AppCommandList,       false, "CommandList",      "",  "Displays all the commands with parameters"}
};

/* Command group for the Client node application to register with QCLI framework */
QCLI_Command_Group_t qmesh_cmd_group =
{
    "QMesh Client",
    (sizeof(qmesh_cmd_list) / sizeof(qmesh_cmd_list[0])),
    qmesh_cmd_list
};

/*----------------------------------------------------------------------------*
 * Initialize_QMesh_Client_Demo
 */
/*! \brief  Registers the Client Mesh Command Group with QCLI. This function is called during the Quartz
 *              bootup.
 *
 *  \param None
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
void Initialize_QMesh_Client_Demo (void)
{
    /* Attempt to reqister the Command Group with the QCLI framework.*/
    mesh_group = QCLI_Register_Command_Group (NULL, &qmesh_cmd_group);
    if(!mesh_group)
    {
        /* Un-register the Group. */
         QCLI_Unregister_Command_Group (mesh_group);
         mesh_group      = NULL;
         return;
    }
    /* mesh_model_group is used in model client code */
    mesh_model_group = mesh_group;

    /* Set the HCI driver information. */
    QAPI_BLE_HCI_DRIVER_SET_COMM_INFORMATION (&HCI_DriverInformation,
                                              1,
                                              115200,
                                              QAPI_BLE_COMM_PROTOCOL_UART_E);
}
