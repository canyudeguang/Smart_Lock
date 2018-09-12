/*=============================================================================
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_hal_ifce.h
 *  \brief HAL interface API definitions and defines.
 *
 *   This file contains the HAL interface API definitions and defines.
 */
 /****************************************************************************/

#ifndef __QMESH_HAL_IFCE_H__
#define __QMESH_HAL_IFCE_H__

#include "qmesh_data_types.h"
#include "qmesh_result.h"
#include "qmesh_mem_mgr.h"
#include "qmesh_soft_timers.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup Platform_Interface
 * @{
 */
/*! Macro to raise an assertion */
#define QMESH_ASSERT_ENABLED (0)

#if (QMESH_ASSERT_ENABLED == 0)
#define QMESH_ASSERT(condition)
#else
#if defined (PLATFORM_QUARTZ)
#include <assert.h>
#define QMESH_ASSERT(condition) assert(condition)
#endif
#if defined (PLATFORM_CSR10XX)
#define QMESH_ASSERT(condition)
#endif
#if ((defined PLATFORM_IOS) || (defined PLATFORM_ANDROID))
#define QMESH_ASSERT(condition)
#endif
#endif /*QMESH_ASSERT_ENABLED*/

/*! Generic Macro to calculate array size */
#define QMESH_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/*!
    \name Timer Constants

    \brief Timer constants used by the core stack that needs to be defined
           as per system units
    \{
*/

/*! Millisecond multiplier for system timer unit in microseconds */
#if defined (PLATFORM_QUARTZ) || defined (PLATFORM_CSR10XX)
#define QMESH_MILLISECOND ((uint32_t)1)
#else
#define QMESH_MILLISECOND ((uint32_t)(1000))
#endif

/*! Second multiplier for system timer unit in milliseconds */
#define QMESH_SECOND ((uint32_t)(1000 * QMESH_MILLISECOND))

/*! Minute multiplier for system timer unit in milliseconds */
#define QMESH_MINUTE      ((uint32_t)(60 * QMESH_SECOND))


/*! \} */

/*! \name Memory Management
 *  \brief Functions called by the stack to access memory
 *
 *  \{
 */

#ifdef QMESH_POOL_BASED_MEM_MGR
/*! \brief Macro to allocate memory of requested size.
 *
 *  \param[in] section  Section from which memory needs to be allocated.
 *
 *  \param[in] size     Size in memory needed in bytes.
 *
 *  \returns Pointer to the memory block. NULL if not successful.
 */
#define QmeshMalloc(section, size)                                  QmeshMemMalloc(section, size)

/*! \brief Macro to allocate memory of requested size and Zero initialize it.
 *
 *  \param[in] section  Section from which memory needs to be allocated.
 *
 *  \param[in] size     Size in memory needed in bytes.
 *
 *  \returns Pointer to the memory block. NULL if not successful.
 */
#define QmeshCalloc(section, size)                                  QmeshMemCalloc(section, size)
#else
#define QmeshCoreMemPoolsInit()
#define QmeshMemMgrHeapInit(heap_base, heap_size)                   QMESH_RESULT_SUCCESS
#define QmeshMemPoolsConfigure(section,pool_configs,no_pools)       QMESH_RESULT_SUCCESS

/*! \brief Macro to allocate memory of requested size.
 *
 *  \param[in] section  Unused when QMESH_POOL_BASED_MEM_MGR is disabled.
 *
 *  \param[in] size     Size in memory needed in bytes.
 *
 *  \returns Pointer to the memory block. NULL if not successful.
 */
#define QmeshMalloc(section, size)                                  QmeshMemMalloc(size)

/*! \brief Macro to allocate memory of requested size and Zero initialize it.
 *
 *  \param[in] section  Unused when QMESH_POOL_BASED_MEM_MGR is disabled.
 *
 *  \param[in] size     Size in memory needed in bytes.
 *
 *  \returns Pointer to the memory block. NULL if not successful.
 */
#define QmeshCalloc(section, size)                                  QmeshMemCalloc(size)
#endif /* QMESH_POOL_BASED_MEM_MGR */

/* Compile-time assertions: force the compilation to abort if the
 * expression is false.  Unlike #if, this can test enums.
 */
#ifndef COMPILE_TIME_ASSERT
#define COMPILE_TIME_ASSERT(expr, msg) struct compile_time_assert_ ## msg { \
    int compile_time_assert_ ## msg [1 - (!(expr))*2]; \
}
#endif


/*----------------------------------------------------------------------------*
 * QmeshMemSet
 */
/*! \brief This function fills a block of memory with the given value
 *
 *  \param[in] dest Pointer to the destination buffer
 *  \param[in] value  Value to be filled in the destination buffer
 *  \param[in] count  Size of the destination buffer
 *
 *  \returns Original pointer to the destination buffer
 */
/*---------------------------------------------------------------------------*/
void *QmeshMemSet(void *dest, uint8_t value, size_t count);

/*----------------------------------------------------------------------------*
 * QmeshMemSet16
 */
/*! \brief This function fills a block of memory with the given value. This
 *         is same as QmeshMemSet except that the count represents the count
 *         of uint16_t size units.
 *
 *  \param[in] dest Pointer to the destination buffer
 *  \param[in] value  Value to be filled in the destination buffer
 *  \param[in] count  Size of the destination buffer
 *
 *  \returns Original pointer to the destination buffer
 */
/*---------------------------------------------------------------------------*/
void *QmeshMemSet16(void *dest, uint16_t value, size_t count);

/*----------------------------------------------------------------------------*
 * QmeshMemChr
 */
/*! \brief Find the first occurrence of 'value' in an array of given size.
 *
 *  \param[in] buff Pointer to the array
 *  \param[in] value Value to be searched
 *  \param[in] length Size of the array
 *
 *  \returns Pointer to the memory address where the value was found. NULL otherwise
 */
/*---------------------------------------------------------------------------*/
const void *QmeshMemChr(const void * buff, int8_t value, size_t length);

/*----------------------------------------------------------------------------*
 * QmeshMemCmp
 */
/*! \brief Compare two arrays of memory.
 *
 *  \param[in] buf1 Pointer to the first buffer
 *  \param[in] buf2 Pointer to the second buffer
 *  \param[in] count Number of bytes to compare
 *
 *  \returns -1 if the first array is "less than" the second in the usual string
               comparison sense, +1 if the first is "greater than" the second, and 0 if they are equal.
 */
/*---------------------------------------------------------------------------*/
int32_t QmeshMemCmp(const void *buf1, const void *buf2, size_t count);

/*----------------------------------------------------------------------------*
 * QmeshMemCpy
 */
/*! \brief Find the first occurrence of 'value' in an array of given size.
 *
 *  \param[in] dest Pointer to the destination buffer
 *  \param[in] src  Pointer to source buffer
 *  \param[in] count Number of bytes to copy
 *
 *  \returns Original pointer to the destination buffer
 */
/*---------------------------------------------------------------------------*/
void *QmeshMemCpy(void *dest, const void *src, size_t count);

/*----------------------------------------------------------------------------*
 * QmeshMemCpy16
 */
/*! \brief Copy memory. This
 *         is same as QmeshMemCpy except that the length represents the count
 *         of uint16_t size units.
 *
 *  \param[in] dest Pointer to the destination buffer
 *  \param[in] src  Pointer to source buffer
 *  \param[in] length Length of the buffer
 *
 *  \returns Original pointer to the destination buffer
 */
/*---------------------------------------------------------------------------*/
void *QmeshMemCpy16(void *dest, const void *src, size_t length);

/*----------------------------------------------------------------------------*
 * QmeshMemCopyPack
 */
/*! \brief Copy memory for a 8 bit buffer into a uint16_t array .
 *         It copies memory, from 2 * uint8_t into uint16_t. Like MemCopy but the
 *         source is a uint8_t array and the destination is a uint16_t array.
 *
 *  \param[in] dest Pointer to the destination buffer
 *  \param[in] src  Pointer to the source buffer
 *  \param[in] count Number of bytes to copy
 *
 *  \returns None
 */
/*---------------------------------------------------------------------------*/
void QmeshMemCopyPack(uint16_t *dest, const uint8_t *src, size_t count);

/*----------------------------------------------------------------------------*
 * QmeshMemCopyUnPack
 */
/*! \brief Copy memory, turning uint16 into 2 * uint8. Like MemCopy but the source
 *         is a uint16 array and the destination is a uint8_t array.
 *
 *  \param[in] dest Pointer to the destination buffer
 *  \param[in] src  Pointer to the source buffer
 *  \param[in] count  Number of words to copy
 *
 *  \returns None
 */
/*---------------------------------------------------------------------------*/
void QmeshMemCopyUnPack(uint8_t *dest, const uint16_t *src, size_t count);

/*----------------------------------------------------------------------------*
 * QmeshMemCopyPackBE
 */
/*! \brief Copies byte array into a uint16_t array in big endian format.
 *         This is same as QmeshMemCpy on platforms which has 8 bit addressable
 *         memory. But it packs the data from 16-bit addressable bytes into
 *         words.
 *
 *  \param[in] dest Pointer to the destination buffer
 *  \param[in] src  Pointer to the source buffer
 *  \param[in] count Number of bytes to copy
 *
 *  \returns None
 */
/*---------------------------------------------------------------------------*/
void QmeshMemCopyPackBE(uint16_t *dest, const uint8_t *src, size_t count);

/*----------------------------------------------------------------------------*
 * QmeshMemCopyUnPackBE
 */
/*! \brief Copy memory, turning uint16_t into 2 * uint8_t in big endian format. Like
 *         MemCopy but the source is a uint16_t array and the destination is a uint8_t
 *         array.
 *
 *  \param[in] dest Pointer to the destination buffer
 *  \param[in] src  Pointer to the source buffer
 *  \param[in] count  Length of the uint8_t Array
 *
 *  \returns None
 */
/*---------------------------------------------------------------------------*/
void QmeshMemCopyUnPackBE(uint8_t *dest, const uint16_t *src, size_t count);

/*----------------------------------------------------------------------------*
 * QmeshBufReadUint8
 */
/*! \brief Returns a uint8_t data from a given buffer pointer and advances
 *         the pointer by 1
 *
 *  \param[in] buf Pointer to the buffer address.
 *
 *  \returns Contents of the buffer
 */
/*---------------------------------------------------------------------------*/
uint8_t QmeshBufReadUint8(uint8_t **buf);

/*----------------------------------------------------------------------------*
 * QmeshBufReadUint16
 */
/*! \brief Returns a uint8_t value at the given address and advances
 *         the pointer by 2 bytes
 *
 *  \param[in] buf Pointer to the buffer address.
 *
 *  \returns uint16_t
 */
/*---------------------------------------------------------------------------*/
uint16_t QmeshBufReadUint16(uint8_t **buf);

/*----------------------------------------------------------------------------*
 * QmeshBufReadUint32
 */
/*! \brief Reads 4 bytes little endian value from the buf pointer and
           advances the pointer by 4 bytes
 *
 *  \param[in] buf Pointer to the buffer address.
 *
 *  \returns Contents of the buffer
 */
/*---------------------------------------------------------------------------*/
uint32_t QmeshBufReadUint32(uint8_t **buf);

/*----------------------------------------------------------------------------*
 * QmeshBufWriteUint8
 */
/*! \brief Writes a uint8_t value at the given address and advances
 *         the pointer by 1 bytes
 *
 *  \param[in] buf Pointer to the buffer address.
 *  \param[in] val Value to be written.
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
void QmeshBufWriteUint8(uint8_t **buf, uint8_t val);

/*----------------------------------------------------------------------------*
 * QmeshBufWriteUint16
 */
/*! \brief Writes a uint16_t value at the given address in little endian order
 *         and advances the pointer by 2 bytes
 *
 *  \param[in] buf Pointer to the buffer address.
 *  \param[in] val Value to be written.
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
void QmeshBufWriteUint16(uint8_t **buf, uint16_t val);


/*----------------------------------------------------------------------------*
 * QmeshBufWriteUint32
 */
/*! \brief Writes a uint32_t value at the given address in little endian order
 *         and advances the pointer by 4 bytes
 *
 *  \param[in] buf Pointer to the buffer address.
 *  \param[in] val Value to be written.
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
void QmeshBufWriteUint32(uint8_t **buf, uint32_t *val);

/*! \} */



/*! \name Memory Allocation
 *
 * \{
 */

#ifdef QMESH_POOL_BASED_MEM_MGR
/*----------------------------------------------------------------------------*
 * QmeshMemMgrHeapInit
 */
/*! \brief Uses the Memory Block as heap for various operations.
 *         Post this call, it is mandatory to call \ref QmeshMemPoolsConfigure
 *         for configuring pools.
 *
 *  \param[in] heap_base Pointer to the heap block. This shall be as big
 *             as requirement of all memory section pools along with pool
 *             mamangement header requirement.
 *  \param[in] heap_size Size of heap memory.
 *
 *  \returns QMESH_RESULT_SUCCESS if successful else returns QMESH_RESULT_FAILURE
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshMemMgrHeapInit(void *heap_base, size_t heap_size);

/*----------------------------------------------------------------------------*
 * QmeshMemPoolsConfigure
 */
/*! \brief Configures the pool size for a particular section. Section once
 *         configured, can't be configured again. This function shall be
 *         called after calling \ref QmeshMemMgrHeapInit. It is mandatory
 *         to configure below memory sections before calling \ref QmeshInit:
 *         - \ref QMESH_MM_SECTION_CORE_LAYER
 *           (See \ref QMESH_CORE_STACK_POOL_CONFIG_TABLE for pool configuration)
 *         - \ref QMESH_MM_SECTION_CORE_DB
 *           (See \ref QMESH_CORE_DB_POOL_CONFIG_TABLE for pool configuration)
 *         - \ref QMESH_MM_SECTION_CORE_UTILS
 *           (See \ref QMESH_UTILS_POOL_CONFIG_TABLE for pool configuration)
 *
 *         In addition application can configure pool for its own use with
 *         memory section \ref QMESH_MM_SECTION_APP.
 *         Models will use \ref QMESH_MM_SECTION_MODEL_LAYER and will be
 *         configured in \ref QmeshModelInitMsgCache.
 *
 *  \param[in] section      Section to be configured
 *  \param[in] pool_configs Pool configuration information
 *  \param[in] no_pools     Total number of pools
 *
 *  \returns QMESH_RESULT_SUCCESS if successful else returns QMESH_RESULT_FAILURE
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshMemPoolsConfigure(QMESH_MM_SECTION_T section,
                                             const QMESH_MM_POOL_CONFIG_T *pool_configs,
                                             uint16 no_pools);
/*----------------------------------------------------------------------------*
 * QmeshMemMalloc
 */
/*! \brief Allocate a memory block of a given size.
 *
 *  \note1hang This function shall be ported to return a naturally aligned
 *             block of memory on that platform. For example, on a 32-bit
 *             machine allocated memory shall be 4-Byte aligned and
 *             8-Byte aligned on a 64-bit machine.
 *
 *  \warning   Applications shall use \ref QmeshMalloc macro instead of directly
 *             using the porting function.
 *
 *  \param[in] section section to be used
 *  \param[in] size Size of requested memory
 *
 *  \returns Pointer to the allocated memory block
 */
/*---------------------------------------------------------------------------*/
extern void* QmeshMemMalloc(QMESH_MM_SECTION_T section, size_t size);

/*----------------------------------------------------------------------------*
 * QmeshMemCalloc
 */
/*! \brief Allocate a zero initialized memory block of a given size
 *
 *  \note1hang This function shall be ported to return a naturally aligned
 *             block of memory on that platform. For example, on a 32-bit
 *             machine allocated memory shall be 4-Byte aligned and
 *             8-Byte aligned on a 64-bit machine.
 *
 *  \warning   Applications shall use \ref QmeshCalloc macro instead of directly
 *             using the porting function.
 *
 *  \param[in] size Size of requested memory
 *
 *  \returns Pointer to the allocated memory block
 */
/*---------------------------------------------------------------------------*/
extern void* QmeshMemCalloc(QMESH_MM_SECTION_T section, size_t size);


#else
/*----------------------------------------------------------------------------*
 * QmeshMemMalloc
 */
/*! \brief Allocate a memory block of a given size
 *
 *  \note1hang This function shall be ported to return a naturally aligned
 *             block of memory on that platform. For example, on a 32-bit
 *             machine allocated memory shall be 4-Byte aligned and
 *             8-Byte aligned on a 64-bit machine.
 *
 *  \warning   Applications shall use \ref QmeshMalloc macro instead of directly
 *             using the porting function.
 *
 *  \param[in] size Size of requested memory
 *
 *  \returns Pointer to the allocated memory block
 */
/*---------------------------------------------------------------------------*/
extern void* QmeshMemMalloc(size_t size);

/*----------------------------------------------------------------------------*
 * QmeshMemCalloc
 */
/*! \brief Allocate a zero initialized memory block of a given size
 *
 *  \note1hang This function shall be ported to return a naturally aligned
 *             block of memory on that platform. For example, on a 32-bit
 *             machine allocated memory shall be 4-Byte aligned and
 *             8-Byte aligned on a 64-bit machine.
 *
 *  \warning   Applications shall use \ref QmeshCalloc macro instead of directly
 *             using the porting function.
 *
 *  \param[in] size Size of requested memory
 *
 *  \returns Pointer to the allocated memory block
 */
/*---------------------------------------------------------------------------*/
extern void* QmeshMemCalloc(size_t size);

#endif

/*----------------------------------------------------------------------------*
 * QmeshFree
 */
/*! \brief Free the allocated memory
 *
 *  \param[in] mem_ptr Pointer to a memory buffer
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
extern void QmeshFree(void *mem_ptr);

/*! \} */


/*! \name Random Number Generation
 *
 * \{
 */
/*----------------------------------------------------------------------------*
 * QmeshRandom16
 */
/*! \brief Returns a 16-bit random number
 *
 *  \returns Random number
 */
/*---------------------------------------------------------------------------*/
uint16_t QmeshRandom16(void);


/*! \name Random Number Generation
 *
 * \{
 */
/*----------------------------------------------------------------------------*
 * QmeshRandom32
 */
/*! \brief Returns a 32-bit random number
 *
 *  \returns Random number
 */
/*---------------------------------------------------------------------------*/
uint32_t QmeshRandom32(void);

/*! \} */

/*! \name Timer API
 *
 *  \{
 */

/*----------------------------------------------------------------------------*
 * QmeshGetCurrentTime
 */
/*! \brief Returns the current system time in microseconds.
 *
 *         The time is represented as a 48 bit value stored in a 3 uint16_t
 *         array
 *
 *  \param[out] current_time [output] Current system time in microseconds.
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
void QmeshGetCurrentTime(uint16_t *current_time);

/*----------------------------------------------------------------------------*
 * QmeshGetCurrentTimeInMs
 */
/*! \brief Returns the current system time in milliseconds.
 *
 *         The time is represented as a 48 bit value stored in a 3 uint16_t
 *         array
 *
 *  \param[out] current_time [output] Current system time in milliseconds.
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
void QmeshGetCurrentTimeInMs (uint16_t *current_time);

/*----------------------------------------------------------------------------*
 * QmeshTimeSub
 */
/*! \brief Returns the difference between 2 48bit time values.
 *
 *         The time is represented as a 48 bit value stored in a uint16_t[3]
 *         array in little endian order
 *
 *  \param diff Difference in time in microseconds
 *  \param[in] t1   First time stamp in mic
 *  \param[in] t2   Second time stamp
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
void QmeshTimeSub(uint16_t *diff, uint16_t *t1, uint16_t *t2);

/*----------------------------------------------------------------------------*
 * QmeshTimeAdd
 */
/*! \brief Returns the sum of 2 48bit time values.
 *
 *         The time is represented as a 48 bit value stored in a uint16_t[3]
 *         array in little endian order
 *
 *  \param sum Sum of time in microseconds
 *  \param[in] t1   First time stamp
 *  \param[in] t2   Second time stamp
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
void QmeshTimeAdd(uint16_t *sum, uint16_t *t1, uint16_t *t2);

/*----------------------------------------------------------------------------*
 * QmeshTimeCmp
 */
/*! \brief Indicates if the timer with the timerHandle is running and valid
 *
 *  \param[in] t1   First time stamp
 *  \param[in] t2   Second time stamp
 *
 *  \returns TRUE if they are same
 */
/*---------------------------------------------------------------------------*/
bool QmeshTimeCmp(uint16_t *t1, uint16_t *t2);

/*----------------------------------------------------------------------------*
 * QmeshGetCurrentTimeInSecs
 */
/*! \brief Gets the current system time in seconds.
 *
 *  \returns Current time in seconds.
 */
/*---------------------------------------------------------------------------*/
uint32_t QmeshGetCurrentTimeInSecs(void);
/* \} */

/*----------------------------------------------------------------------------*
 * QmeshTimeMicroToSec
 */
/*! \brief Takes time in microseconds and returns in seconds.
 *
 *  \returns time in seconds.
 */
/*---------------------------------------------------------------------------*/
uint32_t QmeshTimeMicroToSec(uint16_t *t1);


/*! \name Thread Synchronization
 *
 *  \{
 */
#ifdef PLATFORM_MULTITHREAD_SUPPORT

/*----------------------------------------------------------------------------*
 * QmeshMutexCreate
 */
/*! \brief Creates a mutex and returns the handle to the mutex object
 *
 *  \returns The handle to the mutex
 */
/*---------------------------------------------------------------------------*/
QmeshResult QmeshMutexCreate(QMESH_MUTEX_T *p_mutex);

/*----------------------------------------------------------------------------*
 * QmeshMutexLock
 */
/*! \brief The mutex object referenced by mutex shall be locked by the calling
 *   thread. If the mutex is already locked, the calling thread shall block until
 *   the mutex becomes available. This operation shall return with the mutex object
 *   referenced by p_mutex in the locked state with the calling thread as its
 *   owner
 *
 *  \param[in] p_mutex Pointer to the mutex object
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
void QmeshMutexLock(QMESH_MUTEX_T *p_mutex);

/*----------------------------------------------------------------------------*
 * QmeshMutexUnlock
 */
/*! \brief This function shall release the mutex object referenced by p_mutex.
 *   If there are threads blocked on the p_mutex object, this function shall
 *   result in the mutex becoming available. Which thread will acquire the
 *   mutex is not defined by this function
 *
 *  \param[in] p_mutex Pointer to the mutex object
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
void QmeshMutexUnlock(QMESH_MUTEX_T *p_mutex);

/*----------------------------------------------------------------------------*
 * QmeshMutexDestroy
 */
/*! \brief This function shall destroy the mutex object created by mutex create.
 *
 *  \param[in] p_mutex Pointer to the mutex object
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
void QmeshMutexDestroy(QMESH_MUTEX_T *p_mutex);
#else

#define QmeshMutexCreate(mutex)  QMESH_RESULT_SUCCESS
#define QmeshMutexLock(mutex)
#define QmeshMutexUnlock(mutex)
#define QmeshMutexDestroy(mutex)

#endif /* PLATFORM_MULTITHREAD_SUPPORT */

/* \} */

#ifdef __cplusplus
}
#endif

/*!@} */
#endif /* QMESH_HAL_IFCE_H__ */

