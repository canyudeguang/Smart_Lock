/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/*! \file qmesh_generic_powerlevel_handler.h
 *  \brief defines and functions for SIGmesh Generic Power Level model server
 *
 *   This file contains data types and APIs exposed by the Generic Power Level
 *   server model to the Application
 */
/******************************************************************************/
#ifndef __QMESH_GENERIC_POWERLEVEL_HANDLER_H__
#define __QMESH_GENERIC_POWERLEVEL_HANDLER_H__

#include "qmesh_hal_ifce.h"
#include "qmesh_types.h"

#include "qmesh_generic_onoff_handler.h"
#include "qmesh_generic_level_handler.h"
#include "qmesh_generic_poweronoff_handler.h"
#include "qmesh_generic_poweronoff_setup_handler.h"
#include "qmesh_generic_default_transition_time_handler.h"

/** \addtogroup Model_Generic_Power_Level_Handler
* @{
*/

/*!\brief PowerLevel Set minimum message size */
#define GENERIC_POWERLEVEL_MIN_MSG_LEN       (3)
/*!\brief PowerLevel status maximum message size */
#define GENERIC_POWERLEVEL_MAX_STATUS_MSG_LEN       (7)

/*!\brief Generic PowerLevel state, TID, Transition and Delay offsets for PowerLevel Set messages
 */
#define GENERIC_POWERLEVEL_VALUE_OFFSET       (0)
#define GENERIC_POWERLEVEL_TID_OFFSET    (GENERIC_POWERLEVEL_VALUE_OFFSET + 2)
#define GENERIC_POWERLEVEL_TRANSITION_TIME_OFFSET (GENERIC_POWERLEVEL_TID_OFFSET + 1)
#define GENERIC_POWERLEVEL_DELAY_OFFSET  (GENERIC_POWERLEVEL_TRANSITION_TIME_OFFSET + 1)

/*!\brief Get PowerLevel transition time parameter from message */
#define GENERIC_POWER_LEVEL_GET_TRANS_TIME(opcode, msg, msg_len, elm_idx) \
    (msg_len > GENERIC_POWERLEVEL_MIN_MSG_LEN )? \
     msg[GENERIC_POWERLEVEL_TRANSITION_TIME_OFFSET]: QmeshGetGenericDTT(elm_idx)

/*!\brief Get PowerLevel delay parameter from message */
#define GENERIC_POWER_LEVEL_GET_DELAY_TIME(opcode, msg, msg_len, elm_idx)  \
                             (msg_len > GENERIC_POWERLEVEL_MIN_MSG_LEN )?  \
                             msg[GENERIC_POWERLEVEL_DELAY_OFFSET]:0

/*!\brief Length for PowerLevel Get messages */
#define POWER_LEVEL_GET_MSG_LEN                                  (0)

/*!\brief Minimum Length for PowerLevel Set message */
#define POWER_LEVEL_SET_NON_TRANS_MSG_LEN                        (3)

/*!\brief Maximum Length for PowerLevel Set message */
#define POWER_LEVEL_SET_TRANS_MSG_LEN                            (5)

/*!\brief Length for PowerLevel Last Get message */
#define POWER_LAST_GET_MSG_LEN                                   (0)

/*!\brief Length for PowerLevel Default Get message */
#define POWER_DEFAULT_GET_MSG_LEN                                (0)

/*!\brief Length for PowerLevel Range Get message */
#define POWER_RANGE_GET_MSG_LEN                                  (0)

/*!\brief Length for PowerLevel Default Get message */
#define POWER_DEFAULT_SET_MSG_LEN                                (2)

/*!\brief Length for PowerLevel Range Set message */
#define POWER_RANGE_SET_MSG_LEN                                  (4)

/*!\brief Minimum Length for PowerLevel Status message */
#define POWER_LEVEL_STATUS_NON_TRANS_MSG_LEN                     (4)

/*!\brief Maximum Length for PowerLevel Status message */
#define POWER_LEVEL_STATUS_TRANS_MSG_LEN                         (7)

/*!\brief Length for PowerLevel Last Status message */
#define POWER_LEVEL_LAST_STATUS_MSG_LEN                          (4)

/*!\brief Length for PowerLevel Range Status message */
#define POWER_LEVEL_RANGE_STATUS_MSG_LEN                         (7)

