/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/

#ifndef __MODEL_CLIENT_HANDLER_UTILS_H__
#define __MODEL_CLIENT_HANDLER_UTILS_H__

#include "qcli_api.h"
#include "model_client_common.h"
#include "qmesh_types.h"

/** \addtogroup model_client_handler_utils
 * @{
 * \JAVADOC_AUTOBRIEF 
 * \file model_client_handler_utils.h
 * \brief   
 *
 * \details 
 */

/*----------------------------------------------------------------------------*
 *  NAME
 *      UpdateQmeshMsgHeader
 *
 *  DESCRIPTION
 *      Updates the message header with parameters passed in 'parameters' and increments
 *      the 'index' with number of values read from 'parameters'.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
void UpdateQmeshMsgHeader (QMESH_MSG_HEADER_T *hdr,
                                    QCLI_Parameter_t * parameters,
                                    uint8_t *index,
                                    bool hasReliableParam);


/*!@} */
#endif /* __MODEL_CLIENT_HANDLER_UTILS_H__ */

