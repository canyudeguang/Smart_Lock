#ifndef _PLATFORM_H
#define _PLATFORM_H

/*===========================================================================

                                platform
GENERAL DESCRIPTION
  This header file contains platform specific declarations and definitions

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2010-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.ioe/1.0/platform/apps_proc/src/platform_oem.h#6 $
  $DateTime: 2017/12/12 01:51:07 $ 
  $Author: pwbldsvc $

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*==================================================================================================
                                           DEFINES
==================================================================================================*/
/*************************************
* FULL Operating MODE Module Defines 
**************************************/

/* Define this if I2C modules is needed in FUll operating mode */
#define I2C_INIT_COLD

/* Define this if ADC module is needed in FULL operating mode */
#define ADC_INIT_COLD

/* Define this if PWM module is needed in FULL operating mode */
#define PWM_INIT_COLD

/* Define this if FIleSystem module is needed in FULL operating mode */
#define FS_INIT_COLD

/* Define this if RFS module is needed in FULL operating mode for v2*/
#if defined(FEATURE_QUARTZ_V2)
#define RFS_INIT_COLD
#endif

/*
 * Define this if DIAGNOSTIC SERVICES module is needed in FULL operating mode
 * Uncomment the following to enable DIAG support.
 */
/* #define DIAG_INIT_COLD */

/* Define this if DIAGNOSTIC SERVICES module is needed in FULL operating mode */
#if defined(DIAG_INIT_COLD)
#define FS_DIAG_INIT_COLD
#endif

/* Define this if QCA module is needed in FULL operating mode */
#define QCA_INIT_COLD

/* Define this if PLATFORM_TEST module is needed in FULL operating mode */
//#define PLATFORM_TEST_INIT


/**************************************
* SENSOR Operating MODE Module Defines 
***************************************/

/* Define this if I2C modules is needed in FUll operating mode */
#define I2C_INIT_SENSOR

/* Define this if Temperature Sensor module is needed in FULL operating mode */
#define TSENS_INIT_SENSOR

/* Define this if ADC module is needed in FULL operating mode */
#define ADC_INIT_SENSOR

/* Define this if PWM module is needed in FULL operating mode */
#define PWM_INIT_SENSOR

/* Define this if DIAGNOSTIC SERVICES module is needed in FULL operating mode */
#define DIAG_INIT_SENSOR




#endif