/*!\brief Length for PowerLevel Default Status message */
#define POWER_LEVEL_DEFAULT_STATUS_MSG_LEN                       (4)

/*!\brief Transition Time offset in Level Status messages */
#define POWER_LEVEL_STATUS_MSG_TIME_OFFSET                       (6)

/*\brief Represents Generic Power Level Status message */
typedef struct __attribute__ ((__packed__))
{
    uint16_t prsnt_pwr_actual;    /*!< Present Power Actual state*/
    uint16_t trgt_pwr_actual;     /*!< Target Power Actual state*/
    uint8_t remaining_time;       /*!< LSB 0-5: Num of Steps, 6-7: Step Resolution */
}
QMESH_GENERIC_POWERLEVEL_STATUS_T;

/*!\brief Represents Generic Power Range Status message */
typedef struct __attribute__ ((__packed__))
{
    uint8_t       status_code;        /*!< Status code of the requesting message */
    uint16_t      pwr_range_min;      /*!< The value of the Generic Power Range Min field of the Generic Power Range state */
    uint16_t      pwr_range_max;      /*!< The value of the Generic Power Range Max field of the Generic Power Range state */
}
QMESH_GENERIC_POWER_RANGE_STATUS_T;

/*!\brief Represents Generic Power Default Status message */
typedef struct __attribute__ ((__packed__))
{
    uint16_t      pwr_default;        /*!< Generic Power Default state */
}
QMESH_GENERIC_POWER_DEFAULT_STATUS_T;

/*!\brief Represents Generic Power Last Status message */
typedef struct __attribute__ ((__packed__))
{
    uint16_t      pwr_last;           /*!< Generic Power Last state */
}
QMESH_GENERIC_POWER_LAST_STATUS_T;

/*!\brief Transition data type.This structure contains all variables used during transition time */
typedef struct
{
    uint8_t  rem_count;                   /*!< Remaining number of times the current value need to be incremented by 'linearValue' */
    int16_t   linear_value;               /*!< The value by which the current 'state' value need to be incremented/decremented linearly */
    uint32_t  step_resolution;            /*!< Periodic timer value */
    bool adjust_step_resolution;        /*!< Check remaining transition time or linear value to adjust the linear value at the end of transition */
    uint8_t transition_time;              /*!< LSB 0-5: Num of Steps, 6-7: Step Resolution */
    QMESH_TIMER_HANDLE_T  trans_timer;  /*!< current transition timer handle */
    QMESH_TIMER_HANDLE_T nl_trans_timer; /*!< Current non linear transition timer handle. */
    uint32_t trans_time_ms;               /*!< transition timer in milli seconds. */
} QMESH_GENERIC_POWERLEVEL_TRANSITION_DATA_T;

/*!\brief Represents Generic Power Actual 'state' */
typedef struct
{
    uint16_t current_pwr_actual;          /*!< Current OnOff state */
    uint16_t initial_pwr_actual;          /*!< Initial OnOff state before transition begins */
    uint16_t trgt_pwr_actual;             /*!< Target OnOff state */
    uint32_t target_time;                 /*!< Target system time = current_time + 'Transition Time' */
    QMESH_GENERIC_POWERLEVEL_TRANSITION_DATA_T trans_data;  /*!< Transition data */
    uint8_t delay;                        /*!< Delay in 5 milliSec steps. Valid only if transitionTime is present */
    uint16_t trgt_time[3];                /*!< Target system time = current_time + 'Transition Time' */
} QMESH_GENERIC_POWERLEVEL_STATE_T;

/*!\brief Holds all the details for Generic Power Level server model */
typedef struct
{
    /*!\brief  Generic Power Level Server Data */
    QMESH_GENERIC_POWERLEVEL_STATE_T    pwr_actual_state; /*!< Represents the current Power Actual state */
    uint16_t  power_last;                 /*!< Represents the current Power Last state.0000 - prohibited */
    uint16_t  power_default;              /*!< Represents the current Power Default state. 0000 - prohibited */
    QMESH_GENERIC_POWER_RANGE_STATUS_T power_range; /*!< Represents the current Power Range state */
    uint16_t elm_addr;                    /*!< Element address */
    uint8_t status[GENERIC_POWERLEVEL_MAX_STATUS_MSG_LEN]; /*!< To send various Power Level Status messages */
    uint32_t    last_msg_seq_no;          /*!< Stores the sequence number of the last processed model message */
    uint16_t    last_src_addr;            /*!< Stores the element address of the last processed model message */

    /*!\brief Model Publication related details */
    QMESH_TIMER_HANDLE_T    publish_timer;          /*!< Timer to send status messages to Publish addresses periodically */
    QMESH_TIMER_HANDLE_T    publish_retrans_timer;  /*!< Timer to restransmit status messages to Publish addresses */
    uint8_t    publish_restrans_cnt;                  /*!< Publication retransmission count */
    uint32_t    publish_interval;                     /*!< Publication interval */
    QMESH_MODEL_PUBLISH_STATE_T *publish_state;     /*!< Pointer to the Model Publication State */
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T    key_info;    /*!< Key information used to send response */
    QMESH_MUTEX_T pwr_lvl_mutex;
} QMESH_GENERIC_POWERLEVEL_CONTEXT_T;

