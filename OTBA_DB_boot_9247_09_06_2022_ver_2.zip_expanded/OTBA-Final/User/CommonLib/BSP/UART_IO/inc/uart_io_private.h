/**
 ******************************************************************************
 * @file    uart_io_private.h
 * @brief   UART IO private declarations and definitions
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

 
#ifndef __UART_IO_PRIVATE_H__
#define __UART_IO_PRIVATE_H__

/* Includes */
#include "stream_buffer.h"

/* Definitions */

#define UART_IO__MAX_NUM_OF_DRIVERS		(IRQ_INDEX__UART_COUNT)

#define UART_IO__IRQ_BUFFER				(50u)


/* Typedefs */

typedef struct uart_io_driver_generic {
	osThreadId						tx_task;

	StreamBufferHandle_t			rx_queue;
	StreamBufferHandle_t			tx_queue;

	uint8_t							rx_byte;
	uint8_t							irq_data[UART_IO__IRQ_BUFFER];
} UART_IO_DRIVER_GENERIC, * UART_IO_DRIVER_GENERIC_PTR;

typedef struct uart_io_driver_phy {
	UART_HandleTypeDef*				HAL_UART;						// ST UART driver handler
} UART_IO_DRIVER_PHY, * UART_IO_DRIVER_PHY_PTR;

typedef struct uart_io_driver {
	UART_IO_DRIVER_PHY				PHY;
	UART_IO_DRIVER_GENERIC			g;	
} UART_IO_DRIVER, * UART_IO_DRIVER_PTR;

typedef struct uart_io_driver_bundle {
	UART_IO_DRIVER_PTR				driver[UART_IO__MAX_NUM_OF_DRIVERS];
} UART_IO_DRIVER_BUNDLE, * UART_IO_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

 void _uart_io_tx(void const * argument);
void _uart_io_irq_rx(void* param);

#ifdef __cplusplus
}
#endif

#endif //__UART_IO_PRIVATE_H__
