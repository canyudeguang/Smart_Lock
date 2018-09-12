// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
 
#include <stdlib.h>
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "qurt_mutex.h"

DEFINE_ENUM_STRINGS(LOCK_RESULT, LOCK_RESULT_VALUES);

LOCK_HANDLE Lock_Init(void)
{
	qurt_mutex_t *lock = malloc(sizeof(qurt_mutex_t));
	
	qurt_mutex_create(lock);
	
	return (LOCK_HANDLE)lock;
}

LOCK_RESULT Lock(LOCK_HANDLE handle)
{
    LOCK_RESULT result = LOCK_OK;
    if (handle == NULL)
    {
        result = LOCK_ERROR;
        LogError("(result = %s)", ENUM_TO_STRING(LOCK_RESULT, result));
    }
    else 
    {
		qurt_mutex_lock(handle);
    }    
    return result;
}

LOCK_RESULT Unlock(LOCK_HANDLE handle)
{
    LOCK_RESULT result = LOCK_OK;
    if (handle == NULL)
    {
        result = LOCK_ERROR;
        LogError("(result = %s)", ENUM_TO_STRING(LOCK_RESULT, result));
    }
    else
    {
		qurt_mutex_unlock(handle);
    }
    
    return result;
}

LOCK_RESULT Lock_Deinit(LOCK_HANDLE handle)
{
    LOCK_RESULT result = LOCK_OK;
    if (handle == NULL)
    {
        /*SRS_LOCK_99_007:[ This API on NULL handle passed returns LOCK_ERROR]*/
        result = LOCK_ERROR;
        LogError("(result = %s)", ENUM_TO_STRING(LOCK_RESULT, result));
    }
    else
    {
		qurt_mutex_delete(handle);
		free(handle);
    }
    return result;
}