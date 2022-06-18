/**
 ******************************************************************************
 * @file    spi_io.h
 * @brief   SPI IO public declarations and definitions
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


#ifndef __SPI_IO_H__
#define __SPI_IO_H__

/* Includes */


/* Definitions */

// Error definitions
#define IO_ERROR_SPI_IO__OK							IO_ERROR__OK
#define IO_ERROR_SPI_IO__OUT_OF_HEAP				IO_ERROR_SPI_IO(0x01)
#define IO_ERROR_SPI_IO__UNKNOWN_HAL_DRIVER			IO_ERROR_SPI_IO(0x02)
#define IO_ERROR_SPI_IO__DRV_INDEX_OUT_OF_RANGE		IO_ERROR_SPI_IO(0x03)
#define IO_ERROR_SPI_IO__UNKNOWN_IOCTL				IO_ERROR_SPI_IO(0x04)
#define IO_ERROR_SPI_IO__DATA_TOO_LONG				IO_ERROR_SPI_IO(0x05)
#define IO_ERROR_SPI_IO__UNKNOWN_MODE				IO_ERROR_SPI_IO(0x06)
#define IO_ERROR_SPI_IO__HAL_READ_FAILED			IO_ERROR_SPI_IO(0x07)
#define IO_ERROR_SPI_IO__HAL_WRITE_FAILED			IO_ERROR_SPI_IO(0x08)
#define IO_ERROR_SPI_IO__BUSY						IO_ERROR_SPI_IO(0x09)
#define IO_ERROR_SPI_IO__RX_QUEUE_EMPTY				IO_ERROR_SPI_IO(0x0A)


// IOCTL definitions
//#define IO_IOCTL_SPI_IO__CS_LOW						IO_IOCTL_SPI_IO(0x01)
//#define IO_IOCTL_SPI_IO__CS_HIGH					IO_IOCTL_SPI_IO(0x02)



/* Typedefs */

typedef enum {
	SPI_IO_MODE__REGISTER,								// Writing is blocking, direct reception, CS pin is modulated externally
	SPI_IO_MODE__DATA,									// Writing is non-blocking, reception via queue, CS pin is modulated internally
	
	SPI_IO_MODE_COUNT
} SPI_IO_MODE, * SPI_IO_MODE_PTR;


typedef struct {
	uint32_t						temp;
} SPI_IO_INSTALL_STRUCT, * SPI_IO_INSTALL_STRUCT_PTR;


typedef struct {
	SPI_HandleTypeDef*				HAL_SPI;			// ST HAL handler of SPI
	GPIO_TypeDef*					SPI_NSS_PORT;
	uint16_t						SPI_NSS_PIN;
	
	uint32_t						read_buf_size;		// Read buffer max size
	uint32_t						rw_timeout;			// Blocking Read/Write timeout in ticks
} SPI_IO_INIT_STRUCT, * SPI_IO_INIT_STRUCT_PTR;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_spi_io(SPI_IO_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif


#endif	//__SPI_IO_H__
