/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/******************************************************************************/
/*! \file qmesh_generic_poweronoff_setup_handler.h
 *  \brief defines and functions for SIGmesh Generic_Power OnOff Setup Handler
 *
 *   This file contains data types and APIs exposed by the Generic Power
 *   OnOff Setup model to the Applications
 */
/******************************************************************************/
#ifndef __QMESH_GENERIC_POWERONOFF_SETUP_HANDLER_H__
#define __QMESH_GENERIC_POWERONOFF_SETUP_HANDLER_H__


#include "qmesh_generic_poweronoff_handler.h"

/*! \addtogroup Model_Generic_Power_OnOff_Setup_Handler
 * @{
 */

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerOnOffSetupServerHandler
 */
/*! \brief Generic PowerOnOff Setup Application Event Handler
 *
 *  \param [in] nw_hdr      Network header \ref QMESH_NW_HEADER_INFO_T
 *  \param [in] key_info    Pointer to Access layer key \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T
 *  \param [in] elem_data   Pointer to element data \ref QMESH_ELEMENT_CONFIG_T
 *  \param [in] model_data  Pointer to Model context \ref QMESH_MODEL_DATA_T
 *  \param [in] msg         Pointer to Message Data
 *  \param [in] msg_len     Message Data Length
 *
 *  \returns \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericPowerOnOffSetupServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshGenericPowerOnOffSetupInit
 */
/*! \brief Call this API to initialize power onoff setup server.
 *
 *  \param [in]         model_data
 *
 *  \return None
 */
/*----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffSetupInit (QMESH_GEN_POWERONOFF_DATA_T
                                        *model_data);

/*!@} */
#endif /* __QMESH_GENERIC_POWERONOFF_SETUP_HANDLER_H__ */
