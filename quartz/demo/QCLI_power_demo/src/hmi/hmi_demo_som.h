/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __HMI_DEMO_SOM_H__
#define __HMI_DEMO_SOM_H__

#include "qapi_types.h"

/**
   @brief Initializes the HMI demo in SOM mode.
*/
void Initialize_HMI_Demo_SOM(void);

/**
   @brief called when SOM mode is exited.
*/
void HMI_Demo_SOM_Exit_Mode(void);

#endif

