/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "malloc.h"
#include "string.h"
#include "stringl.h"
#include "util.h"
#include "qcli_api.h"
#include "qcli_util.h"
#include "qc_at_zigbee.h"
#include "qc_at_zcl.h"

#include "qapi_zb.h"
#include "qapi_zb_cl.h"
#include "qapi_zb_cl_device_temp.h"
#include "qosa_util.h"
static void ZCL_DeviceTemp_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_DeviceTemp_Server_Event_Data_t *EventData, uint32_t CB_Param);
static void ZCL_DeviceTemp_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Custom_Cluster_Event_Data_t *EventData, uint32_t CB_Param);

/**
   @brief Handles callbacks for the device temp server cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_DeviceTemp_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_DeviceTemp_Server_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      LOG_AT_EVT("EVT_ZB: Unhandled DeviceTemp server event %d.\n", EventData->Event_Type);
      QCLI_Display_Prompt();
   }
}

/**
   @brief Handles callbacks for the device temp client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_DeviceTemp_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Custom_Cluster_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      LOG_AT_EVT("EVT_ZB: Unhandled DeviceTemp client event %d.\n", EventData->Event_Type);
      QCLI_Display_Prompt();
   }
}

/**
   @brief Creates an DeviceTemp server cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_DeviceTemp_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
{
   qapi_ZB_Cluster_t         Ret_Val;
   qapi_Status_t             Result;
   qapi_ZB_Handle_t          ZigBee_Handle;
   qapi_ZB_CL_Cluster_Info_t ClusterInfo;
   qapi_ZB_CL_Attribute_t    AttributeList[ZCL_DEMO_MAX_ATTRIBUTES_PER_CLUSTER];

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      memset(&ClusterInfo, 0, sizeof(qapi_ZB_CL_Cluster_Info_t));
      ClusterInfo.Endpoint       = Endpoint;
      ClusterInfo.AttributeCount = ZCL_DEMO_MAX_ATTRIBUTES_PER_CLUSTER;
      ClusterInfo.AttributeList  = AttributeList;
      Result = qapi_ZB_CL_DeviceTemp_Populate_Attributes(&(ClusterInfo.AttributeCount), (qapi_ZB_CL_Attribute_t*) ClusterInfo.AttributeList);
      if(Result == QAPI_OK)
      {
         Result = qapi_ZB_CL_DeviceTemp_Create_Server(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_DeviceTemp_Demo_Server_CB, 0);
         if(Result != QAPI_OK)
         {
            LOG_ERR("qapi_ZB_CL_DeviceTemp_Create_Server", Result);
            Ret_Val = NULL;
         }
      }
      else
      {
         LOG_INFO("qapi_ZB_CL_DeviceTemp_Populate_Attributes", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      LOG_ERR("ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an DeviceTemp client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_DeviceTemp_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
{
   qapi_ZB_Cluster_t         Ret_Val;
   qapi_Status_t             Result;
   qapi_ZB_Handle_t          ZigBee_Handle;
   qapi_ZB_CL_Cluster_Info_t ClusterInfo;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      memset(&ClusterInfo, 0, sizeof(qapi_ZB_CL_Cluster_Info_t));
      ClusterInfo.Endpoint       = Endpoint;
      ClusterInfo.AttributeCount = 0;
      ClusterInfo.AttributeList  = NULL;

      Result = qapi_ZB_CL_Create_Cluster(ZigBee_Handle, &Ret_Val, QAPI_ZB_CL_CLUSTER_ID_TEMPERATURE_CONFIG, &ClusterInfo, QAPI_ZB_CL_FRAME_DIRECTION_TO_CLIENT_E, ZCL_DeviceTemp_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         LOG_ERR("qapi_ZB_CL_Create_Cluster", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      LOG_ERR("ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

