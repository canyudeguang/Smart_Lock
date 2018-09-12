/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file timer.c
 * @brief Linux implementation of the timer interface.
 */

/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
* NOT A CONTRIBUTION
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "timer_platform.h"
#include "netutils.h"

bool has_timer_expired(Timer *timer) {
    uint32_t now = app_get_time(NULL);

    if (now >= timer->end_time)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

void countdown_ms(Timer *timer, uint32_t timeout) {

    uint32_t now = app_get_time(NULL);
    
    timer->end_time = now + timeout;
  
}

uint32_t left_ms(Timer *timer) {

    uint32_t now = app_get_time(NULL);
    
    return (timer->end_time - now);    
}

void countdown_sec(Timer *timer, uint32_t timeout) {

    uint32_t now = app_get_time(NULL);
    
    timer->end_time = now + (timeout * 1000);

}

void init_timer(Timer *timer) {

    timer->end_time = 0;
}

#ifdef __cplusplus
}
#endif
