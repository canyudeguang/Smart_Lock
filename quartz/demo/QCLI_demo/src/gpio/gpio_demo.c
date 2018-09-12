/*
 * Copyright (c) 2017 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#include <stdio.h>
#include <stdlib.h>
#include <qcli.h>
#include <qcli_api.h>

QCLI_Command_Status_t gpio_interrupt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t gpio_tlmm(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern QCLI_Group_Handle_t qcli_peripherals_group;  /* Handle for our peripherals subgroup. */

QCLI_Group_Handle_t qcli_gpio_group;                /* Handle for our QCLI Command Group. */

const QCLI_Command_t gpio_cmd_list[] =
{
    // cmd_function      start_thread      cmd_string               usage_string                   description
    { gpio_interrupt,      false,      "gpio_interrupt_test",      "<gpio_pin>, <trigger_value = 0:TRIGGER_LEVEL_HIGH| 1:TRIGGER_LEVEL_LOW|                               2:TRIGGER_EDGE_RISING| 3: TRIGGER_EDGE_FALLING| 4:TRIGGER_EDGE_DUAL>",      "enable/disable interrupt on GPIO" },
    { gpio_tlmm,           false,      "gpio_tlmm",                "<gpio_pin>, <function = 0-15>, <direction = 0:input| 1:output>,                                       <pull = 0:No_Pull| 1:Pull_Down| 2:Pull_Up>, <value = 0:pin_low |1:pin_high>",       "to drive GPIO in io mode" },
};

const QCLI_Command_Group_t gpio_cmd_group =
{
    "GPIO",
    (sizeof(gpio_cmd_list) / sizeof(gpio_cmd_list[0])),
    gpio_cmd_list
};

/* This function is used to register the SPI Command Group with */
/* QCLI.                                                        */
void Initialize_GPIO_Demo(void)
{
    /* Attempt to reqister the Command Groups with the qcli framework.*/
    qcli_gpio_group = QCLI_Register_Command_Group(qcli_peripherals_group, &gpio_cmd_group);
    if (qcli_gpio_group)
    {
        QCLI_Printf(qcli_gpio_group, "GPIO INTR/TLMM Registered\n");
    }
}
