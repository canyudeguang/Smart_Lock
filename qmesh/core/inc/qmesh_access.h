/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_access.h
 *
 *  \brief Qmesh Access Layer API
 *
 *   This file contains the API for Qmesh Access Layer.
 */
 /*****************************************************************************/

#ifndef _QMESH_ACCESS_H_
#define _QMESH_ACCESS_H_

/*============================================================================*
    Header Files
 *============================================================================*/

/*============================================================================*
    Macro Definitions
 *============================================================================*/
#define ACCESS_LAYER_OPCODE_OFFSET 0UL


#define QMESH_ONE_BYTE_OPCODE_SIZE      1UL
#define QMESH_TWO_BYTES_OPCODE_SIZE     2UL
#define QMESH_THREE_BYTES_OPCODE_SIZE   3UL

#define get_opcode_format(opcode) (((opcode) >> 6) & 3)

#define QMESH_ACCESS_LAYER_PACK_TWO_BYTE_OPCODE(opc_b1, opc_b2) (((opc_b1) << 8) | (opc_b2))

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/

/*============================================================================*
    Data Type Definitions
 *============================================================================*/

typedef struct
{
    QMESH_NW_HEADER_INFO_T              nw_hdr;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T     al_key_info;
} QMESH_ACCESS_RX_INFO_T;

/*============================================================================*
    API Definitions
 *============================================================================*/

 QMESH_RESULT_T QmeshAccesssRXPDU(const QMESH_ACCESS_RX_INFO_T *al_rx_info,
                                  const uint8_t *access_pdu, uint16_t access_pdu_len);

#endif /* _QMESH_ACCESS_H_ */

