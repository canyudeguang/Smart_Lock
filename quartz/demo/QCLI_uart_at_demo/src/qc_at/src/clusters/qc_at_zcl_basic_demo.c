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
#include "qapi_zb_cl_basic.h"
#include "qosa_util.h"

extern QCLI_Context_t QCLI_Context;
typedef enum
{
   batUnknown,
   batUint8,
   batString
} ZCL_Basic_Demo_Attr_Type_t;

typedef struct ZCL_Basic_Demo_Attr_Info_s
{
   uint16_t                   AttrId;
   ZCL_Basic_Demo_Attr_Type_t AttrType;
} ZCL_Basic_Demo_Attr_Info_t;

static QCLI_Group_Handle_t ZCL_Basic_QCLI_Handle;

static const ZCL_Basic_Demo_Attr_Info_t BasicAttrInfoList[] =
{
   {QAPI_ZB_CL_BASIC_ATTR_ID_ZCL_VERSION,          batUint8},
   {QAPI_ZB_CL_BASIC_ATTR_ID_APPLICATION_VERSION,  batUint8},
   {QAPI_ZB_CL_BASIC_ATTR_ID_STACK_VERSION,        batUint8},
   {QAPI_ZB_CL_BASIC_ATTR_ID_HW_VERSION,           batUint8},
   {QAPI_ZB_CL_BASIC_ATTR_ID_MANUFACTURER_NAME,    batString},
   {QAPI_ZB_CL_BASIC_ATTR_ID_MODEL_IDENTIFIER,     batString},
   {QAPI_ZB_CL_BASIC_ATTR_ID_DATE_CODE,            batString},
   {QAPI_ZB_CL_BASIC_ATTR_ID_POWER_SOURCE,         batUint8},
   {QAPI_ZB_CL_BASIC_ATTR_ID_LOCATION_DESCRIPTION, batString},
   {QAPI_ZB_CL_BASIC_ATTR_ID_PHYSICAL_ENVIRONMENT, batUint8},
   {QAPI_ZB_CL_BASIC_ATTR_ID_DEVICE_ENABLED,       batUint8},
   {QAPI_ZB_CL_BASIC_ATTR_ID_ALARM_MASK,           batUint8},
   {QAPI_ZB_CL_BASIC_ATTR_ID_DISABLE_LOCAL_CONFIG, batUint8},
   {QAPI_ZB_CL_BASIC_ATTR_ID_SW_BUILD_ID,          batString}
};

#define BASIC_ATTR_INFO_LIST_SIZE                                       (sizeof(BasicAttrInfoList) / sizeof(ZCL_Basic_Demo_Attr_Info_t))

/* Function prototypes. */
static ZCL_Basic_Demo_Attr_Type_t ZCL_Basic_Demo_GetAttrType(uint16_t AttrId);

static void ZCL_Basic_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Basic_Client_Event_Data_t *Event_Data, uint32_t CB_Param);
/* Command list for the ZigBee light demo. */
const QCLI_Command_t ZigBee_Basic_CMD_List[] =
{
   /* cmd_function       thread  cmd_string  usage_string            description */
   {cmd_ZCL_Basic_Help,  false,  "Help",     "",                     "Display available ZCL basic commands."},
   {cmd_ZCL_Basic_Reset, false,  "Reset",    "[DevId][ClientEndpoint]", "Sends a Reset To Factory command to a basic server."},
   {cmd_ZCL_Basic_Read,  false,  "Read",     "[AttrId]",             "Reads an attribute from the local basic server."},
   {cmd_ZCL_Basic_Write, false,  "Write",    "[AttrId][Value]",      "Writes an attribute to the local basic server."}
};

const QCLI_Command_Group_t ZCL_Basic_Cmd_Group = {"Basic", sizeof(ZigBee_Basic_CMD_List) / sizeof(QCLI_Command_t), ZigBee_Basic_CMD_List};

/**
   @brief Gets the type of attribute given its ID.

   @param AttrId is the ID of the attribute to find.

   @return The type of attribute or batUnknown if the ID wasn't found.
*/
static ZCL_Basic_Demo_Attr_Type_t ZCL_Basic_Demo_GetAttrType(uint16_t AttrId)
{
   ZCL_Basic_Demo_Attr_Type_t Ret_Val;
   uint8_t                    Index;

   Ret_Val = batUnknown;
   for(Index = 0; Index < BASIC_ATTR_INFO_LIST_SIZE; Index ++)
   {
      if(BasicAttrInfoList[Index].AttrId == AttrId)
      {
         Ret_Val = BasicAttrInfoList[Index].AttrType;
         break;
      }
   }

   return(Ret_Val);
}

