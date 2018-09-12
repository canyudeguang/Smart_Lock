/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

typedef struct _humidity_val {
	uint8_t   who_am_i;
	uint8_t   av_conf;
	uint8_t   ctrl_reg1;
	uint8_t   ctrl_reg2;
	uint8_t   ctrl_reg3;
	uint8_t   status_reg;
	
	uint16_t  humidity_T_OUT;
	uint16_t  humidity_T0_OUT;
	uint16_t  humidity_T1_OUT;
	uint16_t  humidity_T0_rHx2;
	uint16_t  humidity_T1_rHx2;

	uint16_t  temp_T_OUT;
	uint16_t  temp_T0_OUT;
	uint16_t  temp_T1_OUT;
	uint16_t  temp_T0_DegCx8;
	uint16_t  temp_T1_DegCx8;
} humidity_val_t;

typedef struct _pressure_val {
	uint8_t   id;
	uint8_t   status_reg;
	uint8_t   ctrl_meas;
	
	uint32_t  press;
	uint32_t  temp;
} pressure_val_t;

typedef struct _compass_val {
	uint8_t   company_id;
	uint8_t   device_id;
	uint8_t   status_reg;
	uint8_t   ctrl2;
	
	int16_t   hx;
	int16_t   hy;
	int16_t   hz;
} compass_val_t;

typedef struct _gyroscope_val {
	uint8_t   who_am_i;
	uint8_t   device_id;
	uint8_t   status_reg;
	
	uint32_t  press;
	uint32_t  temp;
} gyroscope_val_t;

typedef struct _light_val {
	uint8_t   part_id;
	uint8_t   manufactory_id;
	uint8_t   control_reg;
	uint8_t   status_reg;
	uint16_t  ch1_val, ch0_val;
} light_val_t;

typedef struct  sensors_val {
	humidity_val_t  humidity;
	pressure_val_t  pressure;
	compass_val_t   compass;
	gyroscope_val_t gyroscope;
	light_val_t     light;	
} sensors_val_t;

extern sensors_val_t	sensor_vals;

/**
   @brief This function registers the SENSORS demo commands with QCLI.
*/

void Initialize_Sensors_Demo(void);

int32_t  Initialize_sensors_handle(void);
int32_t  Deinitialize_sensors_handle(void);

