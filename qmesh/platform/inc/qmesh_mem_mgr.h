/*=============================================================================
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_mem_mgr.h
 *  \brief Data types used by memory manager
 *
 *   This file contains the different data types used for the memory manager
 */
 /****************************************************************************/
#ifndef __QMESH_MEM_MGR_H__
#define __QMESH_MEM_MGR_H__

#include "qmesh_data_types.h"
#include "qmesh_soft_timers.h"

#ifdef QMESH_POOL_BASED_MEM_MGR

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
    Macro Definitions
 *============================================================================*/

/*! \brief Total number of device instance supported.
 *         Changing this will result in change in global memory requirement
 */
#define QMESH_TOTAL_DEVICE_INSTANCES            (1u)

/*! \brief Total maximum number of subnets with a device instance supported.
 *         Changing this will result in change in global memory requirement
 */
#define QMESH_MAX_SUBNETS_PER_DEVICE            (4u)

/*! \brief Total maximum number of application keys with a device instance supported.
 *         Changing this will result in change in global memory requirement
 */
#define QMESH_MAX_APPKEYS_PER_DEVICE            (8u)

/*! \brief Defines pool entry. See \ref QMESH_MM_POOL_CONFIG_T */
#define QMESH_MM_POOLS_ENTRY(pool_size,number)  {pool_size, number},

/*! \brief Memory size required for each pool entry */
#define QMESH_MM_POOL_ENTRY_SIZE                (5u)

/*! \brief Calculates total buffer size required for given pool requirement.
 *         total_pool_mem_size: Total memory required for all pools
 *         pool_entry_count: Total pool entry count
 *         total_num_pools: Total number of pools in all pool entry
 */
#define QMESH_MM_BUFFER_SIZE(total_pool_mem_size, pool_entry_count, total_num_pools) \
    ((total_pool_mem_size) + ((pool_entry_count) * QMESH_MM_POOL_ENTRY_SIZE))

/*! \brief Macros to generate pool memory size. */
#define QMESH_POOL_MEM_SIZE(pool_size, pool_count)      ((pool_count) * (pool_size)) +
#define QMESH_POOL_MEM_SIZE_END()                       0

/*! \brief Macros to generate pool count. */
#define QMESH_POOL_COUNT(pool_size, pool_count)         (pool_count) +
#define QMESH_POOL_COUNT_END()                          0

/*! \brief Macros to generate pool table entry count. */
#define QMESH_TOTAL_POOL_ENTRY_COUNT(pool_size, pool_count)   1 +
#define QMESH_TOTAL_POOL_ENTRY_COUNT_END()                    0

/*! \brief Macros to generate pool table entry. */
#define QMESH_POOL_CONFIG_TABLE_ENTRY(pool_size, pool_count) QMESH_MM_POOLS_ENTRY(pool_size, pool_count)
#define QMESH_POOL_CONFIG_TABLE_ENTRY_END()

/*! \brief Pool config table required for \ref QMESH_MM_SECTION_CORE_LAYER.
 *         This value is critical for correct system startup and should
 *         only be changed on advice from Qualcomm.
 */
#define QMESH_CORE_STACK_POOL_CONFIG_TABLE(X, END)  \
    X(  8,  2)                                      \
    X( 16,  2)                                      \
    X( 19, 32)                                      \
    X( 29,  2)                                      \
    X( 36,  2)                                      \
    X( 40,  2)                                      \
    X( 69,  1)                                      \
    X( 90,  1)                                      \
    X(384,  1)                                      \
    X(480,  1)                                      \
    END()

#define QMESH_CORE_STACK_TOTAL_POOL_SIZE    QMESH_CORE_STACK_POOL_CONFIG_TABLE(QMESH_POOL_MEM_SIZE, QMESH_POOL_MEM_SIZE_END)
#define QMESH_CORE_STACK_TOTAL_POOLS        QMESH_CORE_STACK_POOL_CONFIG_TABLE(QMESH_POOL_COUNT, QMESH_POOL_COUNT_END)
#define QMESH_CORE_STACK_TOTAL_POOL_ENTRY   QMESH_CORE_STACK_POOL_CONFIG_TABLE(QMESH_TOTAL_POOL_ENTRY_COUNT, QMESH_TOTAL_POOL_ENTRY_COUNT_END)
#define QMESH_CORE_STACK_POOL_BUFFER_SIZE   QMESH_MM_BUFFER_SIZE(QMESH_CORE_STACK_TOTAL_POOL_SIZE,  \
                                                                 QMESH_CORE_STACK_TOTAL_POOL_ENTRY, \
                                                                 QMESH_CORE_STACK_TOTAL_POOLS)

