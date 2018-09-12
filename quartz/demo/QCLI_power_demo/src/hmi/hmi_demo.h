/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __HMI_DEMO_H__
#define __HMI_DEMO_H__

/**
   @brief Initializes the HMI demo and registers its commands with QCLI.

   @param IsColdBoot is a flag indicating if this is a cold boot (true) or warm
                     boot (false).
*/
void Initialize_HMI_Demo(qbool_t IsColdBoot);

/**
   @brief Starts the HMI demo.

   @param IsColdBoot is a flag indicating if this is a cold boot (true) or warm
                     boot (false).
*/
void Start_HMI_Demo(qbool_t IsColdBoot);

/**
   @brief Prepares the HMI demo for a mode change.

   @return true if the mode transition can proceed or false if it should be
           aborted.
*/
qbool_t HMI_Prepare_Mode_Switch(Operating_Mode_t Next_Mode);

/**
   @brief Undoes preparation of the HMI demo for a mode change.
*/
void HMI_Cancel_Mode_Switch(void);

/**
   @brief Finalizes the mode change for the HMI demo.
*/
void HMI_Exit_Mode(Operating_Mode_t Next_Mode);

#endif

