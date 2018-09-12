/****************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 *****************************************************************************/
/*! \file qmesh_model_debug.c
 *  \brief Model Library debug implementation
 *  
 *      This file contains logging utility functions for server model library.
 */
/*****************************************************************************/
#include <stdarg.h>
#include <stddef.h>

#include "qmesh_model_debug.h"
#ifndef CSR102x 
#include "qmesh_gw_debug.h"
extern DEBUGFUNC DEBUGF;
extern void * g_debugHdl;
#endif

void model_debug_log(const char *format, ...)
{
#ifndef CSR102x    
    char buf[DBUG_ARR_MAX_LEN];
    va_list ap;
    va_start(ap, format);
    vsprintf (buf, format, ap);
    DEBUGF(g_debugHdl, buf);
    va_end(ap); 
#endif    
}
