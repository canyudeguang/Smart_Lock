/******************************************************************************
 *  Copyright 2017-2018 Qualcomm Technologies International, Ltd.
 *  All Rights Reserved.
 *  Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*
  * This file contains data types and prototypes for Provisioning functions.
 */
/*****************************************************************************/
#ifndef __QMESH_DEMO_PROVISIONER_H__
#define __QMESH_DEMO_PROVISIONER_H__

#include "qmesh.h"
#include "qmesh_demo_config.h"

/* Mesh Application state */
typedef enum
{
    app_state_idle = 0,
    app_state_initialized,
    app_state_discovery,
    app_state_not_provisioned,  /* Application Initial State */
    app_state_provision_started,    /* Application state association started */
    app_state_provisioned,          /* Application state associated */
    app_state_application_key_add,
    app_state_model_key_bind
} QMESHAPP_STATE;

/* Subnet Info stored in NVM */
typedef struct
{
    QMESH_KEY_IDX_INFO_T        netkey_idx;
    QMESH_NODE_IDENTITY_T       node_identity;
    uint16_t                    node_identity_cnt;
    bool                        valid;
} QMESHAPP_SUBNET_DATA_T;

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
} QMESHAPP_PEER_DEVICE_KEY_LIST;

typedef struct
{
    QMESHAPP_STATE                                     app_state;
    QMESH_BEARER_HANDLE_T            bearer_handle[QMESH_BEARER_TYPE_MAX];
    QMESHAPP_PEER_DEVICE_KEY_LIST      *peer_device_key_list;
    uint8_t                                               unprov_device_index;
    bool                                                stack_initialized; /* TRUE if BT stack initialized else FALSE */
    bool                                                provisioning_client; /* TRUE if provisioning the client  node
                                                                                                FALSE if provisioning the server node */
    QMESH_DEVICE_UUID_T                 unprovisioned_devices[QMESH_MAX_NUM_OF_PROVISIONABLE_DEVICES];
    QMESHAPP_SUBNET_DATA_T                     subnet_data[MAX_NUM_OF_SUB_NETWORK];
}QMESHAPP_CONTEXT_T;

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
extern QmeshResult InitializeMesh(QMESHAPP_CONTEXT_T *context);
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
extern QmeshResult StartDeviceDiscovery(QMESH_DEVICE_UUID_T *const uuid_list,
                                                uint16_t uuid_list_len, bool ignore_dup);
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
extern QmeshResult StartProvisioning(QMESH_KEY_IDX_INFO_T net_key, uint16_t pri_elm_addr,
    QMESH_DEVICE_UUID_T *uuid);
#endif

