/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_proxy.h
 *
 *  \brief Qmesh Proxy Protocol function and data types implementations.
 *
 *   This file contains the data types and function prototypes
 *   for proxy protocol.
 */
 /*****************************************************************************/

#ifndef _QMESH_PROXY_H_
#define _QMESH_PROXY_H_

/*============================================================================*
    Header Files
 *============================================================================*/

/*============================================================================*
    Macro Definitions
 *============================================================================*/

#define QMESH_MAX_PROXY_FILTER_LIST_SIZE        (16)

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/

/*============================================================================*
    Data Type Definitions
 *============================================================================*/

/*============================================================================*
    Public function prototypes
 *============================================================================*/
QMESH_RESULT_T QmeshProxyInit(void);

QMESH_RESULT_T QmeshProxyBearerAdd(QMESH_BEARER_TYPE_T bearer_type,
                                   QMESH_BEARER_CONTEXT_T context,
                                   uint16_t mtu_size,
                                   QMESH_BEARER_HANDLE_T *bearer_handle);

QMESH_RESULT_T QmeshProxyBearerDelete(QMESH_BEARER_HANDLE_T bearer_handle);

QMESH_RESULT_T QmeshProxySend(QMESH_BEARER_HANDLE_T bearer_handle,
                              uint16_t dst_addr,
                              QMESH_PDU_TYPE_T pdu_type,
                              const uint8_t *pdu, uint16_t pdu_len);

bool QmeshProxyProvisiongEnabled(void);

QMESH_RESULT_T QmeshUpdateProxyFilterList(QMESH_BEARER_HANDLE_T bearer_handle, uint16_t addr);

QMESH_RESULT_T QmeshProxySendOnAll(uint16_t dst_addr,
                                   QMESH_PDU_TYPE_T pdu_type,
                                   const uint8_t *pdu, uint16_t pdu_len);

void QmeshProxyReceive(const QMESH_BEARER_HEADER_T *bearer_hdr,
                       const uint8_t *proxy_pdu, uint16_t proxy_pdu_len);

void QmeshProxyConfigRXPDU(const QMESH_BEARER_HEADER_T *bearer_header,
                           const QMESH_NETWORK_HEADER_T *nw_hdr,
                           uint8_t *proxy_cfg_pdu, uint8_t proxy_cfg_pdulen);

void QmeshProxyDeInit(void);

#endif /* _QMESH_PROXY_H_ */

