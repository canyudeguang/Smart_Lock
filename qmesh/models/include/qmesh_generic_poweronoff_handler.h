/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/******************************************************************************/
/*! \file qmesh_generic_poweronoff_handler.h
 *  \brief defines and functions for SIGmesh Generic_Power OnOff Handler
 *
 *   This file contains data types and APIs exposed by the Generic Power
 *   OnOff model to the Applications
 */
/******************************************************************************/
#ifndef __QMESH_GENERIC_POWERONOFF_HANDLER_H__
#define __QMESH_GENERIC_POWERONOFF_HANDLER_H__


#include "qmesh_model_common.h"
#include "qmesh_generic_default_transition_time_handler.h"
#include "qmesh_generic_onoff_handler.h"

/*! \addtogroup Model_Generic_Power_OnOff_Handler
 * @{
 */


/*!\brief  PowerOnOff Status message size */
#define POWERONOFF_STATUS_REPORT_SIZE            (3)

#define GENERIC_POWERUP_STATE_VALUE_MAX          (2)

#define  QMESH_GENERIC_ONPOWERUP_MSG_LEN     (1)

/*!\brief  Generic PowerOnOff Server model data */
typedef struct
{
    uint16_t                         elm_addr;
    uint8_t
    onpowerup;                  /* OnPowerUp State Value */
    uint8_t                           status[POWERONOFF_STATUS_REPORT_SIZE];
    uint32_t    last_msg_seq_no; /* Stores the sequence number of the last processed model message */
    uint16_t    last_src_addr; /* Stores the element address of the last processed model message */

    /* Model Publication related details */
    QMESH_TIMER_HANDLE_T
    publish_timer; /* Timer to send status messages to Publish addresses periodically */
    QMESH_TIMER_HANDLE_T
    publish_retrans_timer; /* Timer to restransmit status messages to Publish addresses */
    uint8_t
    publish_restrans_cnt; /*!< \brief Publication retransmission count */
    uint32_t
    publish_interval;         /*!< \brief Publication interval */
    QMESH_MODEL_PUBLISH_STATE_T
    *publish_state; /*!< \brief Pointer to the Model Publication State */

    QMESH_ACCESS_PAYLOAD_KEY_INFO_T key_info;
} QMESH_GEN_POWERONOFF_DATA_T;

/*!\brief Generic Power OnOff NVM state */
typedef struct __attribute__ ((__packed__))
{
    uint8_t      onpowerup;                  /* OnPowerUp State Value */
    uint32_t    last_msg_seq_no; /*!\brief Stores the sequence number of the
                                           last processed model message */
    uint16_t    last_src_addr;   /*!\brief Stores the element address of the
                                         last processed model message */
} QMESH_MODEL_GEN_PWR_ONOFF_NVM_STATE_T;

/*----------------------------------------------------------------------------*
*      QmeshGenericPowerOnOffUpdateNvm
*/
/*! \brief This function writes the current state of Power On Off to NVM.
*
 *  \param [in] model_context
*
 *  \returns nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffUpdateNvm(QMESH_GEN_POWERONOFF_DATA_T
                                                     *model_context);

/*----------------------------------------------------------------------------*
* QmeshGenericPowerOnOffPublishTimerCb
*/
/*! \brief Generic Power On Off Publish timer callback.
 *
 *  \param [in] timerHandle
 *  \param [in] context
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffPublishTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context);

/*----------------------------------------------------------------------------*
* QmeshGenericPowerOnOffStatusSend
*/
/*! \brief Call this API to send the power onoff status message.
 *
 *  \param [in] model_context
 *  \param [in] msg_cmn
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffStatusSend (
    QMESH_GEN_POWERONOFF_DATA_T *model_context,
    QMESH_MODEL_MSG_COMMON_T *msg_cmn);

/*----------------------------------------------------------------------------*
* QmeshGenericPowerOnOffStatusPublish
*/
/*! \brief Publish Generic Power On/Off status message to the publish address.
 *
 *  \param [in] model_context
 *  \return nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffStatusPublish (
    QMESH_GEN_POWERONOFF_DATA_T *model_context);

/*----------------------------------------------------------------------------*
 *      QmeshGenericPowerOnOffUpdatePublicationPeriod
 */
/*! \brief  Publication configuration handler for Power On/Off Model Server.
 *
 *  \param [in] model_data Pointer to the Generic Power On/Off model Data
 *                       \ref QMESH_MODEL_DATA_T
 *
 *  \return bool
 *
 */
/*----------------------------------------------------------------------------*/
bool QmeshGenericPowerOnOffUpdatePublicationPeriod (QMESH_MODEL_DATA_T
        *model_data);

/*----------------------------------------------------------------------------*
 *      QmeshGenericPowerOnOffPublicationHandler
 */
/*! \brief  Publication configuration handler for Generic Power On/Off model
 *
 *  \param [in] model_data   Pointer to the Generic Power On/Off model Data \ref QMESH_MODEL_DATA_T
 *
 *  \return None
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffPublicationHandler (QMESH_MODEL_DATA_T
        *model_data);

/*----------------------------------------------------------------------------*
 * GenericPowerOnOffAppEventHandler
 */
/*! \brief Generic PowerOnOff Application Event Handler
 *
 *  \param [in] nw_hdr          Network header \ref QMESH_NW_HEADER_INFO_T
 *  \param [in] key_info        Pointer to Access layer key \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T
 *  \param [in] elem_data       Pointer to element data \ref QMESH_ELEMENT_CONFIG_T
 *  \param [in] model_data      Pointer to Model context \ref QMESH_MODEL_DATA_T
 *  \param [in] msg             Pointer to Message Data
 *  \param [in] msg_len         Message Data Length
 *
 *  \return \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericPowerOnOffServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerOnOffSet
 */
/*! \brief Application calls this API to set the associated state value.
 *
 *  \param [in] context
 *  \param [in] OnPowerUp
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffSet (QMESH_GEN_POWERONOFF_DATA_T
                                       *context, uint8_t OnPowerUp);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerOnOffInit
 */
/*! \brief Call this API to initialize power onoff server.
 *
 *  \param [in] model_data  Pointer to the Generic Power On/Off model Data \ref QMESH_MODEL_DATA_T
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffInit (QMESH_GEN_POWERONOFF_DATA_T
                                        *model_data);

/*!@} */
#endif /* __QMESH_GENERIC_POWERONOFF_HANDLER_H__ */
