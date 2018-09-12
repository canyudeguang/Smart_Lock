/*
 * Copyright (c) 2015 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "pal_mom.h"

#include "qapi/qapi.h"
#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"
#include "qapi/qapi_omtm.h"

extern void platform_Entry(void);
extern void som_main(void);
extern void mom_main(void);

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

typedef void (*Platform_Function_t)(void);

/*-------------------------------------------------------------------------
 * Static & global Variable Declarations
 *-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Function Definitions
 *-----------------------------------------------------------------------*/

static void PAL_MOM_Initialize(void);
static void PAL_MOM_Exit(void);

Platform_Function_t init_mom_functions[]   = {PAL_MOM_Initialize, NULL};
Platform_Function_t deinit_mom_functions[] = {PAL_MOM_Exit,       NULL};

/**
   @brief Function to initailize the QCLI Demo in MOM mode.
*/
static void PAL_MOM_Initialize(void)
{
   /* Register the MOM operating modes. */
#ifdef V1
   qapi_OMTM_MOM_Register_Operating_Modes(omtm_operating_mode_tbl_sram, 3, 2);
#endif
}

/**
   @brief Function to indicate a mode exit from MOM mode.
*/
static void PAL_MOM_Exit(void)
{
}
