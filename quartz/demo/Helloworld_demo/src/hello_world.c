/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
 
 /*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "qapi_types.h"

#include "pal.h"

#include "qurt_error.h"
#include "qurt_thread.h"
#include "qurt_signal.h"
#include "qurt_timer.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/
#define Sleep(msec)    do { \
                              qurt_time_t qtime;\
                              qtime = qurt_timer_convert_time_to_ticks(msec, QURT_TIME_MSEC);\
                              qurt_thread_sleep(qtime);\
                          } while (0)

#define THREAD_STACK_SIZE                           (3072)
#define THREAD_PRIORITY                             (10)

 
 /*-------------------------------------------------------------------------
 * Function Definitions
 *-----------------------------------------------------------------------*/
 
/**
   @brief This function represents the main thread of execution.
*/
static void HelloWorld_Thread(void *Param)
{
   /* Say Hello World */
   while(true)
   {
      PAL_CONSOLE_WRITE_STRING_LITERAL("Hello World\r\n");
      PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
      Sleep(1000); //sleep for 1 sec
   }
}

/**
   @brief This function is used to pre initialize resource 
          required before starting the demo this is called
 		  from app_init function in pal.c
 */
void Initialize_Demo(void)
{
   PAL_CONSOLE_WRITE_STRING_LITERAL("Hello World demo - Welcome to QCA IoE \r\n");
   PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
}

/**
   @brief This function is used to start the demo thread this
          is called from app_init function in pal.c
 */
void App_Start(qbool_t ColdBoot)
{
   qurt_thread_attr_t Thread_Attribte;
   qurt_thread_t      Thread_Handle;
   int                Result;

   /* Start the main demo app. */
   qurt_thread_attr_init(&Thread_Attribte);
   qurt_thread_attr_set_name(&Thread_Attribte, "HelloThread");
   qurt_thread_attr_set_priority(&Thread_Attribte, THREAD_PRIORITY);
   qurt_thread_attr_set_stack_size(&Thread_Attribte, THREAD_STACK_SIZE);
   Result = qurt_thread_create(&Thread_Handle, &Thread_Attribte, HelloWorld_Thread, NULL);
   if(Result != QURT_EOK)
   {
      PAL_CONSOLE_WRITE_STRING_LITERAL("Failed to start Hello World Main thread.");
      PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
      PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
   }
}

