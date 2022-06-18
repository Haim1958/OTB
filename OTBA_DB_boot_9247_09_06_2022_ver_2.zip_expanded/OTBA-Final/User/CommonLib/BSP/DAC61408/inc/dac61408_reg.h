/**
 ******************************************************************************
 * @file    dac61408_reg.h
 * @brief   DAC61408 registers declarations and definitions
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

 
#ifndef __DAC61408_REG_H__
#define __DAC61408_REG_H__

/* Includes */


/* Definitions */


#define DAC61408_REG_WRITE_BIT		(0x00)
#define DAC61408_REG_READ_BIT		(0x80)


#define DAC61408_DEVICE_ID			(0x0248)


#define DAC61408_RESOLUTION			(4096)		// 12 bit
#define DAC61408_VMAX_MV			(2500)		// +2.5V for the selected range
#define DAC61408_VMIN_MV			(-2500)		// -2.5V for the selected range
#define DAC61408_FSR				(DAC61408_VMAX_MV - DAC61408_VMIN_MV)	// Full scale range
#define DAC61408_MV_TO_COUNT(mv)	(int16_t)(((mv - DAC61408_VMIN_MV) * DAC61408_RESOLUTION) / DAC61408_FSR)



/* Typedefs */

typedef enum
{
	DAC61408_REG__NOP				= 0x00,
	DAC61408_REG__DEVICEID			= 0x01,
	DAC61408_REG__STATUS			= 0x02,
	DAC61408_REG__SPICONFIG			= 0x03,
	DAC61408_REG__GENCONFIG			= 0x04,
	DAC61408_REG__BRDCONFIG			= 0x05,
	DAC61408_REG__SYNCONFIG			= 0x06,
	DAC61408_REG__TOGGCONFIG0		= 0x07,
	DAC61408_REG__TOGGCONFIG1		= 0x08,
	DAC61408_REG__DACPWDWN			= 0x09,
	DAC61408_REG__DACRANGE0			= 0x0B,
	DAC61408_REG__DACRANGE1			= 0x0C,
	DAC61408_REG__TRIGGER			= 0x0E,
	DAC61408_REG__BRDCAST			= 0x0F,
	DAC61408_REG__DAC0				= 0x14,
	DAC61408_REG__DAC1				= 0x15,
	DAC61408_REG__DAC2				= 0x16,
	DAC61408_REG__DAC3				= 0x17,
	DAC61408_REG__DAC4				= 0x18,
	DAC61408_REG__DAC5				= 0x19,
	DAC61408_REG__DAC6				= 0x1A,
	DAC61408_REG__DAC7				= 0x1B,
	DAC61408_REG__OFFSET0			= 0x21,
	DAC61408_REG__OFFSET1			= 0x22,
} DAC61408_REG, * DAC61408_REG_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif //__DAC61408_REG_H__