/*!\brief Generic PowerLevel NVM state */
typedef struct __attribute__ ((__packed__))
{
    uint16_t    current_pwr_actual; /*!\brief Current OnOff state */
    uint16_t    trgt_pwr_actual;   /*!\brief Target OnOff state */
    uint16_t    power_last; /*!\brief Represents the current Power Last state.
                             0000 - prohibited */
    uint16_t    power_default;/*!\brief Represents the current Power Default state.
                            0000 - prohibited */
    uint16_t    pwr_range_min;  /*!\brief The value of the Generic Power Range Min
                                   field of the Generic Power Range state */
    uint16_t    pwr_range_max; /*!\brief The value of the Generic Power Range Max
                                  field of the Generic Power Range state */

    uint32_t    last_msg_seq_no; /*!\brief Stores the sequence number of the
                                           last processed model message */
    uint16_t    last_src_addr;   /*!\brief Stores the element address of the
                                         last processed model message */
} QMESH_MODEL_GEN_PWR_LEVEL_NVM_STATE_T;

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelUpdateNvm
 */
/*! \brief This function writes the current state to NVM.
 *
 *  \param [in] model_context Pointer to the Generic PowerLevel Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *
 *  \returns Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelUpdateNvm(
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T
    *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelActualUpdate
 */
/*! \brief The API updates the Power Actual state to new state value as specificed
 *  in the parameter.
 *
 *  \param [in] model_context Pointer to the Generic PowerLevel Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *  \param [in] pwr_actual New PowerLevel Acutal state value to set
 *
 *  \return None.
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelActualUpdate (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T
    *model_context,
    uint16_t pwr_actual);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelActualGet
 */
/*! \brief The API returns the current Power Actual state.
 *
 *  \param [in] model_context Pointer to the Generic PowerLevel Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *
 *  \return Current PowerLevel Actual state value.
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshGenericPowerLevelActualGet (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T
    *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelActualSet
 */
/*! \brief The API sets the Power Actual state to new state value as specificed in the parameters.
 *
 *  \param [in] model_context Pointer to the Generic PowerLevel Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *  \param [in] transition_time transition time
 *  \param [in] delay delay
 *  \param [in] pwr_actual New PowerLevel Acutal state value to set
 *
 *  \return None.
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelActualSet (QMESH_GENERIC_POWERLEVEL_CONTEXT_T
        *model_context,
        uint8_t transition_time,
        uint8_t delay,
        uint16_t pwr_actual);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelDefaultGet
 */
/*! \brief The API returns the Power Level Default value
 *
 *  \param [in] model_context Pointer to the Generic PowerLevel Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *
 *  \return uint16_t - Power Level Default value
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshGenericPowerLevelDefaultGet (QMESH_GENERIC_POWERLEVEL_CONTEXT_T
        *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelDefaultSet
 */
/*! \brief The API sets the Power Level Default value
 *
 *  \param [in] model_context Pointer to the Generic PowerLevel Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *  \param [in] pwr_default New PowerLevel Default state value to set
 *
 *  \return None.
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelDefaultSet (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context, uint16_t pwr_default);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelLastGet
 */
/*! \brief The API returns the Power Level Last value.
 *
 *  \param [in] model_context Pointer to the Generic PowerLevel Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *
 *  \return uint16_t Last power level
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshGenericPowerLevelLastGet (QMESH_GENERIC_POWERLEVEL_CONTEXT_T
                                      *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelLastSet
 */
