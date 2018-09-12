/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/

#ifndef __LIGHT_HSL_CLIENT_H__
#define __LIGHT_HSL_CLIENT_H__

#include "model_client_common.h"
#include "qmesh_types.h"

/*! \file light_hsl_client.h
 * \brief Defines the structures and APIs used by the Light_hsl_client
 *        model.
 *
 * \details The Light HSL Client model extends the Light Lightness
	 Client model.This model supports subscribing and publishing.This model may
	be used to represent an element that can control an element of a peer device
	that exposes a Light HSL Server model via Light HSL messages .
 */

/** \addtogroup Model_Light_HSL_Client
 * @{
*/

/*! \brief Light HSL client set message. */
typedef struct
{
    uint16_t hSLLightness;                /*!< The target value of the Light HSL Lightness state. */
    uint16_t hSLHue;                      /*!< The target value of the Light HSL Hue state. */
    uint16_t hSLSaturation;               /*!< The target value of the Light HSL Saturation state. */
    uint8_t tid;                          /*!< Transaction Identifier. */
    uint8_t transitionTime;               /*!< Determines how long an element shall take to transition from a present state to a new state. */
    uint8_t delay;                        /*!< Message execution delay in 5 millisecond steps. */
    bool validTransitionTime;           /*!< Transition Time validity flag */
} LIGHT_HSL_CLIENT_SET_T;

/*! \brief Light HSL client hue set message. */
typedef struct
{
    uint16_t hue;                         /*!< The target value of the Light HSL Hue state. */
    uint8_t tid;                          /*!< Transaction Identifier */
    uint8_t transitionTime;               /*!< Identifies the time an element will take to transition to target state from a present state. */
    uint8_t delay;                        /*!< Message execution delay in 5 millisecond steps. */
    bool validTransitionTime;           /*!< Transition Time validity flag */
} LIGHT_HSL_CLIENT_HUE_SET_T;

/*! \brief Light HSL client saturation set message. */
typedef struct
{
    uint16_t saturation;                  /*!< The target value of the Light HSL Saturation state. */
    uint8_t tid;                          /*!< Transaction Identifier */
    uint8_t transitionTime;               /*!< Identifies the time an element will take to transition to target state from a present state. */
    uint8_t delay;                        /*!< Message execution delay in 5 millisecond steps. */
    bool validTransitionTime;           /*!< Transition Time validity flag */
} LIGHT_HSL_CLIENT_SATURATION_SET_T;

/*! \brief Light HSL client default set message. */
typedef struct
{
    uint16_t lightness;                   /*!< The default value of the Light HSL Lightness state. */
    uint16_t hue;                         /*!< The default value of the Light HSL Hue state. */
    uint16_t saturation;                  /*!< The default value of the Light HSL Saturation state. */
} LIGHT_HSL_CLIENT_DEFAULT_SET_T;

/*! \brief Light HSL client range set message. */
typedef struct
{
    uint16_t hueRangeMin;                 /*!< The value of the Hue Range Min field of the Light HSL Hue Range state. */
    uint16_t hueRangeMax;                 /*!< The value of the Hue Range Max field of the Light HSL Hue Range state. */
    uint16_t saturationRangeMin;          /*!< The value of the Saturation Range Min field of the Light HSL Saturation Range state. */
    uint16_t saturationRangeMax;          /*!< The value of the Saturation Range Max field of the Light HSL Saturation Range state. */
} LIGHT_HSL_CLIENT_RANGE_SET_T;


/*----------------------------------------------------------------------------*
 *   LightHslClientGet
 */
/*! \brief Light HSL Get is a reliable message used to get the Light HSL Lightness,
 *        Light HSL Hue and Light HSL Saturation states of an element. The response
 *        to the Light HSL Get message is a Light HSL Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   LightHslClientSet
 */
/*! \brief The Light HSL Set is a message used to set the Light HSL Lightness state,
 *        Light HSL Hue State and Light HSL Saturation state of an element.The response
 *        to the Light HSL Set message is a Light HSL Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointet to \ref LIGHT_HSL_CLIENT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientSet(QMESH_MSG_HEADER_T *header, LIGHT_HSL_CLIENT_SET_T *data);

/*----------------------------------------------------------------------------*
 *   LightHslClientHueGet
 */
/*! \brief Light HSL Hue Get is a reliable message used to get the Light HSL Hue state
 *        of an element.The response to the Light HSL Hue Get message is a Light HSL Hue
 *        Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientHueGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   LightHslClientHueSet
 */
/*! \brief The Light HSL Hue Set is a message used to set the Light HSL Hue state
 *        of an element.The response to the Light HSL Hue Set message is a Light
 *        HSL Hue Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to \ref LIGHT_HSL_CLIENT_HUE_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientHueSet(QMESH_MSG_HEADER_T *header, LIGHT_HSL_CLIENT_HUE_SET_T *data);

/*----------------------------------------------------------------------------*
 *   LightHslClientSaturationGet
 */
/*! \brief Light HSL Saturation Get is a reliable message used to get the Light HSL
 *        Saturation state of an element.The response to the Light HSL Saturation
 *        Get message is a Light HSL Saturation Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientSaturationGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   LightHslClientSaturationSet
 */
/*! \brief The Light HSL Saturation Set is a reliable message used to set the Light
 *        HSL Saturation state of an element.The response to the Light HSL Saturation
 *        Set message is a Light HSL Saturation Status message.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to \ref LIGHT_HSL_CLIENT_SATURATION_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientSaturationSet(QMESH_MSG_HEADER_T *header, LIGHT_HSL_CLIENT_SATURATION_SET_T *data);

/*----------------------------------------------------------------------------*
 *   LightHslClientTargetGet
 */
/*! \brief Light HSL Target Get is a reliable message used to get the the target Light HSL
 *        Lightness, Light HSL Hue and Light HSL Saturation states of an element.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientTargetGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   LightHslClientDefaultGet
 */
/*! \brief Light HSL Default Get is a reliable message used to get the Light Lightness
 *        Default, the Light HSL Hue Default and Light HSL Saturation Default
 *        states of an element.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientDefaultGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   LightHslClientDefaultSet
 */
/*! \brief The Light HSL Default Set is a reliable message used to set the Light Lightness
 *        Default, the Light HSL Hue Default and Light HSL Saturation Default states
 *        of an element.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to \ref LIGHT_HSL_CLIENT_DEFAULT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientDefaultSet(QMESH_MSG_HEADER_T *header, LIGHT_HSL_CLIENT_DEFAULT_SET_T *data);

/*----------------------------------------------------------------------------*
 *   LightHslClientRangeGet
 */
/*! \brief The Light HSL Range Get is a reliable message used to get the Light HSL Hue
 *        Range and Light HSL Saturation Range states of an element.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientRangeGet(QMESH_MSG_HEADER_T *header);

/*----------------------------------------------------------------------------*
 *   LightHslClientRangeSet
 */
/*! \brief Light HSL Range Set is a message used to set the Light HSL Hue Range and
 *        Light HSL Saturation Range states of an element.
 *
 * \param [in] header       Pointer to message header \ref QMESH_MSG_HEADER_T
 * \param [in] data         Pointer to \ref LIGHT_HSL_CLIENT_RANGE_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientRangeSet(QMESH_MSG_HEADER_T *header, LIGHT_HSL_CLIENT_RANGE_SET_T *data);

/*!@} */
#endif /* __LIGHT_HSL_CLIENT_H__ */

