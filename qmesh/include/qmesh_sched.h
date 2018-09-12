/*=============================================================================
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_sched.h
 *  \brief QMESH library LE bearer scheduling configuration and control functions
 *
 *   This file contains the functions to provide the application to
 *   configuration and control of LE Bearer.
 *
 */
/******************************************************************************/

#ifndef __QMESH_SCHED_H__
#define __QMESH_SCHED_H__
#include "qmesh_result.h"
#include "qmesh_data_types.h"

/*! \addtogroup Scheduler
 * @{
 */

/*============================================================================*
    Public Definitions
 *============================================================================*/
/*! \brief Maximum user advertising data length. */
#define QMESH_MAX_USER_ADV_DATA_LEN                   (31)

/*============================================================================*
    Data Type Definitions
 *============================================================================*/


/*============================================================================*
    Public Function Prototypes
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshSchedSetScanParams
 */
 /*! \brief  Sets the LE scan parameters.
 *
 *  \param[in] interval_us  Scan interval in microseconds.
 *                          Range: 2.5 ms to 10.24 s.
 *  \param[in] window_us    Scan window in microseconds.
 *                          Range: 2.5 ms to 10.24 s.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSchedSetScanParams(uint32_t interval_us, uint32_t window_us);

/*----------------------------------------------------------------------------*
 * QmeshSchedSetAdvParams
 */
/*! \brief  Sets the LE advertise parameters.
 *
 *  \param[in] interval_min_us  Qmesh advertising interval in microseconds.
 *                              Range: 20 ms to 10.24 s.
 *  \param[in] interval_max_us  Qmesh advertising window in microseconds.
 *                              Range: 20 ms to 10.24 s.
 *  \param[in] use_nrpa         Whether to use random private address.
 *  \param[in] channel_map      Channel map to be used. \n
 *                              Bit 0: Channel 37 used or not. \n
 *                              Bit 1: Channel 38 used or not. \n
 *                              Bit 2: Channel 39 used or not.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSchedSetAdvParams(uint32_t interval_min_us, uint32_t interval_max_us,
                                             bool use_nrpa, uint8_t channel_map);

/*----------------------------------------------------------------------------*
 * QmeshSchedInit
 */
/*! \brief Initializes the Qmesh scheduler.
 *
 *  This API initialises the Qmesh scheduler
 *
 *  \param[in] bt_stack    Pointer Bluetooth Stack to be used by the scheduler.
 *                     The Bluetooth Stack is initialized by the application
 *                     and the pointer to the stack is passed to the scheduler
 *                     for usage.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSchedInit(void *bt_stack);

/*----------------------------------------------------------------------------*
 * QmeshSchedDeInit
 */
/*! \brief Deinitializes the Qmesh scheduler.
 *
 *  This API Deinitialises the mesh scheduler
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSchedDeInit(void);

/*----------------------------------------------------------------------------*
 * QmeshSchedEnableListening
 */
/*! \brief Starts or stops an LE scan operation
 *
 *  Application uses this api to start/stop LE scan operation.
 *
 *  \param[in] enable Enable (TRUE) or disable (FALSE) the scan operation.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSchedEnableListening(bool enable);

/*----------------------------------------------------------------------------*
 * QmeshIsSchedRunning
 */
/*! \brief Indicates whether the scheduler is ready to send
 *
 *  Application uses this api to check whether scheduler is ready for sending
 *  advtertisements.
 *
 *  \return FALSE if the scheduler is not running.
 */
/*---------------------------------------------------------------------------*/
extern bool QmeshIsSchedRunning(void);

/*----------------------------------------------------------------------------*
 * QmeshSchedStart
 */
/*! \brief Starts scheduling of an LE scan and advertisement.
 *
 *  This API starts scheduling of LE scan and advertisement
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSchedStart(void);

/*----------------------------------------------------------------------------*
 * QmeshSchedStop
 */
/*! \brief Stops scheduling of an LE scan and advertisement.
 *
 *  This API stops scheduling of LE scan and advertisement
 *
 *  \return None.
 */
/*---------------------------------------------------------------------------*/
extern void QmeshSchedStop(void);

/*----------------------------------------------------------------------------*
 * QmeshSchedSetTxPower
 */
/*! \brief Sets the transmit power for the device.
 *
 *  \param[in] level Level of the transmit power to be used. this is a
 *                   platform-dependent value.
 *
 *  \return None.
 */
/*---------------------------------------------------------------------------*/
extern void QmeshSchedSetTxPower(uint8_t level);

/*!@} */

#endif /* __QMESH_SCHED_H__*/

