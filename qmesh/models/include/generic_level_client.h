/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/

#ifndef __GENERIC_LEVEL_CLIENT_H__
#define __GENERIC_LEVEL_CLIENT_H__

#include "model_client_common.h"
#include "qmesh_types.h"

/*! \file generic_level_client.h
 * \brief Defines the structures and APIs used by the Generic_level_client
 *        model.
 *
 * \details The Generic Level Client model is a root model (it does not extend
	any other models).This model supports subscribing and publishing.This model
	may be used to represent a variety of devices that do not fit any of the
	defined not fit any of the defined Model descriptions but can consume and
	control the generic properties of Level. The model operates on states
	defined by the Generic Level Server  model via Generic Level messages.
 */

/** \addtogroup Model_Generic_Level_Client
 * @{
*/

/*! \brief Generic level client set message. */
typedef struct
{
    int16_t level;        /*!< The target value of the Generic Level state */
    uint8_t tid;          /*!<  Transaction Identifier */
    uint8_t transitionTime; /*!< Determines how long an element shall take to transition */
    uint8_t delay;        /*!< Message execution delay in 5 milliseconds steps from a present state to a new state */
    bool validTransitionTime; /*!< Transition Time validity flag */
} GENERIC_LEVEL_CLIENT_SET_T;

/*! \brief Generic level client delta set message. */
typedef struct
{
    int32_t deltaLevel;       /*!< The Delta change of the Generic Level state */
    uint8_t tid;              /*!< Transaction Identifier */
    uint8_t transitionTime;   /*!< Determines how long an element shall take to transition from a present state to a new state */
    uint8_t delay;            /*!< Message execution delay in 5 milliseconds steps */
    bool validTransitionTime; /*!< Transition Time validity flag */
} GENERIC_LEVEL_CLIENT_DELTA_SET_T;

/*! \brief Generic level client move set message. */
typedef struct
{
    int16_t deltaLevel;       /*!< The Delta Level step to calculate Move speed for the Generic Level state. */
    uint8_t tid;              /*!< Transaction Identifier */
    uint8_t transitionTime;  /*!< Determines how long an element shall take to transition from a present state to a new state */
    uint8_t delay;            /*!< Message execution delay in 5 milliseconds steps */
    bool validTransitionTime; /*!< Transition Time validity flag */
} GENERIC_LEVEL_CLIENT_MOVE_SET_T;


/*----------------------------------------------------------------------------*
 *   GenericLevelClientGet
 */
/*! \brief Generic Level Get is a reliable message used to set the Generic Level
 *        state of an element to a new absolute value.The response to the Generic
 *        Level Set message is a Generic Level Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericLevelClientGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   GenericLevelClientSet
 */
/*! \brief Generic Level Set is a reliable message used to set the Generic Level
 *        state of an element to a new absolute value.The response to the Generic
 *        Level Set message is a Generic Level Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to message data \ref GENERIC_LEVEL_CLIENT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericLevelClientSet(QMESH_MSG_HEADER_T *header, GENERIC_LEVEL_CLIENT_SET_T *data);

/*----------------------------------------------------------------------------*
 *   GenericLevelClientDeltaSet
 */
/*! \brief Generic Delta Set is a reliable message used to set the Generic Level
 *        state of an element by a relative value.The message is transactional
 *        – it supports changing the state by a cumulative value with a sequence
 *        of messages that are partof a transaction.The response to the Generic
 *        Delta Set message is a Generic Level Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to message data \ref GENERIC_LEVEL_CLIENT_DELTA_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericLevelClientDeltaSet(QMESH_MSG_HEADER_T *header, GENERIC_LEVEL_CLIENT_DELTA_SET_T *data);

/*----------------------------------------------------------------------------*
 *   GenericLevelClientMoveSet
 */
/*! \brief Generic Move Set is a reliable message used to start a process of
 *        changing the Generic Level state of an element with a defined transition
 *        speed The response to the Generic Move Set message is a Generic Level
 *        Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to message data \ref GENERIC_LEVEL_CLIENT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericLevelClientMoveSet(QMESH_MSG_HEADER_T *header, GENERIC_LEVEL_CLIENT_MOVE_SET_T *data);



/*!@} */
#endif /* __GENERIC_LEVEL_CLIENT_H__ */

