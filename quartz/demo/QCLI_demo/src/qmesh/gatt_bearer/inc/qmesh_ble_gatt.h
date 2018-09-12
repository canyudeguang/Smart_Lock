/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*! \file qmesh_ble_gatt.h
 *  \brief qmesh ble gatt specific functions
 *
 *   This file contains the API for GATT bearer
 *
 */
 /****************************************************************************/

#ifndef __QMESH_BLE_GATT_H__
#define __QMESH_BLE_GATT_H__

#include "qapi.h"
#include "qapi_ble_bttypes.h"
#include "qapi_ble_gap.h"
#include "qapi_ble_gaps.h"
#include "qapi_ble_bsc.h"
#include "qapi_ble_hci.h"
#include "qapi_ble_bsc.h"
#include "qapi_ble_gapstypes.h"
#include "qapi_ble_gatt.h"
#include "qmesh_demo_config.h"

/*! \addtogroup PB_GATT_Bearer
 * @{
 */
typedef struct GATT_DATA_T
{
    APP_GAP_LE_Event_Callback           appGapLeEvtCallback;
    APP_GATT_Connection_Event_Callback  appGattEvtCallback;
    bool                                provisioner_role_enabled;
    qapi_BLE_GATT_Device_Connection_Data_t	newGatConnData;
} Qmesh_Gatt_Data_T;

/*----------------------------------------------------------------------------*
 * InitGattBearer
 */
/*! \brief Initializes GATT bearer
 *
 *  The following function initializes GATT bearer as GATT Client/Sever role
 *  based on the gattData->Provisioner Role Enabled flag.
 *
 *  \param serviceReq  Service required (proxy or provisioning)
 *  \param funcPtr  Pointer to a function to be invoked post enabling notification
 *  \param Qmesh_Gatt_Data_T  Pointer to Qmesh_Gatt_Data_T type structure
 *
 *  \returns TRUE on successful GATT bearer initialization, else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool InitGattBearer(QMESH_GATT_BEARER_SERVICE_TYPE_T serviceReq,
                            APP_GATT_Bearer_Callback funcPtr,
                            Qmesh_Gatt_Data_T * gattData);

/*----------------------------------------------------------------------------*
 * DeInitGattBearer
 */
/*! \brief De Initializes GATT bearer
 *
 *  The following function reomves the pipe clears the GATT bearer data
 *
 *  \returns TRUE on successful GATT bearer deinitialization, else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool DeInitGattBearer(void);

/*----------------------------------------------------------------------------*
 * SetGATTBearerHandler
 */
/*! \brief Sets the handler function
 *
 *  Sets the GATT bearer handler function corresponding to the service
 *
 *  \returns QMESH_GATT_BEARER_CNTXT_INFO_T See \ref QMESH_GATT_BEARER_CNTXT_INFO_T.
 */
/*---------------------------------------------------------------------------*/
QMESH_GATT_BEARER_CNTXT_INFO_T *SetGATTBearerHandler(QMESH_GATT_BEARER_SERVICE_TYPE_T srvcType,
                                                              uint32_t connId);

/*----------------------------------------------------------------------------*
 * SendMtuUpdateEvent
 */
/*! \brief Sends MTU update event to application
 *
 *  Function is used to sends MTU update event from both client/server to 
 *  the application.
 *
 *  \returns True, on able to send event to application. False, otherwise.
 */
/*---------------------------------------------------------------------------*/
bool SendMtuUpdateEvent(uint32_t connId, 
    					qapi_BLE_GATT_Connection_Type_t connType, 
    					const qapi_BLE_BD_ADDR_t *addr, 
    					uint16_t mtu);

/*----------------------------------------------------------------------------*
 * GetNewGattConnData
 */
/*! \brief Provides the newly established GATT-Connection data
 *
 *  Provides the newly established GATT-Connection data.
 *
 *  \returns qapi_BLE_GATT_Device_Connection_Data_t See \ref qapi_BLE_GATT_Device_Connection_Data_t.
 */
/*---------------------------------------------------------------------------*/
qapi_BLE_GATT_Device_Connection_Data_t *GetNewGattConnData(void);

/*----------------------------------------------------------------------------*
 * RemoveGATTBearerHandler
 */
/*! \brief Frees the context pointer
 *
 *  Removes the context information by clearing the memory
 *
 *  \returns 
 */
/*---------------------------------------------------------------------------*/
void RemoveGATTBearerHandler(QMESH_GATT_BEARER_SERVICE_TYPE_T srvcType);

/*----------------------------------------------------------------------------*
 * IsProvisionerRoleEnabled
 */
/*! \brief Gets the role enabled info
 *
 *  True if provisioner role enabled in GATT-bearer else FALSE
 *
 *  \returns TRUE if provisioner role enabled else FALSE.
 */
/*---------------------------------------------------------------------------*/
bool IsProvisionerRoleEnabled(void);

/*----------------------------------------------------------------------------*
 * GATT_Connection_Event_Callback
 */
/*! \brief Recieves connection specific events
 *
 *  Receives GATT connection specific events
 *
 *  \returns None.
 */
/*---------------------------------------------------------------------------*/
void QAPI_BLE_BTPSAPI GATT_Connection_Event_Callback(uint32_t btStackId, 
                            qapi_BLE_GATT_Connection_Event_Data_t *GATT_Connection_Event_Data,
                            uint32_t CallbackParameter);

#endif /* __QMESH_BLE_GATT_H__*/

