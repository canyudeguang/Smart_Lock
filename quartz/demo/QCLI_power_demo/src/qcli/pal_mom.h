/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __PAL_MOM_H__
#define __PAL_MOM_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

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
typedef void (*Initialize_MOM_Function_t)(void);

typedef enum
{
   OPERATING_MODE_FOM_E,
   OPERATING_MODE_SOM_E,
   OPERATING_MODE_MOM_E
} Operating_Mode_t;

typedef enum
{
   OMSM_CLIENT_ID_PAL_E = 100,
   OMSM_CLIENT_ID_QCLI_E,
   OMSM_CLIENT_ID_HMI_DEMO_E,
   OMSM_CLIENT_ID_BLE_E,
   OMSM_CLIENT_ID_COEX_DEMO_E
} OMSM_CLIENT_ID_t;

extern qapi_OMTM_Operating_Mode_t omtm_operating_mode_tbl_sram[3];

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

#endif