/*! \brief Memory size macros used in pool config table for \ref QMESH_MM_SECTION_CORE_DB.
 *         This value is critical for correct system startup and should
 *         only be changed on advice from Qualcomm.
 */
#define QMESH_DEVICE_INSTANCE_LIST_MEM_SIZE         (0x0001u * sizeof(uint16_t))
#define QMESH_DEVICE_INSTANCE_MEM_SIZE              (0x0004u * sizeof(uint16_t))
#define QMESH_NETWORK_INFO_MEM_SIZE                 (0x0026u * sizeof(uint16_t))
#define QMESH_SUBNET_INFO_LIST_MEM_SIZE             (0x0001u * sizeof(uint16_t) * QMESH_MAX_SUBNETS_PER_DEVICE)
#define QMESH_SUBNET_INFO_MEM_SIZE                  (0x0008u * sizeof(uint16_t))
#define QMESH_SUBNET_KEY_INFO_MEM_SIZE              (0x0015u * sizeof(uint16_t))
#define QMESH_SUBNET_NID_INFO_MEM_SIZE              (0x0014u * sizeof(uint16_t))
#define QMESH_APPKEY_INFO_LIST_MEM_SIZE             (0x0001u * sizeof(uint16_t) * QMESH_MAX_APPKEYS_PER_DEVICE)
#define QMESH_APPKEY_INFO_MEM_SIZE                  (0x0004u * sizeof(uint16_t))
#define QMESH_APPKEY_AID_INFO_MEM_SIZE              (0x0009u * sizeof(uint16_t))
#define QMESH_KEYS_PER_ENTRY                        (0x0002u) /* Current and new key (key refresh phase) */

/*! \brief Pool config table required for \ref QMESH_MM_SECTION_CORE_DB.
 *         This value is critical for correct system startup and should
 *         only be changed on advice from Qualcomm.
 */
#define QMESH_CORE_DB_POOL_CONFIG_TABLE(X, END)                                 \
    X(QMESH_DEVICE_INSTANCE_LIST_MEM_SIZE,      QMESH_TOTAL_DEVICE_INSTANCES)   \
    X(QMESH_DEVICE_INSTANCE_MEM_SIZE,           QMESH_TOTAL_DEVICE_INSTANCES)   \
    X(QMESH_NETWORK_INFO_MEM_SIZE,              QMESH_TOTAL_DEVICE_INSTANCES)   \
    X(QMESH_SUBNET_INFO_LIST_MEM_SIZE,          QMESH_TOTAL_DEVICE_INSTANCES)   \
    X(QMESH_SUBNET_INFO_MEM_SIZE,               QMESH_TOTAL_DEVICE_INSTANCES *  \
                                                QMESH_MAX_SUBNETS_PER_DEVICE)   \
    X(QMESH_SUBNET_KEY_INFO_MEM_SIZE,           QMESH_TOTAL_DEVICE_INSTANCES *  \
                                                QMESH_MAX_SUBNETS_PER_DEVICE *  \
                                                QMESH_KEYS_PER_ENTRY)           \
    X(QMESH_SUBNET_NID_INFO_MEM_SIZE,           QMESH_TOTAL_DEVICE_INSTANCES *  \
                                                QMESH_MAX_SUBNETS_PER_DEVICE *  \
                                                QMESH_KEYS_PER_ENTRY)           \
    X(QMESH_APPKEY_INFO_LIST_MEM_SIZE,          QMESH_TOTAL_DEVICE_INSTANCES)   \
    X(QMESH_APPKEY_INFO_MEM_SIZE,               QMESH_TOTAL_DEVICE_INSTANCES *  \
                                                QMESH_MAX_APPKEYS_PER_DEVICE)   \
    X(QMESH_APPKEY_AID_INFO_MEM_SIZE,           QMESH_TOTAL_DEVICE_INSTANCES *  \
                                                QMESH_MAX_APPKEYS_PER_DEVICE *  \
                                                QMESH_KEYS_PER_ENTRY)           \
    END()

