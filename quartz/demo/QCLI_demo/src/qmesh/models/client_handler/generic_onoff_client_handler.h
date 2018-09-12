/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/* Note: this is an auto-generated file. */

#ifndef __GENERIC_ONOFF_CLIENT_HANDLER_H__
#define __GENERIC_ONOFF_CLIENT_HANDLER_H__

#include "sig_types.h"
#include "qcli_api.h"

/** \addtogroup generic_onoff_client_handler
 * @{
 * \JAVADOC_AUTOBRIEF 
 * \file generic_onoff_client_handler.h
 * \brief   This file contains QCLI command functions declaration for
 *          GENERIC_ONOFF_CLIENT model.
 *
 * \details 
 */

/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliGenericOnoffClientGet
 *
 *  DESCRIPTION
 *      QCLI command function of GENERIC_ONOFF_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliGenericOnoffClientGet(uint32_t parameters_count, QCLI_Parameter_t * parameters);

/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliGenericOnoffClientSet
 *
 *  DESCRIPTION
 *      QCLI command function of GENERIC_ONOFF_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliGenericOnoffClientSet(uint32_t parameters_count, QCLI_Parameter_t * parameters);



/*!@} */
#endif /* __GENERIC_ONOFF_CLIENT_HANDLER_H__ */

