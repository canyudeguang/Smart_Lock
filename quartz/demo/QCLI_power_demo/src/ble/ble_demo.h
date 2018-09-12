/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define QAPI_USE_BLE

#include "qapi.h"
#include "qcli_api.h"

/**
   @brief Initializes the BLE demo and registers its commands with QCLI.

   @param IsColdBoot is a flag indicating if this is a cold boot (true) or warm
                     boot (false).
*/
void Initialize_BLE_Demo(qbool_t IsColdBoot);

/**
   @brief Prepares the BLE demo for a mode change.

   @return true if the mode transition can proceed or false if it should be
           aborted.
*/
qbool_t BLE_Prepare_Mode_Switch(Operating_Mode_t Next_Mode);

/**
   @brief Undoes preparation of the BLE demo for a mode change.
*/
void BLE_Cancel_Mode_Switch(void);

/**
   @brief Finalizes the mode change for the BLE demo.
*/
void BLE_Exit_Mode(Operating_Mode_t Next_Mode);

