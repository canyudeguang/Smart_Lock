/******************************************************************************
Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/


#ifndef __QMESH_DEMO_UTILITIES_H__
#define __QMESH_DEMO_UTILITIES_H__

#include "qcli_api.h"
 
/* Convert ASCII to HEX */
char AsciiToHex(char *buf, int bufLen, uint8_t *outBuf);

char AsciiToHexU16_BE(char *buf, int bufLen, uint16_t *outBuf);

/* Prints QCLI Parameters */
void PrintQCLIParameters(uint32_t Parameter_Count, 
                                QCLI_Parameter_t *Parameter_List);

/* Displays a function error message. */
void DisplayFunctionError(char *Function, int Status);

/* Displays a uint16_t array */
void DisplayU16Array(const uint16_t *arr, uint16_t len);

/* Displays a uint32_t array */
void DisplayU32Array(const uint32_t *arr, uint16_t len);

#endif
