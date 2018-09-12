/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_qca402x_sched.h
 *  \brief Qmesh library LE bearer scheduling configuration and control functions
 *
 *   This file contains the functions to provide the application to
 *   configuration and control of LE Bearer.
 *
 */
 /******************************************************************************/

#ifndef __QMESH_QCA402X_SCHED_H__
#define __QMESH_QCA402X_SCHED_H__

#include "qmesh_types.h"
#include "qapi_ble_gap.h"
#include "qmesh_soft_timers.h"
/*! \addtogroup Scheduler
 * @{
 */

/**
   This enumeration represents the valid service codes that can be used to register with bluetopia stack
*/
#define QMESH_GATT_RX_QUEUE_BUFFER_SIZE              (64UL)

/*! \brief GATT bearer service types. */
typedef enum
{
   QMESH_GATT_BEARER_UNKOWN_SVC_E,      /*!< Service type unknown. */
   QMESH_GATT_BEARER_PROV_SVC_E,        /*!< Service type provisioner. */
   QMESH_GATT_BEARER_PROXY_SVC_E        /*!< Service type proxy. */
} QMESH_GATT_BEARER_SERVICE_TYPE_T;

/*! \brief GATT bearer data. */
typedef struct
{
    QMESH_BEARER_HANDLE_T               handle;     /*!< Bearer Handle. */
    QMESH_GATT_BEARER_SERVICE_TYPE_T    srvcType;   /*!< Service type.*/
    uint16_t                            length;     /*!< Length of service data.*/
    uint8_t                             *data;      /*!< Pointer to service data buffer. */
} QMESH_GATT_BEARER_DATA_T;

/*! \brief Mesh message data. */
typedef struct
{
    uint32_t connectionId;              /*!< Connection identifier. */
    uint8_t length;                     /*!< Message length. */
    uint8_t *msg;                       /*!< Pointer to message buffer.*/
} QMESH_MESH_MSG_DATA_T;

/*! \brief GATT bearer send callback function. */
typedef bool(*GATT_Bearer_Send_Callback)(void *);

/*! \brief GATT bearer context information. */
typedef struct
{
    uint32_t connectionId;              /*!< Connection identifier. */
    GATT_Bearer_Send_Callback funcPtr;  /*!< Call back function \ref GATT_Bearer_Send_Callback */
} QMESH_GATT_BEARER_CNTXT_INFO_T;

/*----------------------------------------------------------------------------*
 * QmeshSchedPerformUserScan
 */
/*! \brief Starts the BLE scan with the passed parameters.
 *
 *  This API starts the BLE scan with the settings passed.
 *
 *  \param[in] ScanType The type of the bluetooth scan.
 *  \param[in] ScanInterval The scan interval for the bluetooth scan.
 *  \param[in] ScanWindow The scan window for the bluetooth scan.
 *  \param[in] LocalAddressType The address type to be used by local device during the bluetooth scan.
 *  \param[in] FilterPolicy Filter policy to apply when scanning.
 *  \param[in] FilterDuplicates Specifies whether or not the host controller is to filter duplicate scan responses..
 *  \param[in] GAP_LE_Event_Callback Pointer to a callback function to be used by the GAP layer to dispatch GAP LE Event information for this request.
 *  \param[in] CallbackParameter User defined value to be used by the GAP layer as an input parameter for the callback
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSchedPerformUserScan(qapi_BLE_GAP_LE_Scan_Type_t ScanType, uint32_t ScanInterval, uint32_t ScanWindow,
                                                qapi_BLE_GAP_LE_Address_Type_t LocalAddressType, qapi_BLE_GAP_LE_Filter_Policy_t FilterPolicy,
                                                bool FilterDuplicates, qapi_BLE_GAP_LE_Event_Callback_t GAP_LE_Event_Callback, uint32_t CallbackParameter);
/*----------------------------------------------------------------------------*
 * QmeshSchedCancelUserScan
 */
/*! \brief Cancels the user scan initiated.
 *
 *  This API cancels the BLE ongoing scan
 *
 *  \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSchedCancelUserScan(void);

/*----------------------------------------------------------------------------*
 * QmeshSchedUserAdvertEnable
 */
/*! \brief Starts the BLE advertising procedure with the passed parameters.
 *
 *  This API starts the BLE advertising with the settings passed. The advertising data and scan response data should be set before this function is called to enable advertising.
 *
 *  \param[in] EnableScanResponse Flag which specifies whether or not the device should send scan response data in response to a scan request.
 *  \param[in] GAP_LE_Advertising_Parameters Pointer to advertising parameters that control how the advertising is performed.
 *  \param[in] GAP_LE_Connectability_Parameters Specifies the connectability parameters to use while advertising.
 *  \param[in] GAP_LE_Event_Callback Pointer to a callback function to be used by the GAP layer to dispatch GAP LE Event information for this request.
 *  \param[in] CallbackParameter User defined value to be used by the GAP layer as an input parameter for the callback
 *
 *  \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSchedUserAdvertEnable(bool EnableScanResponse,
                                                 qapi_BLE_GAP_LE_Advertising_Parameters_t *GAP_LE_Advertising_Parameters,
                                                 qapi_BLE_GAP_LE_Connectability_Parameters_t *GAP_LE_Connectability_Parameters,
                                                 qapi_BLE_GAP_LE_Event_Callback_t GAP_LE_Event_Callback, uint32_t CallbackParameter);

/*----------------------------------------------------------------------------*
 * QmeshSchedUserAdvertDisable
 */
/*! \brief Disables the user advertisement initiated.
 *
 *  This API cancels the ongoing BLE advertisements.
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
extern void QmeshSchedUserAdvertDisable(void);

/*----------------------------------------------------------------------------*
 * QmeshSchedSetUserScanRspData
 */
/*! \brief Sets the user scan response data onto the scheduler.
 *
 *  This API is provided to allow the local host the ability to set the scan response data that is used during the advertising procedure
 *
 *  \param[in] Length The Number of significant scan response data bytes contained in the scan response data.
 *  \param[in] Scan_Response_Data Pointer to a buffer that contains the response data.
 *
 *  \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSchedSetUserScanRspData(uint32_t Length, qapi_BLE_Scan_Response_Data_t *Scan_Response_Data);

/*----------------------------------------------------------------------------*
 * QmeshSchedSetUserAdvertData
 */
/*! \brief Sets the user advertisement data onto the scheduler.
 *
 *  This API is provided to allow the local host the ability to set the advertisement data that is used during the advertising procedure
 *
 *  \param[in] Length The Number of significant advertising data bytes contained in the advertising data.
 *  \param[in] Advertising_Data Pointer to a buffer that contains the advertising data.
 *
 *  \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSchedSetUserAdvertData(uint32_t Length, qapi_BLE_Advertising_Data_t *Advertising_Data);

/*----------------------------------------------------------------------------*
 * QmeshQueueGattRxData
 */
/*! \brief Queue the data received over GATT.
 *  \returns see \ref QMESH_RESULT_T
 *           -QMESH_RESULT_FAILURE : Failed to queue the event.
 *            QMESH_RESULT_SUCCESS : Successfully queued the event.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshQueueGattRxData(QMESH_GATT_BEARER_DATA_T *gbd);

/*!@} */

#endif /*__QMESH_QCA402X_SCHED_H__*/

