/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <qcli.h>
#include <qcli_api.h>
#include <qapi_wlan.h>
#include "master_sdcc_demo.h"

extern QCLI_Group_Handle_t qcli_peripherals_group;              /* Handle for our peripherals subgroup. */

QCLI_Group_Handle_t qcli_sdio_group;              /* Handle for our QCLI Command Group. */

QCLI_Command_Status_t cli_sdio_test(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

const QCLI_Command_t sdio_cmd_list[] =
{
   // cmd_function        start_thread          cmd_string               usage_string                   description
   { cli_sdio_test,          false,          "msdcc",                     "",                    "<0|1|2|3|4>"   },
};

const QCLI_Command_Group_t sdio_cmd_group =
{
    "Master SDCC",
    (sizeof(sdio_cmd_list) / sizeof(sdio_cmd_list[0])),
    sdio_cmd_list
};

/* This function is used to register the wlan Command Group with    */
/* QCLI.                                                             */
void Initialize_SDCCM_Demo(void)
{
   /* Attempt to reqister the Command Groups with the qcli framework.*/
   qcli_sdio_group = QCLI_Register_Command_Group(qcli_peripherals_group, &sdio_cmd_group);
   if(qcli_sdio_group)
   {
      QCLI_Printf(qcli_sdio_group, "Master SDCC Registered \n");
   }
}

QCLI_Command_Status_t cli_sdio_test(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int sdio_test( uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List );
	int iRet;
	
    iRet = sdio_test(Parameter_Count, Parameter_List);
	if (iRet != 0)
	{
		QCLI_Printf(qcli_sdio_group, "Master SDCC fails  result:%d\n", iRet);
		return QCLI_STATUS_ERROR_E;		
	}
    return QCLI_STATUS_SUCCESS_E;
}
