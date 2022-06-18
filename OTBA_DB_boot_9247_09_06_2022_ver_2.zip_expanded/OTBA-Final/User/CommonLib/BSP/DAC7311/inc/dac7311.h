/**
 ******************************************************************************
 * @file    dac7311.h
 * @brief   DAC7311 public declarations and definitions
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


#ifndef __DAC7311_H__
#define __DAC7311_H__

/* Includes */


/* Definitions */

// Error definitions
#define IO_ERROR_DAC7311__OK							IO_ERROR__OK
#define IO_ERROR_DAC7311__OUT_OF_HEAP					IO_ERROR_DAC7311(0x01)
#define IO_ERROR_DAC7311__UNKNOWN_HAL_DRIVER			IO_ERROR_DAC7311(0x02)
#define IO_ERROR_DAC7311__DRV_INDEX_OUT_OF_RANGE		IO_ERROR_DAC7311(0x03)
#define IO_ERROR_DAC7311__UNKNOWN_IOCTL					IO_ERROR_DAC7311(0x04)
#define IO_ERROR_DAC7311__INVALID_PARAMETER				IO_ERROR_DAC7311(0x05)


// IOCTL definitions
#define IO_IOCTL_DAC7311__POWER_DOWN					IO_IOCTL_DAC7311(0x01)



/* Typedefs */

typedef struct {
	uint32_t						temp;
} DAC7311_INSTALL_STRUCT, * DAC7311_INSTALL_STRUCT_PTR;


typedef struct {
	IO_HANDLER_PTR					SPI;			// SPI handler using SPI mode 1 (CPOL=0; CPHA=1)
} DAC7311_INIT_STRUCT, * DAC7311_INIT_STRUCT_PTR;



typedef uint16_t	DAC7311_SAMPLE, * DAC7311_SAMPLE_PTR;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_dac7311(DAC7311_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif


#endif	//__DAC7311_H__
