/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*! \file qmesh_ble_gap.h
 *  \brief qmesh ble gap specific functions
 *
 *   This file contains the functions to provide the application to
 *   configuration and control of LE Bearer.
 *
 */
/*****************************************************************************/
#ifndef __QMESH_BLE_GAP_H__
#define __QMESH_BLE_GAP_H__

#include "qapi.h"
#include "qmesh_qca402x_sched.h"
#include "qapi_ble_bttypes.h"
#include "qapi_ble_gap.h"
#include "qapi_ble_gaps.h"
#include "qapi_ble_bsc.h"
#include "qapi_ble_hci.h"
#include "qapi_ble_bsc.h"
#include "qapi_ble_gapstypes.h"
#include "qapi_ble_gatt.h"

/*! \addtogroup PB_GATT_Bearer
 * @{
 */

/* Mesh Provisioning service UUID */
#define MESH_PROVISIONING_SERVICE_UUID                  {0x27, 0x18}
/* Mesh Provisioning Service characteristic UUIDS */
#define MESH_PROVISIONING_DATA_IN_UUID                  {0xDB, 0x2A}
#define MESH_PROVISIONING_DATA_OUT_UUID                 {0xDC, 0x2A}

/* Mesh proxy service UUID */
#define MESH_PROXY_SERVICE_UUID                         {0x28, 0x18}
/* Mesh Proxy Service characteristic UUIDS */
#define MESH_PROXY_DATA_IN_UUID                         {0xDD, 0x2A}
#define MESH_PROXY_DATA_OUT_UUID                        {0xDE, 0x2A}
#define MESH_GATT_CHAR_DESC_UUID                        {0x02, 0x29}

/*Connection Parameters*/
#define SCAN_INTERVAL                                   (4)
#define SCAN_WINDOW                                     (4)
#define MIN_CONNECTION_INTERVAL                         (50)
#define MAX_CONNECTION_INTERVAL                         (80)
#define MAX_PERMISSABLE_CONNECTION_INTERVAL             (200)
#define SLAVE_LATENCY                                   0
#define SUPERVISION_TIMEOUT                             (6000)
#define MIN_CONNECTION_LENGTH                           0
#define MAX_CONNECTION_LENGTH                           (10000)

/*----------------------------------------------------------------------------*
 * APP_GATT_Bearer_Callback
 */
/*! \brief Initiates provisioning procedure over GATT
 *
 *  This function is used as callback function to initiate provisioning/proxy specific operations post 
 *  connection and enabling notification
 *
 *  \param Pointer to a viod to be used in future if required
 *
 *  \returns None.
 */
/*---------------------------------------------------------------------------*/
typedef void (*APP_GATT_Bearer_Callback)(void *);

/*----------------------------------------------------------------------------*
 * APP_GAP_LE_Event_Callback
 */
/*! \brief Receives all GAP-LE specific events
 *
 *  This function is used as callback function to receive all LE specific events from Bluetopia stack
 *
 *  \param btStackId The bluetooth stack id used during initialisation of BT.
 *  \param GAP_LE_Event_Data  Pointer to LE event data
 *  \param CallbackParameter  Callback parameter provided during while registering the callback function
 *
 *  \returns None.
 */
/*---------------------------------------------------------------------------*/
typedef void (QAPI_BLE_BTPSAPI *APP_GAP_LE_Event_Callback)(uint32_t btStackId, 
                                    qapi_BLE_GAP_LE_Event_Data_t *GAP_LE_Event_Data, 
                                    uint32_t CallbackParameter);

/*----------------------------------------------------------------------------*
 * APP_GATT_Connection_Event_Callback
 */
/*! \brief Handles GATT connection event callback.
 *
 *  This function is used as callback function to receive all GATT specific events from Bluetopia Stack
 *
 *  \param btStackId  The bluetooth stack id used during initialisation of BT.
 *  \param GATT_Connection_Event_Data  Pointer to LE event data
 *  \param CallbackParameter  Callback parameter provided during while registering the callback function
 *
 *  \returns None.
 */
