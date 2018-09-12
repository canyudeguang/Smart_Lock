/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/*! \file qmesh_generic_level_handler.h
*  \brief Generic Level Server definitions and defines.
*
*   This file contains Generic Level Server definitions and defines.
*/
/****************************************************************************/

#ifndef __QMESH_GENERIC_LEVEL_HANDLER_H__
#define __QMESH_GENERIC_LEVEL_HANDLER_H__

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "qmesh_model_common.h"
#include "qmesh_cache_mgmt.h"

/** \addtogroup Model_Generic_Level_Server
 * @{
*/

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/*!\brief Level and Delta Level size */
#define LEVEL_SIZE                            (2)
#define DELTA_LEVEL_SIZE                      (4)

/*!\brief Generic Level state, TID, Transition and Delay offsets for Level Set
 * and Move Set messages
 */
#define GENERIC_LEVEL_VALUE_OFFSET            (0)
#define GENERIC_LEVEL_TID_OFFSET              (GENERIC_LEVEL_VALUE_OFFSET + \
                                               LEVEL_SIZE)
#define GENERIC_LEVEL_TRANS_TIME_OFFSET       (GENERIC_LEVEL_TID_OFFSET + 1)
#define GENERIC_LEVEL_DELAY_OFFSET            (GENERIC_LEVEL_TRANS_TIME_OFFSET +\
                                               1)

/*!\brief Generic Level state, TID, Transition and Delay offsets for Delta Set messages */
#define GENERIC_LEVEL_DELTA_TID_OFFSET        (GENERIC_LEVEL_VALUE_OFFSET + \
                                               DELTA_LEVEL_SIZE)
#define GENERIC_LEVEL_DELTA_TRANS_TIME_OFFSET (GENERIC_LEVEL_DELTA_TID_OFFSET +  1)
#define GENERIC_LEVEL_DELTA_DELAY_OFFSET      (GENERIC_LEVEL_DELTA_TRANS_TIME_OFFSET + 1)

/*!\brief Minimum Length for Level Set messages */
#define GENERIC_LEVEL_NON_TRANS_MSG_LEN                   (3)

/*!\brief Maximum Length for Level Set messages */
#define GENERIC_LEVEL_TRANS_MSG_LEN                       (5)

/*!\brief Minimum Length for Delta Level Set messages */
#define GENERIC_LEVEL_DELTA_NON_TRANS_MSG_LEN             (5)

/*!\brief Maximum Length for Delta Level Set messages */
#define GENERIC_LEVEL_DELTA_TRANS_MSG_LEN                 (7)

/*!\brief Length for Level Get messages */
#define GENERIC_LEVEL_GET_MSG_LEN                         (0)

/*!\brief Minimum Length for Level Status messages */
#define GENERIC_LEVEL_STATUS_NON_TRANS_MSG_LEN            (4)

/*!\brief Maximum Length for Level Status messages */
#define GENERIC_LEVEL_STATUS_TRANS_MSG_LEN                (7)

/*!\brief Transition Time offset in Level Status messages */
#define GENERIC_LEVEL_STATUS_TRANS_TIME_OFFSET            (6)

/*!\brief Two bytes Level value */
#define QMESH_PACK_TWO_BYTE_LEVEL(lvl1, lvl2)   (((int16_t)lvl2 << 8) | (lvl1))

/*!\brief  Four bytes Delta Level value */
#define QMESH_PACK_FOUR_BYTE_DELTA_LEVEL(lvl1, lvl2, lvl3, lvl4) \
                                    (((int32_t)lvl4 << 24) | \
                                    ((int32_t)lvl3 << 16) | \
                                    ((int32_t)lvl2 << 8) | \
                                    (lvl1))

#define GENERIC_LEVEL_GET_TRANS_TIME(opcode, msg, msg_len, elm_idx)  \
((opcode == QMESH_GENERIC_DELTA_SET || opcode == QMESH_GENERIC_DELTA_SET_UNRELIABLE) ? \
((msg_len == GENERIC_LEVEL_DELTA_TRANS_MSG_LEN) ? \
msg[GENERIC_LEVEL_DELTA_TRANS_TIME_OFFSET] : QmeshGetGenericDTT(elm_idx)) : \
(((msg_len == GENERIC_LEVEL_TRANS_MSG_LEN) ? \
msg[GENERIC_LEVEL_TRANS_TIME_OFFSET] : QmeshGetGenericDTT(elm_idx))))

#define GENERIC_LEVEL_GET_DELAY(opcode, msg, msg_len)  \
((opcode == QMESH_GENERIC_DELTA_SET || opcode == QMESH_GENERIC_DELTA_SET_UNRELIABLE) ? \
((msg_len == GENERIC_LEVEL_DELTA_TRANS_MSG_LEN) ? \
msg[GENERIC_LEVEL_DELTA_DELAY_OFFSET] : 0) : \
((msg_len == GENERIC_LEVEL_TRANS_MSG_LEN) ? \
msg[GENERIC_LEVEL_DELAY_OFFSET] : 0))

