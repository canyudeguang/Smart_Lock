/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*! \file qmesh_ble_gatt_client.h
 *  \brief qmesh ble gatt client specific functions
 *
 *   This file contains the API for Provisioning/Proxy client service.
 *
 */
 /****************************************************************************/
#ifndef __QMESH_BLE_GATT_CLIENT_H__
#define __QMESH_BLE_GATT_CLIENT_H__

#include "qmesh_ble_gap.h"


typedef struct
{
    qapi_BLE_GATT_Attribute_Handle_Group_t  srvcHndleDetails; /*Holds registered service handle details*/
    uint16_t                                dataInCharValHandle;
    uint16_t                                dataOutCharValHandle;
    uint16_t                                dataOutNtfHandle;
} SRVC_HANDLE_DATA_T;

typedef struct
{
    QMESH_BEARER_HANDLE_T               bearer_handle;      /* Respective bearer handle for the GATT connection */
    uint32_t                            connectionId;       /* Holds Proxy/Provisioning service connection ID */
    int                                 curTransactionId;   /* Holds the current transaction ID */
    SRVC_HANDLE_DATA_T                  handles;            /* Hold information for the service handles for this connection */
    QMESH_GATT_BEARER_SERVICE_TYPE_T    srvcReqd;           /* Holds registered service information */
    qapi_BLE_BD_ADDR_t                  devAddr;            /* bd address of the connected device */
    uint8_t                             mtu;                /* Holds Proxy/Provisioning service MTU size */
} QMESH_GATT_CLIENT_DATA_T;

typedef struct
{
    QMESH_GATT_BEARER_SERVICE_TYPE_T    currentServiceReq;  /* Holds current registered service required */
    APP_GATT_Bearer_Callback            cbFuncPtr[2];       /* Holds callbacks for proxy and provisioning */
} QMESH_GATT_CALLBACK_INFO_T;

/*! \addtogroup PB_GATT_Bearer
 * @{
 */

/*----------------------------------------------------------------------------*
 * DestroyGattClientData
 */
/*! \brief Destroys allocated GATT Client data.
 *
 *  \param connectionId  Connection ID for which the gatt client
 *                       data needs to be removed.
 *
 *  \returns None.
 */
/*---------------------------------------------------------------------------*/
void DestroyGattClientData(uint32_t connectionId);

/*----------------------------------------------------------------------------*
 * IsClientBearerInitAllowed
 */
/*! \brief Check whether the client bearer reinitializaton is required or not
 *  The following function checks the callbacks allocated for the provisioning
 *  and proxy client and returns TRUE or FALSE based on the Init is required.
 *
 *  \returns TRUE, if one of the callback is still not allocated else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool IsClientBearerInitAllowed(void);

/*----------------------------------------------------------------------------*
 * InitializeGattClientBearer
 */
/*! \brief Initializes GATT client bearer
 *
 *  The following function initializes GATT client bearer
 *
 *  \param funcPtr     Pointer to function to be invoked post enabling notification
 *  \param serviceReq  type of service required (proxy or provisioning)
 *
 *  \returns True on successfully able to intialize, else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool InitializeGattClientBearer(APP_GATT_Bearer_Callback funcPtr, QMESH_GATT_BEARER_SERVICE_TYPE_T serviceReq);

/*----------------------------------------------------------------------------*
 * DeInitializeGattClientBearer
 */
/*! \brief DeInitializes GATT client bearer
 *
 *  The following function deinitializes GATT client bearer by freeing the allocated memory 
 *
 *  \returns None.
 */
/*---------------------------------------------------------------------------*/
void DeInitializeGattClientBearer(void);

/*----------------------------------------------------------------------------*
 * ExchangeMtu
 */
/*! \brief Send exchange MTU request
 *
 *  Send exchange MTU request to remote device post connection establishment 
 *
 *  \returns TRUE, if successfully able to send the request else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool ExchangeMtu(qapi_BLE_GATT_Device_Connection_Data_t *data);

/*----------------------------------------------------------------------------*
 * InitiateServiceDiscovery
 */
/*! \brief Discovers services present on the remote device
 *
 *  Initiates request to check fi the remote device supports provisioning/proxy service.
 *
 *  \returns TRUE, if successfully able to send the request else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool InitiateServiceDiscovery(qapi_BLE_GATT_Device_Connection_Data_t *data);


/*----------------------------------------------------------------------------*
 * SendMeshProvClientData
 */
/*! \brief Sends Provisioning service data via write without response
 *
 *  The following function sends data by performing write-without response on provisioning 
 *  service data-in characteristic.
 *
 *  \param meshDataPtr  Pointer to the data to be sent via write command
 *
 *  \returns True on successfully able to send notification, else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool SendMeshGattClientData(void *meshDataPtr);

/*----------------------------------------------------------------------------*
 * HandleServerNtf
 */
/*! \brief Parses the server notification received
 *
 *  The following function parses the invokes the corresponding client to handle server
 *  notification data 
 *
 *  \param ntfData  Pointer to notification data
 *
 *  \returns True if notification is handled, else FALSE..
 */
/*---------------------------------------------------------------------------*/
bool HandleServerNtf(qapi_BLE_GATT_Server_Notification_Data_t *ntfData);

/*----------------------------------------------------------------------------*
 * SetGATTClientServiceType
 */
/*! \brief Sets the GATT Client service used
 *
 *  Set the GATT service used in the client-role
 *
 *  \param srvcReqd  service info
 *
 *  \returns None.
 */
/*---------------------------------------------------------------------------*/
void SetGATTClientServiceType(QMESH_GATT_BEARER_SERVICE_TYPE_T srvcReqd);

/*----------------------------------------------------------------------------*
 * GetGATTClientServiceType
 */
/*! \brief Gets the GATT Client service
 *
 *  Gets the GATT-client service in use
 *
 *  \param connectionId  Connection ID for which the service type is required
 *
 *  \returns client service which was requested for connection.
 */
/*---------------------------------------------------------------------------*/
QMESH_GATT_BEARER_SERVICE_TYPE_T GetGATTClientServiceType(uint32_t connectionId);

/*!@} */

#endif /* __QMESH_BLE_GATT_CLIENT_H__*/

