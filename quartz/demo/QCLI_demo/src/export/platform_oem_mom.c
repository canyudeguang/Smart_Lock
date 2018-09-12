/*=============================================================================

                               PLATFORM_OEM_INIT_MOM

GENERAL DESCRIPTION
  This file contains the initial operations Quartz M4 for MOM mode.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2015-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

  $Header: //components/rel/core.ioe/1.0/platform/apps_proc/src/platform_oem_mom.c#2 $
  $Author: pwbldsvc $
  $DateTime: 2016/05/24 17:51:27 $
=============================================================================*/

/*==================================================================================================
                                           INCLUDES
==================================================================================================*/
#include <stdint.h>
#include "platform_oem.h"
#include "qapi_mom.h"

void mom_oem_init(void);

void mom_oem_init(void)
{
	/* Test only */
	//qapi_MOM_Wakeup_Callback_Register(oem_wakeup_handler, (void*)&oem_counter);
    qapi_MOM_Set_Wakeup_Timer(0x10000, 0);
}

/*************************************************************************************************
**************************************************************************************************

                                 DO NOT EDIT BELOW                                       

**************************************************************************************************
*************************************************************************************************/
typedef void (*PLATFORM_FUNCTION_TYPE)(void);

PLATFORM_FUNCTION_TYPE init_mom_functions[] =
{
   0
};
