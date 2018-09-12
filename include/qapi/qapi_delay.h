/* Copyright (c) 2017  Qualcomm Technologies, Inc.
All rights reserved.

Confidential and Proprietary - Qualcomm Technologies, Inc.

*/

/**
 * @file qapi_delay.h
 *
 * @addtogroup qapi_delay
 * @{
 *
 * @details QAPIs to introduce delays in task execution.
 *
 * @}
 */
 
/**
 * @brief Delay the current task by the time specified in microseconds
 *
 * @param[in] time_us Specifies the delay time in microseconds
 *
 * @return void
 */
#include <stdint.h>

void qapi_Task_Delay(uint32_t time_us);
