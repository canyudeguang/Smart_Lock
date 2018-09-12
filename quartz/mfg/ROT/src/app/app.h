/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _APP_H
#define _APP_H

#include "qapi/qapi_types.h"
#include "qapi/qapi.h"

#include "qapi/qapi_status.h"
#include "qapi/qapi_uart.h"
#include "qurt_error.h"
#include "qurt_thread.h"
#include "qurt_signal.h"

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

/* The following is a simple macro to facilitate printing strings directly
   to the console. As it uses the sizeof operator on the size of the string
   provided, it is intended to be used with string literals and will not
   work correctly with pointers.
*/
#define PAL_CONSOLE_WRITE_STRING_LITERAL(__String__)    do { PAL_Console_Write(sizeof(__String__) - 1, (__String__)); } while(0)

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

#ifdef CONFIG_CDB_PLATFORM
#define PAL_CONSOLE_PORT                                QAPI_UART_DEBUG_PORT_E
#else
#define PAL_CONSOLE_PORT                                QAPI_UART_HS_PORT_E
#endif

#define PAL_RECIEVE_BUFFER_SIZE                         (128)
#define PAL_RECIEVE_BUFFER_COUNT                        (2)

#define PAL_RECEIVE_EVENT_MASK                          (0x00000001)
#define PAL_TRANSMIT_EVENT_MASK                         (0x00000002)

#define PAL_ENTER_CRITICAL()                            do { __asm("cpsid i"); } while(0)
#define PAL_EXIT_CRITICAL()                             do { __asm("cpsie i"); } while(0)



/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

typedef struct PAL_Context_s
{
   qapi_UART_Handle_t       Console_UART;

   char              Rx_Buffer[PAL_RECIEVE_BUFFER_COUNT][PAL_RECIEVE_BUFFER_SIZE];
   char              Rx_Buffer_Length[PAL_RECIEVE_BUFFER_COUNT];
   uint8_t           Rx_In_Index;
   uint8_t           Rx_Out_Index;
   volatile uint32_t Rx_Buffers_Free;

   qurt_signal_t     Uart_Event;
   uint8_t           Uart_Enable;
} PAL_Context_t;
	
	

void PAL_Console_Write(uint32_t Length, const char *Buffer);

#endif

