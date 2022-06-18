/**
 ******************************************************************************
 * @file    ltc2666_reg.h
 * @brief   LTC2666 registers declarations and definitions
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

 
#ifndef __LTC2666_REG_H__
#define __LTC2666_REG_H__

/* Includes */


/* Definitions */


#define LTC2666_REG_WRITE_BIT		(0x00)
#define LTC2666_REG_READ_BIT		(0x80)


#define LTC2666_DEVICE_ID			(0x0248)


#define LTC2666_RESOLUTION			(4096)		// 12 bit
#define LTC2666_VMAX_MV			(2500)		// +2.5V for the selected range
#define LTC2666_VMIN_MV			(-2500)		// -2.5V for the selected range
#define LTC2666_FSR				(LTC2666_VMAX_MV - LTC2666_VMIN_MV)	// Full scale range
#define LTC2666_MV_TO_COUNT(mv)	(int16_t)(((mv - LTC2666_VMIN_MV) * LTC2666_RESOLUTION) / LTC2666_FSR)



/* Typedefs */

typedef enum
{
	LTC2666_REG__WRITE_CODE_n						= 0x00,
	LTC2666_REG__WRITE_CODE_ALL						= 0x08,
	LTC2666_REG__WRITE_SPAN_n						= 0x06,
	LTC2666_REG__WRITE_SPAN_ALL						= 0x0E,
	LTC2666_REG__UPDATE_n_POWER_UP					= 0x01,
	LTC2666_REG__UPDATE_ALL_POWER_UP				= 0x09,
	LTC2666_REG__WRITE_CODE_n_UPDATE_n_POWER_UP		= 0x03,
	LTC2666_REG__WRITE_CODE_n_UPDATE_ALL_POWER_UP	= 0x02,
	LTC2666_REG__WRITE_CODE_ALL_UPDATE_ALL_POWER_UP	= 0x0A,
	LTC2666_REG__POWER_DOWN_n						= 0x04,
	LTC2666_REG__POWER_DOWN_CHIP					= 0x05,
	LTC2666_REG__ANALOG_MUX							= 0x0B,
	LTC2666_REG__TOGGLE_SELECT						= 0x0C,
	LTC2666_REG__GLOBAL_TOGGLE						= 0x0D,
	LTC2666_REG__CONFIG								= 0x07,
	LTC2666_REG__NOP								= 0x0F,
} LTC2666_CMD, * LTC2666_CMD_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif //__LTC2666_REG_H__
