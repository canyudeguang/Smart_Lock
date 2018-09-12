/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/

#ifndef __GENERIC_POWER_ONOFF_CLIENT_H__
#define __GENERIC_POWER_ONOFF_CLIENT_H__

#include "model_client_common.h"
#include "qmesh_types.h"

/*! \file generic_power_onoff_client.h
 * \brief Defines the structures and APIs used by the Generic_power_onoff_client
 *        model.
 *
 * \details The Generic Power OnOff Client model extends the Generic OnOff
	Client model.This model supports subscribing and publishing.The model may
	operate on states defined by the Generic Power OnOff Server model via Generic
	Default Transition Time messages and Generic OnPowerUp messages
 */

/** \addtogroup Model_Generic_Power_OnOff_Client
 * @{
*/

/*! \brief Generic power on/off client on power up set message. */
typedef struct
{
    uint8_t onPowerUp; /*!< The value of the Generic OnPowerUp state. */
} GENERIC_POWER_ONOFF_CLIENT_ONPOWERUP_SET_T;


/*----------------------------------------------------------------------------*
 *   GenericPowerOnoffClientOnpowerupGet
 */
/*! \brief Generic Power OnOff Client's OnPowerUp Get is a reliable message used to get the Generic
 *        OnPowerUp state of an element.The response to the Generic OnPowerUp
 *        Get message is a Generic OnPowerUp Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerOnoffClientOnpowerupGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   GenericPowerOnoffClientOnpowerupSet
 */
/*! \brief Generic Power OnOff Client's OnPowerUp Set is a reliable message used to set the Generic
 *        OnPowerUp state of an element.The response to the Generic OnPowerUp
 *        Set message is a Generic OnPowerUp Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to  GENERIC_POWER_ONOFF_CLIENT_ONPOWERUP_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerOnoffClientOnpowerupSet(QMESH_MSG_HEADER_T *header, GENERIC_POWER_ONOFF_CLIENT_ONPOWERUP_SET_T *data);

/*!@} */
#endif /* __GENERIC_POWER_ONOFF_CLIENT_H__ */

