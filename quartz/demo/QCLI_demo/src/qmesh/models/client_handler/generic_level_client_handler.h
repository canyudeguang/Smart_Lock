/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/* Note: this is an auto-generated file. */

#ifndef __GENERIC_LEVEL_CLIENT_HANDLER_H__
#define __GENERIC_LEVEL_CLIENT_HANDLER_H__

#include "sig_types.h"
#include "qcli_api.h"

/** \addtogroup generic_level_client_handler
 * @{
 * \JAVADOC_AUTOBRIEF 
 * \file generic_level_client_handler.h
 * \brief   This file contains QCLI command functions declaration for
 *          GENERIC_LEVEL_CLIENT model.
 *
 * \details 
 */

/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliGenericLevelClientGet
 *
 *  DESCRIPTION
 *      QCLI command function of GENERIC_LEVEL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliGenericLevelClientGet(uint32_t parameters_count, QCLI_Parameter_t * parameters);

/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliGenericLevelClientSet
 *
 *  DESCRIPTION
 *      QCLI command function of GENERIC_LEVEL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliGenericLevelClientSet(uint32_t parameters_count, QCLI_Parameter_t * parameters);

/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliGenericLevelClientDeltaSet
 *
 *  DESCRIPTION
 *      QCLI command function of GENERIC_LEVEL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliGenericLevelClientDeltaSet(uint32_t parameters_count, QCLI_Parameter_t * parameters);

/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliGenericLevelClientMoveSet
 *
 *  DESCRIPTION
 *      QCLI command function of GENERIC_LEVEL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliGenericLevelClientMoveSet(uint32_t parameters_count, QCLI_Parameter_t * parameters);



/*!@} */
#endif /* __GENERIC_LEVEL_CLIENT_HANDLER_H__ */

