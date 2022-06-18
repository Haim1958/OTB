/**
 ******************************************************************************
 * @file    opt_adc_private.h
 * @brief   Optical ADC private declarations and definitions
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

 
#ifndef __OPT_ADC_PRIVATE_H__
#define __OPT_ADC_PRIVATE_H__

/* Includes */


/* Definitions */



/* Typedefs */

typedef struct opt_adc_driver_generic {
	uint32_t						raw_reading;
	float							calibrated_temperature;
	uint8_t							cool_start_temp;
	uint8_t							cool_stop_temp;
} OPT_ADC_DRIVER_GENERIC, * OPT_ADC_DRIVER_GENERIC_PTR;

typedef struct opt_adc_driver_phy {
	ADC_HandleTypeDef*				HAL_ADC;						// ST ADC driver handler
	ADC_ChannelConfTypeDef			CH_CONFIG;
	float							VREF_MV;
} OPT_ADC_DRIVER_PHY, * OPT_ADC_DRIVER_PHY_PTR;

typedef struct opt_adc_driver_bundle {
	OPT_ADC_DRIVER_PHY				PHY;
	OPT_ADC_DRIVER_GENERIC			g;
} OPT_ADC_DRIVER_BUNDLE, * OPT_ADC_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif


int32_t _opt_adc_init(OPT_ADC_DRIVER_BUNDLE_PTR driver_bundle);
int32_t _opt_adc_sample(OPT_ADC_DRIVER_BUNDLE_PTR driver_bundle, int32_t *samples_buf, uint32_t samples_count);

#ifdef __cplusplus
}
#endif

#endif //__OPT_ADC_PRIVATE_H__
