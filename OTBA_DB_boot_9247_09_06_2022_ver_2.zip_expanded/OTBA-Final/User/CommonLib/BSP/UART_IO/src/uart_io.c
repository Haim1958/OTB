/**
 ******************************************************************************
 * @file    uart_io.c
 * @brief   UART IO driver functions implementations
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


/* Includes */
#include <string.h>
#include <stdlib.h>
#include "UserLib.h"
#include "stream_buffer.h"
#include "../inc/uart_io.h"
#include "../inc/uart_io_private.h"

/* Global variables */


/* Functions definitions */

static int32_t _uart_io_open(IO_HANDLER_PTR, void*);
static int32_t _uart_io_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _uart_io_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _uart_io_ioctl(IO_HANDLER_PTR, uint32_t, void*);


/* Functions definitions */

/*******************************************************************************
* Function Name: install_uart_io
********************************************************************************
Description: Installs the UART driver
Inputs:
UART_IO_INSTALL_STRUCT_PTR - UART install parameters
Outputs:
IO_HANDLER_PTR - installed handler
NULL - the instal process failed
*******************************************************************************/
int32_t install_uart_io
	(
		UART_IO_INSTALL_STRUCT_PTR uart_io_init
	)
{
	UART_IO_DRIVER_BUNDLE_PTR	driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_uart_io_open,
											_uart_io_read,
											_uart_io_write,
											_uart_io_ioctl
										};
	int32_t err;

	
	/* Allocate driver bundle */
	driver_bundle = (UART_IO_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(UART_IO_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_UART_IO__OUT_OF_HEAP;
	}
	
	memset(driver_bundle, 0, sizeof(UART_IO_DRIVER_BUNDLE));
	
	init.bundle = driver_bundle;
	
/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__UART_IO, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}
	
	return IO_ERROR_UART_IO__OK;
}


/*******************************************************************************
* Function Name: _uart_io_open
********************************************************************************
Description: Opens and initializes the UART driver
Inputs:
IO_HANDLER_PTR - UART handler
void * - UART init parameters (UART_IO_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_UART_IO__OK - initialization OK
*******************************************************************************/
static int32_t _uart_io_open(IO_HANDLER_PTR handler, void * init)
{
	UART_IO_DRIVER_BUNDLE_PTR driver_bundle = (UART_IO_DRIVER_BUNDLE_PTR) handler->bundle;
	UART_IO_INIT_STRUCT_PTR uart_init = (UART_IO_INIT_STRUCT_PTR) init;
	UART_IO_DRIVER_PTR uart_driver;
	uint8_t irq_index;
	
	/* Check if the HAL driver is correct, and choose the correct IRQ index */
	if (USART1 == uart_init->HAL_UART->Instance)
	{
		irq_index = IRQ_INDEX__UART__UART1;
	}
	else if (USART2 == uart_init->HAL_UART->Instance)
	{
		irq_index = IRQ_INDEX__UART__UART2;
	}
	else if (USART3 == uart_init->HAL_UART->Instance)
	{
		irq_index = IRQ_INDEX__UART__UART3;
	}
	else if (UART4 == uart_init->HAL_UART->Instance)
	{
		irq_index = IRQ_INDEX__UART__UART4;
	}
	else if (UART5 == uart_init->HAL_UART->Instance)
	{
		irq_index = IRQ_INDEX__UART__UART5;
	}
	else if (USART6 == uart_init->HAL_UART->Instance)
	{
		irq_index = IRQ_INDEX__UART__UART6;
	}
	else if (UART7 == uart_init->HAL_UART->Instance)
	{
		irq_index = IRQ_INDEX__UART__UART7;
	}
	else if (UART8 == uart_init->HAL_UART->Instance)
	{
		irq_index = IRQ_INDEX__UART__UART8;
	}
	else
	{
		return IO_ERROR_UART_IO__UNKNOWN_HAL_DRIVER;
	}
	
	/* Allocate driver */
	uart_driver = (UART_IO_DRIVER_PTR) pvPortMalloc(sizeof(UART_IO_DRIVER));
	if (NULL == uart_driver)
	{
		return IO_ERROR_UART_IO__OUT_OF_HEAP;
	}
	
	memset(uart_driver, 0, sizeof(UART_IO_DRIVER));
	
	/* Allocate the RX queue */
	uart_driver->g.rx_queue = xStreamBufferCreate(uart_init->rx_buf_size, 1u);
	if (NULL == uart_driver->g.rx_queue)
	{
		return IO_ERROR_UART_IO__OUT_OF_HEAP;
	}	
	
	/* Allocate the TX queue */
	uart_driver->g.tx_queue = xStreamBufferCreate(uart_init->tx_buf_size, 1u);
	if (NULL == uart_driver->g.tx_queue)
	{
		return IO_ERROR_UART_IO__OUT_OF_HEAP;
	}	
	
	uart_driver->PHY.HAL_UART	= uart_init->HAL_UART;
	
	/* Register interrupt callback for RX */
	irq_register(IRQ_TYPE__UART_RX,
				 irq_index,
				 _uart_io_irq_rx,
				 uart_driver);
	
	/* IP allocation task */               /*** osPriorityAboveNormal     ***/
	osThreadDef(UART_TX_TASK, _uart_io_tx, osPriorityNormal - 1, 0, 512);
	uart_driver->g.tx_task = osThreadCreate(osThread(UART_TX_TASK), uart_driver);
	if (NULL == uart_driver->g.tx_task)
	{
		return IO_ERROR_UART_IO__OUT_OF_HEAP;
	}

	driver_bundle->driver[irq_index] = uart_driver;
	handler->instance = uart_driver;
	
	/* Activate RX */
	HAL_UART_Receive_IT(uart_driver->PHY.HAL_UART,
						&uart_driver->g.rx_byte,
						1);
	
	return IO_ERROR_UART_IO__OK;
}


