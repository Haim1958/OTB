/**
 ******************************************************************************
 * @file    cpu_temp_phy.c
 * @brief   CPU Temperature driver physical functions implementations
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



/* Includes */
#include <string.h>
#include <assert.h>
#include "UserLib.h"
#include "../inc/cpu_temp.h"
#include "../inc/cpu_temp_private.h"

/* Global variables */


/* Functions definitions */

static int32_t _cpu_temp_configure_adc(CPU_TEMP_DRIVER_BUNDLE_PTR driver_bundle)
{
	ADC_ChannelConfTypeDef config = {0};

	config.Channel = ADC_CHANNEL_TEMPSENSOR;
	config.Rank = ADC_REGULAR_RANK_1;
	config.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if (HAL_ADC_ConfigChannel(driver_bundle->PHY.HAL_ADC, &config) != HAL_OK)
	{
		return IO_ERROR_CPU_TEMP__FAILED_TO_CONF_PHY;
	}

	return IO_ERROR_CPU_TEMP__OK;
}


static int32_t _cpu_temp_sample_adc(CPU_TEMP_DRIVER_BUNDLE_PTR driver_bundle)
{
	HAL_StatusTypeDef hal_err;

	__HAL_ADC_ENABLE(driver_bundle->PHY.HAL_ADC);

	// Disable Vbat signal from input channel and wake up temp sensor from power down mode
	ADC->CCR |= ADC_CCR_TSVREFE;
	ADC->CCR &= ~ADC_CCR_VBATE ;

	hal_err = HAL_ADC_Start(driver_bundle->PHY.HAL_ADC);
	if (hal_err != HAL_OK)
	{
		return IO_ERROR_CPU_TEMP__FAILED_TO_CONF_PHY;
	}

	hal_err = HAL_ADC_PollForConversion(driver_bundle->PHY.HAL_ADC, 100);
	HAL_ADC_Stop(driver_bundle->PHY.HAL_ADC);
	if (hal_err == HAL_OK)
	{
	    driver_bundle->g.raw_reading = HAL_ADC_GetValue(driver_bundle->PHY.HAL_ADC);
	    return IO_ERROR_CPU_TEMP__OK;
	}
	else
	{
		driver_bundle->g.raw_reading = 0;
		return IO_ERROR_CPU_TEMP__FAILED_TO_READ_ADC;
	}
}


static int32_t _cpu_temp_calculate_temperature(CPU_TEMP_DRIVER_BUNDLE_PTR driver_bundle)
{
// Datasheet STM32F745xx STM32F746xx page 151.
#define TEMP110                                                     (110.0f)
#define TEMP30                                                      (30.0f)

	// scale constants to current reference voltage
	float sensor_value = (float)driver_bundle->g.raw_reading;

//	driver_bundle->g.calibrated_temperature = ((TEMP110 - TEMP30) * (sensor_value - adcCalTemp30C) / (adcCalTemp110C - adcCalTemp30C)) + TEMP30;
	driver_bundle->g.calibrated_temperature = ((TEMP110 - TEMP30) * (sensor_value - driver_bundle->PHY.CAL_ADC_TEMP_3V3_30C) / (driver_bundle->PHY.CAL_ADC_TEMP_3V3_110C - driver_bundle->PHY.CAL_ADC_TEMP_3V3_30C)) + TEMP30;
//	driver_bundle->g.calibrated_temperature = ((TEMP110 - TEMP30) * (sensor_value - 940.0f) / (1200.0f - 940.0f)) + TEMP30;

//	driver_bundle->g.calibrated_temperature = 42.0;

	return IO_ERROR_CPU_TEMP__OK;
}


int32_t _cpu_temp_sample_temperature(CPU_TEMP_DRIVER_BUNDLE_PTR driver_bundle)
{
	int32_t err;

	err = _cpu_temp_configure_adc(driver_bundle);
	if (err != IO_ERROR_CPU_TEMP__OK)
	{
		return err;
	}

	err = _cpu_temp_sample_adc(driver_bundle);
	if (err != IO_ERROR_CPU_TEMP__OK)
	{
		return err;
	}

	err = _cpu_temp_calculate_temperature(driver_bundle);
	if (err != IO_ERROR_CPU_TEMP__OK)
	{
		return err;
	}

	return IO_ERROR_CPU_TEMP__OK;
}


int32_t _cpu_temp_init(CPU_TEMP_DRIVER_BUNDLE_PTR driver_bundle)
{
	const float CALIBRATION_REFERENCE_VOLTAGE = 3300.0f;

	driver_bundle->PHY.CAL_ADC_TEMP_3V3_30C = (float)(*(uint16_t*)((uint32_t)0x1FF0F44C)) * (driver_bundle->PHY.VREF_MV / CALIBRATION_REFERENCE_VOLTAGE);		// Given calibration value, acquired at 30 deg., vdda=3.3V
	driver_bundle->PHY.CAL_ADC_TEMP_3V3_110C = (float)(*(uint16_t*)((uint32_t)0x1FF0F44E)) * (driver_bundle->PHY.VREF_MV / CALIBRATION_REFERENCE_VOLTAGE);		// Given calibration value, acquired at 110 deg., vdda=3.3V

	return IO_ERROR_CPU_TEMP__OK;
}
