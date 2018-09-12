/*=============================================================================

                               PLATFORM_OEM_INIT_SOM

GENERAL DESCRIPTION
  This file contains the initial operations Quartz M4 for SOM mode.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2015-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

  $Header: //components/rel/core.ioe/1.0/platform/apps_proc/src/platform_oem_som.c#3 $
  $Author: pwbldsvc $
  $DateTime: 2017/04/26 15:38:48 $
=============================================================================*/

/*==================================================================================================
                                           INCLUDES
==================================================================================================*/
#include <stdint.h>
#include "platform_oem.h"


/*TEST ONLY - This function need to be defined by OEM Application */
extern void som_app_init(void);

/* TEST ONLY */
extern void som_app_entry_internal(void);

/*==================================================================================================
                      OEM Application for SOM - Can be Moved out to OEM specific file
==================================================================================================*/
void som_app_entry(void)
{
   /* TEST ONLY */
   som_app_entry_internal();
}

/*************************************************************************************************
**************************************************************************************************

                                 DO NOT EDIT BELOW                                       

**************************************************************************************************
*************************************************************************************************/

extern void i2cm_init(void);
extern void adc_init(void);
extern void pwm_init(void);
extern void diagnostic_sensormode_init(void);

extern void adc_deinit(void);
extern void pwm_deinit(void);
extern void diagnostic_sensormode_deinit(void);
extern void i2cm_deinit(void);


typedef void (*PLATFORM_FUNCTION_TYPE)(void);


PLATFORM_FUNCTION_TYPE init_sensormode_functions[] =
{
   #ifdef I2C_INIT_COLD
   /* I2C initialization*/
   i2cm_init,
   #endif

   #ifdef ADC_INIT_COLD
   /* ADC initialization */
   adc_init,
   #endif

   #ifdef PWM_INIT_COLD
   /* PWM initialization */
   pwm_init,
   #endif

   #ifdef DIAG_INIT_COLD
   /* Diagnostic services */
   diagnostic_sensormode_init,
   #endif
   
   som_app_init,
   
   0,
};

PLATFORM_FUNCTION_TYPE deinit_sensormode_functions[] =
{

   #ifdef DIAG_INIT_COLD
   /* de-initialize diag */
   diagnostic_sensormode_deinit,
   #endif

   #ifdef PWM_INIT_COLD
   /* de-initialize pwm */
   pwm_deinit,
   #endif

   #ifdef ADC_INIT_COLD
   /* de-initialize adc */
   adc_deinit,
   #endif

   #ifdef I2C_INIT_COLD
   /* de-initialize i2c master */
   i2cm_deinit,
   #endif

   0,
};
