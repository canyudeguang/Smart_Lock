/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __LOG_UTIL_H__
#define __LOG_UTIL_H__


enum {
    FAILURE = -1,
    SUCCESS = 0,
};

extern QCLI_Group_Handle_t qcli_onboard;

#define LOG_ERROR(...)                    QCLI_Printf(qcli_onboard, __VA_ARGS__)
#define LOG_WARN(...)                     QCLI_Printf(qcli_onboard, __VA_ARGS__)
#define LOG_INFO(...)                     QCLI_Printf(qcli_onboard, __VA_ARGS__)
#define LOG_VERBOSE(...)                  QCLI_Printf(qcli_onboard, __VA_ARGS__)

#endif
