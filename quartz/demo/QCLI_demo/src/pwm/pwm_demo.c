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

#include "pwm_demo.h"

extern QCLI_Group_Handle_t qcli_peripherals_group;              /* Handle for our peripherals subgroup. */

QCLI_Group_Handle_t qcli_pwm_group;              /* Handle for our QCLI Command Group. */

QCLI_Command_Status_t pwm_test(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t google_gpio(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

const QCLI_Command_t pwm_cmd_list[] =
{
   { pwm_test,    false,          "Output",                     "",                    "Output freq duty phase channel_mask interval"   },
};

const QCLI_Command_Group_t pwm_cmd_group =
{
    "PWM",
    (sizeof(pwm_cmd_list) / sizeof(pwm_cmd_list[0])),
    pwm_cmd_list
};

   /* This function is used to register the wlan Command Group with    */
   /* QCLI.                                                             */
void Initialize_PWM_Demo(void)
{
   /* Attempt to reqister the Command Groups with the qcli framework.*/
   qcli_pwm_group = QCLI_Register_Command_Group(qcli_peripherals_group, &pwm_cmd_group);
   if(qcli_pwm_group)
   {
      QCLI_Printf(qcli_pwm_group, "PWM Registered \n");
   }
}

QCLI_Command_Status_t pwm_test(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int32_t pwm_driver_test( uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List );
    int32_t  result;
    
    result = pwm_driver_test(Parameter_Count, Parameter_List);
    if (result != 0)
    {
      	QCLI_Printf(qcli_pwm_group, "PWM fails\n");
        return QCLI_STATUS_ERROR_E;
    }
    return QCLI_STATUS_SUCCESS_E;
}

