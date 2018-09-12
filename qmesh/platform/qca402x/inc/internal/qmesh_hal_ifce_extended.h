/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*
 *  FILE
 *      qmesh_hal_ifce_extended.h
 *
 *  DESCRIPTION
 *      This file provides definitions of OS dependent multi-threading
 *      utilities used by non-core code such as bearer, transport, serializer
 *      and applications.
 *      It provides abstraction for -
 *          threads,
 *          mutex,
 *          signals,
 *          file handling,
 *          buffer management,
 *          transport
 *
 *      Core stack code used csr_mesh_hal_ifce.h as an OS abstraction.
 *      It provides abstraction for,
 *          memory managment,
 *          mutex (does not define trylock)
 *
 *****************************************************************************/
#ifndef __QMESH_HAL_IFCE_EXTENDED_H__
#define __QMESH_HAL_IFCE_EXTENDED_H__
/*============================================================================*
 *  Standard Header Files
 *===========================================================================*/
/*============================================================================*
 *  Local Header Files
 *===========================================================================*/
/* To be defined for each platform */
#include "qmesh_extended_types.h"
#include "qmesh_gw_debug.h"
#include "qmesh_soft_timers.h"

/*============================================================================*
 *  Function Definitions
 *============================================================================*/
void  QmeshDebugHandleSet (DEBUGFUNC func, void * handle);
/* Mutex Methods */
QMESH_RESULT_T QmeshMutexTryLock (QMESH_MUTEX_T * mutex);

/* Thread Methods */
QMESH_RESULT_T QmeshThreadCreate (QMESH_THREAD_ENTRY_T entryfunc,
                          int priority, int size,
                        void * parameters, QMESH_THREADID_T * threadid);

/* Conditions */
QMESH_RESULT_T QmeshConditionCreate (QMESH_CONDITION_T * cond);
QMESH_RESULT_T QmeshConditionSet (QMESH_CONDITION_T * cond, uint32_t mask);
QMESH_RESULT_T QmeshConditionClear (QMESH_CONDITION_T * cond, uint32_t mask);
uint32_t QmeshConditionWait (QMESH_CONDITION_T * cond, uint32_t mask);
QMESH_RESULT_T QmeshConditionTimedWait (QMESH_CONDITION_T * cond, uint32_t time_in_ms,
                               uint32_t mask, uint32_t *outmask);
QMESH_RESULT_T QmeshConditionDestroy (QMESH_CONDITION_T * cond);

/* File operations */
QMESH_RESULT_T QmeshFileOpen (const char* path, int flag, QMESH_FILE_HDL_T* handle);
QMESH_RESULT_T QmeshFileSize (const char * path, uint32_t * size);
QMESH_RESULT_T QmeshFileClose (QMESH_FILE_HDL_T* handle);
QMESH_RESULT_T QmeshFileRead (QMESH_FILE_HDL_T * handle, uint32_t len, uint8_t* buffer);
QMESH_RESULT_T QmeshFileWrite (QMESH_FILE_HDL_T * handle, uint8_t* buffer, uint32_t len);
QMESH_RESULT_T QmeshFileSeek (QMESH_FILE_HDL_T* handle, int32_t offset, int whence);

/* Sleep for milliseconds */
#define QmeshSleep(ms)  do { \
                               qurt_time_t qtime;\
                               qtime = qurt_timer_convert_time_to_ticks(ms, QURT_TIME_MSEC);\
                               qurt_thread_sleep(qtime);\
                           } while (0)

QMESH_RESULT_T QmeshPipeCreate(QMESH_PIPE_T *pipeId, int32_t numOfElem, QSize elemSize);
void QmeshPipeDelete(QMESH_PIPE_T pipeId);
void QmeshPipeSend(QMESH_PIPE_T pipeId, void *data);
QMESH_RESULT_T QmeshPipeSendTimed(QMESH_PIPE_T pipeId, void *data, qurt_time_t timeout);
QMESH_RESULT_T QmeshPipeTrySend(QMESH_PIPE_T pipeId, void *data);
void QmeshPipeRecieve(QMESH_PIPE_T pipeId, void *data);
QMESH_RESULT_T QmeshPipeTryRecieve(QMESH_PIPE_T pipeId, void *data);
QMESH_RESULT_T QmeshPipeFlush(QMESH_PIPE_T pipeId);

/* Crypto APIs */
#define QMESH_ECDH_256_PUBLIC_KEYSIZE 32
#define QMESH_ECDH_256_PRIVATE_KEYSIZE 32
#define QMESH_AES_CMAC_MAC_LEN 16
#define QMESH_AES_CMAC_NONCE_LEN 13

typedef enum {
    QMESH_AES_CMAC_DIR_DECRYPT = 0,
    QMESH_AES_CMAC_DIR_ENCRYPT = 1
} QMESH_AES_CMAC_DIR_T;

typedef struct
{
    uint8_t x [QMESH_ECDH_256_PUBLIC_KEYSIZE]; /* x-coordinate */
    uint8_t y [QMESH_ECDH_256_PUBLIC_KEYSIZE]; /* y-coordinate */
} QMESH_EC256_AFFINE_PT_T;

QMESH_RESULT_T QmeshCryptoAesEcb (const uint8_t *input, const uint8_t *key,
                          uint8_t *output, QMESH_AES_CMAC_DIR_T encrypt);
QMESH_RESULT_T QmeshCryptoAesCCMEncrypt (const uint8_t *key,
                                 const uint8_t *aad, uint16_t aad_len,
                                 const uint8_t *nonce, uint8_t nonce_len,
                                 const uint8_t *input, QInt16 input_len,
                                 uint8_t* output, uint32_t output_len,
                                 uint8_t mic_len);
QMESH_RESULT_T QmeshCryptoAesCCMDecrypt (const uint8_t *key,
                                 const uint8_t *aad, uint16_t aad_len,
                                 const uint8_t *nonce, uint8_t nonce_len,
                                 const uint8_t *input, QInt16 input_len,
                                 uint8_t* output, uint8_t mic_len);
QMESH_RESULT_T QmeshCryptoAesCmac (const uint8_t *key, const uint8_t *input,
                           uint8_t length, uint8_t *mac, uint8_t maclen);

QMESH_RESULT_T QmeshCryptoEc256GenSharedSecret (uint8_t *privateKey,
                                        QMESH_EC256_AFFINE_PT_T *publicKey,
                                        QMESH_EC256_AFFINE_PT_T *sharedSecret);
QMESH_RESULT_T QmeshCryptoEc256GenKeyPair (uint8_t *privateKey,
                                   QMESH_EC256_AFFINE_PT_T *publicKey);

#endif /* __QMESH_HAL_IFCE_EXTENDED_H__ */

