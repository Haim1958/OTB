/**
 ******************************************************************************
 * @file    spi_io_private.h
 * @brief   SPI IO private declarations and definitions
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

 
#ifndef __SPI_IO_PRIVATE_H__
#define __SPI_IO_PRIVATE_H__

/* Includes */
#include "stream_buffer.h"

/* Definitions */

#define SPI_IO__MAX_NUM_OF_DRIVERS		(3u)
#define SPI3_IO__MAX_NUM_OF_DRIVERS		(3u)

/* Typedefs */

typedef struct spi_io_driver_generic {
	// Common parameters
	uint32_t						read_buffer_size;				// Received bytes buffer size
	uint8_t							*read_buffer;					// Buffer for received bytes
	
	uint32_t						received_size;					// Number of received bytes in the last communication

	uint32_t						rw_timeout;						// Blocking Read/Write timeout in ticks	
} SPI_IO_DRIVER_GENERIC, * SPI_IO_DRIVER_GENERIC_PTR;

typedef struct spi_io_driver_phy {
	SPI_HandleTypeDef*				HAL_SPI;						// ST SPI driver handler
	GPIO_TypeDef*					SPI_NSS_PORT;
	uint16_t						SPI_NSS_PIN;
} SPI_IO_DRIVER_PHY, * SPI_IO_DRIVER_PHY_PTR;

typedef struct spi_io_driver {
	uint32_t 						index;							// Driver index
	
	SPI_IO_DRIVER_PHY				PHY;
	SPI_IO_DRIVER_GENERIC			g;
} SPI_IO_DRIVER, * SPI_IO_DRIVER_PTR;

typedef struct spi_io_driver_bundle {
	SPI_IO_DRIVER_PTR				driver[SPI_IO__MAX_NUM_OF_DRIVERS];
} SPI_IO_DRIVER_BUNDLE, * SPI_IO_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif
	 
int32_t _spi_io_open(IO_HANDLER_PTR, void*);
int32_t _spi_io_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t _spi_io_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t _spi_io_ioctl(IO_HANDLER_PTR, uint32_t, void*);

void _spi_io_irq(void* param);
void _spi_io_handle_read_write(SPI_IO_DRIVER_BUNDLE_PTR driver_bundle);

#ifdef __cplusplus
}
#endif

#endif //__SPI_IO_PRIVATE_H__
