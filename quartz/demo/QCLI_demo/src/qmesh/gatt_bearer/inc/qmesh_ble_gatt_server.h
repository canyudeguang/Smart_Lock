/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*! \file qmesh_ble_gatt_server.h
 *  \brief qmesh ble gatt server specific functions
 *
  *   This file contains the API for Provisioning/Proxy Server.
 *
 */
 /****************************************************************************/
#ifndef __QMESH_BLE_GATT_SERVER_H__
#define __QMESH_BLE_GATT_SERVER_H__

#include "qmesh_ble_gap.h"


typedef struct
{
    qapi_BLE_BD_ADDR_t         devAddr;     /* Holds remote device BD_ADDR*/
    uint32_t                   connectionId;   /* Holds Proxy/Provisioning service connection ID*/
    uint8_t                    mtu;            /* Holds Proxy/Provisioning service MTU size */
    bool                       notifyEnabled;  /* Holds Proxy/Provisioning service Notification Status */
    uint8_t                    ccdHndlValue[2]; /* Holds Proxy/Provisioning service CCD Handle Value*/
    QMESH_BEARER_HANDLE_T      bearerHandle;  /* Respective bearer handle for the GATT connection */
} QMESH_GATT_DEVICE_INFO_T;

/*! \addtogroup PB_GATT_Bearer
 * @{
 */

/*----------------------------------------------------------------------------*
 * InitializeGattServerBearer
 */
/*! \brief Initializes GATT server bearer
 *
 *  The following function initializes GATT server bearer
 *
 *
 *  \returns TRUE on successfully able to intialize, else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool InitializeGattServerBearer(void);

/*----------------------------------------------------------------------------*
 * DeInitializeGattServerBearer
 */
/*! \brief DeInitializes GATT server bearer
 *
 *  The following function deinitializes GATT server bearer by freeing the allocated memory 
 *
 *
 *  \returns None.
 */
/*---------------------------------------------------------------------------*/
void DeInitializeGattServerBearer(void);


/*----------------------------------------------------------------------------*
 * RegisterGattBearerService
 */
/*! \brief Registers a Proxy/Provisioning service with bluetopia stack
 *
 *  The following function registers the Proxy/Provisioning service.
 *
 *  \param serviceType  Has service to be registered info
 *
 *  \returns TRUE on successfully able to register the requested service, else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool RegisterGattBearerService(QMESH_GATT_BEARER_SERVICE_TYPE_T serviceType);

/*----------------------------------------------------------------------------*
 * UnregisterGattBearerService
 */
/*! \brief UnRegisters a Proxy/Provisioning service with bluetopia stack
 *
 *  The following function unregisters the Proxy/Provisioning service.
 *
 *  \param serviceType  Has service to be registered info
 *
 *  \returns TRUE on successfully able to unregister the requested service, else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool UnregisterGattBearerService(QMESH_GATT_BEARER_SERVICE_TYPE_T serviceType);

/*----------------------------------------------------------------------------*
 * SendMeshProvDataOutNtf
 */
/*! \brief Sends Provisioning service data via notification
 *
 *  The following function sends notification via provisioning service data-out characteristic.
 *
 *  \param meshDataPtr  Pointer to data to be sent via notification
 *
 *  \returns TRUE on successfully able to send notification, else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool SendMeshProvDataOutNtf(void *meshDataPtr);

/*----------------------------------------------------------------------------*
 * SendMeshProxyDataOutNtf
 */
/*! \brief Sends Proxy service data via notification
 *
 *  The following function sends notification via proxy service data-out characteristic.
 *
 *  \param meshDataPtr  Pointer to data to be sent via notification
 *
 *  \returns TRUE on successfully able to send notification, else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool SendMeshProxyDataOutNtf(void *meshDataPtr);

/*----------------------------------------------------------------------------*
 * GetGATTBearerServiceType
 */
/*! \brief Gets the service requested info
 *
 *  Get the service requested information in the server-role
 *
 *  \param 
 *
 *  \returns service-type which was requested for connection.
 */
/*---------------------------------------------------------------------------*/
QMESH_GATT_BEARER_SERVICE_TYPE_T GetGATTBearerServiceType(void);

/*----------------------------------------------------------------------------*
 * IsGATTProxyConnected
 */
/*! \brief Checks if the proxy service is connected
 *
 *  Checks if the proxy service is connected
 *
 *  \param 
 *
 *  \returns TRUE if GATT Proxy is connected, else FALSE
 */
/*---------------------------------------------------------------------------*/
bool IsGATTProxyConnected(void);

/*----------------------------------------------------------------------------*
 * setGATTConnBearerHandle
 */
/*! \brief Sets the GATT connection bearer handle
 *
 *  Sets the GATT connection bearer handle
 *
 *  \param connId  Device Connection ID
 *  \param bearer_handle  Bearer Handle
 *
 *  \returns TRUE if Bearer Handle is Set, else FALSE
 */
/*---------------------------------------------------------------------------*/
bool setGATTConnBearerHandle(uint32_t connId, QMESH_BEARER_HANDLE_T bearer_handle);


/*!@} */

#endif /* __QMESH_BLE_GATT_SERVER_H__*/

