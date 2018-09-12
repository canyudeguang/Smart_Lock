/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/*! \file qmesh_cache_mgmt.h
 *  \brief Mesh model cache data structures and APIs exposed.
 *   This file contains the implementation on 6 seconds window cache management. As per Model
 *   specification, any duplicate messages received within last 6 seconds should be dropped.
 */
/******************************************************************************/
#ifndef __QMESH_CACHE_MGMT_H__
#define __QMESH_CACHE_MGMT_H__

#include "qmesh_model_common.h"


/** \addtogroup Model_Cache_Management
* @{
*/

/* Number of model messages to store per element */
#define NUM_OF_CACHE_MSG_PER_ELEMENT        (10)

/* Maximum number of model messages stored in the cache */
#define QMESH_MODEL_MSG_CACHE_SIZE   \
                  (QMESH_NUMBER_OF_ELEMENTS * NUM_OF_CACHE_MSG_PER_ELEMENT)

/* Maximum number of delay cache entries */
#define NUM_DELAY_CACHE_ENTRIES 10u
#define MODEL_CACHE_ENTRY_SIZE  16u

/* Invalid Index */
#define INVALID_INDEX (0xFF)

/*! \brief Structure representing an entry in the 6 second cache list.*/
typedef struct
{
    uint16_t    ts[3];                  /*!< Time stamp when the message is added into the cache */
    QMESH_MODEL_MSG_COMMON_T    msg;    /*!< Model Message */
}QMESH_MODEL_CACHE_NODE_T;


/*! \brief Model message structure for delay cache.*/
typedef struct
{
    QMESH_MODEL_MSG_COMMON_T  cmn_msg;  /*!< Common message */
    uint8_t transition_time;            /*!< Transition time */
    uint8_t delay;                      /*!< Delay */
    uint16_t timestamp[3];              /*!< Timestamp at which message is received */
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T  key_info;  /*!< Key Info */
    uint8_t *msg;                         /*!< Opcode specific message */
    uint16_t  msg_len;                  /*!< Opcode specific message length */
} QMESH_MODEL_MSG_T;

/*! \brief Delay cache Information.*/
typedef struct
{
    QMESH_MODEL_MSG_T data;             /*!< Model message data */
    void (*fn_callback)(QMESH_MODEL_MSG_T *model_msg, void *model_context);      /*!< Callback function */
    void *model_context;                /*!<  Model Context */
    QMESH_TIMER_HANDLE_T timer_id;      /*!< Timer ID */
    uint8_t index;                      /*!< Index at which message is stored */
} QMESH_DELAY_CACHE_INFO_T;


#ifdef QMESH_CACHE_DEBUG
/*! \brief This function initialises the handle for debug prints in duplicate cache.*/
void  QmeshCacheDebugHandleSet (DEBUGFUNC func, void * handle);
#endif /* QMESH_CACHE_DEBUG */

/*! \brief Model Callback function .*/
typedef void (*modelCallbackFunction)(QMESH_MODEL_MSG_T *model_msg, void *context);

/*----------------------------------------------------------------------------*
 *  QmeshModelInitMsgCache
 */
/*! \brief Initialises the model message cache
 *
 * \return None
 *
 *----------------------------------------------------------------------------*/
void QmeshModelInitMsgCache(void);

/*----------------------------------------------------------------------------*
 * QmeshModelCacheAddMsg
 */
/*! \brief The function adds the 'msg' into the cache if the 'msg' is not found in the 6 seconds timestamp
 *        window. If the 'msg' is not found, the 'msg' will be added/overwriten in the next slot. The search is
 *        limited to messages with timestamp not greater than 6 seconds old. This reduces the search time.
 *        If 'transactional' is TRUE, the message will be added into the cache in the next slot even if a message
 *        exists with same TID in the last 6 seconds window. This is required for 'Transactional' messages like
 *        'Generic Delta Set' which will have same TID but 'level' param will have delta change. Such message
 *        need to be processed always.
 *
 * \param [in] msg                  Pointer to message \ref QMESH_MODEL_MSG_COMMON_T
 * \param [in] transactional        specifies true for transactional messages else false
 *
 * \return  returns TRUE if message is adde into the cache FALSE if message is already present or othe failure
 *
 *----------------------------------------------------------------------------*/
bool QmeshModelCacheAddMsg(QMESH_MODEL_MSG_COMMON_T *msg, bool transactional);

/*----------------------------------------------------------------------------*
 *  QmeshInitDelayCache
 */
/*! \brief This function initialises the delay cache and free_index array indexes
 *
 * \return None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshInitDelayCache(void);

/*----------------------------------------------------------------------------*
 *  QmeshAddMsgToDelayCache
 */
/*! \brief This function adds a new message to delay cache
 *
 * \param [in] model_msg            Pointer to message \ref QMESH_MODEL_MSG_T
 * \param [in] callbackFunction     Model call back function
 * \param [in] context              Model context information
 * \param [in] msg                  Pointer to message buffer
 * \param [in] msg_len              Message buffer length
 *
 * \return \ref QMESH_RESULT_T
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshAddMsgToDelayCache(QMESH_MODEL_MSG_T *model_msg ,
                                           modelCallbackFunction callbackFunction,
                                           void *context,
                                           const uint8_t *msg,
                                           uint16_t msg_len);

/*----------------------------------------------------------------------------*
 *  QmeshDeInitDelayCache
 */
/*! \brief This function de-initialises the delay cache and free_index array indexes
 *
 * \return None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshDeInitDelayCache(void);

/*----------------------------------------------------------------------------*
 *  QmeshModelCacheDump
 */
/*! \brief Prints all the model messages in the cache  from latest to oldest message.
 *
 * \return None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshModelCacheDump(void);

/*!@} */

#endif
