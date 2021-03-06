/**
 ******************************************************************************
 * @file    ltc2666.h
 * @brief   LTC2666 public declarations and definitions
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


#ifndef __LTC2666_H__
#define __LTC2666_H__

/* Includes */


/* Definitions */

// Error definitions
#define IO_ERROR_LTC2666__OK							IO_ERROR__OK
#define IO_ERROR_LTC2666__OUT_OF_HEAP					IO_ERROR_LTC2666(0x01)
#define IO_ERROR_LTC2666__UNKNOWN_HAL_DRIVER			IO_ERROR_LTC2666(0x02)
#define IO_ERROR_LTC2666__DRV_INDEX_OUT_OF_RANGE		IO_ERROR_LTC2666(0x03)
#define IO_ERROR_LTC2666__UNKNOWN_IOCTL					IO_ERROR_LTC2666(0x04)
#define IO_ERROR_LTC2666__OUT_OF_RANGE					IO_ERROR_LTC2666(0x05)
#define IO_ERROR_LTC2666__IBIT_FAILED					IO_ERROR_LTC2666(0x06)



// IOCTL definitions
//#define IO_IOCTL_LTC2666__SET_LOW						IO_IOCTL_LTC2666(0x01)
#define IO_IOCTL_LTC2666__POWER_DOWN					IO_IOCTL_LTC2666(0x02)
#define IO_IOCTL_LTC2666__POWER_UP						IO_IOCTL_LTC2666(0x03)
#define IO_IOCTL_LTC2666__SET_MUX						IO_IOCTL_LTC2666(0x04)



/* Typedefs */

typedef struct {
	uint32_t						temp;
} LTC2666_INSTALL_STRUCT, * LTC2666_INSTALL_STRUCT_PTR;


typedef enum {
	LTC2666_SPAN__10v_10v_mid_man,					// MSP=0 ::: +/-10V, mid-scale, manual span
	LTC2666_SPAN__5v_5v_mid_man,					// MSP=1 ::: +/-5V, mid-scale, manual span
	LTC2666_SPAN__2v5_2v5_mid_man,					// MSP=2 ::: +/-2.5V, mid-scale, manual span
	LTC2666_SPAN__0v_10v_zero_man,					// MSP=3 ::: 0V-10V, zero-scale, manual span
	LTC2666_SPAN__0v_10v_mid_man,					// MSP=4 ::: 0V-10V, mid-scale, manual span
	LTC2666_SPAN__0v_5v_zero_man,					// MSP=5 ::: 0V-5V, zero-scale, manual span
	LTC2666_SPAN__0v_5v_mid_man,					// MSP=6 ::: 0V-5V, mid-scale, manual span
	LTC2666_SPAN__0v_5v_zero_soft,					// MSP=7 ::: 0V-5V, zero-scale, soft-span

	LTC2666_SPAN_COUNT
} LTC2666_SPAN, * LTC2666_SPAN_PTR;

typedef enum {
	LTC2666_RES__12b,
	LTC2666_RES__16b,

	LTC2666_RES_COUNT
} LTC2666_RES, * LTC2666_RES_PTR;


typedef struct {
	IO_HANDLER_PTR					SPI;			// SPI handler using SPI mode 1 (CPOL=0; CPHA=1)
	GPIO_TypeDef*					LDAC_PORT;
	uint16_t						LDAC_PIN;
	LTC2666_SPAN					MSPAN;			// value of MSP2, MSP1, MSP0 bits
	LTC2666_RES						RES;			// Resolution
} LTC2666_INIT_STRUCT, * LTC2666_INIT_STRUCT_PTR;



typedef enum {
	LTC2666_CHANNEL__0,
	LTC2666_CHANNEL__1,
	LTC2666_CHANNEL__2,
	LTC2666_CHANNEL__3,
	LTC2666_CHANNEL__4,
	LTC2666_CHANNEL__5,
	LTC2666_CHANNEL__6,
	LTC2666_CHANNEL__7
} LTC2666_CHANNEL, * LTC2666_CHANNEL_PTR;


typedef struct {
	LTC2666_CHANNEL					CH;
	int16_t							VAL_MV;			// Signed 12-bit value in millivolts
	int16_t							DAC_Shadow[6];//RF_TABLE_NUMBER_ROWS];
} LTC2666_SAMPLE, * LTC2666_SAMPLE_PTR;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_ltc2666(LTC2666_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif


#endif	//__LTC2666_H__
