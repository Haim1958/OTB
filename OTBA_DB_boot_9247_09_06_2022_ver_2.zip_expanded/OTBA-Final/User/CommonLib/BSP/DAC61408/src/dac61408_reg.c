/**
 ******************************************************************************
 * @file    dac61408_phy.c
 * @brief   DAC61408 driver physical functions implementations
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
#include "../inc/dac61408.h"
#include "../inc/dac61408_reg.h"
#include "../inc/dac61408_private.h"

/* Global variables */


/* Functions definitions */


static int32_t _dac61408_write_register(DAC61408_DRIVER_BUNDLE_PTR driver_bundle, DAC61408_REG reg, uint16_t value)
{
	uint8_t buf[3] = {0};

	buf[0] = (uint8_t)reg | DAC61408_REG_WRITE_BIT;
	buf[1] = (uint8_t)(value >> 8);
	buf[2] = (uint8_t)value;

	return io_write(driver_bundle->PHY.SPI, buf, sizeof(buf));
}


static int32_t _dac61408_read_register(DAC61408_DRIVER_BUNDLE_PTR driver_bundle, DAC61408_REG reg, uint16_t *value)
{
	uint8_t buf[3] = {0};
	int32_t err;

	buf[0] = (uint8_t)reg | DAC61408_REG_READ_BIT;

	err = io_write(driver_bundle->PHY.SPI, buf, sizeof(buf));
	if (err != IO_ERROR_SPI_IO__OK)
	{
		return err;
	}

	// Any command
	memset(buf, 0, sizeof(buf));
	err = io_write(driver_bundle->PHY.SPI, buf, sizeof(buf));
	if (err != IO_ERROR_SPI_IO__OK)
	{
		return err;
	}

	err = io_read(driver_bundle->PHY.SPI, buf, sizeof(buf));
	if (err != IO_ERROR_SPI_IO__OK)
	{
		return err;
	}

	// Shall be Echo from previous access cycle
	if ((buf[0] & 0x1F) != reg)
	{
		return IO_ERROR_DAC61408__FAILED_READING_REGISTER;
	}

	*value = ((uint16_t)buf[1] << 8) + buf[2];

	return IO_ERROR_DAC61408__OK;
}


int32_t dac61408_write_value(DAC61408_DRIVER_BUNDLE_PTR driver_bundle, DAC61408_SAMPLE_PTR sample)
{
	uint16_t reg_val;
	DAC61408_REG reg_addr;

	switch (sample->CH)
	{
		case DAC61408_CHANNEL__CH0:	reg_addr = DAC61408_REG__DAC0; break;
		case DAC61408_CHANNEL__CH1:	reg_addr = DAC61408_REG__DAC1; break;
		case DAC61408_CHANNEL__CH2:	reg_addr = DAC61408_REG__DAC2; break;
		case DAC61408_CHANNEL__CH3:	reg_addr = DAC61408_REG__DAC3; break;
		case DAC61408_CHANNEL__CH4:	reg_addr = DAC61408_REG__DAC4; break;
		case DAC61408_CHANNEL__CH5:	reg_addr = DAC61408_REG__DAC5; break;
		case DAC61408_CHANNEL__CH6:	reg_addr = DAC61408_REG__DAC6; break;
		case DAC61408_CHANNEL__CH7:	reg_addr = DAC61408_REG__DAC7; break;
		default: return IO_ERROR_DAC61408__INVALID_PARAMETER;
	}

	reg_val = DAC61408_MV_TO_COUNT(sample->VAL_MV) << 4;

	return _dac61408_write_register(driver_bundle, reg_addr, reg_val);
}


static int32_t dac61408_ibit(DAC61408_DRIVER_BUNDLE_PTR driver_bundle)
{
	uint16_t device_id;
	int32_t err;

	// Read device id
	err = _dac61408_read_register(driver_bundle, DAC61408_REG__DEVICEID, &device_id);
	if (err != IO_ERROR_DAC61408__OK)
	{
		return err;
	}

	device_id = (device_id >> 2) & 0x3FFF;

	if (device_id != DAC61408_DEVICE_ID)
	{
		return IO_ERROR_DAC61408__UNEXPECTED_DEVICE_ID;
	}

	return IO_ERROR_DAC61408__OK;
}


