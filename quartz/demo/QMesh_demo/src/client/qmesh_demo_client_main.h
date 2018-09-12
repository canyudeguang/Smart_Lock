/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*
  * This file contains data types and prototypes for Client node Application interfacing QCLI framework
 */
/*****************************************************************************/
#ifndef __QMESH_DEMO_CLIENT_MAIN_H__
#define __QMESH_DEMO_CLIENT_MAIN_H__

#include "qmesh_demo_utilities.h"
#include "qmesh_demo_macros.h"

extern QCLI_Command_Group_t             qmesh_cmd_group;
extern QCLI_Command_Group_t             qmesh_Model_group;

/*****************************************************************************
 * QMesh QCLI Command groups 
 *****************************************************************************/
/* Group Handle for the main Qmesh commands registered with QCLI */
extern QCLI_Group_Handle_t                             mesh_group;
/* Handle for our main QCLI Command*/
QCLI_Group_Handle_t                             mesh_model_group;
/* QMesh model command group */
extern QCLI_Command_Group_t       qmesh_ConfigModel_group;

/*----------------------------------------------------------------------------*
 * Initialize_QMesh_Client_Demo
 */
/*! \brief  Registers the Client Mesh Command Group with QCLI.
 *
 *  \param None
 *
 *  \returns Nothing.
 */
/*---------------------------------------------------------------------------*/
extern void Initialize_QMesh_Client_Demo(void);
#endif
