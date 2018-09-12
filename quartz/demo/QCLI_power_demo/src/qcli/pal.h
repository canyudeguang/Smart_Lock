/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __PAL_H__
#define __PAL_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "pal_som.h"
#include "qapi_types.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/**
   This definition indicates the character that is inpretted as an end of
   line for inputs from the console.
*/
#define PAL_INPUT_END_OF_LINE_CHARACTER                     '\r'

/**
   This definition represents the string that is used as the end of line
   for outputs to the console.
*/
#define PAL_OUTPUT_END_OF_LINE_STRING                       "\r\n"

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   @brief Function to initialize the demo application and register its commands
          with QCLI.

   @param IsColdBoot is a flag indicating if this is a cold boot (true) or warm
                     boot (false).
*/
typedef void (*Initialize_Function_t)(qbool_t IsColdBoot);

/**
   @brief Function to inform the demo application that a mode switch is about to
          happen.

   This function should allocate resources necessary for the mode switch and
   verify that it can be performed correctly.

   @return true if the mode transition can proceed or false if it should be
           aborted.
*/
typedef qbool_t (*Prepare_Mode_Swith_Function_t)(Operating_Mode_t Next_Mode);

/**
   @brief Function to inform the demo application that a mode switch was
          canceled.
*/
typedef void (*Cancel_Mode_Switch_Function_t)(void);

/**
   @brief Function to inform the demo application that a mode switch is now
          occuring.
*/
typedef void (*Exit_Mode_Function_t)(Operating_Mode_t Next_Mode);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief This function is used to write a buffer to the console. Note
          that when this function returns, all data from the buffer will
          be written to the console or buffered locally.

   @param Length is the length of the data to be written.
   @param Buffer is a pointer to the buffer to be written to the console.
*/
void PAL_Console_Write(uint32_t Length, const char *Buffer);

/**
   @brief This function disables the UART for a specified period of time.

   @param Time is the time to disable the UART in milliseconds. Specify
          0xFFFFFFFF to disable indefinitely.
*/
void PAL_DisableUART(uint32_t Time);

/**
   @brief This function tells the PAL layer to change the current operating
          mode.

   @param Operating_Mode is the mode to switch to.
*/
void PAL_Switch_Operating_Mode(Operating_Mode_t Operating_Mode);

#endif