static int32_t _dac61408_enable_internal_reference(DAC61408_DRIVER_BUNDLE_PTR driver_bundle, uint8_t enable)
{
	// Enable Internal Reference
	uint16_t enable_mask = enable ? 0x4000 : 0x0000;
	uint16_t reg_val = 0x7F00 & ~enable_mask;		// REF-PWDWN bit disabled

	return _dac61408_write_register(driver_bundle, DAC61408_REG__GENCONFIG, reg_val);
}


static int32_t _dac61408_configure_channels(DAC61408_DRIVER_BUNDLE_PTR driver_bundle, uint8_t enabled_channels)
{
	uint16_t reg_val;
	int32_t err;

	// Configure channels range
	reg_val = 0xEEEE;							// DACn-RANGE set to -2.5V to +2.5V
	err = _dac61408_write_register(driver_bundle, DAC61408_REG__DACRANGE0, reg_val);
	if (err != IO_ERROR_DAC61408__OK)
	{
		return err;
	}

	for (uint32_t ch = 0; ch < 8; ch++)
	{
		DAC61408_SAMPLE sample;
		sample.CH = ch;
		sample.VAL_MV = 0;
		dac61408_write_value(driver_bundle, &sample);
	}

	err = _dac61408_write_register(driver_bundle, DAC61408_REG__DACRANGE1, reg_val);
	if (err != IO_ERROR_DAC61408__OK)
	{
		return err;
	}

	// Enable channels
	reg_val = 0xFFFF & ~(((uint16_t)enabled_channels) << 4);		// Disable DACn-PWDWN bits

	return _dac61408_write_register(driver_bundle, DAC61408_REG__DACPWDWN, reg_val);
}


static int32_t dac61408_configure(DAC61408_DRIVER_BUNDLE_PTR driver_bundle)
{
	int32_t err;

	err = _dac61408_configure_channels(driver_bundle, driver_bundle->g.enabled_ch);
	if (err != IO_ERROR_DAC61408__OK)
	{
		return err;
	}

	err = _dac61408_enable_internal_reference(driver_bundle, driver_bundle->g.internal_ref);
	if (err != IO_ERROR_DAC61408__OK)
	{
		return err;
	}

	return IO_ERROR_DAC61408__OK;
}


int32_t dac61408_power_up(DAC61408_DRIVER_BUNDLE_PTR driver_bundle)
{
	uint16_t reg_val = 0x0AA4 & ~0x0020;		// DEV-PWDWN bit disabled
	
	return _dac61408_write_register(driver_bundle, DAC61408_REG__SPICONFIG, reg_val);
}


int32_t dac61408_power_down(DAC61408_DRIVER_BUNDLE_PTR driver_bundle) //LB TODO:This is a hack
{
	int32_t err;
	DAC61408_SAMPLE sample;
	sample.VAL_MV = 0;

	for(int i=0;i<8;i++)
	{
		sample.CH = (DAC61408_CHANNEL)i;

		err = dac61408_write_value(driver_bundle, &sample);

		if(err!= IO_ERROR_DAC61408__OK)
			return err;
	}

	return IO_ERROR_DAC61408__OK;
}


static int32_t _dac61408_reset(DAC61408_DRIVER_BUNDLE_PTR driver_bundle)
{
	HAL_GPIO_WritePin(driver_bundle->PHY.RESET_PORT, driver_bundle->PHY.RESET_PIN, GPIO_PIN_RESET);
	for (uint32_t i = 0; i < 100000; i++);
	
	HAL_GPIO_WritePin(driver_bundle->PHY.RESET_PORT, driver_bundle->PHY.RESET_PIN, GPIO_PIN_SET);
	for (uint32_t i = 0; i < 100000; i++);
	
	return IO_ERROR_DAC61408__OK;
}


int32_t dac61408_initialize(DAC61408_DRIVER_BUNDLE_PTR driver_bundle)
{
	int32_t err;

	HAL_GPIO_WritePin(driver_bundle->PHY.LDAC_PORT, driver_bundle->PHY.LDAC_PIN, GPIO_PIN_SET);

	err = _dac61408_reset(driver_bundle);
	if (err != IO_ERROR_DAC61408__OK)
	{
		return err;
	}
	
	err = dac61408_power_up(driver_bundle);
	if (err != IO_ERROR_DAC61408__OK)
	{
		return err;
	}

	err = dac61408_power_down(driver_bundle);
	if (err != IO_ERROR_DAC61408__OK)
	{
		return err;
	}

	err = dac61408_ibit(driver_bundle);
	if (err != IO_ERROR_DAC61408__OK)
	{
		return err;
	}

	return dac61408_configure(driver_bundle);
}
