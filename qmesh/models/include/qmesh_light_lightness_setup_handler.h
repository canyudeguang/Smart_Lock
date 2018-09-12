/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/******************************************************************************/
/*! \file qmesh_light_lightness_setup_handler.h
 *  \brief defines and functions for Light Lightness Setup Model
 *
 *   This file contains data types and APIs exposed by the Light Lightness setup
 *   model to the Applications
 */
/********************************************************************************/
#ifndef __QMESH_LIGHT_LIGHTNESS_SETUPMODEL_H__
#define __QMESH_LIGHT_LIGHTNESS_SETUPMODEL_H__

/*! \addtogroup Model_Light_Lightness_Setup_Handler
 * @{
 */

#include "qmesh_light_lightness_handler.h"

/*----------------------------------------------------------------------------*
 * QmeshLightnessSetupServerHandler
 */
/*! \brief This method is called by the stack to handle lightness setup Messages.
 *         if any.
 *
 *  \param [in] nw_hdr      Network header \ref QMESH_NW_HEADER_INFO_T
 *  \param [in] key_info    Pointer to Access layer key \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T
 *  \param [in] elem_data   Pointer to element data \ref QMESH_ELEMENT_CONFIG_T
 *  \param [in] model_data  Pointer to Model context \ref QMESH_MODEL_DATA_T
 *  \param [in] msg         Pointer to Message Data
 *  \param [in] msg_len     Message Data Length
 *
 *  \return \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightnessSetupServerHandler(const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                               const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
                                               const QMESH_ELEMENT_CONFIG_T *elem_data,
                                               QMESH_MODEL_DATA_T *model_data,
                                               const uint8_t *msg,
                                               uint16_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshLightnessSetupServerAppInit
 */
/*! \brief Application calls this API to register the callback for the model
 *         messages.
 *
 *  \return \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightnessSetupServerAppInit(void);

/*----------------------------------------------------------------------------*
 * QmeshLightnessSetupServerAppDeInit
 */
/*! \brief Application calls this API to de-register the callback for the model
 *         messages.
 *
 *  \return \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightnessSetupServerAppDeInit(void);

/*!@} */
#endif /* __QMESH_GENERIC_POWERONOFFSETUP_MODEL_H__ */
