/**
 ******************************************************************************
 * @file    io_errors.h
 * @brief   IO errors definitions
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

	 
	 
#ifndef __IO_ERRORS_H__
#define __IO_ERRORS_H__

/* Includes */

#include "../../inc/errors.h"

/* Typedefs */


#define IO_ERROR__OK				ERROR_OK
#define IO_ERROR_HANDLER(n)			(IO_ERROR(0x00000100) | n)
#define IO_ERROR_TCPCOM(n)			(IO_ERROR(0x00000200) | n)
#define IO_ERROR_SPI_IO(n)			(IO_ERROR(0x00000300) | n)
#define IO_ERROR_FPGA_OTBA(n)		(IO_ERROR(0x00000400) | n)
#define IO_ERROR_FPGA_OTBB(n)		(IO_ERROR(0x00000500) | n)
#define IO_ERROR_FPGA_OTBC(n)		(IO_ERROR(0x00000600) | n)
#define IO_ERROR_UART_IO(n)			(IO_ERROR(0x00000700) | n)
#define IO_ERROR_TELNET(n)			(IO_ERROR(0x00000800) | n)
#define IO_ERROR_CLI(n)				(IO_ERROR(0x00000900) | n)
#define IO_ERROR_SERCOM(n)			(IO_ERROR(0x00000A00) | n)
#define IO_ERROR_CPU_TEMP(n)		(IO_ERROR(0x00000B00) | n)
#define IO_ERROR_I2C_IO(n)			(IO_ERROR(0x00000C00) | n)
#define IO_ERROR_M24512(n)			(IO_ERROR(0x00000D00) | n)
#define IO_ERROR_SETTINGS(n)		(IO_ERROR(0x00000E00) | n)
#define IO_ERROR_DAC61408(n)		(IO_ERROR(0x00000F00) | n)
#define IO_ERROR_DAC7311(n)			(IO_ERROR(0x00001000) | n)
#define IO_ERROR_OPT_ADC(n)			(IO_ERROR(0x00001100) | n)
#define IO_ERROR_LTC2666(n)			(IO_ERROR(0x00001200) | n)
#define IO_ERROR_OTBBCOM(n)			(IO_ERROR(0x00001300) | n)
#define IO_ERROR_ADS8681_OTBA(n)	(IO_ERROR(0x00001400) | n)
/* Declarations */


#endif //__IO_ERRORS_H__
