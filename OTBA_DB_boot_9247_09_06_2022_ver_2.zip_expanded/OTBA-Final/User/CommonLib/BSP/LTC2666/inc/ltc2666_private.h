/**
 ******************************************************************************
 * @file    ltc2666_private.h
 * @brief   LTC2666 private declarations and definitions
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

 
#ifndef __LTC2666_PRIVATE_H__
#define __LTC2666_PRIVATE_H__

/* Includes */


/* Definitions */



/* Typedefs */

typedef struct ltc2666_driver_generic {
	LTC2666_SPAN					MSPAN;			// value of MSP2, MSP1, MSP0 bits
	LTC2666_RES						RES;			// resolution
} LTC2666_DRIVER_GENERIC, * LTC2666_DRIVER_GENERIC_PTR;


typedef struct ltc2666_driver_phy {
	IO_HANDLER_PTR					SPI;

	GPIO_TypeDef*					LDAC_PORT;
	uint16_t						LDAC_PIN;
} LTC2666_DRIVER_PHY, * LTC2666_DRIVER_PHY_PTR;


typedef struct ltc2666_driver_bundle {
	LTC2666_DRIVER_PHY				PHY;
	LTC2666_DRIVER_GENERIC			g;
} LTC2666_DRIVER_BUNDLE, * LTC2666_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t ltc2666_initialize(LTC2666_DRIVER_BUNDLE_PTR driver_bundle);
int32_t ltc2666_write_channel_value(LTC2666_DRIVER_BUNDLE_PTR driver_bundle, LTC2666_SAMPLE_PTR sample);
int32_t ltc2666_write_all_channels_value(LTC2666_DRIVER_BUNDLE_PTR driver_bundle, int16_t value_mv);
int32_t ltc2666_power_down(LTC2666_DRIVER_BUNDLE_PTR driver_bundle);
int32_t ltc2666_set_mux(LTC2666_DRIVER_BUNDLE_PTR driver_bundle);

#ifdef __cplusplus
}
#endif

#endif //__LTC2666_PRIVATE_H__
