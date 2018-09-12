/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_network.h
 *
 *  \brief Qmesh Network Layer function and data types implementations.
 *
 *   This file contains the data types and function prototypes for network
 *   layer functions.
 */
 /*****************************************************************************/

#ifndef _QMESH_NETWORK_H_
#define _QMESH_NETWORK_H_

/*============================================================================*
    Header Files
 *============================================================================*/
#include "qmesh_types.h"
#include "qmesh_result.h"
#include "qmesh_db.h"

// TODO: These headers need clean-up.
#include "qmesh_bearer_ifce.h"
#include "qmesh_bearer.h"

/*============================================================================*
    Macro Definitions
 *============================================================================*/

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/

/*============================================================================*
    Data Type Definitions
 *============================================================================*/

/*! \brief Network PDU Header Information.
 * +--------------+----------+-----------------------------------------------------------------------------------------------------------+
 * |  Field Name  |   Bits   |                                                   Notes                                                   |
 * +--------------+----------+-----------------------------------------------------------------------------------------------------------+
 * | IVI          | 1        | Least significant bit of IV Index                                                                         |
 * | NID          | 7        | Value derived from the NetKey used to identify the Encryption Key and Privacy Key used to secure this PDU |
 * | CTL          | 1        | Network Control  (0 - Access Message, NetMIC = 32; 1 - Control Message, NetMIC = 64)                      |
 * | TTL          | 7        | Time To Live                                                                                              |
 * | SEQ          | 24       | Sequence Number                                                                                           |
 * | SRC          | 16       | Source Address                                                                                            |
 * | DST          | 16       | Destination Address                                                                                       |
 * | TransportPDU | 8 to 128 | Transport Protocol Data Unit                                                                              |
 * | NetMIC       | 32 or 64 | Message Integrity Check for Network                                                                       |
 * +--------------+----------+-----------------------------------------------------------------------------------------------------------+
 */
typedef struct
{
    uint32_t                        iv_index;           /*!< IV Index for Encryption/Decryption of TX/RX Packet */
    QMESH_SUBNET_KEY_IDX_INFO_T     key_idx;            /*!< Network Key Index information. See \ref QMESH_SUBNET_KEY_IDX_INFO_T. */
    uint16_t                        frnd_lpn_addr;      /*!< Unicast Address of FRND or LPN. QMESH_UNASSIGNED_ADDR when it a subnet message.*/
    uint32_t                        seq_num;            /*!< Sequence Number. */
    uint16_t                        src_addr;           /*!< Source Address. */
    uint16_t                        dst_addr;           /*!< Destination Address. */
    QMESH_NETWORK_MESG_T            msg_type;           /*!< Network Layer Message Type. See \ref QMESH_NETWORK_MESG_T */
    uint8_t                         ttl;                /*!< Time To Live. */
} QMESH_NETWORK_HEADER_T;

/*============================================================================*
    Public function prototypes
 *============================================================================*/
/*----------------------------------------------------------------------------*
 * QmeshNetworkLayerInit
 */
/*! \brief Initialize Network Layer.
 *
 *  \returns QMESH_RESULT_T See \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
QMESH_RESULT_T QmeshNetworkLayerInit(void);

/*----------------------------------------------------------------------------*
 * QmeshNetworkLayerTxPDU
 */
/*! \brief Send a network layer PDU on all the network interfaces.
 *
 *  \param[in]  nw_hdr      Network Layer Header information. See \ref QMESH_NETWORK_HEADER_T.
 *  \param[in]  lt_pdu      Pointer to the Lower Transport PDU.
 *  \param[in]  lt_pdu_len  Length of Lower Transport PDU.
 *
 *  \returns QMESH_RESULT_T See \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
QMESH_RESULT_T QmeshNetworkLayerTxPDU(const QMESH_NETWORK_HEADER_T *nw_hdr,
                                      const uint8_t *lt_pdu, uint8_t lt_pdu_len);


QMESH_RESULT_T QmeshCreateProxyConfigPDU(const QMESH_NETWORK_HEADER_T *nw_hdr,
                                         const uint8_t *proxy_cfg_pdu,
                                         uint8_t proxy_cfg_pdulen,
                                         uint8_t *proxy_cfg_net_pdu);


/*----------------------------------------------------------------------------*
 * QmeshNetworkLayerRxPDU
 */
/*! \brief Receives a Network PDU from a Mesh bearer and process it.
 *
 *  \param[in] bearer_header    Pointer to the bearer header.
 *  \param[in] pdu_type         Type of PDU. See \ref QMESH_PDU_TYPE_T.
 *  \param[in] nw_pdu           Pointer to the Network PDU.
 *  \param[in] nw_pdu_len       Network PDU length in octets.
 *
 *  \returns QMESH_RESULT_T     See \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
QMESH_RESULT_T QmeshNetworkLayerRxPDU(const QMESH_BEARER_HEADER_T *bearer_header,
                                      QMESH_PDU_TYPE_T pdu_type, uint8_t *net_pdu, uint8_t net_pdu_len);

/*----------------------------------------------------------------------------*
 * QmeshNetworkLayerDeInit
 */
/*! \brief Removes all Network Layer State Data.
 *
 *  \returns none.
 */
/*---------------------------------------------------------------------------*/
void QmeshNetworkLayerDeInit(void);

#endif /* _QMESH_NETWORK_H_ */

