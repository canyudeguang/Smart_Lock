/*
 * Copyright (c) 2017 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#include <stdio.h>
#include <stdlib.h>
#include <qcli.h>
#include <qcli_api.h>
#include <spi_demo.h>

extern QCLI_Group_Handle_t qcli_peripherals_group;   /* Handle for our peripherals subgroup. */

QCLI_Group_Handle_t qcli_spi_group;                  /* Handle for our QCLI Command Group. */

QCLI_Command_Status_t spi_test(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

const QCLI_Command_t spi_cmd_list[] =
{
    //cmd_function      start_thread      cmd_string      usage_string      description
    { spi_test,      false,      "spi_test",      "<mode = 0:Disable_Loopback|  1:Enable_Loopback>",      "spi test"   },
};

const QCLI_Command_Group_t spi_cmd_group =
{
    "SPI",
    (sizeof(spi_cmd_list) / sizeof(spi_cmd_list[0])),
    spi_cmd_list
};

/* This function is used to register the SPI Command Group with    */
/* QCLI.                                                             */
void Initialize_SPI_Demo(void)
{
    /* Attempt to reqister the Command Groups with the qcli framework.*/
    qcli_spi_group = QCLI_Register_Command_Group(qcli_peripherals_group, &spi_cmd_group);
    if (qcli_spi_group)
    {
        QCLI_Printf(qcli_spi_group, "SPI Registered\n");
    }
}
