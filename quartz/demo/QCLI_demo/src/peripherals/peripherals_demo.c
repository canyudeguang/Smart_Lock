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

#include "sensors_demo.h"
#include "adc_demo.h"
#include "pwm_demo.h"
#include "pwm_demo.h"
#include "peripherals_demo.h"
#include "master_sdcc_demo.h"
#include "htc_slave_demo.h"
#include "htc_demo.h"
#ifdef CONFIG_CDB_PLATFORM
#include "spi_demo.h"
#include "gpio_demo.h"
#endif
#include "keypad_demo.h"

QCLI_Group_Handle_t qcli_peripherals_group;


const QCLI_Command_Group_t peripherals_cmd_group =
{
    "Peripherals",
    0,
    NULL
};

   /* This function is used to register the portfolio subgroup with    */
   /* QCLI.                                                             */
void Initialize_Peripherals_Demo(void)
{
   /* Attempt to reqister the Command Groups with the qcli framework.*/
   qcli_peripherals_group = QCLI_Register_Command_Group(NULL, &peripherals_cmd_group);

   if(qcli_peripherals_group != NULL)
   {
	   Initialize_ADC_Demo();
	   Initialize_Sensors_Demo();
	   Initialize_PWM_Demo();
	   Initialize_SDCCM_Demo();
	   Initialize_HTCSlave_Demo();
	   Initialize_HTCHost_Demo();
#ifdef CONFIG_CDB_PLATFORM
   	   Initialize_SPI_Demo();
	   Initialize_GPIO_Demo();
#endif
	   Initialize_Keypad_Demo();
   }
   
   QCLI_Printf(qcli_peripherals_group, "Peripherals Registered \n");
}
