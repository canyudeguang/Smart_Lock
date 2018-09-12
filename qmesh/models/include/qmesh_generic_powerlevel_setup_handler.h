/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/*! \file qmesh_generic_powerlevel_setup_handler.h
 *  \brief defines and functions for SIGmesh Generic Power Level model server
 *
 *   This file contains data types and APIs exposed by the Generic Power Level
 *   server model to the Application
 */
/******************************************************************************/
#ifndef __QMESH_GENERIC_POWERLEVEL_SETUP_HANDLER_H__
#define __QMESH_GENERIC_POWERLEVEL_SETUP_HANDLER_H__

#include "qmesh_generic_powerlevel_handler.h"

/** \addtogroup Model_Generic_Power_Level_Setup_Handler
* @{
*/

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelSetupServerHandler
 */
/*! \brief This method is the Application model callback.
 *              This API gets called from the model library whenever mesh stack sends the model message.
 *              This API will also be called from other extended models which has bound state with this model.
 *
 *  \param [in] nw_hdr          Network header \ref QMESH_NW_HEADER_INFO_T
 *  \param [in] key_info        Pointer to Access layer key \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T
 *  \param [in] elem_data       Pointer to element data \ref QMESH_ELEMENT_CONFIG_T
 *  \param [in] model_data      Pointer to Model context \ref QMESH_MODEL_DATA_T
 *  \param [in] msg             Pointer to Message Data
 *  \param [in] msg_len         Message Data Length
 *
 *  \return QMESH_RESULT_T.
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericPowerLevelSetupServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelSetupServerAppInit
 */
/*! \brief Initialise Generic PowerLevel Setup server model and registers callback
 *  with the mesh stack.
 *
 *  \param [in] model_context Pointer to the Generic PowerLevel Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *
 *  \return \ref QMESH_RESULT_T.
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericPowerLevelSetupServerAppInit (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerLevelSetupServerAppDeInit
 */
/*! \brief This function De-initializes Generic PowerLevel Setup model state data and Unregisters the model
 *  from the stack.
 *
 *  \param [in] model_context Pointer to the Generic Level Data
 *                       \ref QMESH_GENERIC_POWERLEVEL_CONTEXT_T
 *
 *  \return Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelSetupServerAppDeInit (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T  *model_context);
/*!@} */

#endif
