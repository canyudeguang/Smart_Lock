/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*
  * This file contains functions and definitions for Provisioning Application interfacing QCLI framework
 */
/*****************************************************************************/
#include "qmesh_demo_provisioner.h"
#include "qmesh_demo_provisioner_main.h"

#include "qmesh_ble_gap.h"
#include "qapi_ble_hcitypes.h"

static qapi_BLE_HCI_DriverInformation_t HCI_DriverInformation;
/* Group Handle for the main Qmesh commands registered with QCLI */
QCLI_Group_Handle_t                             mesh_group;
QMESHAPP_CONTEXT_T                          app_context = {0};

/*----------------------------------------------------------------------------*
 * AppStartDeviceDiscovery
 */
/*! \brief  Initiates the devices discovery of unprovisioned devices.
 *
 *  \param None.
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
QCLI_Command_Status_t AppStartDeviceDiscovery(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    QmeshResult  res = QMESH_RESULT_SUCCESS;

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

    /*Check if scanning is in progress */
    if(app_context.app_state == app_state_discovery)
    {
        QCLI_LOGE (mesh_group, "Device discovery already in progress\n");
        return QCLI_STATUS_SUCCESS_E;
    }

    /* Reset the discovered device list */
    memset(app_context.unprovisioned_devices, 0, sizeof(app_context.unprovisioned_devices));
    app_context.unprov_device_index = 0;

    /* Start mesh device discovery */
    res = StartDeviceDiscovery(NULL, 0, FALSE);

    if(res != QMESH_RESULT_SUCCESS)
    {
        QCLI_LOGE (mesh_group, "StartDeviceDiscovery failed, ret=0x%x\n", res);
        return QCLI_STATUS_ERROR_E;
    }
    else
    {
        /* Move the application state discovery */
        app_context.app_state = app_state_discovery;
        QCLI_LOGI (mesh_group, "\nDevice Discovery started...\n");
    }

    return(ret_val);
}

/*----------------------------------------------------------------------------*
 * AppStartProvisionDevice
 */
/*! \brief  Starts provisioning remote device discovered during device discovery.
 *
 *    Note that remote device can implement both Client and Server models. For demostration purpose
 *    this application will provision two differnent devices as Client and Server nodes.
 *
 *  \param 
 *   Param 1 - Index of the UUID discovered during device discovery procedure
 *   Param 2 - Type of remote device. 
 *                    0 = Client device implementing Generic OnOff Client model,
 *                    1=Server device implementing Generic OnOff Server model
 *
 *  \returns QCLI_Command_Status_t.
 */
