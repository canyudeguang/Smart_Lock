/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/*! \file qmesh_generic_default_transition_time_handler.h
*  \brief Generic Default Transition Time Server definitions and defines.
*
*   This file contains Generic Default Transition Time Server definitions and
*   defines.
*/
/****************************************************************************/

#ifndef __QMESH_GENERIC_DEFAULT_TRANSITION_TIME_HANDLER_H__
#define __QMESH_GENERIC_DEFAULT_TRANSITION_TIME_HANDLER_H__

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "qmesh_model_common.h"
#include "qmesh_cache_mgmt.h"

/*!\brief Defines the structures used by the Generic Default Transition Time
 *        Server model.
 *
 * \details Generic Default Transition Time Server
 */

/** \addtogroup Model_Generic_Default_Transition_Time_Server
 * @{
*/

/*============================================================================*
 *  Public Definitions
 *============================================================================*/
/*!\brief Generic Default Transition Time Server Set message length */
#define GENERIC_DTT_SET_MSG_LEN           (1)

/*!\brief Generic Default Transition Time Server Get message length */
#define GENERIC_DTT_GET_MSG_LEN           (0)

/*!\brief Generic Default Transition Time Server Status message length */
#define GENERIC_DTT_STATUS_MSG_LEN        (3)

/*!\brief Offsets of transition time in Generic Default Transition Time Server
 * Set Message
 */
#define OFFSET_DEFAULT_TRANSITION         (0)

/*============================================================================*
 *  Public Data Types
 *============================================================================*/

/*!\brief Generic Default Transition Time Server State information */
typedef struct
{
    uint16_t elm_addr;                                  /*!< Element address */
    uint8_t transition_time;                            /*!< Transition Time in format LSB 0-5: Num of Steps, 6-7: Step Resolution */
    uint8_t dtt_status[GENERIC_DTT_STATUS_MSG_LEN];     /*!< DTT Status */
    uint32_t    last_msg_seq_no;                        /*!< Stores the sequence number of the last processed model message */
    uint16_t    last_src_addr;                          /*!< Stores the element address of the last processed model message */

    /* Model Publication related details */
    QMESH_TIMER_HANDLE_T    publish_timer;              /*!< Timer to send status messages to Publish addresses periodically */
    QMESH_TIMER_HANDLE_T    publish_retrans_timer;      /*!< Timer to restransmit status messages to Publish addresses */
    uint8_t    publish_restrans_cnt;                    /*!< Publication retransmission count */
    uint32_t   publish_interval;                        /*!< Publication interval */
    QMESH_MODEL_PUBLISH_STATE_T    *publish_state;      /*!< Pointer to the Model Publication State */
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T key_info;           /*!< Key information */
    QMESH_MUTEX_T dtt_mutex;                            /*!< DTT Mutex */
    uint8_t status[GENERIC_DTT_STATUS_MSG_LEN];         /*!< DTT Status*/
} QMESH_GENERIC_DTT_CONTEXT_T;


/*!\brief Generic Default Transition Time Server NVM state */
typedef struct __attribute__ ((__packed__))
{
    uint8_t     transition_time;/*!\brief Transition Time in format
                                           LSB 0-5: Num of Steps, 6-7: Step Resolution */
    uint32_t    last_msg_seq_no; /*!\brief Stores the sequence number of the
                                           last processed model message */
    uint16_t    last_src_addr;   /*!\brief Stores the element address of the
                                         last processed model message */
} QMESH_MODEL_GEN_DTT_NVM_STATE_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/*----------------------------------------------------------------------------*
 * QmeshGenericDTTServerAppInit
 */
/*! \brief This function initialises Generic Default Transition Time Server
 *         state data.
 *
 *  \param [in] model_context Pointer to the Generic Level Data \ref QMESH_GENERIC_DTT_CONTEXT_T
 *
 *  \return QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericDTTServerAppInit (QMESH_GENERIC_DTT_CONTEXT_T
        *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericDTTServerAppDeInit
 */
