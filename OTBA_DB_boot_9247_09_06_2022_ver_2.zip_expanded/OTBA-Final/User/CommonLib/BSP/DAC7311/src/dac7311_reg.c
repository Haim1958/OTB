/**
 ******************************************************************************
 * @file    dac7311_phy.c
 * @brief   DAC7311 driver physical functions implementations
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
#include "../inc/dac7311.h"
#include "../inc/dac7311_reg.h"
#include "../inc/dac7311_private.h"

/* Global variables */


/* Functions definitions */


static int32_t _dac7311_write_register(DAC7311_DRIVER_BUNDLE_PTR driver_bundle, uint16_t value)
{
	uint8_t buf[2] = {0};

	buf[0] = (uint8_t)(value >> 8);
	buf[1] = (uint8_t)value;

	return io_write(driver_bundle->PHY.SPI, buf, sizeof(buf));
}


int32_t dac7311_write_value(DAC7311_DRIVER_BUNDLE_PTR driver_bundle, DAC7311_SAMPLE sample)
{
	uint16_t reg_val;

	reg_val = (sample & 0x0FFF) << 2;

	return _dac7311_write_register(driver_bundle, reg_val);
}


int32_t dac7311_power_down(DAC7311_DRIVER_BUNDLE_PTR driver_bundle)
{
	uint16_t reg_val = 0xC000;
	
	return _dac7311_write_register(driver_bundle, reg_val);
}


int32_t dac7311_initialize(DAC7311_DRIVER_BUNDLE_PTR driver_bundle)
{
	return IO_ERROR_DAC7311__OK;
}
