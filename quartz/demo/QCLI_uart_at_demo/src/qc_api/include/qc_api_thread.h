/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*-------------------------------------------------------------------------
 *  Include Files
 *-----------------------------------------------------------------------*/
#ifndef __QC_API_THREAD_H
#define __QC_API_THREAD_H

#include <stdarg.h>

#include "malloc.h"
#include "string.h"
#include "stringl.h"
#include "qurt_timer.h"

#include "qapi_twn.h"
#include "qcli_api.h"
#include "qcli_util.h"
#include "qapi_socket.h"
#include "qapi_ns_utils.h"


void Print_Network_State_Event(qapi_TWN_Network_State_t State);
void Print_Joiner_Result_Event(qapi_TWN_Joiner_Result_t Result);
void DisplayNetworkInfo(void);
void TWN_Event_CB(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Event_t *TWN_Event, uint32_t CB_Param);

QCLI_Command_Status_t qc_api_th_Initialize(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_SetPollPeriod(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_Shutdown(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t qc_api_th_Start(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_Stop(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t qc_api_th_GetDeviceConfiguration(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_GetNetworkConfiguration(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_SetExtendedAddress(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_SetChildTimeout(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_SetLinkMode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_SetChannel(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_SetPANID(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_SetExtendedPANID(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_SetNetworkName(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_SetMasterKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t qc_api_th_MeshCoP_CommissionerStart(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_MeshCoP_CommissionerStop(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_MeshCoP_CommissionerAddJoiner(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_MeshCoP_CommissionerDelJoiner(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t qc_api_th_MeshCoP_JoinerStart(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_MeshCoP_JoinerStop(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t qc_api_th_AddBorderRouter(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_RemoveBorderRouter(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_AddExternalRoute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_RemoveExternalRoute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_RegisterServerData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t qc_api_th_Thread_UseDefaultInfo(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t qc_api_th_Thread_StartBorderAgent(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_Thread_StopBorderAgent(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t qc_api_th_Thread_UpdatePSKc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t qc_api_th_Thread_ClearPersist(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t qc_api_th_Thread_BecomeRouter(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_Thread_BecomeLeader(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t qc_api_th_Thread_SetIPStackInteg(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_Thread_AddUnicastAddress(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_Thread_RemoveUnicastAddress(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_Thread_SubscribeMulticast(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_Thread_UnsubscribeMulticast(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_th_Thread_SetPingEnabled(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t qc_api_th_Thread_SetProvisioningUrl(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

#ifdef ALLOW_OPENTHREAD_DEBUG_LOGS
QCLI_Command_Status_t qc_api_th_Thread_SetLogging(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
#endif

void DisplayNetworkInfo(void);

#endif // _THREAD_API_H_
