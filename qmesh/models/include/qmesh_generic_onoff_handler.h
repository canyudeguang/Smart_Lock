/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/*! \file qmesh_generic_onoff_handler.h
 *  \brief defines and functions for SIGmesh Generic_OnOff model
 *
 *   This file contains data types and APIs exposed by the Generic OnOff model
 *   to the Applications
 */
/******************************************************************************/
#ifndef __QMESH_GENERIC_ONOFF_HANDLER_H__
#define __QMESH_GENERIC_ONOFF_HANDLER_H__

#include "qmesh_cache_mgmt.h"
#include "qmesh_model_common.h"

/** \addtogroup Model_Generic_OnOff_Server
* @{
*/

/*!\brief Length for Generic OnOff Get message */
#define GENERIC_ONOFF_GET_MSG_LEN                            (0)

/*!\brief Length for Generic OnOff Set message with transition and delay fields */
#define GENERIC_ONOFF_SET_NON_TRANS_MSG_LEN                    (2)

/*!\brief Length for Generic OnOff Set message without transition and dealy fields */
#define GENERIC_ONOFF_SET_TRANS_MSG_LEN                    (4)

/*!\brief Generic OnOff message offset values for various fields */
#define GENERIC_ONOFF_VALUE_OFFSET                                   (0)
#define GENERIC_ONOFF_TID_OFFSET                                     (1)
#define GENERIC_ONOFF_TRANSITION_TIME_OFFSET                         (2)
#define GENERIC_ONOFF_DELAY_OFFSET                                   (3)

#define GENERIC_ONOFF_STATE_ON                                       (1)
#define GENERIC_ONOFF_STATE_OFF                                      (0)

/*!\brief Generic OnOff status message constants including 2 bytes opcode */
#define GENERIC_ONOFF_STATUS_MSG_PRESENT_ONOFF_OFFSET    (2)
#define GENERIC_ONOFF_STATUS_MSG_TARGET_ONOFF_OFFSET      (3)
#define GENERIC_ONOFF_STATUS_MSG_TIME_OFFSET                         (4)
#define GENERIC_ONOFF_STATUS_NON_TRANS_MSG_LEN                  (3)
#define GENERIC_ONOFF_STATUS_TRANS_MSG_LEN                            (5)

#define GENERIC_ONOFF_GET_TRANS_TIME(opcode, msg, msg_len, elm_data) \
                (msg_len > GENERIC_ONOFF_SET_NON_TRANS_MSG_LEN )? \
                 msg[GENERIC_ONOFF_TRANSITION_TIME_OFFSET]: QmeshGetGenericDTT(elm_data)

#define GENERIC_ONOFF_GET_DELAY_TIME(opcode, msg, msg_len, elm_data)  \
                (msg_len > GENERIC_ONOFF_SET_NON_TRANS_MSG_LEN )?  \
                msg[GENERIC_ONOFF_DELAY_OFFSET]:0

/*!\brief Generic OnOff Status message */
typedef struct __attribute__ ((__packed__))
{
    uint8_t prsnt_onoff;        /*!< Present OnOff state */
    uint8_t trgt_onoff;         /*!< Target OnOff state */
    uint8_t remaining_time;     /*!< LSB 0-5: Num of Steps, 6-7: Step Resolution */
}
QMESH_GENERIC_ONOFF_STATUS_T;

/*!\brief Generic OnOff 'state' */
typedef struct
{
    uint8_t cur_onoff;          /*!< Current OnOff state */
    uint8_t initial_onoff;      /*!< Initial OnOff state before transition begins */
    uint8_t trgt_onoff;         /*!< Target OnOff state */
    uint16_t trgt_time[3];      /*!< Target system time = current_time + 'Transition Time' */
    uint16_t cur_time[3];       /*!< Target system time = current_time + 'Transition Time' */
    QMESH_TIMER_HANDLE_T  cur_timer;       /*!< current tranistion timer handle */
} QMESH_GENERIC_ONOFF_STATE_T;

