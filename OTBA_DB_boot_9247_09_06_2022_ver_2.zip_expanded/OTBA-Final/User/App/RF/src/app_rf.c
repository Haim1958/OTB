/**
 ******************************************************************************
 * @file    rf.c
 * @brief   RF management algorithm and calibration methods
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; LEGAL INFORMATION</center></h2>
 *
*  RAFAEL ADVANCED DEFENCE SYSTEMS LTD. All rights reserved. Proprietary. CONFIDENTIAL.
*  All Rights Reserved.
*
*  NOTICE:  All information contained herein is, and remains
*  the property of RAFAEL ADVANCED DEFENCE SYSTEMS LTD. and its suppliers,
*  if any.  The intellectual and technical concepts contained
*  herein are proprietary to RAFAEL ADVANCED DEFENCE SYSTEMS LTD.
*  and its suppliers and may be covered by U.S. and Foreign Patents,
*  patents in process, and are protected by trade secret or copyright law.
*  Dissemination of this information or reproduction of this material
*  is strictly forbidden unless prior written permission is obtained
*  from RAFAEL ADVANCED DEFENCE SYSTEMS LTD.
 *
 ******************************************************************************
 */

#include <string.h>
#include "UserLib.h"
#include "app.h"


extern ADC_HandleTypeDef hadc3;
const char* const RF_TABLE_TABLE_LUT[RF_TABLE_NUMBER_TABLES] = {
		TABLE_1
};

const char* const RF_TABLE_ROW_LUT[] = {
		ROW_VG1_A1,
		ROW_VC_A1,
		ROW_VG1_A2,
		ROW_VC_A2,
		ROW_VG1_A3,
		ROW_VC_A3
};

const char* const RF_TABLE_CHANNEL_LUT[] = {
		CHANNEL_VG1_A1,
		CHANNEL_VC_A1,
		CHANNEL_VG1_A2,
		CHANNEL_VC_A2,
		CHANNEL_VG1_A3,
		CHANNEL_VC_A3
};

const DAC61408_CHANNEL DAC_CHANNEL_LUT[RF_TABLE_NUMBER_ROWS] = {
		DAC61408_CHANNEL__CH3,									// VG1_A1
		DAC61408_CHANNEL__CH2,									// VC_A1
		DAC61408_CHANNEL__CH0,									// VG1_A2
		DAC61408_CHANNEL__CH1,									// VC_A2
		DAC61408_CHANNEL__CH4,									// VG1_A3
		DAC61408_CHANNEL__CH5,									// VC_A3
};


const char* const RF_TABLE_COLUMN_LUT[RF_TABLE_NUMBER_COLUMNS] = {
		COLUMN_40_DEG,
		COLUMN_25_DEG,
		COLUMN_85_DEG
};

static const RF_TABLE_STRUCT RF_TABLE1_DEFAULT = {
		0, 									//start_calib is false
		{
		//		-40		25		85
			{	-1500,	-1000,	-500	},			// VG1_A1
			{	-1500,	0,		1500	},			// VC_A1
			{	-1500,	-1000,	-500	},			// VG1_A2
			{	-1500,	0,		1500	},			// VC_A2
			{	-1500,	-1000,	-500	},			// VG1_A3
			{	-1500,	0,		1500	}			// VC_A3
		}
};


int32_t app_rf_get_number_of_tables(void)
{
	return RF_TABLE_NUMBER_TABLES;
}


int32_t app_rf_get_number_of_rows(void)
{
	return RF_TABLE_NUMBER_ROWS;
}


float app_rf_get_temp_column_value_0(void)
{
	return TEMP_COLUMN_0;
}


float app_rf_get_temp_column_value_1(void)
{
	return TEMP_COLUMN_1;
}


float app_rf_get_temp_column_value_2(void)
{
	return TEMP_COLUMN_2;
}


int32_t app_rf_table_find_table(const char* table)
{
	int32_t i;

	for (i = 0;
		 i < RF_TABLE_NUMBER_TABLES && strcmp(table, RF_TABLE_TABLE_LUT[i]);
		 i++) {}

	if (i >= RF_TABLE_NUMBER_TABLES)
	{
		return -1;
	}
	else
	{
		return i;
	}
}


int32_t app_rf_table_find_row(const char* row)
{
	int32_t i;

	for (i = 0;
		 i < RF_TABLE_NUMBER_ROWS && strcmp(row, RF_TABLE_ROW_LUT[i]);
		 i++) {}

	if (i >= RF_TABLE_NUMBER_ROWS)
	{
		return -1;
	}
	else
	{
		return i;
	}
}


int32_t app_rf_table_find_column(const char* row)
{
	int32_t i;

	for (i = 0;
		 i < RF_TABLE_NUMBER_COLUMNS && strcmp(row, RF_TABLE_COLUMN_LUT[i]);
		 i++) {}

	if (i >= RF_TABLE_NUMBER_COLUMNS)
	{
		return -1;
	}
	else
	{
		return i;
	}
}


int32_t app_rf_table_set_default(RF_STRUCT_PTR RF)
{
	int32_t err;

	err = io_write(RF->TABLE_FILE[0], (uint8_t*)&RF_TABLE1_DEFAULT, sizeof(RF_TABLE_STRUCT));
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	err = io_ioctl(RF->TABLE_FILE[0], IO_IOCTL_SETTINGS__PULL, NULL);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	return RF_ERROR__OK;
}


int32_t app_rf_init(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR RF_DAC, IO_HANDLER_PTR TEMP_SENSOR)
{
int32_t err=0;

	HAL_GPIO_WritePin(EN_5_GPIO_Port, EN_5_Pin, GPIO_PIN_RESET);

	err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_DAC_HANDLER,(IO_HANDLER_PTR) RF_DAC);
	err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_TEMP_SENSOR_HANDLER,(IO_HANDLER_PTR) TEMP_SENSOR);
	err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_INIT_HANDLER,NULL);
	//rf_init(&OTS_APP->RF, RF_DAC, TEMP_SENSOR); //replace with new


	HAL_GPIO_WritePin(EN_3_3_GPIO_Port, EN_3_3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(EN_5_GPIO_Port, EN_5_Pin, GPIO_PIN_SET);


	return err;
}
float  dac_temperature;
//temperature can be calculated as TJ = 25?C + (1.4V ?VMUXOUT)/(3.7mV/?C).
//*************************************************************************************************************************************
