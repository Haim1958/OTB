/**
 ******************************************************************************
 * @file    dac61408_private.h
 * @brief   DAC61408 private declarations and definitions
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

 
#ifndef __DAC61408_PRIVATE_H__
#define __DAC61408_PRIVATE_H__

/* Includes */


/* Definitions */



/* Typedefs */

typedef struct dac61408_driver_generic {
	uint8_t							enabled_ch;		// Enabled channels bitmap; LSbit - CH0; MSbit - CH7
	uint8_t							internal_ref;	// Internal reference enabled: 0 - disabled; 1 - enabled
} DAC61408_DRIVER_GENERIC, * DAC61408_DRIVER_GENERIC_PTR;


typedef struct dac61408_driver_phy {
	IO_HANDLER_PTR					SPI;

	GPIO_TypeDef*					RESET_PORT;
	uint16_t						RESET_PIN;

	GPIO_TypeDef*					LDAC_PORT;
	uint16_t						LDAC_PIN;
} DAC61408_DRIVER_PHY, * DAC61408_DRIVER_PHY_PTR;


typedef struct dac61408_driver_bundle {
	DAC61408_DRIVER_PHY				PHY;
	DAC61408_DRIVER_GENERIC			g;
} DAC61408_DRIVER_BUNDLE, * DAC61408_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t dac61408_initialize(DAC61408_DRIVER_BUNDLE_PTR driver_bundle);
int32_t dac61408_write_value(DAC61408_DRIVER_BUNDLE_PTR driver_bundle, DAC61408_SAMPLE_PTR sample);
int32_t dac61408_power_down(DAC61408_DRIVER_BUNDLE_PTR driver_bundle);
int32_t dac61408_power_up(DAC61408_DRIVER_BUNDLE_PTR driver_bundle);

#ifdef __cplusplus
}
#endif

#endif //__DAC61408_PRIVATE_H__
