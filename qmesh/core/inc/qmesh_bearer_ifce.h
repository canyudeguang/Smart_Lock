/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_bearer_ifce.h
 *  \brief Qmesh bearer APIs for Mesh Profile
 *
 *   This file contains the functions and definitions for the all the bearer
 *   interface used by the stack
 */
 /******************************************************************************/

#ifndef __QMESH_BEARER_IFCE_H__
#define __QMESH_BEARER_IFCE_H__

#include "qmesh_bearer.h"

/*! \brief Qmesh Bearer interface
 *  @{
 */

typedef enum
{
    QMESH_PDU_TYPE_NETWORK       = 0,
    QMESH_PDU_TYPE_BEACON        = 1,
    QMESH_PDU_TYPE_PROXY_CFG     = 2,
    QMESH_PDU_TYPE_PROVISIONING  = 3
} QMESH_PDU_TYPE_T;

QMESH_RESULT_T QmeshBearerInit(bool proxy_enable);

QMESH_RESULT_T QmeshSendPDU(QMESH_BEARER_HANDLE_T bearer_handle,
                            const QMESH_BEARER_MSG_PARAM_T *bearer_param,
                            uint16_t dst_addr,
                            QMESH_PDU_TYPE_T pdu_type, const uint8_t *pdu, uint16_t pdu_len);

QMESH_RESULT_T QmeshSendOnAdvBearer(const QMESH_BEARER_MSG_PARAM_T *bearer_param,
                                    QMESH_PDU_TYPE_T pdu_type, const uint8_t *pdu, uint16_t pdu_len);

QMESH_RESULT_T QmeshSendOnAllBearer(const QMESH_BEARER_MSG_PARAM_T *bearer_param,
                                    uint16_t dst_addr,
                                    QMESH_PDU_TYPE_T pdu_type, const uint8_t *pdu, uint16_t pdu_len);

void QmeshReceivePDU(const QMESH_BEARER_HEADER_T *bearer_hdr, bool proxy,
                     QMESH_PDU_TYPE_T pdu_type, uint8_t *pdu, uint16_t pdu_len);

void QmeshBearerDeInit(void);

/*!@} */
#endif /* __QMESH_BEARER_IFCE_H__ */

