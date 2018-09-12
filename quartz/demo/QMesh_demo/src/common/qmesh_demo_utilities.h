/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*
  * This file function prototypes of common utility functions.
 */
/*****************************************************************************/

#ifndef __QMESH_DEMO_UTILITIES_H__
#define __QMESH_DEMO_UTILITIES_H__

#include "qcli_api.h"

#ifdef  DEBUG
#define  QCLI_LOGI(m,...) QCLI_Printf(m,__VA_ARGS__)
#define  QCLI_LOGE(m,...) QCLI_Printf(m,__VA_ARGS__)
#else
#define  QCLI_LOGI(m,...)
#endif
#define  QCLI_LOGE(m,...) QCLI_Printf(m,__VA_ARGS__)
/* Convert ASCII to HEX */
extern char AsciiToHex(char *buf, int bufLen, uint8_t *outBuf);
/* This function converts ASCII characters to Hexadecimal value in BigEndian format */
extern char AsciiToHexU16_BE(char *buf, int bufLen, uint16_t *outBuf);
/* Prints QCLI Parameters */
extern void PrintQCLIParameters(uint32_t Parameter_Count, 
                                QCLI_Parameter_t *Parameter_List);
/* Displays a function error message. */
extern void DisplayFunctionError(char *Function, int Status);
/* Displays a uint16_t array */
extern void DisplayU16Array(const uint16_t *arr, uint16_t len);
/* Displays a uint32_t array */
extern void DisplayU32Array(const uint32_t *arr, uint16_t len);
#endif
