/******************************************************************************
Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/

#ifndef __QMESH_DEMO_DEBUG_H__
#define __QMESH_DEMO_DEBUG_H__

#include "qcli_api.h"
#include "qmesh_demo_config.h"

/* Set this flag to 1 to enable PTS related behaviour in application. */
#ifndef APP_PTS_MODE
#define APP_PTS_MODE        (0)
#endif

#ifdef  DEBUG
#define  QCLI_LOGI(m,...) QCLI_Printf(m,__VA_ARGS__)
#else
#define  QCLI_LOGI(m,...)
#endif

#define  QCLI_LOGE(m,...) QCLI_Printf(m,__VA_ARGS__)

#endif /* __QMESH_DEMO_DEBUG_H__ */
