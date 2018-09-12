// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdint.h>
#include <stdlib.h>
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/xlogging.h"
#include "qurt_error.h"
#include "qurt_thread.h"
#include "qurt_timer.h"

DEFINE_ENUM_STRINGS(THREADAPI_RESULT, THREADAPI_RESULT_VALUES);

#define AZ_THREAD_PRIORITY 15
#define AZ_THREAD_STACK_SIZE 4000

typedef struct THREAD_INSTANCE_TAG
{
    qurt_thread_attr_t Thread_Attribte;
    qurt_thread_t      Thread_Handle;
    THREAD_START_FUNC ThreadStartFunc;
    void* Arg;
} THREAD_INSTANCE;


static void ThreadWrapper(void* threadInstanceArg)
{
    THREAD_INSTANCE* threadInstance = (THREAD_INSTANCE*)threadInstanceArg;
    int result = threadInstance->ThreadStartFunc(threadInstance->Arg);
    return (void)(intptr_t)result;
}

THREADAPI_RESULT ThreadAPI_Create(THREAD_HANDLE* threadHandle, THREAD_START_FUNC func, void* arg)
{
    THREADAPI_RESULT result;

    if ((threadHandle == NULL) ||
        (func == NULL))
    {
        result = THREADAPI_INVALID_ARG;
        LogError("(result = %s)", ENUM_TO_STRING(THREADAPI_RESULT, result));
    }
    else
    {
        THREAD_INSTANCE* threadInstance = malloc(sizeof(THREAD_INSTANCE));
        if (threadInstance == NULL)
        {
            result = THREADAPI_NO_MEMORY;
            LogError("(result = %s)", ENUM_TO_STRING(THREADAPI_RESULT, result));
        }
        else
        {
            threadInstance->ThreadStartFunc = func;
            threadInstance->Arg = arg;
			qurt_thread_attr_init(&(threadInstance->Thread_Attribte));
			qurt_thread_attr_set_name(&(threadInstance->Thread_Attribte), "Azure Thread");
			qurt_thread_attr_set_priority(&(threadInstance->Thread_Attribte), AZ_THREAD_PRIORITY);
			qurt_thread_attr_set_stack_size(&(threadInstance->Thread_Attribte), AZ_THREAD_STACK_SIZE);
			result = qurt_thread_create(&(threadInstance->Thread_Handle), &(threadInstance->Thread_Attribte), ThreadWrapper, threadInstance/*threadInstance->ThreadStartFunc, threadInstance->Arg*/);
			
			if(result != QURT_EOK)
			{
                free(threadInstance);

                result = THREADAPI_ERROR;
                LogError("(result = %s)", ENUM_TO_STRING(THREADAPI_RESULT, result));
			} else
			{
                *threadHandle = threadInstance;
                result = THREADAPI_OK;
			}
        }
    }
    return result;
}

THREADAPI_RESULT ThreadAPI_Join(THREAD_HANDLE threadHandle, int* res)
{
    /*Not supported on QCA402X*/
	return THREADAPI_ERROR;
    
}

void ThreadAPI_Exit(int res)
{
	qurt_thread_stop();
}

void ThreadAPI_Sleep(unsigned int milliseconds)
{
	qurt_thread_sleep(qurt_timer_convert_time_to_ticks(milliseconds, QURT_TIME_MSEC));
}