/*******************************************************************************
* Function Name: _uart_io_read
********************************************************************************
Description: Reads the UART driver
Inputs:
IO_HANDLER_PTR - UART handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_UART_IO__OK - read OK
*******************************************************************************/
static int32_t _uart_io_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	UART_IO_DRIVER_PTR uart_driver = (UART_IO_DRIVER_PTR) handler->instance;
	size_t size;
	
	size = xStreamBufferReceive(uart_driver->g.rx_queue, ptr, length, 0);
	if (size != length)
	{
		return IO_ERROR_UART_IO__RX_QUEUE_EMPTY;
	}
	
	return IO_ERROR_UART_IO__OK;
}


/*******************************************************************************
* Function Name: _uart_io_write
********************************************************************************
Description: Writes to the UART driver
Inputs:
IO_HANDLER_PTR - UART handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_UART_IO__OK - write OK
*******************************************************************************/
static int32_t _uart_io_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	UART_IO_DRIVER_PTR uart_driver = (UART_IO_DRIVER_PTR) handler->instance;
	size_t bytes_written = 0;
	
	if (length == 0)
	{
		return IO_ERROR_UART_IO__OK;
	}
	
	bytes_written = xStreamBufferSend(uart_driver->g.tx_queue, ptr, length, 0);

	if (bytes_written < length) // Not all the data could be written to the queue, not enough space
		return IO_ERROR_UART_IO__TX_QUEUE_FULL;

	return IO_ERROR_UART_IO__OK;
}


/*******************************************************************************
* Function Name: _uart_io_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - UART handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_UART_IO__OK - command execution OK
IO_ERROR_UART_IO__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
static int32_t _uart_io_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
//	UART_IO_DRIVER_PTR uart_driver = (UART_IO_DRIVER_PTR)handler->instance;
	int32_t err = IO_ERROR_UART_IO__OK;
	
	switch (ctl)
	{
	  default:
		err = IO_ERROR_UART_IO__UNKNOWN_IOCTL;
		break;
	}
	
	return err;
}