/*!\brief Holds all the details for Generic OnOff model */
typedef struct
{
    uint16_t    elm_addr;               /*!< Element address */
    uint8_t     transition_time;        /*!< LSB 0-5: Num of Steps, 6-7: Step Resolution */
    uint8_t     delay;                  /*!< Delay in 5 milliSec steps. Valid only if transitionTime is present */
    QMESH_GENERIC_ONOFF_STATE_T onoff_state; /*!< Represents the current state */
    uint8_t     status[5];              /*!< On/off status */
    uint8_t     remaining_time;         /*!< Remaining time for the transition to complete */
    uint32_t    last_msg_seq_no;        /*!< Stores the sequence number of the last processed model message */
    uint16_t    last_src_addr;          /*!< Stores the element address of the last processed model message */

    /*!\brief Model Publication related details */
    QMESH_TIMER_HANDLE_T    publish_timer;          /*!< Timer to send status messages to Publish addresses periodically */
    QMESH_TIMER_HANDLE_T    publish_retrans_timer;  /*!< Timer to restransmit status messages to Publish addresses */
    uint8_t    publish_restrans_cnt;                /*!< Publication retransmission count */
    uint32_t   publish_interval;                    /*!< Publication interval */
    QMESH_MODEL_PUBLISH_STATE_T    *publish_state;  /*!< Pointer to the Model Publication State */

    QMESH_ACCESS_PAYLOAD_KEY_INFO_T    key_info;    /*!< Network Key to use for sending response/status message.
                                                        Valid for the transition In-Progress state */
    QMESH_MODEL_MSG_COMMON_T *msg_common;           /*!< Current message being processed.Used for sending response/status message */
    QMESH_MUTEX_T onoff_mutex;                      /*!< OnOff mutex */
} QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T;

/*!\brief Generic OnOff NVM state */
typedef struct __attribute__ ((__packed__))
{
    uint8_t cur_onoff;           /*!\brief Current OnOff state */
    uint8_t trgt_onoff;          /*!\brief Target OnOff state */
    uint32_t    last_msg_seq_no; /*!\brief Stores the sequence number of the
                                           last processed model message */
    uint16_t    last_src_addr;   /*!\brief Stores the element address of the
                                         last processed model message */
} QMESH_MODEL_GEN_ONOFF_NVM_STATE_T;

/*----------------------------------------------------------------------------*
 *      QmeshGenericOnOffStatusSend
 */
/*! \brief  This function sends the current Generic OnOff Status message to the
 *              remote device as mentioned in 'msg_cmn' parameter.
 *
 *  \param [in] model_context   Pointer to the Generic OnOff Data \ref QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
 *  \param [in] msg_cmn         Pointer to \ref QMESH_MODEL_MSG_COMMON_T
 *
 *  \return None
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericOnOffStatusSend (
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context,
    QMESH_MODEL_MSG_COMMON_T *msg_cmn);

/*----------------------------------------------------------------------------*
 *      QmeshGenericOnOffStatusPublish
 */
/*! \brief  This function sends the current Generic OnOff Status message to the
 *              Publish address assigned for this model.
 *
 *  \param [in] model_context Pointer to the Generic OnOff Data \ref QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
 *
 *  \return None
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericOnOffStatusPublish (
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context);

/*----------------------------------------------------------------------------*
 *      QmeshGenericOnOffSet
 */
/*! \brief  This function sets the Generic OnOff state to the value passed in the
 *              'onoff' parameter as per transition_time and delay parameters.
 *
 *  Note: The caller should abort any ongoing transtion by calling
 *        QmeshGenericOnOffAbortTransition before calling this API.
 *
 *  \param [in] model_context Pointer to the Generic OnOff Data
 *                       \ref QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
 *  \param [in] transition_time     transition time
 *  \param [in] delay               delayed execution time
 *  \param [in] onoff               new onoff state value to set
 *
 *  \return None
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericOnOffSet (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
                                  *model_context, uint8_t transition_time, uint8_t delay, uint8_t onoff);

/*----------------------------------------------------------------------------*
 *      QmeshGenericOnOffGet
 */
