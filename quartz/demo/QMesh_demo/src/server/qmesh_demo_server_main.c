/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*
  * This file contains functions and definitions for Server node Application interfacing QCLI framework
 */
/*****************************************************************************/
#include "qmesh_demo_server.h"
#include "qmesh_demo_server_main.h"

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

/* Transition time for the Generic OnOff message */
#define     QMESH_DEMO_CLIENT_ONOFF_TRANSITION_TIME           (0)
/* Delay time for the Generic OnOff message */
#define     QMESH_DEMO_CLIENT_ONOFF_DELAY                                (0)

/*----------------------------------------------------------------------------*
 * AppStartDeviceDiscovery
 */
/*! \brief  Starts sending unprovisioned beacons for the provisioner to discovery this device.
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
        QCLI_LOGE (mesh_group, "StopDiscoverable failed, ret=0x%x\n", res);
        return QCLI_STATUS_ERROR_E;
    }

    app_context.app_state = app_state_initialized;
    return(ret_val);
}

/*----------------------------------------------------------------------------*
 * AppCommandList
 */
/*! \brief  Helper function to list all the application commands for the Server application.
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

    return QCLI_STATUS_SUCCESS_E;
}

/*****************************************************************************
 * QCLI Command Group Definitions for the Server node Application
 *****************************************************************************/
const QCLI_Command_t qmesh_cmd_list[] =
{
   /* cmd_function       start_thread      cmd_string                 usage_string description */
   { AppStartDiscoverable,   false, "StartDiscoverable",      "",  "Starts sending unprovisioned beacons"},
   { AppStopDiscoverable,   false, "StopDiscoverable",       "",  "Starts sending unprovisioned beacons"},
   { AppCommandList,       false, "CommandList",      "",  "Displays all the commands with parameters"}
};

/* Command group for the Server node application to register with QCLI framework */
QCLI_Command_Group_t qmesh_cmd_group =
{
    "QMesh Server",
    (sizeof(qmesh_cmd_list) / sizeof(qmesh_cmd_list[0])),
    qmesh_cmd_list
};

/*----------------------------------------------------------------------------*
 * Initialize_QMesh_Server_Demo
 */
/*! \brief  Registers the Server Mesh Command Group with QCLI. This function is called during the Quartz
 *              bootup.
 *
 *  \param None
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
void Initialize_QMesh_Server_Demo (void)
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
    /* mesh_model_group is used in model server code */
    mesh_model_group = mesh_group;

    /* Set the HCI driver information. */
    QAPI_BLE_HCI_DRIVER_SET_COMM_INFORMATION (&HCI_DriverInformation,
                                              1,
                                              115200,
                                              QAPI_BLE_COMM_PROTOCOL_UART_E);
}
