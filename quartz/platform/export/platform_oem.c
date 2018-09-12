/*=============================================================================

                               PLATFORM_INIT 

GENERAL DESCRIPTION
  This file contains the initial operations Quartz M4.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2015-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

  $Header: //components/rel/core.ioe/1.0/platform/apps_proc/src/platform_oem.c#9 $
  $Author: pwbldsvc $
  $DateTime: 2017/04/26 15:38:48 $
=============================================================================*/

/*==================================================================================================
                                           INCLUDES
==================================================================================================*/
#include <stdint.h>
#include "platform_oem.h"

/*************************************************************************************************
**************************************************************************************************

                                 DO NOT EDIT BELOW                                       

**************************************************************************************************
*************************************************************************************************/

extern void i2cm_init(void);
extern void adc_init(void);
extern void pwm_init(void);
extern void diagnostic_init(void);
extern void diagnostic_sensormode_init(void);
extern void qca_module_init(void);
extern void fs_init(void);
extern void fs_diag_init(void);
extern void rfs_server_init(void);

extern void adc_deinit(void);
extern void pwm_deinit(void);
extern void diagnostic_deinit(void);
extern void diagnostic_sensormode_deinit(void);
extern void i2cm_deinit(void);
extern void fs_deinit(void);
extern void fs_diag_deinit(void);

typedef void (*PLATFORM_FUNCTION_TYPE)(void);

PLATFORM_FUNCTION_TYPE init_coldboot_functions[] =
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
   diagnostic_init,
   #endif

   #ifdef FS_INIT_COLD
   /* Initializes the filesystem */
   fs_init,
   #endif
   
   #ifdef FS_DIAG_INIT_COLD
   /* Initializes the filesystem */
   fs_diag_init,
   #endif

   #ifdef RFS_INIT_COLD
   /* Initializes the remote filesystem */
   rfs_server_init,
   #endif

   #ifdef QCA_INIT_COLD
   /* Initializes QCA modules */
   qca_module_init,
   #endif

   #ifdef PLATFORM_TEST_INIT
   platform_test_1,
   #endif

   0,
};

PLATFORM_FUNCTION_TYPE deinit_coldboot_functions[] =
{
   /* TODO: qca_deinit */

   #ifdef FS_DIAG_INIT_COLD
   /* de-initialize file system */
   fs_diag_deinit,
   #endif

   #ifdef FS_INIT_COLD
   /* de-initialize file system */
   fs_deinit,
   #endif

   #ifdef DIAG_INIT_COLD
   /* de-initialize diag */
   diagnostic_deinit,
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
