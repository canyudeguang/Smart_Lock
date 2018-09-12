/******************************************************************************
Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/

#include "qmesh_demo_core.h"
#include "qmesh_demo_menu.h"

/*****************************************************************************
 * qcli Command Group Definitions
 *****************************************************************************/
const QCLI_Command_t qmesh_cmd_list[] =
{
   /* cmd_function       start_thread      cmd_string                 usage_string description */
   { InitializeMesh,            false, "InitializeMesh",            "[Device UUID (16 octets in hex (32 chars))(Optional)] [[Public Key OOB Mask (1 octet)] [Static OOB Mask (1 octet)] [Output OOB Action (1 octet)] [Input OOB Action (1 octet)] (Optional:All OOB params present or none)]",  "Initializes mesh stack (must be called before any other commands). " },
   { CreateDeviceInstance,      false, "CreateDeviceInstance",      "[Device Instance Index] [Role(0=Provisioner, 1=Node)] [Type(0=Client, 1=Server)]",  "Creates device instance with the given instance number (below max_devices)"},
   { SendUnProvDeviceBeacon,    false, "SendUnProvDeviceBeacon",    "", "Send Unprovisioned device beacon"},
   { SendMessage,               false, "SendMessage",               "[Provisioned Network Index (1 octet)] [KeyType(0=Application Key, 1=Self Device Key, 2=Peer Device Key)] [Global Network or App Key Index] [Broadcast or Virtual or Group or Dest Element Addr] [Source Element Addr] [TTL] [Payload Len in characters] [Payload] [MIC size(4 or 8)] [ACK(0=Not Required, 1=Required)]",  "Send Model or Application Message over SIG network"},
   { SetNetworkIVIndex,         false, "SetNetworkIVIndex",         "[Provisioned Network Index(1 octet)] [IV index(4 octets)]",  "Sets the Network IVIndex for the given Provisioned Network"},
   { SendSecureNwBeacon,        false, "SendSecureNwBeacon",        "[Provisioned Network Index (1 octet)] [Global Network Key Index for the Provisioned network (2 octets)]",  "Sends secure network Beacon"},
   { AdvtForProvOverGatt,       false, "AdvertiseProvisioningOverGatt",  "", "Start sending adverts for provisioning of device over GATT-Bearer"},
   { EnableGattProxyService,    false, "EnableGattProxyService",  "[State (0=Disable, 1=Enable)]", "Enable GATT Proxy Service"},
   { SetOobData,                false, "SetOobData",                "[Authentication Value (16 octets)]",  "SetOobData, User to set the OOB data on receiving QMESH_OOB_ACTION_EVENT"},
   { AddPeerDeviceKey,          false, "AddPeerDeviceKey",          "[Provisioned Network Index (1 octet)] [Peer Primary element address (2 octets)] [Number of elememts in the Peer (1 octet)] [Device Key (16 octets in hex(32 chars))]",  "Adds Peer Device Key to the peer device key list maintained by App"},

    /* Provisioning related APIs */
   { AddNetwork,                false, "AddNetwork",                "[Provisioned Network Index (1 octet)] [Global Network Key Index (12 bit value 0x000 - 0xFFF)] [Network Key (16 octets in hex(32 chars))]",  "Add a new network (Maximum of 2 sub-networks supported)"},
   { AddApplicationKey,         false, "AddApplicationKey",         "[Provisioned Network Index (1 octet)] [Global Network Key Index (12 bit value 0x000 - 0xFFF)] [Global App Key Index (12 bit value 0x000 - 0xFFF)] [Application Key (16 octets in hex(32 chars))]",  "Adds Application Key and binds to network ID"},
   { RemoveNetwork,             false, "RemoveNetwork",             "[Provisioned Network Index (1 octet)] [Global Network Key Index (12 bit value 0x000 - 0xFFF)]",  "Remove a network (Maximum of 2 sub-networks supported)"},
   { StartDeviceDiscovery,      false, "StartDeviceDiscovery",      "[Bearer (0=PB-ADV, 1=PB-GATT)] [Service (0=Provisioning, 1=Proxy)] Service is valid for PB-GATT only",  "Starts device discovery. Must be called in Provisioner role only"},
   { StopDeviceDiscovery,       false, "StopDeviceDiscovery",       "",      "Stops the ongoing device discovery. Must be called in Provisioner role only"},
   { DisplayUnProvDevList,      false, "DisplayUnProvDevList",      "[Bearer (0=PB-ADV, 1=PB-GATT)]",  "Displays list of unprovisioned device found for provisioning over ADV or GATT"},
   { StartProvisionDevice,      false, "StartProvisionDevice",      "[Bearer (0=PB-ADV, 1=PB-GATT)] [Provisioned Network Index(1 octet)] [Global Network Key Index (12 bit value 0x000 - 0xFFF)] [Device Index (from StartDeviceDiscovery)] [Element address (2 octets)] [Attention duration (1 octet) in seconds)]",  "Start provisioning the device. Must be called in Provisioner role only"},
   { StopProvision,             false, "StopProvision",             "",  "Stops the ongoing provisioning session. Must be called in Provisioner role only"},
   { SelectPeerCapability,      false, "SelectPeerCapability",      "[Public Key Type (1 octet)] [Authentication Method (1 octet)] [Authentication Action (1 octet)] [Authentication Size (1 octet)]",  "Select Peer Capability from the event QMESH_DEVICE_PABILITY_EVENT"},
   { DisplayDiscoveredProxyDevices, false, "DisplayDiscoveredProxyDevList",  "", "Display Discovered Proxy Devices"},
   { FindNodeFromProxyDevList, false, "FindNodeFromProxyDevList",  "[Provisioned Network Index (1 octet)] [Global Network Key Index (12 bit value 0x000 - 0xFFF)] [Element Address (1 octet)]", "Find Node (Advert Node Identity) from Discovered Proxy Device List"},
   { EstablishGattProxyConnection, false, "EstablishGattProxyConnection",  "[Device Index (from StartDeviceDiscovery for Proxy Service)]", "Establish GATT Proxy Connection"},
   { ListConnectedGattDevices, false, "ListConnectedGattDevices",  "", "List all connected GATT devices"},
   { SendGattProxySetFilterType, false, "SendGattProxySetFilterType",  "[Provisioned Network Index (1 octet)] [Global Network Key Index (12 bit value 0x000 - 0xFFF)] [Filter Type (0- White List, 1- Black List)] [Connection Index (0 - 3) (taken from ListConnectedGattDevices)]", "Send GATT Proxy Set Filter Type"},
   { SendGattProxyAddRemoveAddr, false, "SendGattProxyAddRemoveAddr",  "[Provisioned Network Index (1 octet)] [Global Network Key Index (12 bit value 0x000 - 0xFFF)] [Add Remove Address (0- Add Address, 1- Remove Address)] [Address (2*N octets, N - Number of Addresses)] [Connection Index (0 - 3) (taken from ListConnectedGattDevices)]", "Send GATT Proxy Add / Remove Device Addresses to / from Filter"},
   { DisconnectGattProxyConnection, false, "DisconnectGattProxyConnection",  "[Connection Index (0 - 3) (taken from ListConnectedGattDevices)]", "Disconnect GATT Proxy Connection"},
   { IncrementNetworkIVIndex,   false, "IncrementNetworkIVIndex",   "[Provisioned Network Index (1 octet)] ",  "Increments IV Index for the given Provisioned Network"},
   { UpdateKey,                 false, "UpdateKey",                 "[Provisioned Network Index (1 octet)] [KeyType (1=Application Key, 2=Network Key)] [Global Network Key Index (12 bit value 0x000 - 0xFFF)] [Network or Application Key (16 octets in hex(32 chars))] [Global App Key Index (12 bit value 0x000 - 0xFFF) (if Key Type is 1)]",  "Updates the existing Key at the specified Key Index. Must be called in Provisioner role"},
   { UpdateKeyRefreshPhase,     false, "UpdateKeyRefreshPhase",     "[Provisioned Network Index (1 octet)] [Global Network Key Index (12 bit value 0x000 - 0xFFF)] [New Phase(0=Normal, 1=Phase 1, 2=Phase 2, 3=Phase 3)]",  "Updates the Key Refresh Phase. Must be called in Provisioner role only"},
   { UpdateModelAppBindList,    false, "UpdateModelAppBindList",    "[Provisioned Network Index (1 octet)] [Element Address (2 octets)] [Global App Key Index (12 bit value 0x000 - 0xFFF)] [Model ID (2 or 4 octets)] [Model Type (0=SIG model, 1=Vendor model)] [0=Unbind, 1=Bind]",  "Updates Model to Application Binding List"},
   { ClearReplayProtectionEntries, false, "ClearReplayProtectionEntries", "[Provisioned Network Index (1 octet)] [Primary Element Address(2 octets) [Number of elements in the peer deivce (1 octet)]", "Clear RPL for the specified provisioned network"},

#if (APP_PTS_MODE == 1)
   { AddDelAdvBearers,             false, "AddorDeleteLEAdvBearers", "[Enable (0=Disable, 1=Enable)]",  "Adds or Deletes the LE ADV Bearers"},
   { AddSelfDeviceKey,             false, "AddSelfDeviceKey",        "[Provisioned Network Index (1 octet)] [Global Network Key Index (12 bit value 0x000 - 0xFFF)] [Device Key (16 octets in hex(32 chars))]",  "Adds the Self Device Key. Typically used when PTS tests needs to be run without provisioning. "},
   { SetStackPTSmode,              false, "SetStackPTSmode",          "[State (0=Disable, 1=Enable)]",  "PTS mode: Disable/Enable"},
   { SetSecureNetworkBeaconState,  false, "SetSecureNetworkBeaconState",   "[Provisioned Network Index (1 octet)] [State (0=Disable, 1=Enable)]",  "PTS mode: Sets the secure network beacon broadcast state"},
   { SetSecureNetworkBeaconPeriod, false, "SetSecureNetworkBeaconPeriod",   "[Provisioned Network Index (1 octet)] [Global Network Key Index (12 bit value 0x000 - 0xFFF)] [Beacon Period in seconds]",  "PTS mode: Sets the Beacon Period in seconds"},
   { SetMaxSubnets,                false, "SetMaxSubnets",  "[Provisioned Network Index (1 octet)] [Max Subnets per network (2 octets)]"},
   { EnableIVTest,                 false, "IV Test Mode Enable", "[State (1 octet)(0=Disable, 1=Enable)]"},
   { SendIVTestSignal,             false, "IV Signal Send", "[Provisioned Network Index (1 octet)] [IV Signal (0 - To Normal, 1 - In Progress]"},
   { ClearRpl,                  false, "ClearRpl", "[Provisioned Network Index (1 octet)]", "Clear replay protection list"},
#endif
   /* co-habitation API */
   { SendUserLEAdvert,          false, "SendUserAdvertisement",
                                       "[Advert Data (Max. 31 octets)] [Advert Length (1 octet)] [Advert Interval Min (in ms)] [Advert Interval Max (in ms)]",
                                       "Send User LE Advertisement"},
   { StopUserLEAdvert,          false, "StopUserAdvertisement",
                                       "[None]",
                                       "Stop User LE Advertisement"},
   { DisplayCompData,           false, "Display composition data"},
   { UserNodeIdentity,          false, "Enable User Triggered Node Identity Adverts"},
   { EnableFriendMode,          false, "Enable Friend Mode", "[Friend Mode(1 octet)(0=Disable, 1=Enable)]"},
   { StartIVRecovery,           false, "Enable IV Recovery", "[Provisioned Network Index (1 octet)] [Recovery (0=Disable, 1=Enable)]"},
   { DeviceReset,               false, "DeviceReset",               "[Provisioned Net ID]",  "Deletes the network and clears the PS information. " },
   { SaveStateAndReset,         false, "Save Keys and Reset", "[None]", "Save Keys to NVM and perform cold reset."},
   { PrintMemHeapUsage,         false, "Print Mem Heap Usage", "[None]", "Print the Memory Heap Usage"},
};