/*! \brief The API sets the Power Level Last value.
 *
 *  \param model_context Pointer to the Generic PowerLevel Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *
 *  \param [in] pwr_last New PowerLevel Last state value to set
 *
 *  \return None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelLastSet (QMESH_GENERIC_POWERLEVEL_CONTEXT_T
                                      *model_context, uint16_t  pwr_last);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelMinRangeGet
 */
/*! \brief The API returns the Power Level Range Minimum state value.
 *
 *  \param [in] model_context
 *
 *  \return uint16_t    Power level minimum range
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshGenericPowerLevelMinRangeGet (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelMaxRangeGet
 */
/*! \brief The API returns the Power Level Range Maximum state value.
 *
 *  \param [in] model_context
 *
 *  \return uint16_t    Power level maximum range
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshGenericPowerLevelMaxRangeGet (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context);

/*----------------------------------------------------------------------------*
 *      QmeshGenericPowerLevelContextReset
 */
/*! \brief The API resets the state context values related to the current transition
 *
 *  \param [in] model_context
 *
 *  \return  Nothing
 *
 *----------------------------------------------------------------------------*/
void QmeshGenericPowerLevelContextReset (QMESH_GENERIC_POWERLEVEL_CONTEXT_T
                                      *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelAbortTransition
 */
/*! \brief The API aborts the current running transition
 *
 *  \param [in] model_context Pointer to the Generic PowerLevel Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *
 *  \return Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelAbortTransition (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context);

/*----------------------------------------------------------------------------*
 *  QmeshGenericPowerLevelUpdatePublicationPeriod
 */
/*! \brief  Publication configuration handler for PowerLevel Model Server.
 *
 *  \param [in] model_data Pointer to the Generic PowerLevel model Data
 *                       \ref QMESH_MODEL_DATA_T
 *
 *  \return TRUE/FALSE
 *
 */
/*----------------------------------------------------------------------------*/
extern bool QmeshGenericPowerLevelUpdatePublicationPeriod (
    QMESH_MODEL_DATA_T *model_data);

/*----------------------------------------------------------------------------*
 *      QmeshGenericPowerLevelPublicationHandler
 */
/*! \brief  Publication configuration handler for Generic PowerLevel model
 *
 *  \param [in] model_data  Pointer to the Generic PowerLevel model Data \ref QMESH_MODEL_DATA_T
 *
 *  \return None
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelPublicationHandler (QMESH_MODEL_DATA_T
        *model_data);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelStatusSend
 */
/*! \brief This function sends the current PowerLevel Status message to the remote device as mentioned
 *  in 'msg_cmn' parameter.
 *
 *  \param [in] model_context Pointer to the Generic PowerLevel Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *  \param [in] msg_cmn Pointer to \ref QMESH_MODEL_MSG_COMMON_T
 *
 *  \return Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelStatusSend (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context,
    QMESH_MODEL_MSG_COMMON_T *msg_cmn);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelStatusPublish
 */
/*! \brief This function sends the current PowerLevel Status message to the Publish address assigned for
 *  this model.
 *
 *  \param [in] model_context Pointer to the Generic PowerLevel Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *
 *  \return Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelStatusPublish (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelServerHandler
 */
/*! \brief This method is the Application model callback.
 *              This API gets called from the model library whenever mesh stack sends the model message.
 *              This API will also be called from other extended models which has bound state with this model.
 *
 *  \param [in] nw_hdr Network header \ref QMESH_NW_HEADER_INFO_T
 *  \param [in] key_info Pointer to Access layer key \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T
 *  \param [in] elem_data Pointer to element data \ref QMESH_ELEMENT_CONFIG_T
 *  \param [in] model_data Pointer to Model context \ref QMESH_MODEL_DATA_T
 *  \param [in] msg  Pointer to Message Data
 *  \param [in] msg_len Message Data Length
 *
 *  \return QMESH_RESULT_T.
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericPowerLevelServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelServerAppInit
 */
/*! \brief Initialise Generic PowerLevel server  model and registers callback
 *  with the mesh stack.
 *
 *  \param [in] model_context Pointer to the Generic PowerLevel Data \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *
 *  \return QMESH_RESULT_T.
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericPowerLevelServerAppInit (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelServerAppDeInit
 */
/*! \brief This function De-initializes Generic PowerLevel model state data and Unregisters the model from
 *  the stack.
 *
 *  \param [in] model_context Pointer to the Generic Level Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *
 *  \return Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelServerAppDeInit (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T  *model_context);
/*!@} */

#endif
