/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/

/* Note: this is an auto-generated file. */

#ifndef __SIG_TYPES_H__
#define __SIG_TYPES_H__

#include <stdint.h>
#include "qmesh_data_types.h"
#include "qmesh_result.h"

extern char AsciiToHex(char *buf, int bufLen, uint8_t *outBuf);

/* sigmesh generic model specific definitions and validation APIs */

typedef enum _SIG_MESH_CLIENT_MODELS_T
{
    GENERIC_ONOFF_CLIENT,
    GENERIC_LEVEL_CLIENT,
    GENERIC_DEFAULT_TRANSITION_TIME_CLIENT,
    GENERIC_POWER_ONOFF_CLIENT,
    GENERIC_POWER_LEVEL_CLIENT,
    GENERIC_BATTERY_CLIENT,
    GENERIC_LOCATION_CLIENT,
    GENERIC_PROPERTY_CLIENT
} SIG_MESH_CLIENT_MODELS_T;

typedef enum _GENERIC_ONOFF
{
    GENERIC_ONOFF_OFF = 0x00,
    GENERIC_ONOFF_ON = 0x01
} GENERIC_ONOFF_T;

typedef enum _GENERIC_ONPOWERUP
{
    GENERIC_ONPOWERUP_OFF = 0x00,
    GENERIC_ONPOWERUP_DEFAULT = 0x01,
    GENERIC_ONPOWERUP_LAST = 0x02
} GENERIC_ONPOWERUP_T;

typedef enum _GENERIC_GLOBAL_LATITUDE
{
    GENERIC_GLOBAL_LATITUDE_NOT_CONFIGURED = 0x80000000
} GENERIC_GLOBAL_LATITUDE_T;

typedef enum _GENERIC_GLOBAL_LONGITUDE
{
    GENERIC_GLOBAL_LONGITUDE_NOT_CONFIGURED = 0x80000000
} GENERIC_GLOBAL_LONGITUDE_T;

typedef enum _GENERIC_GLOBAL_ALTITUDE
{
    GENERIC_GLOBAL_ALTITUDE_ALTITUDE_ABOVE_32765_METRES = 0x7FFE,
    GENERIC_GLOBAL_ALTITUDE_NOT_CONFIGURED = 0x7FFF
} GENERIC_GLOBAL_ALTITUDE_T;

typedef enum _GENERIC_LOCAL_NORTH
{
    GENERIC_LOCAL_NORTH_NOT_CONFIGURED = 0x8000
} GENERIC_LOCAL_NORTH_T;

typedef enum _GENERIC_LOCAL_EAST
{
    GENERIC_LOCAL_EAST_NOT_CONFIGURED = 0x8000
} GENERIC_LOCAL_EAST_T;

typedef enum _GENERIC_LOCAL_ALTITUDE
{
    GENERIC_LOCAL_ALTITUDE_LOCAL_ALTITUDE_ABOVE_3276_8_METRES = 0x7FFE,
    GENERIC_LOCAL_ALTITUDE_NOT_CONFIGURED = 0x7FFF
} GENERIC_LOCAL_ALTITUDE_T;

typedef enum _GENERIC_FLOOR_NUMBER
{
    GENERIC_FLOOR_NUMBER_FLOOR_UNDER_GROUND_20_OR_LOWER = 0,
    GENERIC_FLOOR_NUMBER_FLOOR_232_OR_HIGHER = 232,
    GENERIC_FLOOR_NUMBER_FLOOR_0_AND_GROUND_FLOOR = 253,
    GENERIC_FLOOR_NUMBER_FLOOR_1_AND_GROUND_FLOOR = 254,
    GENERIC_FLOOR_NUMBER_NOT_CONFIGURED = 255
} GENERIC_FLOOR_NUMBER_T;

