/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/

#ifndef __LIGHT_LIGHTNESS_CLIENT_H__
#define __LIGHT_LIGHTNESS_CLIENT_H__

#include "model_client_common.h"
#include "qmesh_types.h"

/*! \file light_lightness_client.h
 * \brief Defines the structures and APIs used by the Light_lightness_client
 *        model.
 *
 * \details The Light Lightness Client model extends the Generic Power OnOff
	Client model.This model supports subscribing and publishing.This model may
	be used to represent an element that can control an element of a peer device
	that exposes a Light Lightness Server model via Light Lightness messages .
 */

/** \addtogroup Model_Light_Lightness_Client
 * @{
*/

/*! \brief Light Lightness client set message. */
typedef struct
{
    uint16_t lightness;           /*!< The target value of the Light Lightness Actual state. */
    uint8_t tid;                  /*!< Transaction Identifier */
    uint8_t transitionTime;       /*!< determines how long an element shall take to transition from a present state to a new state */
    uint8_t delay;                /*!< Message execution delay in 5 millisecond steps. */
    bool validTransitionTime;   /*!< Transition Time validity flag */
} LIGHT_LIGHTNESS_CLIENT_SET_T;

/*! \brief Light Lightness client linear set message. */
typedef struct
{
    uint16_t lightness;           /*!< The target value of the Light Lightness Linear state. */
    uint8_t tid;                  /*!< Transaction Identifier */
    uint8_t transitionTime;       /*!< determines how long an element shall take to transition from a present state to a new state */
    uint8_t delay;                /*!< Message execution delay in 5 millisecond steps. */
    bool validTransitionTime;   /*!< Transition Time validity flag */
} LIGHT_LIGHTNESS_CLIENT_LINEAR_SET_T;

/*! \brief Light Lightness client default set message. */
typedef struct
{
    uint16_t lightness;           /*!< The value of the Light Lightness Default state */
} LIGHT_LIGHTNESS_CLIENT_DEFAULT_SET_T;

/*! \brief Light Lightness client range set message. */
typedef struct
{
    uint16_t rangeMin;            /*!< The value of the Lightness Range Min field of the Light Lightness Range state */
    uint16_t rangeMax;            /*!< The value of the Lightness Range Max field of the Light Lightness Range state */
} LIGHT_LIGHTNESS_CLIENT_RANGE_SET_T;


/*----------------------------------------------------------------------------*
 *   LightLightnessClientGet
 */
/*! \brief Light Lightness Get is a reliable message used to get the Light Lightness
 *        Actual state of an element.The response to the Light Lightness Get message
 *        is a Light Lightness Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   LightLightnessClientSet
 */
/*! \brief The Light Lightness Set is a reliable message used to set the Light
 *        Lightness Actual state of an element.The response to the Light Lightness
 *        Set message is a Light Lightness Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to \ref LIGHT_LIGHTNESS_CLIENT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientSet(QMESH_MSG_HEADER_T *header, LIGHT_LIGHTNESS_CLIENT_SET_T *data);

/*----------------------------------------------------------------------------*
 *   LightLightnessClientLinearGet
 */
/*! \brief Light Lightness Linear Get is a reliable message used to get the Light
 *        Lightness Linear state of an element.The response to the Light Lightness
 *        Linear Get message is a Light Lightness Linear Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \return \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientLinearGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   LightLightnessClientLinearSet
 */
/*! \brief The Light Lightness Linear Set is a reliable message used to set the Light
 *        Lightness Linear state of an element.The response to the Light Lightness
 *        Linear Set message is a Light Lightness Linear Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to \ref LIGHT_LIGHTNESS_CLIENT_LINEAR_SET_T
 *
 * \return \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientLinearSet(QMESH_MSG_HEADER_T *header, LIGHT_LIGHTNESS_CLIENT_LINEAR_SET_T *data);

/*----------------------------------------------------------------------------*
 *   LightLightnessClientLastGet
 */
/*! \brief Light Lightness Last Get is a reliable message used to get the Light Lightness
 *        Last state of an element.The response to the Light Lightness Last Get message
 *        is a Light Lightness Last Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \return \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientLastGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   LightLightnessClientDefaultGet
 */
/*! \brief Light Lightness Default Get is a reliable message used to get the Light
 *        Lightness Default state of an element.The response to the Light Lightness
 *        Default Get message is a Light Lightness Default Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \return \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientDefaultGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   LightLightnessClientDefaultSet
 */
/*! \brief The Light Lightness Default Set is a reliable message used to set the Light
 *        Lightness Default state of an element
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data    LIGHT_LIGHTNESS_CLIENT_DEFAULT_SET_T
 *
 * \return \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientDefaultSet(QMESH_MSG_HEADER_T *header, LIGHT_LIGHTNESS_CLIENT_DEFAULT_SET_T *data);

/*----------------------------------------------------------------------------*
 *   LightLightnessClientRangeGet
 */
/*! \brief The Light Lightness Range Get is a reliable message used to get the Light
 *        Lightness Range state of an element.The response to the Light Lightness
 *        Range Get message is a Light Lightness Range Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \return \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientRangeGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   LightLightnessClientRangeSet
 */
 /*! \brief Light Lightness Range Set is a reliable message used to set the Light Lightness
 *        Range state of an element.The response to the Light Lightness Range Get message
 *        is a Light Lightness Range Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to \ref LIGHT_LIGHTNESS_CLIENT_RANGE_SET_T
 *
 * \return \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientRangeSet(QMESH_MSG_HEADER_T *header, LIGHT_LIGHTNESS_CLIENT_RANGE_SET_T *data);



/*!@} */
#endif /* __LIGHT_LIGHTNESS_CLIENT_H__ */

