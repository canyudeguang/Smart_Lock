/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdint.h>
#include <stdlib.h>

#define ENABLE_MOST_USED_FUNCTIONS_THREAD_ID_LOGGING 1

#define MAX_FUNCTION_INFO_LIST_LENGTH 10
#define MAX_THREAD_IDS_TO_RECORD 5


typedef struct thread_info_s {
    uint32_t thread_id;
    uint32_t count;
} thread_info_t;


typedef struct function_info_s {
    uint32_t index;
    uint32_t count;
#if ENABLE_MOST_USED_FUNCTIONS_THREAD_ID_LOGGING
    thread_info_t thread_info_list[MAX_THREAD_IDS_TO_RECORD];
    uint32_t      thread_info_list_length;
#endif
} function_info_t;


typedef struct cpu_profiler_ctxt_s {
    int32_t sock;
    uint32_t functions_count;
    uint32_t * function_addresses;
    uint16_t * function_samples;

    function_info_t function_info_list[MAX_FUNCTION_INFO_LIST_LENGTH];
    uint32_t function_info_list_length;

    uint32_t count_of_pcs_to_capture;
    uint32_t count_of_pcs_captured;

    uint32_t original_isr;
} cpu_profiler_ctxt_t;



void Initialize_CpuProfiler_Demo(void);
