/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * 2015-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <qapi_adc.h>

#define   MAX_CHANNELS		8

typedef struct _adc_result {
	qapi_ADC_Read_Result_t chan_result;
	qapi_ADC_Range_t  range;
} adc_result_t;

extern adc_result_t chan_result[MAX_CHANNELS];

/**
   @brief This function registers the ADC demo commands with QCLI.
*/
void Initialize_ADC_Demo(void);
