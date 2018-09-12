/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/


#ifndef __GENERIC_POWER_LEVEL_CLIENT_H__
#define __GENERIC_POWER_LEVEL_CLIENT_H__

#include "model_client_common.h"
#include "qmesh_types.h"

/*! \file generic_power_level_client.h
 * \brief Defines the structures and APIs used by the Generic_power_level_client
 *        model.
 *
 * \details The Generic Power Level Client model extends the Generic Power OnOff
	Client model.This model supports subscribing and publishing. This model may
	be used to represent an element that can control an element of a peer device
	that exposes a Generic Power Level Server model via Generic Power Level
	messages .
 */

/** \addtogroup Model_Generic_Power_Level_Client
 * @{
*/

/*! \brief Generic power level client set message. */
typedef struct
{
    int16_t power;        /*!< The target value of the Generic Power Actual state */
    uint8_t tid;          /*!< Transaction Identifier */
    uint8_t transitionTime; /*!< The value of the Generic Default Transition Time state. */
    uint8_t delay;        /*!< Message execution delay in 5 milliseconds steps */
    bool validTransitionTime; /* Transition Time validity flag */
} GENERIC_POWER_LEVEL_CLIENT_SET_T;

/*! \brief Generic power level client default set message. */
typedef struct
{
    int16_t power;        /*!< The value of the Generic Power Last state. */
} GENERIC_POWER_LEVEL_CLIENT_DEFAULT_SET_T;

/*! \brief Generic power level client range set message. */
typedef struct
{
    uint16_t rangeMin;    /*!< The value of the Generic Power Range Min field of the Generic Power Range state. */
    uint16_t rangeMax;    /*!< The value of the Generic Power Range Max field of the Generic Power Range state. */
} GENERIC_POWER_LEVEL_CLIENT_RANGE_SET_T;


/*----------------------------------------------------------------------------*
 *   GenericPowerLevelClientGet
 */
/*! \brief Generic Power Level Get message is a reliable message used to get the
 *        Generic Power Actual state of an element. The response to the Generic
 *        Power Level Get message is a Generic Power Level Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   GenericPowerLevelClientSet
 */
/*! \brief Generic Power Level Set is a reliable message used to set the Generic
 *        Power Actual state of an element.The response to the Generic Power
 *        Level Set message is a Generic Power Level Status message.
 *
 * \param [in] header  QMESH_MSG_HEADER_T
 * \param [in] data    GENERIC_POWER_LEVEL_CLIENT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientSet(QMESH_MSG_HEADER_T *header, GENERIC_POWER_LEVEL_CLIENT_SET_T *data);

/*----------------------------------------------------------------------------*
 *   GenericPowerLevelClientLastGet
 */
/*! \brief Generic Power Last Get is a reliable message used to get the Generic
 *        Power Last state of an element.The response to a Generic Power Last
 *        Get message is a Generic Power Last Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientLastGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   GenericPowerLevelClientDefaultGet
 */
/*! \brief Generic Power Default Get is a reliable message used to get the
 *        Generic Power Default state of an element.The response to a Generic
 *        Power Default Get message is a Generic Power Default Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientDefaultGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   GenericPowerLevelClientDefaultSet
 */
/*! \brief Generic Power Default Set is a reliable message used to set the
 *        Generic Power Default state of an element.The response to the Generic
 *        Power Default Set message is a Generic Power Default Status message.
 *
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to data \ref GENERIC_POWER_LEVEL_CLIENT_DEFAULT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientDefaultSet(QMESH_MSG_HEADER_T *header, GENERIC_POWER_LEVEL_CLIENT_DEFAULT_SET_T *data);

/*----------------------------------------------------------------------------*
 *   GenericPowerLevelClientRangeGet
 */
/*! \brief Generic Power Range Get is a reliable message used to get the Generic
 *        Power Range state of an element.The response to the Generic Power Range
 *        Get message is a Generic Power Range Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientRangeGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   GenericPowerLevelClientRangeSet
 */
/*! \brief Generic Power Range Set is a reliable message used to set the Generic
 *        Power Range state of an element.The response to the Generic Power Range
 *        Set message is a Generic Power Range Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to data \ref GENERIC_POWER_LEVEL_CLIENT_RANGE_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientRangeSet(QMESH_MSG_HEADER_T *header, GENERIC_POWER_LEVEL_CLIENT_RANGE_SET_T *data);



/*!@} */
#endif /* __GENERIC_POWER_LEVEL_CLIENT_H__ */

