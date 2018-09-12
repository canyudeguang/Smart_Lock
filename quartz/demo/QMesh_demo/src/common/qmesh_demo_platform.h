/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*
  * This file contains data structures required for platform specific implementation.
 */
/*****************************************************************************/
#ifndef __QMESH_DEMO_PLATFORM_H__
#define __QMESH_DEMO_PLATFORM_H__

#include "qcli_api.h"

/* Maximum number of soft timers required by the Application */
#define QMESH_APP_NUM_SOFT_TIMERS               (1)

extern int32_t InitializePlatform();
extern QMESH_TIMER_GROUP_HANDLE_T app_timer_ghdl;
extern QCLI_Group_Handle_t                             mesh_group;
#endif
