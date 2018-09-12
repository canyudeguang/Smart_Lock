/******************************************************************************
Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/

#ifndef __QMESH_DEMO_MENU_H__
#define __QMESH_DEMO_MENU_H__

#include "qapi.h"
#include "qcli_api.h" 
#include "qmesh_demo_utilities.h"

extern QCLI_Command_Group_t qmesh_cmd_group;	

extern QCLI_Group_Handle_t mesh_group;       /* Handle for our main QCLI Command*/
extern QCLI_Group_Handle_t mesh_model_group; /* Handle for our main QCLI Command*/

/*****************************************************************************
 * QMesh QCLI Command groups 
 *****************************************************************************/
/* QMesh model command group */
extern QCLI_Command_Group_t qmesh_Model_group;
extern QCLI_Command_Group_t qmesh_ConfigModel_group;

/* This function is used to register the SIGMesh Command Group with QCLI */
void Initialize_QMesh_Demo(void);
#endif