typedef enum _STEP_RESOLUTION
{
    STEP_RESOLUTION_100MS_STEPS = 0x00,
    STEP_RESOLUTION_1S_STEPS = 0x01,
    STEP_RESOLUTION_10S_STEPS = 0x02,
    STEP_RESOLUTION_10M_STEPS = 0x03
} STEP_RESOLUTION_T;

typedef enum _NUMBER_OF_STEPS
{
    NUMBER_OF_STEPS_IMMEDIATE = 0x00,
    NUMBER_OF_STEPS_UNKNOWN = 0x3F
} NUMBER_OF_STEPS_T;

typedef enum _PRESENCE
{
    PRESENCE_NOT_PRESENT = 0x00,
    PRESENCE_REMOVABLE = 0x01,
    PRESENCE_NON_REMOVABLE = 0x02,
    PRESENCE_UNKNOWN = 0x03
} PRESENCE_T;

typedef enum _INDICATOR
{
    INDICATOR_CRITICAL = 0x00,
    INDICATOR_LOW = 0x01,
    INDICATOR_GOOD = 0x02,
    INDICATOR_UNKNOWN = 0x03
} INDICATOR_T;

typedef enum _CHARGING
{
    CHARGING_NOT_CHARGEABLE = 0x00,
    CHARGING_NOT_CHARGING = 0x01,
    CHARGING_CHARGING = 0x02,
    CHARGING_UNKNOWN = 0x03
} CHARGING_T;

typedef enum _SERVICEABILITY
{
    SERVICEABILITY_NOT_ALLOWED = 0x00,
    SERVICEABILITY_NOT_REQUIRE_SERVICE = 0x01,
    SERVICEABILITY_REQUIRES_SERVICE = 0x02,
    SERVICEABILITY_UNKNOWN = 0x03
} SERVICEABILITY_T;

typedef enum _STATIONARY
{
    STATIONARY_STATIONARY = 0,
    STATIONARY_MOBILE = 1
} STATIONARY_T;


bool validate_generic_onoff_t(GENERIC_ONOFF_T val);
bool validate_generic_onpowerup_t(GENERIC_ONPOWERUP_T val);
bool validate_generic_global_latitude_t(GENERIC_GLOBAL_LATITUDE_T val);
bool validate_generic_global_longitude_t(GENERIC_GLOBAL_LONGITUDE_T val);
bool validate_generic_global_altitude_t(GENERIC_GLOBAL_ALTITUDE_T val);
bool validate_generic_local_north_t(GENERIC_LOCAL_NORTH_T val);
bool validate_generic_local_east_t(GENERIC_LOCAL_EAST_T val);
bool validate_generic_local_altitude_t(GENERIC_LOCAL_ALTITUDE_T val);
bool validate_generic_floor_number_t(GENERIC_FLOOR_NUMBER_T val);
bool validate_step_resolution_t(STEP_RESOLUTION_T val);
bool validate_number_of_steps_t(NUMBER_OF_STEPS_T val);
bool validate_presence_t(PRESENCE_T val);
bool validate_indicator_t(INDICATOR_T val);
bool validate_charging_t(CHARGING_T val);
bool validate_serviceability_t(SERVICEABILITY_T val);
bool validate_stationary_t(STATIONARY_T val);

int size_of_generic_onoff_t(void);
int size_of_generic_onpowerup_t(void);
int size_of_generic_global_latitude_t(void);
int size_of_generic_global_longitude_t(void);
int size_of_generic_global_altitude_t(void);
int size_of_generic_local_north_t(void);
int size_of_generic_local_east_t(void);
int size_of_generic_local_altitude_t(void);
int size_of_generic_floor_number_t(void);
int size_of_step_resolution_t(void);
int size_of_number_of_steps_t(void);
int size_of_presence_t(void);
int size_of_indicator_t(void);
int size_of_charging_t(void);
int size_of_serviceability_t(void);
int size_of_stationary_t(void);

bool validate_type_specific(int type, uint8* value, uint8 valueLen);

#endif /* __SIG_TYPES_H__ */