/*! \brief This function deinitialises Generic Default Transition Time Server
 *         state data.
 *
 *  \param [in] model_context Pointer to the Generic Level Data \ref QMESH_GENERIC_DTT_CONTEXT_T
 *
 *  \return Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void QmeshGenericDTTServerAppDeInit (QMESH_GENERIC_DTT_CONTEXT_T
        *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericDTTServerAppHandler
 */
/*! \brief This function handles events for Generic Default Transition Time
 *         Server.
 *
 *  \param [in] nw_hdr          Network header \ref QMESH_NW_HEADER_INFO_T
 *  \param [in] key_info        Pointer to Access layer key \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T
 *  \param [in] elem_data       Pointer to element data \ref QMESH_ELEMENT_CONFIG_T
 *  \param [in] model_data      Pointer to Model context \ref QMESH_MODEL_DATA_T
 *  \param [in] msg             Pointer to Message Data
 *  \param [in] msg_len         Message Data Length
 *
 *  \return Nothing.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericDTTServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshGenericDTTGetTransitionTime
 */
/*! \brief This function returns transition time as non zero value if DTT is
 *         supported on that element
 *
 *  \param [in] model_context       Pointer to Generic Default Transition Time Server Data \ref QMESH_GENERIC_DTT_CONTEXT_T
 *
 *  \return uint8_t Returns the Transition time
 */
/*---------------------------------------------------------------------------*/
extern uint8_t QmeshGenericDTTGetTransitionTime (QMESH_GENERIC_DTT_CONTEXT_T
        *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericDTTTransitionTimeSet
 */
/*! \brief This function sets the default transition time
 *
 *  \param [in] model_context       Pointer to Generic Default Transition Time Server Data \ref QMESH_GENERIC_DTT_CONTEXT_T
 *  \param [in] transition_time     transition time
 *
 *  \return Nothing.
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericDTTTransitionTimeSet (QMESH_GENERIC_DTT_CONTEXT_T
        *model_context, uint8_t transition_time);

/*----------------------------------------------------------------------------*
 * QmeshGenericDTTStatusSend
 */
/*! \brief This function sends the DTT status message in response to reliable
 *         GET/SET message
 *
 *  \param [in] model_context       Pointer to the Generic Default Transition Time Server Data \ref QMESH_GENERIC_DTT_CONTEXT_T
 *  \param [in] msg_cmn             Message containing common parameters
 *
 *  \return Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void QmeshGenericDTTStatusSend (QMESH_GENERIC_DTT_CONTEXT_T
                                       *model_context,
                                       QMESH_MODEL_MSG_COMMON_T *msg_cmn);

/*----------------------------------------------------------------------------*
 * QmeshGenericDTTStatusPublish
 */
/*! \brief This function sends the Generic Default Transition Time Server status
 *          message to assigned publish address .
 *
 *  \param [in] model_context Pointer to the Generic Default Transition Time Server Data \ref QMESH_GENERIC_DTT_CONTEXT_T
 *
 *  \return Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void QmeshGenericDTTStatusPublish (QMESH_GENERIC_DTT_CONTEXT_T
        *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericDTTPublicationHandler
 */
/*! \brief Publication configuration handler for Generic Default Time Transition server model
 *
 *  \param [in] model_data Pointer to the Model Data \ref QMESH_MODEL_DATA_T
 *
 *  \return Nothing.
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericDTTPublicationHandler (QMESH_MODEL_DATA_T *model_data);

/*----------------------------------------------------------------------------*
 * QmeshGenericDTTUpdatePublicationPeriod
 */
/*! \brief Calculates the publish interval from the Publish state data for the model.
 *
 *  \param [in] model_data Pointer to the Model Data \ref QMESH_MODEL_DATA_T
 *
 *  \return TRUE/FALSE.
 */
/*----------------------------------------------------------------------------*/
extern bool QmeshGenericDTTUpdatePublicationPeriod (QMESH_MODEL_DATA_T
        *model_data);
/*!@} */
#endif /* __QMESH_GENERIC_DEFAULT_TRANSITION_TIME_HANDLER_H__*/
