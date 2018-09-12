// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/* This is a template file used for porting */

/* TODO: If all the below adapters call the standard C functions, please simply use the agentime.c that already exists. */

#include <time.h>
#include "qurt_timer.h"
#include "azure_c_shared_utility/agenttime.h"
#include "qapi/qapi_status.h"
#include "qapi/qapi_rtc.h"

time_t get_time(time_t* p)
{
	uint64_t epoch_sec = 0;
	time_t t = (qurt_timer_convert_ticks_to_time(qurt_timer_get_ticks(), QURT_TIME_MSEC))/1000;
	
	qapi_Core_RTC_GPS_Epoch_Get(&epoch_sec);
	epoch_sec = epoch_sec/1000;
	
	if(p!=NULL)
		*p = t;

	return (t+epoch_sec);
}


struct tm* get_gmtime(time_t* currentTime)
{
	/* TODO: replace the gmtime call below with your own. Note that converting to struct tm needs to be done ... 
    return gmtime(currentTime);*/
	/*Not supported for now*/
	return NULL;
}

time_t get_mktime(struct tm* cal_time)
{
	/* TODO: replace the mktime call below with your own. Note that converting to time_t needs to be done ... 
	return mktime(cal_time);*/
	/*Not supported for now*/
	return (time_t)NULL;
}

char* get_ctime(time_t* timeToGet)
{
	/* TODO: replace the ctime call below with calls available on your platform.
    return ctime(timeToGet);*/
	/*Not supported for now*/
	return NULL;
}

double get_difftime(time_t stopTime, time_t startTime)
{
	return ((double)(stopTime - startTime));
}