/*=============================================================================
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
 /*! \file qmesh_friendship.h
  *  \brief Qmesh API
  *
  *   This file contains the API for Qmesh friendship feature.
  *
  */
/******************************************************************************/

#ifndef __QMESH_FRIENDSHIP_H__
#define __QMESH_FRIENDSHIP_H__

#include "qmesh_types.h"
#include "qmesh_friend_lpn_common.h"
#include "qmesh_soft_timers.h"

#ifdef __cplusplus
 extern "C" {
#endif

/*! \addtogroup Core_Friendship
 * @{
 */

/*! \brief  Slot state of cache */
typedef enum
{
    QMESH_SLOT_STATE_EMPTY,             /*!< Slot is empty. */
    QMESH_SLOT_STATE_OCCUPIED,          /*!< Slot is occupied. */
} QMESH_FRND_CACHE_SLOT_STATE_T;

/*! \brief Type of message in friend cache */
typedef enum
{
    QMESH_FRND_CACHE_MSG_TYPE_SEG,     /*!< Segmented message. */
    QMESH_FRND_CACHE_MSG_TYPE_UNSEG,   /*!< Unsegmented message. */
    QMESH_FRND_CACHE_MSG_TYPE_ACK,     /*!< Acknowledgment message. */
    QMESH_FRND_CACHE_MSG_TYPE_UPDATE,  /*!< Update message. */
} QMESH_FRND_CACHE_MSG_TYPE_T;


/*! \brief  States of friend operation */
typedef enum
{
    QMESH_FRND_LPN_STATE_DISABLED,     /*!< Friend is in disabled state. */
    QMESH_FRND_LPN_STATE_IDLE,         /*!< Friend is in idle state. */
    QMESH_FRND_LPN_STATE_ESTABLISHING, /*!< Friend is in establishing state. */
    QMESH_FRND_LPN_STATE_ESTABLISHED,  /*!< Friend is in established state. */
} QMESH_FRND_LPN_STATE_T;


/*! \brief Network PDU Header Information. */
typedef struct
{
    uint32_t                        iv_index;           /*!< IV Index for Encryption/Decryption of TX/RX Packet. */
    QMESH_SUBNET_KEY_IDX_INFO_T     key_idx;            /*!< Network Key Index information. See \ref QMESH_SUBNET_KEY_IDX_INFO_T. */
    uint16_t                        frnd_lpn_addr;      /*!< Unicast Address of FRND or LPN. QMESH_UNASSIGNED_ADDR when it a subnet message.*/
    uint32_t                        seq_num;            /*!< Sequence Number. */
    uint16_t                        src_addr;           /*!< Source Address. */
    uint16_t                        dst_addr;           /*!< Destination Address. */
    QMESH_FRND_CACHE_MSG_TYPE_T           msg_type;           /*!< Message type. Refer \ref QMESH_FRND_CACHE_MSG_TYPE_T. */
    bool                            is_ctrl_msg;        /*!< Control message or not */
    uint8_t                         ttl;                /*!< Time To Live. */
} QMESH_NETWORK_HEADER_INFO_T;


/*! \brief  Information of one single friend cache slot */
typedef struct
{
    QMESH_FRND_CACHE_SLOT_STATE_T                             slot_state; /*!< Cache Slot state. Refer \ref QMESH_FRND_CACHE_SLOT_STATE_T. */
    uint16_t                                            src_addr;   /*!< Source address of the received message. */
    QMESH_FRND_CACHE_MSG_TYPE_T                               msg_type;   /*!< Message Type. Refer \ref QMESH_FRND_CACHE_MSG_TYPE_T. */
    uint8_t                                             num_segments;   /*!< Number of segments. */
    QMESH_NETWORK_HEADER_INFO_T                         nw_hdr;         /*!< Network Header info. */
    uint8_t                                             lt_pdu[TRANSPORT_PDU_MAX_SIZE_IN_BYTES]; /*!< Lower Transport PDU. */
    uint8_t                                             lt_pdu_len; /*!< Length of the Lower Transport PDU. */
    uint8_t                                             Updateflag; /*!< Update Flag. */
    uint32_t                                            iv_idx;     /*!< IV index. */
} QMESH_FRND_CACHE_SLOT_T;


/*! \brief  Information of each individual LPN */
typedef struct
{
    bool                        frnd_valid; /*!< Friend is valid or not. */

    QMESH_FRND_LPN_STATE_T             state;      /*!< Friendship state. */
    uint16_t                    lpn_addr;   /*!< Address of the LPN. */
    int8_t                      rssi_val;   /*!< Rssi value of received friend request. */
    uint8_t                     recv_delay; /*!< Receive delay value. */
    uint32_t                    poll_timeout;   /*!< Poll timeout value. */
    uint16_t                    prev_frnd_addr; /*!< Previous friend address. */
    uint8_t                     num_elms;       /*!< Number of elements. */
    uint16_t                    lpn_counter;    /*!< LPN counter. */
    QMESH_SUBNET_KEY_IDX_INFO_T key_idx;        /*!< Key index. */

    QMESH_TIMER_HANDLE_T        poll_tid;       /*!< Poll timer Id. */
    QMESH_TIMER_HANDLE_T        rcv_delay_tid;  /*!< Receive delay timer id. */

    uint32_t                    frnd_clr_time;  /*!< Friend clear time. */
    QMESH_TIMER_HANDLE_T        frnd_clr_rpt_tid; /*!< Friend clear repeat timer id. */
    QMESH_TIMER_HANDLE_T        frnd_clr_proc_tid; /*!< Friend clear procedure timer id. */

    uint8_t                     len_sub_list;   /*!< Number of entries in subscription list. */
    uint8_t                     sub_list_size;  /*!< Size of the subscription List. */
    uint16_t                    *sub_list;      /*!< Subscription List. */

    bool                        is_last_opcode_poll;    /*!< If last opcode received is POLL. */
    uint8_t                     last_trans_num;         /*!< Transaction number. */
    bool                        remove_last_sent_msg;   /*!< Last sent message state. */
    int8_t                      last_sent_slot_idx;     /*!< Index of the last sent message. */
    int8_t                      tail_idx;               /*!< Tail index. */
    uint8_t                     head_idx;               /*!< Head index. */
    uint8_t                     num_free_slots;         /*!< Num of Free slots available. */

    uint8_t                     last_fsn;               /*!< Last friend sequence number. */
    uint8_t                     len_frnd_cache;         /*!< Length of the friend cache. */
    QMESH_FRND_CACHE_SLOT_T     *frnd_cache;            /*!< frnd_cache is pointer to an array of \ref QMESH_FRND_CACHE_SLOT_T of
                                                         *   length len_frnd_cache placed at contiguous locations. 
                                                         */
} QMESH_LPN_INFO_T;

/*============================================================================*
    Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * QmeshEnableFriendMode
 */
/*! \brief Enables/Disables the friend mode on the device
 *
 * \param[in] enable flag to enable/disable friend mode
 * \param[in] recv_window Receive window of friend, common for all LPNs
 * \param[in] num_lpn Number of lpns this device supports
 * \param[in] lpn_list Pointer to array of QMESH_LPN_INFO_T, with the array size as indicated by num_lpn. 
 *                     Refer \ref QMESH_LPN_INFO_T.
 *
 *  \returns None.
 */
/*----------------------------------------------------------------------------*/
extern void QmeshEnableFriendMode(bool enable, uint8_t recv_window,
                                  uint8_t num_lpn, QMESH_LPN_INFO_T *lpn_list);

/*! @} */
#ifdef __cplusplus
 }
#endif
#endif /* __QMESH_FRIENDSHIP_H__ */

