/**
 ******************************************************************************
 * @file    opt_adc_phy.c
 * @brief   Optical ADC driver physical functions implementations
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
#include "../inc/opt_adc.h"
#include "../inc/opt_adc_private.h"

/* Global variables */
ADC_ChannelConfTypeDef sConfig;

/* Functions definitions */


static int32_t _opt_adc_start_adc(OPT_ADC_DRIVER_BUNDLE_PTR driver_bundle)
{
	HAL_StatusTypeDef hal_err;

	hal_err = HAL_ADC_ConfigChannel(driver_bundle->PHY.HAL_ADC, &driver_bundle->PHY.CH_CONFIG);
	if (hal_err != HAL_OK)
	{
		return IO_ERROR_OPT_ADC__FAILED_TO_CONF_PHY;
	}

	hal_err = HAL_ADC_Start(driver_bundle->PHY.HAL_ADC);
	if (hal_err != HAL_OK)
	{
		return IO_ERROR_OPT_ADC__FAILED_TO_CONF_PHY;
	}

	return IO_ERROR_OPT_ADC__OK;
}


static int32_t _opt_adc_stop_adc(OPT_ADC_DRIVER_BUNDLE_PTR driver_bundle)
{
	HAL_StatusTypeDef hal_err;

	hal_err = HAL_ADC_Stop(driver_bundle->PHY.HAL_ADC);
	if (hal_err != HAL_OK)
	{
		return IO_ERROR_OPT_ADC__FAILED_TO_CONF_PHY;
	}

	return IO_ERROR_OPT_ADC__OK;
}


int32_t _opt_adc_sample(OPT_ADC_DRIVER_BUNDLE_PTR driver_bundle, int32_t *buffer, uint32_t number_samples)
{
	uint32_t i;

	HAL_StatusTypeDef hal_err;
	int32_t err;
	// set Adc3 channel   Photodiode is defolt
if (sConfig.Channel != Photodiode)
			{err=	 ADC3_channel_select(sConfig.Channel);}
	err = _opt_adc_start_adc(driver_bundle);
	if (err != IO_ERROR_OPT_ADC__OK)
	{
		return err;
	}

	for (i = 0; i < number_samples; i++)
	{
		hal_err = HAL_ADC_PollForConversion(driver_bundle->PHY.HAL_ADC, 100);
		if (hal_err != HAL_OK)
		{
			goto _opt_adc_sample_complete;
		}

		buffer[i] = HAL_ADC_GetValue(driver_bundle->PHY.HAL_ADC);
	}

_opt_adc_sample_complete:
	_opt_adc_stop_adc(driver_bundle);
	if (sConfig.Channel != Photodiode)
				{err=	 ADC3_channel_select(Photodiode);}


	return err;
}


int32_t _opt_adc_init(OPT_ADC_DRIVER_BUNDLE_PTR driver_bundle)
{
	return IO_ERROR_OPT_ADC__OK;
}

int32_t ADC3_channel_select(uint32_t ADC_CHANNEL)
{
	int32_t	 err=0;
	int16_t	 dammy_read;
	 sConfig.Channel = ADC_CHANNEL_5;
	 if (ADC_CHANNEL==ADC_CHANNEL_5)
	 { sConfig.Rank = ADC_REGULAR_RANK_1;}
	 else
	 { sConfig.Rank = ADC_REGULAR_RANK_2;}

	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
		  err= HAL_ADC_ConfigChannel(&hadc3, &sConfig) ;
	//dammy read
		  HAL_ADC_Start(&hadc3);
		  HAL_ADC_PollForConversion(&hadc3,10);
		  dammy_read=HAL_ADC_GetValue(&hadc3);
		  return err;
}
