/******************************************************************************
 *  Copyright 2017-2018 Qualcomm Technologies International, Ltd.
 *  All Rights Reserved.
 *  Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*
  * This file contains data types and prototypes for Provisioning functions.
 */
/*****************************************************************************/
#ifndef __QMESH_DEMO_CLIENT_H__
#define __QMESH_DEMO_CLIENT_H__

#include "qmesh.h"
#include "qmesh_demo_client_config.h"

/* Mesh Application state */
typedef enum
{
    app_state_idle = 0,
    app_state_initialized,
    app_state_discoverable,
    app_state_provision_started,    /* Application state association started */
    app_state_provisioned          /* Application state associated */
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
/*! \brief Client application context */
typedef struct
{
    QMESHAPP_STATE                                     app_state;
    bool                                                stack_initialized; /* TRUE if BT stack initialized else FALSE */
    QMESH_BEARER_HANDLE_T            bearer_handle[QMESH_BEARER_TYPE_MAX];
    QMESHAPP_SUBNET_DATA_T                     subnet_data[MAX_NUM_OF_SUB_NETWORK];
}QMESHAPP_CONTEXT_T;

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
QmeshResult InitializeMesh(QMESHAPP_CONTEXT_T *app_context);

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
QmeshResult StartDiscoverable(void);

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
QmeshResult StopDiscoverable(void);

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
QmeshResult GenericOnOffModelGet(void);

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
    bool validTransitionTime);
#endif
