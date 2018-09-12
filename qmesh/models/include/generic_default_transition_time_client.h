/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/

#ifndef __GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_H__
#define __GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_H__

#include "model_client_common.h"
#include "qmesh_types.h"

/*! \file generic_default_transition_time_client.h
 * \brief Defines the structures and APIs used by the Generic_default_transition_time_client
 *        model.
 *
 * \details The Generic Default Transition Time Client model is a root model
	(it does not extend any other models).This model supports subscribing and
	publishing.The model may operate on states defined by the Generic Default
	Transition Time Server model via Generic Default Transition Time messages.
 */

/** \addtogroup Model_Generic_Default_Transition_Time_Client
* @{
*/

/*! \brief Generic Default transition time client. */
typedef struct
{
    uint8_t transitionTime;               /*!< The value of the Generic Default Transition Time state. */
} GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_SET_T;


/*----------------------------------------------------------------------------*
 *   GenericDefaultTransitionTimeClientGet
 */
/*! \brief Generic Default Transition Time Get is a reliable message used to get the
 *        Generic Default Transition Time state of an element.The response to the
 *        Generic Default Transition Time Get message is a Generic Default Transition
 *        Time Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \return \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericDefaultTransitionTimeClientGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   GenericDefaultTransitionTimeClientSet
 */
/*! \brief Generic Default Transition Time Set is a reliable message used to set the
 *        Generic Default Transition Time state of an element.The response to the Generic
 *        Default Transition Time Set message is a Generic Default Transition Time
 *        Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Default transition data to be set. \ref GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_SET_T
 *
 * \return \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericDefaultTransitionTimeClientSet(QMESH_MSG_HEADER_T *header, GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_SET_T *data);

/*!@} */
#endif /* __GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_H__ */