/*---------------------------------------------------------------------------*/
typedef void (QAPI_BLE_BTPSAPI *APP_GATT_Connection_Event_Callback)(uint32_t btStackId, 
                             qapi_BLE_GATT_Connection_Event_Data_t *GATT_Connection_Event_Data, 
                             uint32_t CallbackParameter);

/*----------------------------------------------------------------------------*
 * InitializeBluetooth
 */
/*! \brief Initializes Bluetopia stack.
 *
 *  Initializes Bluetopia stack and sets the stack ID
 *
 *  \param bt_stack_id Pointer to hold the bluetooth stack id.
 *                     The stack id value is filled into this, which can be
 *                     used by the application for bluetooth concurrency.
 *
 *  \returns TRUE if Bluetooth Stack initialized successfuly, else False.
 */
/*---------------------------------------------------------------------------*/
bool InitializeBluetooth(void);

/*----------------------------------------------------------------------------*
 * GetBluetoothStackId
 */
/*! \brief Returns the Bluetooth Stack ID.
 *
 *  Returns the value of Bluetooth Stack ID
 *
 *  \returns Returns the value of Bluetooth Stack ID.
 */
/*---------------------------------------------------------------------------*/
int32_t GetBluetoothStackId(void);

/*----------------------------------------------------------------------------*
 * RegisterGapGattCallbacks
 */
/*! \brief Registers a Proxy/Provisioning service with bluetopia stack
 *
 *  The following function registers the Proxy/Provisioning service.
 *
 *  \param gapLeEvtCallback  GAP LE Event Callback
 *  \param gattConnEvtCallback  GATT Connection Event Callback
 *
 *  \returns TRUE on successfully able to register the requested service, else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool RegisterGapGattCallbacks(APP_GAP_LE_Event_Callback gapLeEvtCallback, 
                                       APP_GATT_Connection_Event_Callback gattConnEvtCallback);

/*----------------------------------------------------------------------------*
 * InitiateGattConnection
 */
/*! \brief Creates GATT connection
 *
 *  The following function sends notification via proxy service data-out characteristic.
 *
 *  \param remDevAddrType  address type of the remote device
 *  \param remDevAddr  address of the remote device
 *  \param srvc  Provisioning/Proxy service for the which connection is being created
 *
 *  \returns TRUE on successfully able to initiate connection, else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool InitiateGattConnection(qapi_BLE_GAP_LE_Address_Type_t remDevAddrType, 
                             qapi_BLE_BD_ADDR_t *remDevAddr, 
                             QMESH_GATT_BEARER_SERVICE_TYPE_T srvc);

/*----------------------------------------------------------------------------*
 * SendConnectionParamUpdateRequest
 */
/*! \brief Sends connection parameter upadate request
 *
 *  The following function sends connection parameter update request to the remote device.
 *
 *  \param remDevAddr  address of the remote device
 *
 *  \returns TRUE on successfully sending update-request, FALSE otherwise.
 */
/*---------------------------------------------------------------------------*/
bool SendConnectionParamUpdateRequest(qapi_BLE_BD_ADDR_t remDevAddr);

/*----------------------------------------------------------------------------*
 * SendConnectionParamUpdateResponse
 */
/*! \brief Sends connection parameter update response
 *
 *  The following function sends response to connection parameter update request received.
 *
 *  \param remDevAddr  address of the remote device
 *
 *  \returns TRUE on successfully sending update-response, FALSE otherwise.
 */
/*---------------------------------------------------------------------------*/
bool SendConnectionParamUpdateResponse(qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t *data);

/*----------------------------------------------------------------------------*
 * InitiateGattDisconnection
 */
/*! \brief Initiates GATT disconnection
 *
 *  The following function initiates disconnection with the remote device based on address
 *
 *  \param remoteAddr  Pointer to the address of remote device
 *
 *  \returns None.
 */
/*---------------------------------------------------------------------------*/
void InitiateGattDisconnection(qapi_BLE_BD_ADDR_t remoteAddr);
/*!@} */

#endif /* __QMESH_BLE_GAP_H__*/

