/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "qcli.h"
#include "qcli_api.h"
#include "qosa_util.h"

uint32_t LogLevel = LOG_LVL_DEFAULT;

void qc_api_SetLogLevel(int32_t Level)
{
    LogLevel = (Level | LOG_LVL_AT);
}

int32_t qc_api_GetLogLevel(void)
{
    return LogLevel;
}

