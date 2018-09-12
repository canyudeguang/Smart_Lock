// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <stdint.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"
#include "qurt_types.h"
#include "qurt_timer.h"

typedef struct TICK_COUNTER_INSTANCE_TAG
{
	int dummy : 1;
} TICK_COUNTER_INSTANCE;

TICK_COUNTER_HANDLE tickcounter_create(void)
{
    TICK_COUNTER_INSTANCE* result = (TICK_COUNTER_INSTANCE*)malloc(sizeof(TICK_COUNTER_INSTANCE));
    if (result == NULL)
    {
		/* add any per instance initialization (starting a timer for example) here if needed (most platforms will not need this) */
		
		LogError("Cannot create tick counter");
    }
	
    return result;
}

void tickcounter_destroy(TICK_COUNTER_HANDLE tick_counter)
{
    if (tick_counter != NULL)
    {
		
        free(tick_counter);
    }
}

int tickcounter_get_current_ms(TICK_COUNTER_HANDLE tick_counter, tickcounter_ms_t * current_ms)
{
    int result = 0;
	
    if (tick_counter == NULL)
    {
        LogError("tickcounter failed: Invalid Arguments.");
        result = __FAILURE__;
    }
    else
    {
		
		*current_ms = qurt_timer_convert_ticks_to_time(qurt_timer_get_ticks(), QURT_TIME_MSEC); /* converted value in milliseconds */
    }
	
    return result;
}
