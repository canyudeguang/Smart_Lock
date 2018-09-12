/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_lower_transport.h
 *
 *  \brief Qmesh Lower Transport Layer API
 *
 *   This file contains the API for Qmesh Lower Transport Layer.
 */
 /*****************************************************************************/

#ifndef _QMESH_LOWER_TRANSPORT_H_
#define _QMESH_LOWER_TRANSPORT_H_

#include "qmesh_network.h"

/*============================================================================*
    Macro Definitions
 *============================================================================*/
#define MAX_SEG_CONTROL_PDU_LEN         (8)
#define MAX_SEG_ACCESS_PDU_LEN          (12)

#define MAX_UNSEG_CONTROL_PDU_LEN       (11)
#define MAX_UNSEG_ACCESS_PDU_LEN        (15)


#define AKFAID_OPCODE_OFFSET            (0)
#define AKFAID_OPCODE_MASK              (0x7F)

#define SZMIC_OFFSET                    (1)
#define SZMIC_MASK                      (0x80)
#define SZMIC_BIT_POS                   (0x7)

#define SEGN_OFFSET                     (3)
#define SEGN_MASK                       (0x1F)

#define SEG_MESG_BITMASK                (0x80)

#define LT_SEG_PDU_HEADER_LEN           (4)
#define LT_UNSEG_PDU_HEADER_LEN         (1)

#define SEG_PDU_PAYLOAD_OFFSET          (LT_SEG_PDU_HEADER_LEN)
#define UNSEG_PDU_PAYLOAD_OFFSET        (LT_UNSEG_PDU_HEADER_LEN)

#define GET_MAX_UNSEG_PDU_LEN(mt)       ((QMESH_NETWORK_MESG_ACCESS != (mt)) ? \
                                         (MAX_UNSEG_CONTROL_PDU_LEN) : \
                                         (MAX_UNSEG_ACCESS_PDU_LEN))

#define GET_MAX_SEG_PDU_LEN(mt)         ((QMESH_NETWORK_MESG_ACCESS != (mt)) ? \
                                         (MAX_SEG_CONTROL_PDU_LEN) : \
                                         (MAX_SEG_ACCESS_PDU_LEN))

#define GET_NUMSEG_FROM_UTPDU_LEN(mt, ut_pdu_len)  \
                                        (((ut_pdu_len)+ GET_MAX_SEG_PDU_LEN(mt) - 1)/ \
                                         (GET_MAX_SEG_PDU_LEN(mt)))

#define GET_MAX_LTPDU_LEN(mt, is_seg)   ((is_seg) ? \
                                         (GET_MAX_SEG_PDU_LEN(mt) + LT_SEG_PDU_HEADER_LEN) : \
                                         (GET_MAX_UNSEG_PDU_LEN(mt) + LT_UNSEG_PDU_HEADER_LEN))

#define GET_MAX_LTDATA_LEN(mt, is_seg)  ((is_seg) ? \
                                         (GET_MAX_SEG_PDU_LEN(mt)) : \
                                         (GET_MAX_UNSEG_PDU_LEN(mt)))


#define GET_SEGO(seg_hdr)               (((seg_hdr[2] & 0x3) << 3) | \
                                         ((seg_hdr[3] >> 5) & 0x7))

#define GET_SEGN(seg_hdr)               (seg_hdr[SEGN_OFFSET] & SEGN_MASK)

#define QMESH_MAX_UT_PDU_LEN            (384)

#define SZMIC_CNTRL_OCTETS              (0)
#define SZMIC_32BIT_OCTETS              (4)
#define SZMIC_64BIT_OCTETS              (8)

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/
 /*!< \brief Upper Transport PDU. */
 typedef struct
 {
     uint16_t            pdu_len;
     uint8_t             mic_len;
     uint8_t            *pdu;
     uint8_t            *mic;
 } QMESH_UT_PDU_T;

/*============================================================================*
    Data Type Definitions
 *============================================================================*/

typedef QMESH_NETWORK_HEADER_T QMESH_NETWORK_INFO_T;

typedef struct
{
    uint8_t                     num_retries;            /*!< Number of retries to be used for sending the message. */
    uint8_t                     akfaid_opcode;          /*!< AKF and AID Flags. This needs to be Opcode for Control Message. */
    bool                        is_seg;                 /*!< Segmented or Unsegmented message. */
} QMESH_LT_TX_INFO_T;

typedef struct
{
    uint32_t                    seq_num;        /*!< Sequence Number for the PDU. */
    uint8_t                     pdu_len;        /*!< Lower Transport PDU Length. */
    uint8_t                     pdu[1];         /*!< Lower Transport PDU. */
} QMESH_LT_PDU_T;

typedef struct
{
    uint32_t                    auth_seqnum;
    uint8_t                     akf_aid_opcode;
} QMESH_LT_RX_INFO_T;

typedef struct
{
    uint8_t                     num_segments;   /*!< Number of Message Segments. */
    QMESH_LT_PDU_T            **lt_pdu_list;    /*!< Pointer to the Array of LT PDU Pointers. */
} QMESH_LT_PDU_LIST_T;

/*============================================================================*
    API Definitions
 *============================================================================*/

QMESH_RESULT_T QmeshLowerTransportInit(void);

bool QmeshLowerTransportIsPendingTX(uint8_t net_idx, uint32_t iv_index);

QMESH_RESULT_T QmeshLowerTransportTXPDU(const QMESH_NETWORK_INFO_T *nw_info, const QMESH_LT_TX_INFO_T *lt_info,
                                        const QMESH_UT_PDU_T *ut_pdu);

void QmeshLowerTransportUpdateLpnFSN(const QMESH_NETWORK_HEADER_T *nw_hdr);

bool QmeshLowerTransportRXPDU(const QMESH_BEARER_HEADER_T *bearer_hdr, const QMESH_NETWORK_HEADER_T *nw_hdr,
                              uint8_t *lt_pdu, uint8_t lt_pdu_len);

void QmeshLowerTransportClearSeqAuthCache(uint8_t net_idx, uint16_t start_addr, uint16_t num_elem);

void QmeshLowerTransportDeInit(void);

#endif /* _QMESH_LOWER_TRANSPORT_H_ */