QCLI_Command_Status_t cmd_ZCL_Basic_Help(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
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
   @brief Executes the "Reset" command to send a Reset To Factory command to a
          basic server.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Basic client cluster to use to send the
                     command.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
QCLI_Command_Status_t cmd_ZCL_Basic_Reset(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                   Result;
   QCLI_Command_Status_t           Ret_Val;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_BASIC, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qc_drv_ZB_CL_Basic_Send_Reset_To_Factory(qc_api_get_qc_drv_context(), ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  LOG_INFO("qc_drv_ZB_CL_Basic_Send_Reset_To_Factory");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  LOG_ERR("qc_drv_ZB_CL_Basic_Send_Reset_To_Factory", Result);
               }
            }
            else
            {
               LOG_ERR("Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
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
      LOG_WARN("ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Read" command to read an attribute from the local basic
          server.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
QCLI_Command_Status_t cmd_ZCL_Basic_Read(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t              Result;
   QCLI_Command_Status_t      Ret_Val;
   qapi_ZB_Handle_t           ZB_Handle;
   uint16_t                   AttrId;
   uint16_t                   Length;
   uint8_t                    Data[36];
   ZCL_Basic_Demo_Attr_Type_t AttrType;

   ZB_Handle = GetZigBeeHandle();
   if(ZB_Handle != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFFFF)))
      {
         AttrId   = Parameter_List[0].Integer_Value;
         AttrType = ZCL_Basic_Demo_GetAttrType(AttrId);

         Result = qc_drv_ZB_CL_Basic_Server_Read_Attribute(qc_api_get_qc_drv_context(), ZB_Handle, AttrId, &Length, Data);
         if(Result == QAPI_OK)
         {
            LOG_INFO("qc_drv_ZB_CL_Basic_Server_Read_Attribute");

            switch(AttrType)
            {
               case batUint8:
                  LOG_INFO("Attribute Value: %d\n", Data[0]);
                  break;
               case batString:
                  LOG_INFO("Attribute Value: %s\n", Data);
                  break;
               case batUnknown:
               default:
                  LOG_INFO("Attribute Value:\n", Data);
                  Dump_Data("  ", Length, Data);
                  break;
            }

            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Ret_Val = QCLI_STATUS_ERROR_E;
            LOG_ERR("qc_drv_ZB_CL_Basic_Server_Read_Attribute", Result);
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      LOG_WARN("ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Write" command to write an attribute to the local Basic
          server.

   Parameter_List[0] is the ID of the attribute to write.
   Parameter_List[1] is the value for the attribute.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
QCLI_Command_Status_t cmd_ZCL_Basic_Write(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t       Ret_Val;
   qapi_Status_t               Result;
   qapi_ZB_Handle_t            ZB_Handle;
   uint16_t                    AttrId;
   uint16_t                    Length;
   uint8_t                     IntData;
   uint8_t                    *Data;
   ZCL_Basic_Demo_Attr_Type_t  AttrType;

   ZB_Handle = GetZigBeeHandle();
   if(ZB_Handle != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFFFF)))
      {
         AttrId   = Parameter_List[0].Integer_Value;
         AttrType = ZCL_Basic_Demo_GetAttrType(AttrId);

         Data    = NULL;
         Length  = 0;
         Ret_Val = QCLI_STATUS_SUCCESS_E;

         switch(AttrType)
         {
            case batUint8:
               if(Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF))
               {
                  IntData = (uint8_t)(Parameter_List[1].Integer_Value);
                  Data    = &IntData;
                  Length  = sizeof(uint8_t);
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
               break;
            case batString:
               Data    = (uint8_t *)(Parameter_List[1].String_Value);
               Length  = strlen(Parameter_List[1].String_Value);
               Ret_Val = QCLI_STATUS_SUCCESS_E;
               break;
            case batUnknown:
            default:
               LOG_WARN("Unknown attribute type.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
               Data    = NULL;
               Length  = 0;
               break;
         }

         /* Format the destination addr. mode, address, and endpoint. */
         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            Result = qc_drv_ZB_CL_Basic_Server_Write_Attribute(qc_api_get_qc_drv_context(), ZB_Handle, AttrId, Length, Data);
            if(Result == QAPI_OK)
            {
               LOG_INFO("qc_drv_ZB_CL_Basic_Server_Write_Attribute");
            }
            else
            {
               Ret_Val = QCLI_STATUS_ERROR_E;
               LOG_ERR("qc_drv_ZB_CL_Basic_Server_Write_Attribute", Result);
            }
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      LOG_WARN("ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Handles callbacks for the Basic client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Basic_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Basic_Client_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_BASIC_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
            LOG_AT_EVT("EVT_ZB: Basic Client Default Response:\n");
            LOG_AT_EVT("EVT_ZB: Status:        %d\n", EventData->Data.Default_Response.Status);
            LOG_AT_EVT("EVT_ZB: CommandID:     0x%02X\n", EventData->Data.Default_Response.CommandId);
            LOG_AT_EVT("EVT_ZB: CommandStatus: %d\n", EventData->Data.Default_Response.CommandStatus);
            break;

         default:
            LOG_AT_EVT("EVT_ZB: Unhandled Basic client event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Initializes the ZCL Basic demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the Basic demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_Basic_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register Basic command group. */
   ZCL_Basic_QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_Basic_Cmd_Group);
   if(ZCL_Basic_QCLI_Handle != NULL)
   {
      Ret_Val = true;
   }
   else
   {
      LOG_WARN("Failed to register ZCL Basic command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Basic client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Basic_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
{
   qapi_ZB_Cluster_t         Ret_Val;
   qapi_Status_t             Result;
   qapi_ZB_Handle_t          ZigBee_Handle;
   qapi_ZB_CL_Cluster_Info_t ClusterInfo;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      memset(&ClusterInfo, 0, sizeof(qapi_ZB_CL_Cluster_Info_t));
      ClusterInfo.Endpoint = Endpoint;

      Result = qapi_ZB_CL_Basic_Create_Client(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_Basic_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         LOG_ERR("qapi_ZB_CL_Basic_Create_Client", Result);
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

