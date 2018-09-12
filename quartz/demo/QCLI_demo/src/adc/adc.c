/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * 2015-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */ 
 
#include <stdio.h>
#include "stdint.h"
#include <qcli.h>
#include <qcli_api.h>
#include "qapi/qurt_signal.h"
#include "qapi/qurt_thread.h"
#include "qapi/qapi_status.h"
#include "qapi/qapi_adc.h"
#include "adc_demo.h"


adc_result_t chan_result[MAX_CHANNELS];

qapi_ADC_Config_t adc_config = {
   0xFF,  /**< Mask of which channels to enable. */
   10,     /**< Continious mode measurement period in microseconds. */
   QAPI_ADC_POWER_MODE_NORMAL_E,  /**< Power mode. */	
};

int32_t adc_driver_test( void *pvParameters )
{
	qapi_Status_t status;
	uint32_t     i;
	qapi_ADC_Handle_t handle;
	uint32_t    attributes, num_channels;
	qbool_t     keep_enabled = 1;
	
	attributes = QAPI_ADC_ATTRIBUTE_NONE;	
	status = qapi_ADC_Open(&handle, attributes);
	qapi_ADC_Set_Config(handle, &adc_config);

	status = qapi_ADC_Get_Num_Channels(handle, &num_channels);
	if (QAPI_OK != status) {		
		return -1;
	}
	
	for (i=0; i < num_channels && i < MAX_CHANNELS; i++)
	{
		status = qapi_ADC_Get_Range( handle, i, &chan_result[i].range );
		if (QAPI_OK != status) {		
			return -2;
		}
		
		status = qapi_ADC_Read_Channel(handle, i, &chan_result[i].chan_result);
		if (QAPI_OK != status) {		
			return -3;
		}		
	}

	qapi_ADC_Close(handle, keep_enabled);
        
        return 0;
}

void AdcSingleShotConv() 
{ 
  qapi_Status_t adcReturnVal;
  qapi_ADC_Handle_t handle = NULL;
  uint8 Attributes = QAPI_ADC_ATTRIBUTE_NONE;
  uint32_t  uNum_of_chan, uChan;
  qapi_ADC_Range_t  range;
			
  adcReturnVal = qapi_ADC_Open(&handle, Attributes);
  if (QAPI_OK != adcReturnVal) {		
    return;
  }
	
  adcReturnVal = qapi_ADC_Get_Num_Channels(handle, &uNum_of_chan); 
  if ( (QAPI_OK != adcReturnVal) || (0 == uNum_of_chan) ) {		
    return ;
  }
		
  for(uChan = 0; uChan < uNum_of_chan; uChan++) {
    adcReturnVal =  qapi_ADC_Get_Range( handle, uChan, &range );
    if (QAPI_OK != adcReturnVal) {		
      return ;
    }
		
    adcReturnVal =  qapi_ADC_Read_Channel( handle, uChan, &chan_result[uChan].chan_result );
    if (QAPI_OK != adcReturnVal) {		
      return ;
    }
		
#if 0  
    eAdcPowerVal = ADCGetActivePowerState();
    if ( QAPI_ADC_ATTRIBUTE_BUFFERING == Attributes) {			
      if ( ADC_TEST_PS_RET != eAdcPowerVal ) {
        return ;
      }
    }
    else {
      if ( ADC_TEST_PS_OFF != eAdcPowerVal) {
        return ;
      }
    }			
#endif

  }
		
  adcReturnVal = qapi_ADC_Close(handle, (qbool_t)false);
  if (QAPI_OK != adcReturnVal) {		
    return;
  }	
		
  return;
}