/*---------------------------------------------------------------------------*/
QCLI_Command_Status_t AppStartProvisionDevice(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;

    PrintQCLIParameters(Parameter_Count, Parameter_List);

    /* Make sure that all of the parameters required for this function*/
    /* appear to be at least semi-valid.                              */
    if((Parameter_Count == 2) && (Parameter_List) &&
        (Parameter_List[0].Integer_Is_Valid) && (Parameter_List[1].Integer_Is_Valid) &&
        (app_context.app_state >= app_state_initialized && 
         app_context.app_state < app_state_provision_started))
    {
        QmeshResult res;
        const uint16_t *uuid;
        QMESH_SUBNET_KEY_IDX_INFO_T net_key;

        /* Retrieve UUID from param */
        if(Parameter_List[0].Integer_Value >= QMESH_MAX_NUM_OF_PROVISIONABLE_DEVICES)
        {
            QCLI_LOGE (mesh_group, "\nInvalid device index!!\n");
            return QCLI_STATUS_ERROR_E;
        }

        uuid =  app_context.unprovisioned_devices[Parameter_List[0].Integer_Value];

        /* Display the UUID of device selected */
        QCLI_LOGI (mesh_group, "\nProvisioning Device with UUID: ");
        DisplayU16Array(uuid, QMESH_UUID_SIZE_HWORDS);

        net_key.prov_net_idx = 0;
        net_key.key_idx = 0;

        /* Provision Client node */
        if(Parameter_List[1].Integer_Value == QMESH_APP_REMOTE_DEVICE_TYPE_CLIENT)
        {
            QCLI_LOGE (mesh_group, "\nProvisioning Client device with Primary element address=0x%x\n",QMESH_DEMO_CLIENT_ELEMENT_ADDRESS);
            app_context.provisioning_client = TRUE;
            /* Initiate Provisioning Client node over PB-ADV bearer */
            res = StartProvisioning(net_key, QMESH_DEMO_CLIENT_ELEMENT_ADDRESS, (QMESH_DEVICE_UUID_T*)uuid);
        }
        /* Provision Server node */
        else if(Parameter_List[1].Integer_Value == QMESH_APP_REMOTE_DEVICE_TYPE_SERVER)
        {
            QCLI_LOGE (mesh_group, "\nProvisioning Server device with Primary element address=0x%x\n",QMESH_DEMO_SERVER_ELEMENT_ADDRESS);
            app_context.provisioning_client = FALSE;
            /* Initiate Provisioning Server node over PB-ADVbearer */
            res = StartProvisioning(net_key, QMESH_DEMO_SERVER_ELEMENT_ADDRESS, (QMESH_DEVICE_UUID_T*)uuid);
        }
        /* Invalid device type */
        else
        {
            QCLI_LOGE (mesh_group, "\nInvalid device type. Should be 0 for Client, 1 for Server\n");
            return QCLI_STATUS_USAGE_E;
        }

        if(res != QMESH_RESULT_SUCCESS)
        {
            QCLI_LOGE (mesh_group, "\nStartProvisionDevice failed, ret=0x%x\n", res);
            return QCLI_STATUS_ERROR_E;
        }

        app_context.app_state = app_state_provision_started;
        ret_val = QCLI_STATUS_SUCCESS_E;
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
 * AppCommandList
 */
/*! \brief  Helper function to list all the application commands
 *
 *  \param None
 *
 *  \returns QCLI_Command_Status_t.
 */
/*---------------------------------------------------------------------------*/
/* Helper function to list all the application commands */
 QCLI_Command_Status_t AppCommandList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_LOGE (mesh_group, "\n4. DiscoverDevice - Starts device discovery. Parameters:none\n");
    QCLI_LOGE (mesh_group, "\n5. StartProvisioning -Starts provisioning. Parameters are [Device Index (from StartDeviceDiscovery)] [Remote device (0=Client, 1=Server)]\n");

    return QCLI_STATUS_SUCCESS_E;
}

/*****************************************************************************
 * QCLI Command Group Definitions for the Provisioner Application
 *****************************************************************************/
const QCLI_Command_t qmesh_cmd_list[] =
{
   /* cmd_function       start_thread      cmd_string                 usage_string description */
   { AppStartDeviceDiscovery,      false, "DiscoverDevice",      "",  "Starts device discovery"},
   { AppStartProvisionDevice,      false, "StartProvisioning",      "[Device Index (from StartDeviceDiscovery)] [Remote device (0=Client, 1=Server)]",  "Start provisioning the device"},
   { AppCommandList,      false, "CommandList",      "",  "Displays all the commands with parameters"}
};

/* Command group for the Provisioner application to register with QCLI framework */
QCLI_Command_Group_t qmesh_cmd_group =
{
    "QMesh Provisioner",
    (sizeof(qmesh_cmd_list) / sizeof(qmesh_cmd_list[0])),
    qmesh_cmd_list
};

/*----------------------------------------------------------------------------*
 * Initialize_QMesh_Demo
 */
/*! \brief  Registers the Mesh Command Group with QCLI. This function is called during the Quartz
 *              bootup.
 *
 *  \param None
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
void Initialize_QMesh_Demo (void)
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
    /* Set the HCI driver information. */
    QAPI_BLE_HCI_DRIVER_SET_COMM_INFORMATION (&HCI_DriverInformation,
                                              1,
                                              115200,
                                              QAPI_BLE_COMM_PROTOCOL_UART_E);
}
