/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/******************************************************************************/
/*! \file qmesh_light_hsl_setup_handler.h
 *  \brief defines and functions for SIGmesh Light HSL Setup Handler
 *
 *   This file contains data types and APIs exposed by the Light HSL setup
 *
 */
/******************************************************************************/
#ifndef __QMESH_LIGHT_HSL_SETUP_HANDLER_H__
#define __QMESH_LIGHT_HSL_SETUP_HANDLER_H__

/*! \addtogroup  Model_Lightness_HSL_Setup_Handler
 * @{
 */

#include "qmesh_hal_ifce.h"
#include "qmesh_types.h"
#include "qmesh_cache_mgmt.h"
#include "qmesh_light_hsl_handler.h"

/*============================================================================*
 *  Public Definitions
 *============================================================================*/
/*!\brief  Light HSL Setup Status Report Size */
#define LIGHT_HSL_SETUP_STATUS_SIZE                   (11)
#define LIGHT_HSL_SETUP_DEFAULT_STATUS_SIZE           (8)

/*!\brief  Lightness Message offsets */
#define OFFSET_VALUE_BYTE_LSB                         (0)
#define OFFSET_VALUE_BYTE_MSB                         (1)

/*!\brief  Lightness Range Error defines */
#define LIGHT_HSL_SETUP_RANGE_SET_SUCCESS             (0)
#define LIGHT_HSL_SETUP_CANNOT_SET_RANGE_MIN          (1)
#define LIGHT_HSL_SETUP_CANNOT_SET_RANGE_MAX          (2)

/*----------------------------------------------------------------------------*
 * QmeshLightHSLSetupServerHandler
 */
/*! \brief Light HSL Setup Event Handler
 *
 *  \param [in] nw_hdr          Network header \ref QMESH_NW_HEADER_INFO_T
 *  \param [in] key_info        Pointer to Access layer key \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T
 *  \param [in] elem_data       Pointer to element data \ref QMESH_ELEMENT_CONFIG_T
 *  \param [in] model_data      Pointer to Model context \ref QMESH_MODEL_DATA_T
 *  \param [in] msg             Pointer to Message Data
 *  \param [in] msg_len         Message Data Length
 *
 *  \returns nothing
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLSetupServerHandler(const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                            const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
                                            const QMESH_ELEMENT_CONFIG_T *elem_data,
                                            QMESH_MODEL_DATA_T *model_data,
                                            const uint8_t *msg,
                                            uint16_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshLightHSLSetupServerAppInit
 */
/*! \brief Initializes the Light HSL Setup server
 *
 *  \returns \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLSetupServerAppInit(void);

/*----------------------------------------------------------------------------*
 * QmeshLightHSLSetupServerAppDeInit
 */
/*! \brief De-Initializes the Light HSL Setup server
 *
 *  \returns \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLSetupServerAppDeInit(void);

/*!@} */
#endif /* __QMESH_LIGHT_HSL_SETUP_HANDLER_H__ */
