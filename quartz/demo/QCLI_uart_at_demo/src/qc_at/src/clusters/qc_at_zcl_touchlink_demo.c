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
#include "qapi_zb_cl_touchlink.h"
#include "qosa_util.h"

#define ZCL_Touchlink_DEMO_MAX_Touchlink                                      (4)

static QCLI_Group_Handle_t ZCL_Touchlink_QCLI_Handle;
extern QCLI_Context_t QCLI_Context;

static void ZCL_Touchlink_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Touchlink_Client_Event_Data_t *EventData, uint32_t CB_Param);
static void ZCL_Touchlink_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Touchlink_Server_Event_Data_t *EventData, uint32_t CB_Param);

/* Command list for the ZigBee light demo. */
static const QCLI_Command_t ZigBee_Touchlink_CMD_List[] =
{
   /* cmd_function                  thread  cmd_string       usage_string             description */
   {cmd_ZCL_Touchlink_Help,         false,  "Help",         "",                       "Display the all ZCL touchlink commands."},
   {cmd_ZCL_Touchlink_Start,        false,  "Start",        "[Endpoint][DeviceType]", "Starts the touchlink commissioning process."},
   {cmd_ZCL_Touchlink_Scan,         false,  "Scan",         "[ClientEndpoint]",       "Performs a touchlink scan operation."},
   {cmd_ZCL_Touchlink_FactoryReset, false,  "FactoryReset", "[ClientEndpoint]",       "Performs a touchlink factory reset."},
};

const QCLI_Command_Group_t ZCL_Touchlink_Cmd_Group = {"Touchlink", sizeof(ZigBee_Touchlink_CMD_List) / sizeof(QCLI_Command_t), ZigBee_Touchlink_CMD_List};

QCLI_Command_Status_t cmd_ZCL_Touchlink_Help(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t Ret_Val;
    uint32_t              Result;
    int32_t               Index;

    if(TAKE_LOCK(QCLI_Context.CLI_Mutex))
    {
        Result = Display_Help(QCLI_Context.Executing_Group, Parameter_Count, Parameter_List);

        /* if there was an error parsing the command list, print out an error
           message here (this is in addition to the usage message that will be
           printed out). */
        if(Result > 0)
        {
            LOG_INFO("Command \"%s", Parameter_List[0].String_Value);

            for(Index = 1; Index < Result; Index ++)
            {
                LOG_INFO(" %s", Parameter_List[Index].String_Value);
            }

            LOG_WARN("\" not found.\n");

            Ret_Val = QCLI_STATUS_USAGE_E;
        }
        else
        {
            Ret_Val = QCLI_STATUS_SUCCESS_E;
            LOG_AT_OK();
        }

        RELEASE_LOCK(QCLI_Context.CLI_Mutex);
    }
    else
    {
        Ret_Val = QCLI_STATUS_ERROR_E;
        LOG_AT_ERROR();
    }

    return(Ret_Val);

}