QCLI_Command_Group_t qmesh_cmd_group =
{
    "QMesh",
    (sizeof(qmesh_cmd_list) / sizeof(qmesh_cmd_list[0])),
    qmesh_cmd_list
};


/*============================================================================*
 *  Private Variables
 *============================================================================*/

static qapi_BLE_HCI_DriverInformation_t HCI_DriverInformation;
                                            /* The HCI Driver structure that   */
                                            /* holds the HCI Transports        */
                                            /* settings used to initialize     */
                                            /* Bluetopia.                      */



/*****************************************************************************
 * SIG Mesh App Public API
 *
 * This function is used to register the SPPLE Command Group with QCLI.
 *****************************************************************************/
void Initialize_QMesh_Demo (void)
{
    /* Attempt to reqister the BLE Command Group with the qcli framework.*/
    mesh_group = QCLI_Register_Command_Group (NULL, &qmesh_cmd_group);
    if(mesh_group)
    {
        /* Set the HCI driver information. */
        QAPI_BLE_HCI_DRIVER_SET_COMM_INFORMATION (&HCI_DriverInformation,
                                                  1,
                                                  115200,
                                                  QAPI_BLE_COMM_PROTOCOL_UART_E);
        mesh_model_group = QCLI_Register_Command_Group (NULL, &qmesh_Model_group);

        QCLI_Register_Command_Group (NULL, &qmesh_ConfigModel_group);
    }
    else
    {
        /* Un-register the BLE Group. */
         QCLI_Unregister_Command_Group (mesh_group);
         mesh_group      = NULL;
    }
}

