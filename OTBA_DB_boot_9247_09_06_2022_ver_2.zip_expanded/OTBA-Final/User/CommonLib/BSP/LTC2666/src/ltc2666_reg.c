/**
 ******************************************************************************
 * @file    ltc2666_reg.c
 * @brief   LTC2666 driver physical functions implementations
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
#include "../inc/ltc2666.h"
#include "../inc/ltc2666_reg.h"
#include "../inc/ltc2666_private.h"

/* Global variables */


/* Functions definitions */


static int32_t _ltc2666_write_command_24b(LTC2666_DRIVER_BUNDLE_PTR driver_bundle, LTC2666_CMD cmd, LTC2666_CHANNEL channel, uint16_t value)
{
	uint8_t buf[3] = {0};

	buf[0] = ((cmd & 0x0F) << 4) | (channel & 0x0F);
	buf[1] = (uint8_t)(value >> 8);
	buf[2] = (uint8_t)value;

	return io_write(driver_bundle->PHY.SPI, buf, sizeof(buf));
}


// Note: the difference between 32b and 24b is that after 32b command, the chip will return the first 32b command data on the MISO during the transmission of the second 32b command
static int32_t _ltc2666_write_command_32b(LTC2666_DRIVER_BUNDLE_PTR driver_bundle, LTC2666_CMD cmd, LTC2666_CHANNEL channel, uint16_t value)
{
	uint8_t buf[4] = {0};

	buf[0] = 0;	// don't care
	buf[1] = ((cmd & 0x0F) << 4) | (channel & 0x0F);
	buf[2] = (uint8_t)(value >> 8);
	buf[3] = (uint8_t)value;

	return io_write(driver_bundle->PHY.SPI, buf, sizeof(buf));
}


static int32_t _ltc2666_mv_to_counts(LTC2666_DRIVER_BUNDLE_PTR driver_bundle, int16_t mv, uint16_t *counts)
{
	static const uint16_t MAX_COUNT_LUT[LTC2666_RES_COUNT] = {
			0x0FFF,		// 12 bit
			0xFFFF		// 16 bit
	};
	struct span_desc {
		int16_t MIN_MV;
		int16_t MAX_MV;
	};
	static const struct span_desc SPAN_DESC_LUT[LTC2666_SPAN_COUNT] = {
			{-10000,	10000},		// LTC2666_SPAN__10v_10v_mid_man,					// MSP=0 ::: +/-10V, mid-scale, manual span
			{-5000,		5000},		// LTC2666_SPAN__5v_5v_mid_man,						// MSP=1 ::: +/-5V, mid-scale, manual span
			{-2500,		2500},		// LTC2666_SPAN__2v5_2v5_mid_man,					// MSP=2 ::: +/-2.5V, mid-scale, manual span
			{0,			10000},		// LTC2666_SPAN__0v_10v_zero_man,					// MSP=3 ::: 0V-10V, zero-scale, manual span
			{0,			10000},		// LTC2666_SPAN__0v_10v_mid_man,					// MSP=4 ::: 0V-10V, mid-scale, manual span
			{0,			5000},		// LTC2666_SPAN__0v_5v_zero_man,					// MSP=5 ::: 0V-5V, zero-scale, manual span
			{0,			5000},		// LTC2666_SPAN__0v_5v_mid_man,						// MSP=6 ::: 0V-5V, mid-scale, manual span
			{0,			5000},		// LTC2666_SPAN__0v_5v_zero_soft,					// MSP=7 ::: 0V-5V, zero-scale, soft-span
	};
	struct span_desc span = SPAN_DESC_LUT[driver_bundle->g.MSPAN];
	if (mv < span.MIN_MV || mv > span.MAX_MV)
	{
		return IO_ERROR_LTC2666__OUT_OF_RANGE;
	}

	uint16_t full_scale = span.MAX_MV - span.MIN_MV;
	uint32_t max_count = MAX_COUNT_LUT[driver_bundle->g.RES];
	*counts = (uint16_t)(((uint32_t)(mv - span.MIN_MV) * max_count) / full_scale);

	return IO_ERROR_LTC2666__OK;
}


int32_t ltc2666_write_channel_value(LTC2666_DRIVER_BUNDLE_PTR driver_bundle, LTC2666_SAMPLE_PTR sample)
{
	uint16_t counts = 0;
	int32_t err;

	err = _ltc2666_mv_to_counts(driver_bundle, sample->VAL_MV, &counts);
	if (err != IO_ERROR_LTC2666__OK)
	{
		return err;
	}

	return _ltc2666_write_command_24b(driver_bundle, LTC2666_REG__WRITE_CODE_n_UPDATE_n_POWER_UP, sample->CH, counts);
}


int32_t ltc2666_write_all_channels_value(LTC2666_DRIVER_BUNDLE_PTR driver_bundle, int16_t value_mv)
{
	uint16_t counts = 0;
	int32_t err;

	err = _ltc2666_mv_to_counts(driver_bundle, value_mv, &counts);
	if (err != IO_ERROR_LTC2666__OK)
	{
		return err;
	}

	return _ltc2666_write_command_24b(driver_bundle, LTC2666_REG__WRITE_CODE_ALL_UPDATE_ALL_POWER_UP, 0, counts);
}


static int32_t ltc2666_ibit(LTC2666_DRIVER_BUNDLE_PTR driver_bundle)
{
	uint16_t value = 0xA581;
	int32_t err;

	// Need to write the command twice. Upon the second write, the data shifted in is the copy of the first command
	err = _ltc2666_write_command_32b(driver_bundle, LTC2666_REG__NOP, LTC2666_CHANNEL__7, value);
	err = _ltc2666_write_command_32b(driver_bundle, LTC2666_REG__NOP, LTC2666_CHANNEL__7, value);
	if (err != IO_ERROR_LTC2666__OK)
	{
		return err;
	}

	uint8_t buf[4] = {0};
	err = io_read(driver_bundle->PHY.SPI, buf, 4);
	if (buf[0] == 0 &&
			buf[1] == ((LTC2666_REG__NOP << 4) | LTC2666_CHANNEL__7) &&
			buf[2] == (uint8_t)(value >> 8) &&
			buf[3] == (uint8_t)value)
	{
		return IO_ERROR_LTC2666__OK;
	}
	else
	{
		return IO_ERROR_LTC2666__IBIT_FAILED;
	}
}


int32_t ltc2666_power_down(LTC2666_DRIVER_BUNDLE_PTR driver_bundle)
{
	return _ltc2666_write_command_24b(driver_bundle, LTC2666_REG__POWER_DOWN_CHIP, LTC2666_CHANNEL__0, 0);
}

int32_t ltc2666_set_MUX(LTC2666_DRIVER_BUNDLE_PTR driver_bundle)
{
	return _ltc2666_write_command_24b(driver_bundle, LTC2666_REG__ANALOG_MUX, LTC2666_CHANNEL__0, 0x1A);
}

int32_t ltc2666_initialize(LTC2666_DRIVER_BUNDLE_PTR driver_bundle)
{
	int32_t err;

	HAL_GPIO_WritePin(driver_bundle->PHY.LDAC_PORT, driver_bundle->PHY.LDAC_PIN, GPIO_PIN_SET);

	err = ltc2666_power_down(driver_bundle);
	if (err != IO_ERROR_LTC2666__OK)
	{
		return err;
	}

	return ltc2666_ibit(driver_bundle);
}
