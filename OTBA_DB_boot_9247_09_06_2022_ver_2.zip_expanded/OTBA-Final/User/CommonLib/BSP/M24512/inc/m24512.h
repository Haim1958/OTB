/**
 ******************************************************************************
 * @file    m24512.h
 * @brief   M24512 public declarations and definitions
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


#ifndef __M24512_H__
#define __M24512_H__

/* Includes */

/* Definitions */

#define M24512_PAGE_SIZE_BYTES						(128)
#define M24512_I2C_ADDR								(0x50)
#define M24512_TOTAL_SIZE_BYTES						(524288 / 8) // 512Kb
#define M24512_NUMBER_PAGES							(M24512_TOTAL_SIZE_BYTES / M24512_PAGE_SIZE_BYTES)

// Error definitions
#define IO_ERROR_M24512__OK							IO_ERROR__OK
#define IO_ERROR_M24512__OUT_OF_HEAP				IO_ERROR_M24512(0x01)
#define IO_ERROR_M24512__UNKNOWN_IOCTL				IO_ERROR_M24512(0x02)
#define IO_ERROR_M24512__INIT_FAIL					IO_ERROR_M24512(0x03)
#define IO_ERROR_M24512__READ_FAIL					IO_ERROR_M24512(0x04)
#define IO_ERROR_M24512__WRITE_FAIL					IO_ERROR_M24512(0x05)
#define IO_ERROR_M24512__WRITE_SIZE_EXCEEDED		IO_ERROR_M24512(0x06)
#define IO_ERROR_M24512__OUT_OF_RANGE				IO_ERROR_M24512(0x07)


// IOCTL definitions
#define IO_IOCTL_M24512__LOCK_AND_SEEK				IO_IOCTL_M24512(0x01)
#define IO_IOCTL_M24512__UNLOCK						IO_IOCTL_M24512(0x02)

/* Typedefs */

typedef struct 
{
	uint32_t						temp;

} M24512_INSTALL_STRUCT, * M24512_INSTALL_STRUCT_PTR;

typedef struct 
{
	IO_HANDLER_PTR							i2c_driver;
} M24512_INIT_STRUCT, * M24512_INIT_STRUCT_PTR;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_m24512(M24512_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif


#endif	//__M24512_H__
