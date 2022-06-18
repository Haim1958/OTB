/**
 ******************************************************************************
 * @file    io_ioctl.h
 * @brief   IO Control definitions
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


	 
	 
#ifndef __IO_IOCTL_H__
#define __IO_IOCTL_H__

/* Includes */

/* Typedefs */

#define IO_IOCTL_BIT				(0x00010000)
#define IO_IOCTL(n)					(IO_IOCTL_BIT | (n & 0x0000FFFF))

#define IO_IOCTL_HANDLER(n)			(IO_IOCTL(0x00000100) | n)
#define IO_IOCTL_TCPCOM(n)			(IO_IOCTL(0x00000200) | n)
#define IO_IOCTL_SPI_IO(n)			(IO_IOCTL(0x00000300) | n)
#define IO_IOCTL_FPGA_OTBA(n)		(IO_IOCTL(0x00000400) | n)
#define IO_IOCTL_FPGA_OTBB(n)		(IO_IOCTL(0x00000500) | n)
#define IO_IOCTL_FPGA_OTBC(n)		(IO_IOCTL(0x00000600) | n)
#define IO_IOCTL_UART_IO(n)			(IO_IOCTL(0x00000700) | n)
#define IO_IOCTL_TELNET(n)			(IO_IOCTL(0x00000800) | n)
#define IO_IOCTL_CLI(n)				(IO_IOCTL(0x00000900) | n)
#define IO_IOCTL_SERCOM(n)			(IO_IOCTL(0x00000A00) | n)
#define IO_IOCTL_CPU_TEMP(n)		(IO_IOCTL(0x00000B00) | n)
#define IO_IOCTL_I2C_IO(n)			(IO_IOCTL(0x00000C00) | n)
#define IO_IOCTL_M24512(n)			(IO_IOCTL(0x00000D00) | n)
#define IO_IOCTL_SETTINGS(n)		(IO_IOCTL(0x00000E00) | n)
#define IO_IOCTL_DAC61408(n)		(IO_IOCTL(0x00000F00) | n)
#define IO_IOCTL_DAC7311(n)			(IO_IOCTL(0x00001000) | n)
#define IO_IOCTL_OPT_ADC(n)			(IO_IOCTL(0x00001100) | n)
#define IO_IOCTL_LTC2666(n)			(IO_IOCTL(0x00001200) | n)
#define IO_IOCTL_OTBBCOM(n)			(IO_IOCTL(0x00001300) | n)
#define IO_IOCTL_ADS8681_OTBA(n)	(IO_IOCTL(0x00001400) | n)
/* Declarations */


#endif //__IO_IOCTL_H__
