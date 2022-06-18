/**
 ******************************************************************************
 * @file    dac61408.h
 * @brief   DAC61408 public declarations and definitions
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


#ifndef __DAC61408_H__
#define __DAC61408_H__

/* Includes */


/* Definitions */

// Error definitions
#define IO_ERROR_DAC61408__OK							IO_ERROR__OK
#define IO_ERROR_DAC61408__OUT_OF_HEAP					IO_ERROR_DAC61408(0x01)
#define IO_ERROR_DAC61408__UNKNOWN_HAL_DRIVER			IO_ERROR_DAC61408(0x02)
#define IO_ERROR_DAC61408__DRV_INDEX_OUT_OF_RANGE		IO_ERROR_DAC61408(0x03)
#define IO_ERROR_DAC61408__UNKNOWN_IOCTL				IO_ERROR_DAC61408(0x04)
#define IO_ERROR_DAC61408__FAILED_READING_REGISTER		IO_ERROR_DAC61408(0x05)
#define IO_ERROR_DAC61408__UNEXPECTED_DEVICE_ID			IO_ERROR_DAC61408(0x06)
#define IO_ERROR_DAC61408__INVALID_PARAMETER			IO_ERROR_DAC61408(0x07)


// IOCTL definitions
//#define IO_IOCTL_DAC61408__SET_LOW					IO_IOCTL_DAC61408(0x01)
#define IO_IOCTL_DAC61408__POWER_DOWN					IO_IOCTL_DAC61408(0x02)
#define IO_IOCTL_DAC61408__POWER_UP						IO_IOCTL_DAC61408(0x03)



/* Typedefs */

typedef struct {
	uint32_t						temp;
} DAC61408_INSTALL_STRUCT, * DAC61408_INSTALL_STRUCT_PTR;


typedef struct {
	IO_HANDLER_PTR					SPI;			// SPI handler using SPI mode 1 (CPOL=0; CPHA=1)
	GPIO_TypeDef*					RESET_PORT;
	uint16_t						RESET_PIN;
	GPIO_TypeDef*					LDAC_PORT;
	uint16_t						LDAC_PIN;

	uint8_t							enabled_ch;		// Enabled channels bitmap; LSbit - CH0; MSbit - CH7
	uint8_t							internal_ref;	// Internal reference enabled: 0 - disabled; 1 - enabled
} DAC61408_INIT_STRUCT, * DAC61408_INIT_STRUCT_PTR;



typedef enum
{
	DAC61408_CHANNEL__CH0,
	DAC61408_CHANNEL__CH1,
	DAC61408_CHANNEL__CH2,
	DAC61408_CHANNEL__CH3,
	DAC61408_CHANNEL__CH4,
	DAC61408_CHANNEL__CH5,
	DAC61408_CHANNEL__CH6,
	DAC61408_CHANNEL__CH7,
} DAC61408_CHANNEL;


typedef struct {
	DAC61408_CHANNEL				CH;
	int16_t							VAL_MV;			// Signed 12-bit value in millivolts
} DAC61408_SAMPLE, * DAC61408_SAMPLE_PTR;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_dac61408(DAC61408_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif


#endif	//__DAC61408_H__
