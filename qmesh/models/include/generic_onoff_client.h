/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/

#ifndef __GENERIC_ONOFF_CLIENT_H__
#define __GENERIC_ONOFF_CLIENT_H__

#include "model_client_common.h"
#include "qmesh_types.h"

/*! \file generic_onoff_client.h
 * \brief Defines the structures and APIs used by the Generic_onoff_client
 *        model.
 *
 * \details The Generic OnOff Client model is a root model (it does not extend
	any other models).This model supports subscribing and publishing.This model
	may be used to represent a variety of elements that do not fit any of the
	model descriptions defined but can consume and control the generic properties
	of On/Off. The model may operate on states defined by the Generic Power OnOff
	Server model via Generic OnOff Messages.
 */

/** \addtogroup Model_Generic_OnOff_Client
 * @{
*/

/*! \brief Generic on/off client set message. */
typedef struct
{
    uint8_t onOff;        /*!< The target value of the Generic OnOff state */
    uint8_t tid;          /*!< Transaction Identifier */
    uint8_t transitionTime; /*!< Determines how long an element shall take to transition from a present state to a new state */
    uint8_t delay;        /*!< Message execution delay in 5 millisecond steps */
    bool validTransitionTime; /*!< Transition Time validity flag */
} GENERIC_ONOFF_CLIENT_SET_T;


/*----------------------------------------------------------------------------*
 *   GenericOnoffClientGet
 */
/*! \brief Generic OnOff Get is a reliable message used to get the Generic OnOff
 *        state of an element.The response to the Generic OnOff Get message is a
 *        Generic OnOff Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericOnoffClientGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   GenericOnoffClientSet
 */
/*! \brief Generic OnOff Set is a reliable message used to set the Generic OnOff
 *        state of an element.The response to the Generic OnOff Set message is a
 *        Generic OnOff Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to message data \ref GENERIC_ONOFF_CLIENT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericOnoffClientSet(QMESH_MSG_HEADER_T *header, GENERIC_ONOFF_CLIENT_SET_T *data);



/*!@} */
#endif /* __GENERIC_ONOFF_CLIENT_H__ */

