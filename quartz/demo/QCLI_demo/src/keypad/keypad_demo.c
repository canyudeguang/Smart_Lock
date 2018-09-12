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

#include "keypad_demo.h"

extern QCLI_Group_Handle_t qcli_peripherals_group;              /* Handle for our peripherals subgroup. */

QCLI_Group_Handle_t qcli_keybrd_group;              /* Handle for our QCLI Command Group. */

QCLI_Command_Status_t keybrd_qapi_test(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t keybrd_qapi_test_quit(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

const QCLI_Command_t keybrd_cmd_list[] =
{
   { keybrd_qapi_test,         true,      "keypad",            "",               "keypad"   },
   { keybrd_qapi_test_quit,    false,     "Quit",              "",               "Quit"   },
};

const QCLI_Command_Group_t keybrd_cmd_group =
{
    "Keypad",
    (sizeof(keybrd_cmd_list) / sizeof(keybrd_cmd_list[0])),
    keybrd_cmd_list
};

   /* This function is used to register the keypad Command Group with    */
   /* QCLI.                                                             */
void Initialize_Keypad_Demo(void)
{
   /* Attempt to reqister the Command Groups with the qcli framework.*/
   qcli_keybrd_group = QCLI_Register_Command_Group(qcli_peripherals_group, &keybrd_cmd_group);
   if(qcli_keybrd_group)
   {
      QCLI_Printf(qcli_keybrd_group, "Keypad Registered \n");
   }
}

QCLI_Command_Status_t keybrd_qapi_test(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
	int32_t keyboard_driver_qapi_test( uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List );
    int32_t  result;
    
    result = keyboard_driver_qapi_test(Parameter_Count, Parameter_List);
    if (result != 0)
    {
      	QCLI_Printf(qcli_keybrd_group, "Keypad fails\n");
        return QCLI_STATUS_ERROR_E;
    }
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t keybrd_qapi_test_quit(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
	int32_t keypad_test_quit( uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List );
    int32_t  result;
    
    result = keypad_test_quit(Parameter_Count, Parameter_List);
    if (result != 0)
    {
      	QCLI_Printf(qcli_keybrd_group, "keypad fails\n");
        return QCLI_STATUS_ERROR_E;
    }
    return QCLI_STATUS_SUCCESS_E;
}



