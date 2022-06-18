/**
 ******************************************************************************
 * @file    dac7311_private.h
 * @brief   DAC7311 private declarations and definitions
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

 
#ifndef __DAC7311_PRIVATE_H__
#define __DAC7311_PRIVATE_H__

/* Includes */


/* Definitions */



/* Typedefs */

typedef struct dac7311_driver_generic {
	uint32_t						temp;
} DAC7311_DRIVER_GENERIC, * DAC7311_DRIVER_GENERIC_PTR;


typedef struct dac7311_driver_phy {
	IO_HANDLER_PTR					SPI;
} DAC7311_DRIVER_PHY, * DAC7311_DRIVER_PHY_PTR;


typedef struct dac7311_driver_bundle {
	DAC7311_DRIVER_PHY				PHY;
	DAC7311_DRIVER_GENERIC			g;
} DAC7311_DRIVER_BUNDLE, * DAC7311_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t dac7311_initialize(DAC7311_DRIVER_BUNDLE_PTR driver_bundle);
int32_t dac7311_write_value(DAC7311_DRIVER_BUNDLE_PTR driver_bundle, DAC7311_SAMPLE sample);
int32_t dac7311_power_down(DAC7311_DRIVER_BUNDLE_PTR driver_bundle);

#ifdef __cplusplus
}
#endif

#endif //__DAC7311_PRIVATE_H__