/**
   @brief Executes the "Start" command to view a scene.

   Parameter_List[0] Endpoint of the Touchlink cluster to start commissioning
                     on.
   Parameter_List[1] Type of device to initialize touchlink as.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
QCLI_Command_Status_t cmd_ZCL_Touchlink_Start(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t               Ret_Val;
   qapi_Status_t                       Result;
   ZCL_Demo_Cluster_Info_t            *ClusterInfo;
   qapi_ZB_CL_Touchlink_Device_Type_t  DeviceType;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 3)))
      {
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_TOUCHLINK_COMMISSIONING, ZCL_DEMO_CLUSTERTYPE_UNKNOWN);
         DeviceType  = (qapi_ZB_CL_Touchlink_Device_Type_t)(Parameter_List[1].Integer_Value);

         if(ClusterInfo != NULL)
         {
            Ret_Val = QCLI_STATUS_SUCCESS_E;

//xxx add persist support once added to the demo
            Result = qapi_ZB_CL_Touchlink_Start(ClusterInfo->Handle, DeviceType, NULL, 0);
            if(Result == QAPI_OK)
            {
               LOG_INFO("qapi_ZB_CL_Touchlink_Start");
            }
            else
            {
               Ret_Val = QCLI_STATUS_ERROR_E;
               LOG_ERR("qapi_ZB_CL_Touchlink_Start", Result);
            }
         }
         else
         {
            LOG_ERR("Invalid Cluster Index.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      LOG_WARN("Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Scan" command to perform scan operation.

   Parameter_List[0] Endpoint of the Touchlink client cluster to use.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
QCLI_Command_Status_t cmd_ZCL_Touchlink_Scan(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t               Ret_Val;
   qapi_Status_t                       Result;
   ZCL_Demo_Cluster_Info_t            *ClusterInfo;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_TOUCHLINK_COMMISSIONING, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         if(ClusterInfo != NULL)
         {
            Result = qapi_ZB_CL_Touchlink_Scan_Request(ClusterInfo->Handle);
            if(Result == QAPI_OK)
            {
               LOG_INFO("qapi_ZB_CL_Touchlink_Scan_Request");
            }
            else
            {
               Ret_Val = QCLI_STATUS_ERROR_E;
               LOG_ERR("qapi_ZB_CL_Touchlink_Scan_Request", Result);
            }
         }
         else
         {
            LOG_ERR("Invalid Cluster Index.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      LOG_WARN("Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "FactoryReset" command to remove a scene.

   Parameter_List[0] Endpoint of the Touchlink client cluster to use.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
QCLI_Command_Status_t cmd_ZCL_Touchlink_FactoryReset(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t    Ret_Val;
   qapi_Status_t            Result;
   ZCL_Demo_Cluster_Info_t *ClusterInfo;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_TOUCHLINK_COMMISSIONING, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            Result = qapi_ZB_CL_Touchlink_Factory_Reset(ClusterInfo->Handle);
            if(Result == QAPI_OK)
            {
               LOG_INFO("qapi_ZB_CL_Touchlink_Factory_Reset");
            }
            else
            {
               Ret_Val = QCLI_STATUS_ERROR_E;
               LOG_ERR("qapi_ZB_CL_Touchlink_Factory_Reset", Result);
            }
         }
         else
         {
            LOG_ERR("Invalid Cluster Index.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      LOG_WARN("Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Handles callbacks for the Touchlink client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Touchlink_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Touchlink_Client_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_TOUCHLINK_CLIENT_EVENT_TYPE_START_RESULT_E:
            LOG_AT_EVT("EVT_ZB: Touchlink Client Start Result:\n");
            LOG_AT_EVT("EVT_ZB: Status: %d\n", EventData->Data.StartResult);
            break;

         default:
            LOG_AT_EVT("EVT_ZB: Unhandled Touchlink client event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Handles callbacks for the Touchlink client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Touchlink_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Touchlink_Server_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_TOUCHLINK_SERVER_EVENT_TYPE_START_RESULT_E:
            LOG_AT_EVT("EVT_ZB: Touchlink Server Start Result:\n");
            LOG_AT_EVT("EVT_ZB: Status: %d\n", EventData->Data.StartResult);
            break;

         default:
            LOG_AT_EVT("EVT_ZB: Unhandled Touchlink server event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Initializes the ZCL Touchlink demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the Touchlink demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_Touchlink_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register Touchlink command group. */
   ZCL_Touchlink_QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_Touchlink_Cmd_Group);
   if(ZCL_Touchlink_QCLI_Handle != NULL)
   {
      Ret_Val = true;
   }
   else
   {
      LOG_WARN("Failed to register ZCL Touchlink command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Touchlink server cluster.

   @param Endpoint   is the endpoint the cluster will be part of.
   @param DeviceName is the string representation of the endpoint the cluster
                     will be part of.

   @return is the ClusterIndex of the newly created cluster or a negative value
           if there was an error.
*/
qapi_ZB_Cluster_t ZCL_Touchlink_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
{
   qapi_ZB_Cluster_t        Ret_Val;
   qapi_Status_t            Result;
   qapi_ZB_Handle_t         ZigBee_Handle;
   ZCL_Demo_Cluster_Info_t *IdentifyClusterInfo;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      /* Try to find an identify cluster on the same endpoint to link with. */
      IdentifyClusterInfo = ZCL_FindClusterByEndpoint(Endpoint, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, ZCL_DEMO_CLUSTERTYPE_SERVER);
      if(IdentifyClusterInfo != NULL)
      {
         Result = qapi_ZB_CL_Touchlink_Create_Server(ZigBee_Handle, &Ret_Val, Endpoint, IdentifyClusterInfo->Handle, ZCL_Touchlink_Demo_Server_CB, 0);
         if(Result != QAPI_OK)
         {
            LOG_ERR("qapi_ZB_CL_Touchlink_Create_Server", Result);
            Ret_Val = NULL;
         }
      }
      else
      {
         LOG_ERR("Failed to find Identify server on the endpoint.\n");
         Ret_Val = NULL;
      }
   }
   else
   {
      LOG_WARN("ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Touchlink client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Touchlink_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
{
   qapi_ZB_Cluster_t Ret_Val;
   qapi_Status_t     Result;
   qapi_ZB_Handle_t  ZigBee_Handle;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      Result = qapi_ZB_CL_Touchlink_Create_Client(ZigBee_Handle, &Ret_Val, Endpoint, ZCL_Touchlink_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         LOG_ERR("qapi_ZB_CL_Touchlink_Create_Client", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      LOG_WARN("ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