#define GENERIC_LEVEL_GET_LEVEL_SIZE(opcode) \
((opcode == QMESH_GENERIC_DELTA_SET || opcode == QMESH_GENERIC_DELTA_SET_UNRELIABLE) ? \
DELTA_LEVEL_SIZE : LEVEL_SIZE)

/*============================================================================*
 *  Public Data Types
 *============================================================================*/

/*!\brief Transition data type.This structure contains all variables used during transition time */
typedef struct
{
    uint8_t rem_count;              /*!< Remaining number of times the current value need
                                          to be incremented/decremented by 'linearValue'*/
    int32_t linear_value;           /*!< Value by which the current 'state' value need to be incremented/decremented linearly */
    uint32_t step_resolution;       /*!< Periodic timer value */
    bool adjust_step_resolution;    /*!< Check remaining transition time or linear value to adjust the current value at the end of transition */
    uint8_t transition_time;        /*!< Transition Time with format LSB 0-5: Num of Steps, 6-7: Step Resolution */
    QMESH_TIMER_HANDLE_T trans_timer;/*!< Current Linear transition timer handle.This timer is used to send linear values as status during transition */
    uint32_t trans_time_ms;          /*!< Transition time in milliseconds */
} QMESH_GENERIC_LEVEL_TRANSITION_DATA_T;

/*!\brief  Generic Delta Level data type */
typedef struct
{
    int16_t initial_level;          /*!< Initial Level state for Delta Set Messages */
    uint8_t tid;                    /*!< TID received as part of last Delta Set Message */
    uint16_t src_addr;              /*!< Source Address received as part of last Delta Set Message */
    uint16_t dst_addr;              /*!< Destination Address received as part of last Delta Set Message */
    uint16_t timestamp[3];          /*!< Timestamp received as part of last Delta Set Message */
} QMESH_GENERIC_DELTA_LEVEL_DATA_T;

/* Generic Level state information */
typedef struct
{
    uint16_t elm_addr;              /*!< Element Index */
    int16_t cur_level;              /*!< Current level state */
    int16_t target_level;           /*!< Target Level state */
    uint8_t delay;                  /*!< Delay with 5 milliseconds resolution */
    uint8_t status[GENERIC_LEVEL_STATUS_TRANS_MSG_LEN];     /*!< Level status message */
    uint32_t    last_msg_seq_no;    /*!< Stores the sequence number of the last processed model message */
    uint16_t    last_src_addr;      /*!< Stores the element address of the last processed model message */

    /* Model Publication related details */
    QMESH_TIMER_HANDLE_T publish_timer;         /*!< Timer to send status messages to Publish addresses periodically */
    QMESH_TIMER_HANDLE_T publish_retrans_timer; /*!< Timer to restransmit status messages to Publish addresses */
    uint8_t    publish_restrans_cnt;            /*!< Publication retransmission count */
    uint32_t    publish_interval;               /*!< Publication interval */
    QMESH_MODEL_PUBLISH_STATE_T *publish_state; /*!< Pointer to the Model Publication State */

    QMESH_GENERIC_LEVEL_TRANSITION_DATA_T trans_data;           /*!< Transition data */
    QMESH_GENERIC_DELTA_LEVEL_DATA_T    delta_data;             /*!< Generic Delta Level data */
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T    key_info;                /*!< Key information */
    QMESH_MUTEX_T level_mutex;                                  /*!< Level Mutex */
} QMESH_GENERIC_LEVEL_CONTEXT_T;

/*!\brief Generic Level NVM state */
typedef struct __attribute__ ((__packed__))
{
    int16_t cur_level;    /*!\brief Current OnOff state */
    int16_t target_level;     /*!\brief Target OnOff state */
    uint32_t    last_msg_seq_no; /*!\brief Stores the sequence number of the
                                           last processed model message */
    uint16_t    last_src_addr;   /*!\brief Stores the element address of the
                                         last processed model message */
} QMESH_MODEL_GEN_LEVEL_NVM_STATE_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/*----------------------------------------------------------------------------*
 * QmeshGenericLevelServerAppInit
 */
/*! \brief This function initialises Generic Level server model state data .
 *
 *  \param [in] model_context Pointer to the Generic Level Data \ref QMESH_GENERIC_LEVEL_CONTEXT_T
 *
 *  \return \ref QMESH_RESULT_T
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericLevelServerAppInit (QMESH_GENERIC_LEVEL_CONTEXT_T
        *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericLevelServerAppDeInit
 */
/*! \brief This function deinitialises Generic Level server model state data.
 *
 *  \param [in] model_context Pointer to the Generic Level Data \ref QMESH_GENERIC_LEVEL_CONTEXT_T
 *
 *  \return None.
 */
