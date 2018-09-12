/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*
  * This file contains the platform specific routines to initialize the platfrom and BT controller.
 */
/*****************************************************************************/
#include "qmesh.h"
#include "qmesh_ble_gap.h"
#include "qapi_ble_hcitypes.h"
#include "qmesh_demo_utilities.h"
#include "qmesh_soft_timers.h"
#include "qmesh_demo_platform.h"
#include "qmesh_gw_debug.h"

/* Timer handle group */
QMESH_TIMER_GROUP_HANDLE_T app_timer_ghdl = QMESH_TIMER_INVALID_GROUP_HANDLE;
extern QCLI_Group_Handle_t                             mesh_group;

/******************************************************************************
 * Local data
 *****************************************************************************/
/* The HCI Driver structure that holds the HCI Transports settings used to initialize Bluetopia */


/******************************************************************************
 * Initialize the Timer and BT controller and returns BT stack handle
 *****************************************************************************/
int32_t InitializePlatform()
{
    int32_t bt_stack_id = -1;

    /* Enable the debug logs */
    QmeshDebugHandleSet (&QCLI_Printf, mesh_group);

    /* Initialize bluetooth */
    if (!InitializeBluetooth())
    {
       QCLI_LOGE (mesh_group, "Bluetooth initialization failed!!\n");
       return -1;
    }

    bt_stack_id = GetBluetoothStackId();
    QCLI_LOGE (mesh_group, "Platform Initialized\n");
    return bt_stack_id;
}
