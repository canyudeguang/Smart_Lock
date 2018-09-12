/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QCLI_H__   // [
#define __QCLI_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_types.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief This function is used to initialize the QCLI module.

   This function must be called before any other QCLI functions.

   @return
    - true if QCLI was initialized successfully.
    - false if initialization failed.
*/
qbool_t QCLI_Initialize(void);

/**
   @brief This function handles Mode Switch Prepare indications.

   @param Next_Mode is the mode the chip will be transitioning to.

   @return
    - true if the mode switch can occur.
    - false if the mode-switch should be aborted.
*/
qbool_t QCLI_Prepare_Mode_Switch(Operating_Mode_t Next_Mode);

/**
   @brief This function handles the mode switch cancel indications.
*/
void QCLI_Cancel_Mode_Switch(void);

/**
   @brief This function handles the mode exit callback indications.

   @param Next_Mode is the mode the chip is transitioning to.
*/
void QCLI_Exit_Mode(Operating_Mode_t Next_Mode);

/**
   @brief This function is intended to restore the state of QCLI after a warm
          boot.  It should be called after all demo applications have
          initialized.
*/
void QCLI_Restore_State(void);

/**
   @brief This function passes characters input from the command line to
          the QCLI module for processing.

   @param Length is the number of bytes in the provided buffer.
   @param Buffer is a pointer to the buffer containing the inputted data.

   @return
    - true if QCLI was initialized successfully.
    - false if initialization failed.
*/
void QCLI_Process_Input_Data(uint32_t Length, char *Buffer);

/**
   @brief This function displays the current command list.

   It is intended to provide a means for the initial command list to be
   displayed once platform initialization is complete.
*/
void QCLI_Display_Command_List(void);

#endif // ] #ifndef __QCLI_H__

