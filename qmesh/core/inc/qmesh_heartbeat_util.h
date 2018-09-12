/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_heartbeat_util.h
 *  \brief This is a utility header file for heartbeat operation
 */
 /*****************************************************************************/

#ifndef __QMESH_HEART_BEAT_UTIL_H__
#define __QMESH_HEART_BEAT_UTIL_H__

#include "qmesh.h"
#include "qmesh_hal_ifce.h"

/* Address mask defines */
#define QMESH_UNICAST_ADDR_MASK                     (0x8000)
#define QMESH_GROUP_ADDR_MASK                       (0xC000)
#define QMESH_VIRTUAL_ADDR_MASK                     (0x8000)
#define QMESH_ADDR_TYPE_UNASSIGNED_ADDR             (0x0000)

#define QMESH_HB_KEY_INDEX_VALIDATION_MASK          (0xF000)

/* 4-bit Feature mask */
#define QMESH_HB_FEATURES_MASK                      (0x000F)

/* HeartBeat period and count default values */
#define QMESH_HB_PERIOD_LOG_DISABLE                 (0x00)
#define QMESH_HB_COUNT_LOG_DISABLE                  (0x00)
#define QMESH_HB_PERIOD_DISABLE                     (0x00)

/* HeartBeat period log min and max values */
#define QMESH_HB_PERIOD_LOG_MIN                     (0x01)
#define QMESH_HB_PERIOD_LOG_MAX                     (0x11)

#define QMESH_HB_MAX_HOPS                           (0x7F)
#define QMESH_HB_MIN_HOPS                           (0x00)

#define QMESH_HB_PERIOD_MAX                         (0xFFFF)

#define QMESH_HB_COUNT_LOG_MAX                      (0x11)
#define QMESH_HB_TTL_MAX                            (0x7F)

#define QMESH_HB_COUNT_MAX                          (0xFFFF)

#define PROVISIONED_NET_ID                          (0x00)

/* HeartBeat Publication set message index */
#define QMESH_HB_PUB_SET_DST_ADDR_IDX               (0x00)
#define QMESH_HB_PUB_SET_COUNT_LOG_IDX              (0x02)
#define QMESH_HB_PUB_SET_PERIOD_LOG_IDX             (0x03)
#define QMESH_HB_PUB_SET_TTL_IDX                    (0x04)
#define QMESH_HB_PUB_SET_FEATURES_IDX               (0x05)
#define QMESH_HB_PUB_SET_NET_KEY_IDX                (0x07)

/* HeartBeat publicatrion status message index */
#define QMESH_HB_PUB_STATUS_OPCODE_IDX              (0x00)
#define QMESH_HB_PUB_STATUS_RESULT_IDX              (0x01)
#define QMESH_HB_PUB_STATUS_DST_ADDR_IDX            (0x02)
#define QMESH_HB_PUB_STATUS_COUNT_LOG_IDX           (0x04)
#define QMESH_HB_PUB_STATUS_PERIOD_LOG_IDX          (0x05)
#define QMESH_HB_PUB_STATUS_TTL_IDX                 (0x06)
#define QMESH_HB_PUB_STATUS_FEATURES_IDX            (0x07)
#define QMESH_HB_PUB_STATUS_NET_KEY_IDX             (0x09)

/* HeartBeat subscription set message index */
#define QMESH_HB_SUB_SET_SRC_ADDR_IDX               (0x00)
#define QMESH_HB_SUB_SET_DST_ADDR_IDX               (0x02)
#define QMESH_HB_SUB_SET_PERIOD_LOG_IDX             (0x04)

/* HeartBeat subscription status message index */
#define QMESH_HB_SUB_STATUS_OPCODE_IDX              (0x00)
#define QMESH_HB_SUB_STATUS_RESULT_IDX              (0x02)
#define QMESH_HB_SUB_STATUS_SRC_ADDR_IDX            (0x03)
#define QMESH_HB_SUB_STATUS_DST_ADDR_IDX            (0x05)
#define QMESH_HB_SUB_STATUS_PERIOD_LOG_IDX          (0x07)
#define QMESH_HB_SUB_STATUS_COUNT_LOG_IDX           (0x08)
#define QMESH_HB_SUB_STATUS_MIN_HOPS_IDX            (0x09)
#define QMESH_HB_SUB_STATUS_MAX_HOPS_IDX            (0x0A)


#define QMESH_HB_MSG_LEN_IN_BYTES                   (0x03)
#define QMESH_HB_RFU_TTL_IDX                        (0x00)
#define QMESH_HB_FEATURES_IDX                       (0x01)


#define QMESH_HB_IS_VALID_SRC_ADDR(addr)    \
    ((addr & QMESH_UNICAST_ADDR_MASK)== QMESH_ADDR_TYPE_UNASSIGNED_ADDR)

#define QMESH_HB_IS_VALID_DST_ADDR(addr)    \
    ((addr & QMESH_GROUP_ADDR_MASK) != QMESH_VIRTUAL_ADDR_MASK)

#define QMESH_HB_IS_PERIOD_LOG_INVALID(period_log)   \
    (period_log > QMESH_HB_PERIOD_LOG_MAX)

#define QMESH_HB_IS_COUNT_LOG_INVALID(count_log)   \
    ((count_log > QMESH_HB_COUNT_LOG_MAX) && (count_log != 0xFF))

#define QMESH_HB_IS_TTL_INVALID(ttl)   (ttl > QMESH_HB_TTL_MAX)

/* Size of log value transformation table */
#define QMESH_LOG_VALUE_LOOKUP_TABLE_SIZE       (16)


/*-----------------------------------------------------------------------------*
 * QmeshHeartBeatGetLogValue
 */
/*! \brief
 *
 *   This function encodes a 16-bit count value into an 8-bit log value.
 *
 *  \param
 *      log_value - address of 8-bit log value
 *      count - 16 bit of count value
 *
 *  \returns None
 *
 */
/*----------------------------------------------------------------------------*/
extern uint8_t QmeshHeartBeatGetLogValue(uint16_t value);

/*-----------------------------------------------------------------------------*
 * QmeshHeartBeatGetPublicationLogValue
 */
/*! \brief
 *
 *   This function encodes a count value into an 8-bit log value "n" such that
 *   the log value 2^(n-1) >= heart beat count value.
 *
 *  \param
 *      count - 32 bit of count value
 *
 *  \returns 8 bit log value;
 *
 */
/*----------------------------------------------------------------------------*/
extern uint8_t QmeshHeartBeatGetPublicationLogValue(uint32_t value);

#endif  /* __QMESH_HEART_BEAT_UTIL_H__ */