/*---------------------------------------------------------------------------*/
extern void QmeshGenericLevelServerAppDeInit (QMESH_GENERIC_LEVEL_CONTEXT_T
        *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericLevelServerHandler
 */
/*! \brief This function handles the Generic Level messages from the stack.
 *
 *  \param [in] nw_hdr          Network header \ref QMESH_NW_HEADER_INFO_T
 *  \param [in] key_info        Pointer to Access layer key \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T
 *  \param [in] elem_data       Pointer to element data \ref QMESH_ELEMENT_CONFIG_T
 *  \param [in] model_data      Pointer to Model context \ref QMESH_MODEL_DATA_T
 *  \param [in] msg             Pointer to Message Data
 *  \param [in] msg_len         Message Data Length
 *
 *  \return QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericLevelServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len);

/*----------------------------------------------------------------------------*
 *      QmeshGenericLevelUpdatePublicationPeriod
 */
/*! \brief  Publication configuration handler for Level Model Server.
 *
 *  \param [in] model_data  Pointer to the Generic Level model Data \ref QMESH_MODEL_DATA_T
 *
 *  \return bool
 *
 */
/*----------------------------------------------------------------------------*/
bool QmeshGenericLevelUpdatePublicationPeriod (QMESH_MODEL_DATA_T *model_data);

/*----------------------------------------------------------------------------*
 *      QmeshGenericLevelPublicationHandler
 */
/*! \brief  Publication configuration handler for Generic Level model
 *
 *  \param [in] model_data Pointer to the Generic Level model Data \ref QMESH_MODEL_DATA_T
 *
 *  \return None
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericLevelPublicationHandler (QMESH_MODEL_DATA_T
        *model_data);

/*----------------------------------------------------------------------------*
  * QmeshGenericLevelTransitionHandler
  */
/*! \brief This function handles transition and state changes for Generic Level
 *         server.
 *
 *  \param [in] model_msg Pointer to the message stored in cache \ref QMESH_MODEL_MSG_T.
 *                     This message should be copied locally before exiting the function.
 *  \param [in] model_context Pointer to Generic Level Data \ref QMESH_GENERIC_LEVEL_CONTEXT_T
 *
 *  \return None.
 */
/*---------------------------------------------------------------------------*/
extern void QmeshGenericLevelTransitionHandler (QMESH_MODEL_MSG_T *model_msg,
        void *model_context);

/*----------------------------------------------------------------------------*
  * QmeshGenericLevelAbortTransition
  */
/*! \brief This function aborts the current running level transition and resets transition related variables
 *
 *  \param [in] model_context Pointer to Generic Level Data \ref QMESH_GENERIC_LEVEL_CONTEXT_T
 *
 *  \return None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericLevelAbortTransition (QMESH_GENERIC_LEVEL_CONTEXT_T
        *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericLevelUpdate
 */
/*! \brief This function updates the generic level value changed due to bound states
 *
 *  \param [in] model_context Pointer to Generic Level Data \ref QMESH_GENERIC_LEVEL_CONTEXT_T
 *  \param [in] new_level     New level data int16_t
 *
 *  \return None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericLevelUpdate (QMESH_GENERIC_LEVEL_CONTEXT_T
                                     *model_context,
                                     int16_t new_level);

/*----------------------------------------------------------------------------*
 * QmeshGenericLevelSet
 */
/*! \brief This function sets the generic level value to new state as mentioned in the parameters
 *
 *  \param [in] model_context       Pointer to Generic Level Data \ref QMESH_GENERIC_LEVEL_CONTEXT_T
 *  \param [in] transition_time     Transition time uint8_t
 *  \param [in] delay               delay time uint8_t
 *  \param [in] new_level           New level value int16_t
 *
 *  \return None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericLevelSet (QMESH_GENERIC_LEVEL_CONTEXT_T *model_context,
                                  uint8_t transition_time, uint8_t delay, int16_t new_level);

/*----------------------------------------------------------------------------*
 * QmeshGenericLevelGet
 */
/*! \brief This function returns Generic Level
 *
 *  \param [in] model_context Pointer to Generic Level Data \ref QMESH_GENERIC_LEVEL_CONTEXT_T
 *
 *  \return int16_t
 *
 *----------------------------------------------------------------------------*/
extern int16_t QmeshGenericLevelGet (QMESH_GENERIC_LEVEL_CONTEXT_T
                                   *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericLevelStatusSend
 */
/*! \brief This function sends the reliable level status message to client.
 *
 *  \param [in] model_context Pointer to Generic Level Data \ref QMESH_GENERIC_LEVEL_CONTEXT_T
 *  \param [in] msg_cmn Pointer to \ref QMESH_MODEL_MSG_COMMON_T
 *
 *  \return None.
 */
/*---------------------------------------------------------------------------*/
extern void QmeshGenericLevelStatusSend (QMESH_GENERIC_LEVEL_CONTEXT_T
        *model_context,
        QMESH_MODEL_MSG_COMMON_T
        *msg_cmn);

/*----------------------------------------------------------------------------*
 * QmeshGenericLevelStatusPublish
 */
/*! \brief This function sends the level status message to assigned
 *         publish address .
 *
 *  \param [in] model_context Pointer to the Generic Level Data \ref QMESH_GENERIC_LEVEL_CONTEXT_T
 *
 *  \return None.
 */
/*---------------------------------------------------------------------------*/
extern void QmeshGenericLevelStatusPublish (QMESH_GENERIC_LEVEL_CONTEXT_T
        *model_context);

/*!@} */
#endif /* __QMESH_GENERIC_LEVEL_HANDLER_H__ */
