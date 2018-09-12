/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __PAL_SOM_H__
#define __PAL_SOM_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "pal_mom.h"
#include "qapi_types.h"
#include "qapi_status.h"
#include "qapi_omtm.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   @brief Function to initialize the demo application and register its commands
          with QCLI.

   @param IsColdBoot is a flag indicating if this is a cold boot (true) or warm
                     boot (false).
*/
typedef void (*Initialize_SOM_Function_t)(void);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief This function tells the PAL layer to back to the FOM operation mode.
*/
void PAL_Exit_SOM(void);

#endif
