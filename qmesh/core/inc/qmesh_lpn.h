/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_lpn.h
 *  \brief
 *
 *   This file contains the data types and function prototypes for
 *      Low power node functions
 */
 /*****************************************************************************/

#ifndef __QMESH_LPN_H__
#define __QMESH_LPN_H__

#include "qmesh_types.h"
#include "qmesh_low_power_node.h"
#include "qmesh_soft_timers.h"
#include "qmesh_config.h"

#define QMESH_FRND_CRITERIA_RSSI_BIT_POSITION              (0x05)
#define QMESH_FRND_CRITERIA_RCV_WINDOW_BIT_POSITION        (0x03)
#define MAX_FRIEND_OFFER_CACHE_SIZE                        (3)

#define QMESH_LPN_PRIMARY_ELEMENT_INDEX                    (0x00)
#define QMESH_LPN_ESTABLISH_RETRY_COUNT_MAX                (0x06)
#define QMESH_LPN_POLL_RETRY_COUNT_MAX                     (0x0a)
#define QMESH_LPN_POLL_RETRY_COUNT_MIN                     (0x01)

#define QMESH_LPN_ESTABLISH_SLEEP_TIME                     (100 * QMESH_MILLISECOND)
#define QMESH_LPN_ESTABLISH_RECEIVE_WINDOW_TIME            (1 * QMESH_SECOND)

/*! \brief States of LPN operation
 */
typedef enum
{
    lpn_state_idle,
    lpn_state_sleep,
    lpn_state_discovery,
    lpn_state_frnd_offer,
    lpn_state_establishing,
    lpn_state_established,
    lpn_state_poll_req_sent,
    lpn_state_queue_not_empty,
    lpn_state_queue_empty,
    lpn_state_message_received,
} QMESH_LPN_STATE_T;

/*! \brief LPN Provisioned and Subnet Index Information.
 */
typedef struct
{
    uint8_t prov_net_index;
    uint8_t subnet_index;
} QMESH_LPN_INDEX_INFO_T;

/*! \brief LPN subnet context information */
typedef struct
{
    uint16_t frnd_addr;             /*!< Address of friend node */
    uint16_t prev_frnd_addr;        /*!< Address of previous friend */
    uint16_t lpn_counter;           /*!< LPN counter */
    uint32_t poll_timeout_time;     /*!< Initial value of the PollTimeout timer set by LPN */
    uint8_t rcv_delay;              /*!< Receive delay requested by the LPN */
    uint16_t frnd_req_lpn_counter;  /*!< LPN counter */
    QMESH_FRND_CRITERIA_T criteria; /*!< Criteria set by LPN */
    bool send_next_friend_req;      /*!< Flag to check if friend request can be sent again */
    uint8_t rcv_window;             /*!< Friend receive window */
    uint8_t poll_retry_count;       /*!< Poll retry count */
    uint8_t trans_num;              /*!< Transaction number */
    uint8_t fsn;                    /*!< Friend sequence number */
    QMESH_LPN_STATE_T state;                /*!< State of LPN */
    QMESH_TIMER_HANDLE_T lpn_establish_tid; /*!< Timer used in lpn establishing phase */
    uint16_t subnet_key_idx;        /*!< Subnet Key Index Value associated with the context  */
    bool subscription_update;       /*!< Flag to check if subscription update message has been received */
    QMESH_LPN_INDEX_INFO_T lpn_info; /*!< LPN Index information for passing to Timer */
    uint16_t frnd_offer_expiry_time[3]; /*!< Expected expiry time of friend offer */
    bool is_configured;             /*!< Subnet is configured or not */       
} QMESH_LPN_SUBNET_CONTEXT_T;

/*! \brief LPN network context information */
typedef struct
{
    uint8_t prov_net_index;        /*!< Network Index associated with the context */
    QMESH_LPN_SUBNET_CONTEXT_T subnet[QMESH_MAX_SUBNETS]; /*!< Subnet Information */
} QMESH_LPN_DEVICE_CONTEXT_T;

/*! \brief LPN context information */
typedef struct
{
    QMESH_TIMER_HANDLE_T poll_tid;          /*!< Poll timeout tid */
    QMESH_TIMER_HANDLE_T lpn_messaging_tid; /*!< Receive window timer */
    uint16_t min_poll_req_time[3];          /*!< Time at which next poll request will be sent */
    uint16_t max_delay_rcv_window;          /*!< Maximum value of sum(delay,receive window) betweens all subnets */
    uint32_t min_polltimeout;               /*!< Minimum value of polltimeout between all subnets */
    uint32_t adjust_min_polltimeout;        /*!< Adjusted Minimum value of polltimeout between all subnets */
    uint8_t min_delay;                      /*!< Delay value to use in messaging phase.Minimum value of delay is taken between all subnets */
    uint32_t frnd_status_bitmask;           /*!< Friend status bitmask.Bit is set to 0, if no friend is there else it is set to 1. */
    uint8_t lpn_messaging_rcv_window;       /*!< Receive window value to use in messagong phase. */
    bool send_poll_request;                 /*!< Flag to check if poll request needs to be sent */
    QMESH_MUTEX_T lpn_mutex;                /*!< LPN Mutex */
    QMESH_LPN_DEVICE_CONTEXT_T frnd_dev[QMESH_MAX_NETWORKS]; /*!< LPN Context Info */
} QMESH_LPN_CONTEXT_T;

#endif /* __QMESH_LPN_H__ */