#define QMESH_CORE_DB_TOTAL_POOL_SIZE       QMESH_CORE_DB_POOL_CONFIG_TABLE(QMESH_POOL_MEM_SIZE, QMESH_POOL_MEM_SIZE_END)
#define QMESH_CORE_DB_TOTAL_POOLS           QMESH_CORE_DB_POOL_CONFIG_TABLE(QMESH_POOL_COUNT, QMESH_POOL_COUNT_END)
#define QMESH_CORE_DB_TOTAL_POOL_ENTRY      QMESH_CORE_DB_POOL_CONFIG_TABLE(QMESH_TOTAL_POOL_ENTRY_COUNT, QMESH_TOTAL_POOL_ENTRY_COUNT_END)
#define QMESH_CORE_DB_POOL_BUFFER_SIZE      QMESH_MM_BUFFER_SIZE(QMESH_CORE_DB_TOTAL_POOL_SIZE,     \
                                                                 QMESH_CORE_DB_TOTAL_POOL_ENTRY,    \
                                                                 QMESH_CORE_DB_TOTAL_POOLS)

/*! \brief Memory size macros used in pool config table for \ref QMESH_MM_SECTION_UTILS_LAYER. */
#define QMESH_UTILS_PER_TIMER_SIZE                  (8u)
#define QMESH_UTILS_PER_TIMER_GROUP_SIZE            (5u)

/*! \brief Pool config table required for \ref QMESH_MM_SECTION_UTILS_LAYER. */
#define QMESH_UTILS_POOL_CONFIG_TABLE(X, END)                   \
    X(QMESH_UTILS_PER_TIMER_GROUP_SIZE,     MAX_TIMER_GROUPS)   \
    X(QMESH_UTILS_PER_TIMER_SIZE,           MAX_VIRTUAL_TIMERS) \
    END()

#define QMESH_UTILS_TOTAL_POOL_SIZE         QMESH_UTILS_POOL_CONFIG_TABLE(QMESH_POOL_MEM_SIZE, QMESH_POOL_MEM_SIZE_END)
#define QMESH_UTILS_TOTAL_POOLS             QMESH_UTILS_POOL_CONFIG_TABLE(QMESH_POOL_COUNT, QMESH_POOL_COUNT_END)
#define QMESH_UTILS_TOTAL_POOL_ENTRY        QMESH_UTILS_POOL_CONFIG_TABLE(QMESH_TOTAL_POOL_ENTRY_COUNT, QMESH_TOTAL_POOL_ENTRY_COUNT_END)
#define QMESH_UTILS_POOL_BUFFER_SIZE        QMESH_MM_BUFFER_SIZE(QMESH_UTILS_TOTAL_POOL_SIZE,   \
                                                                 QMESH_UTILS_TOTAL_POOL_ENTRY,  \
                                                                 QMESH_UTILS_TOTAL_POOLS)

#define QMESH_CORE_POOLS_BUFFER_SIZE       (QMESH_CORE_STACK_POOL_BUFFER_SIZE + \
                                            QMESH_CORE_DB_POOL_BUFFER_SIZE +    \
                                            QMESH_UTILS_POOL_BUFFER_SIZE)


/*============================================================================*
    Public Data Type Definitions
 *============================================================================*/
/*! \brief Structure specifying the configuration of a particular pool. */
typedef struct
{
    size_t buf_size;   /*!< Size in bytes of each pool buffer. */
    size_t buf_count;  /*!< Number of buffers within the pool. */
} QMESH_MM_POOL_CONFIG_T;

/*! \brief Enumeration with the different memory sections available. */
typedef enum
{
    QMESH_MM_SECTION_CORE_LAYER,    /*!< QMesh Core memory section. */
    QMESH_MM_SECTION_CORE_DB,       /*!< QMesh Core DB memory section. */
    QMESH_MM_SECTION_UTILS_LAYER,   /*!< QMesh Utility memory section. */
    QMESH_MM_SECTION_MODEL_LAYER,   /*!< QMesh Model layer memory section. */
    QMESH_MM_SECTION_APP,           /*!< Application specific memory section. */
    QMESH_MAX_SECTIONS
} QMESH_MM_SECTION_T;

#ifdef __cplusplus
}
#endif

#endif /* QMESH_POOL_BASED_MEM_MGR */

#endif /* __QMESH_MEM_MGR_H__ */