/*! \brief  This function returns the current Generic OnOff state.
 *
 *  Note: The caller should abort any ongoing transtion by calling QmeshGenericOnOffAbortTransition
 *        before calling this API.
 *
 *  \param [in] model_context Pointer to the Generic OnOff Data \ref QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
 *
 *  \return uint8_t Returns the current state of on/off
 */
/*----------------------------------------------------------------------------*/
extern uint8_t QmeshGenericOnOffGet (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
                                   *model_context);

/*----------------------------------------------------------------------------*
 *      QmeshGenericOnOffUpdate
 */
/*! \brief  This function sets the Generic OnOff state to the value passed in the
 *          'onoff' parameter. Note: The caller should abort any ongoing transtion by calling
 *          QmeshGenericOnOffAbortTransition before calling this API.
 *
 *  \param [in] model_context Pointer to the Generic OnOff Data
 *                       \ref QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
 *  \param [in] onoff New onoff state value to set
 *
 *  \return None
 *
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericOnOffUpdate (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
                                     *model_context, uint8_t onoff);

/*----------------------------------------------------------------------------*
 *      QmeshGenericOnOffAbortTransition
 */
/*! \brief  This function aborts any ongoing transition and resets the state to
 * initial state of the transtion.
 *
 *  \param [in] model_context Pointer to the Generic OnOff Data
 *                       \ref QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
 *
 *  \return None
 *
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericOnOffAbortTransition (void *model_context);

/*----------------------------------------------------------------------------*
 *      QmeshGenericOnOffUpdatePublicationPeriod
 */
/*! \brief  Publication configuration handler for OnOff Model Server.
 *
 *  \param [in] model_data      Pointer to the Generic OnOf model Data \ref QMESH_MODEL_DATA_T
 *
 *  \return TRUE/FALSE
 *
 */
/*----------------------------------------------------------------------------*/
bool QmeshGenericOnOffUpdatePublicationPeriod (QMESH_MODEL_DATA_T *model_data);

/*----------------------------------------------------------------------------*
 *      QmeshGenericOnOffPublicationHandler
 */
/*! \brief  Publication configuration handler for Generic OnOff model
 *
 *  \param [in] model_data Pointer to the Generic OnOff model Data
 *                       \ref QMESH_MODEL_DATA_T
 *
 *  \return None
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericOnOffPublicationHandler (QMESH_MODEL_DATA_T
        *model_data);

/*----------------------------------------------------------------------------*
 *      QmeshGenericOnOffServerHandler
 */
/*! \brief  Generic OnOff server model handler callback registered with the mesh
 *  stack. This function gets called from the model mesh stack to send the model message.
 *  This API will also be called from other extended models which has bound
 *  state with this model.
 *
 *  \param [in] nw_hdr          Network header \ref QMESH_NW_HEADER_INFO_T
 *  \param [in] key_info        Pointer to Access layer key \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T
 *  \param [in] elem_data       Pointer to element data \ref QMESH_ELEMENT_CONFIG_T
 *  \param [in] model_data      Pointer to Model context \ref QMESH_MODEL_DATA_T
 *  \param [in] msg             Pointer to Message Data
 *  \param [in] msg_len         Message Data Length
 *
 *  \return \ref QMESH_RESULT_T
 *
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericOnOffServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len);

/*----------------------------------------------------------------------------*
 *      QmeshGenericOnOffServerAppInit
 */
/*! \brief  This function initialises Generic OnOff Server model state data.
 *
 *  \param [in] model_context Pointer to the Generic OnOff Data
 *                       \ref QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
 *
 *  \return \ref QMESH_RESULT_T
 *
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericOnOffServerAppInit (
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context);

/*----------------------------------------------------------------------------*
 *      QmeshGenericOnOffServerAppDeInit
 */
/*! \brief  This function de-initialises Generic OnOff Server model state data
 *
 *  \param [in] model_context Pointer to the Generic OnOff Data
 *                       \ref QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
 *
 *  \return None
 *
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericOnOffServerAppDeInit (
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context);

/*!@} */
#endif/* __QMESH_GENERIC_ONOFF_HANDLER_H__ */
