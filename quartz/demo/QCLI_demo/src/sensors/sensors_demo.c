/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * 2015-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <qcli.h>
#include <qcli_api.h>
#include <qapi_wlan.h>
#include "qurt_thread.h"
#include "sensors_demo.h"

extern QCLI_Group_Handle_t qcli_peripherals_group;              /* Handle for our peripherals subgroup. */

QCLI_Group_Handle_t qcli_sensors_group;              /* Handle for our QCLI Command Group. */

QCLI_Command_Status_t sensors_humidity(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t sensors_pressure(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t sensors_compass(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t sensors_gyroscope(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t sensors_light(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
#ifdef CONFIG_CDB_PLATFORM
QCLI_Command_Status_t sensors_pir(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t sensors_read_all(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
#endif

const QCLI_Command_t sensors_cmd_list[] =
{
   // cmd_function        start_thread          cmd_string               usage_string                   description
   { sensors_humidity,     false,          "humidity",                     "",                    "humidity temperature"   },
   { sensors_pressure,     false,          "pressure",                     "",                    "pressure"   },
   { sensors_compass,      false,          "compass",                     "",                    "compass"   },
   { sensors_gyroscope,    false,          "gyroscope",                     "",                    "gyroscope"   },
   { sensors_light,        false,          "light",                     "",                    "light"   },
#ifdef CONFIG_CDB_PLATFORM
   { sensors_read_all,     false,          "read_sensors",                 "",                    "all sensor readings"   },
   { sensors_pir,          false,          "pir",                          "",                    "pir motion sensor"   },
#endif
};

const QCLI_Command_Group_t sensors_cmd_group =
{
    "SENSORS",
    (sizeof(sensors_cmd_list) / sizeof(sensors_cmd_list[0])),
    sensors_cmd_list
};

   /* This function is used to register the wlan Command Group with    */
   /* QCLI.                                                             */
void Initialize_Sensors_Demo(void)
{
   /* Attempt to reqister the Command Groups with the qcli framework.*/
   qcli_sensors_group = QCLI_Register_Command_Group(qcli_peripherals_group, &sensors_cmd_group);
   if(qcli_sensors_group)
   {
      QCLI_Printf(qcli_sensors_group, "SENSORS Registered \n");
   }
}

#ifdef CONFIG_CDB_PLATFORM
QCLI_Command_Status_t sensors_pir(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int32_t sensors_pir_driver_test(uint32_t Parameter_Count, QCLI_Parameter_t *pvParameters);
    int32_t result;

    result = sensors_pir_driver_test(Parameter_Count, Parameter_List);
    if (result != 0)
    {
       QCLI_Printf(qcli_sensors_group, "PIR failed!\n");
       return  QCLI_STATUS_ERROR_E;
    }

    return QCLI_STATUS_SUCCESS_E;
}
#endif

QCLI_Command_Status_t sensors_humidity(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int32_t sensors_humidity_driver_test(uint32_t Parameter_Count, QCLI_Parameter_t *pvParameters );
    int32_t  result;
    
    result = sensors_humidity_driver_test(Parameter_Count, Parameter_List);
    if (result != 0)
    {
       QCLI_Printf(qcli_sensors_group, "Humidity fails\n");
       return  QCLI_STATUS_ERROR_E;
    }

    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t sensors_pressure(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int32_t sensors_pressure_driver_test( void *pvParameters );
    int32_t  result;
    
    result = sensors_pressure_driver_test(Parameter_List);
    if (result != 0)
    {
	QCLI_Printf(qcli_sensors_group, "Pressure fails\n");
        return  QCLI_STATUS_ERROR_E;
    }
	
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t sensors_compass(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int32_t sensors_compass_driver_test( uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List );
    int32_t result;
    
    result = sensors_compass_driver_test(Parameter_Count, Parameter_List);
    if (result != 0)
    {
		QCLI_Printf(qcli_sensors_group, "Compass fail\n");
        return  QCLI_STATUS_ERROR_E;
    }
	
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t sensors_gyroscope(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
	int32_t sensors_gyroscope_driver_test( uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List );
    int32_t  result;

    result = sensors_gyroscope_driver_test(Parameter_Count, Parameter_List);
    if (result != 0)
    {
       QCLI_Printf(qcli_sensors_group, "Gyroscope fails\n");
       return  QCLI_STATUS_ERROR_E;
    }

    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t sensors_light(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int32_t sensors_light_driver_test( uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List );
    int32_t result;
    
    result = sensors_light_driver_test(Parameter_Count, Parameter_List);
    if (result != 0)
    {
		QCLI_Printf(qcli_sensors_group, "Light fails\n");
        return  QCLI_STATUS_ERROR_E;
    }
    return QCLI_STATUS_SUCCESS_E;
}

