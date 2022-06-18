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
#include <assert.h>
#include "UserLib.h"
#include "../inc/uart_io.h"
#include "../inc/uart_io_private.h"

/* Global variables */


/* Functions definitions */

/*******************************************************************************
Function Name     :     _uart_io_tx
Parameters        :     None
Description       :     UART TX task handler function
********************************************************************************/
void _uart_io_tx(void const * argument)
{
	UART_IO_DRIVER_PTR uart_driver = (UART_IO_DRIVER_PTR)argument;
	uint8_t byte;
	
	for (;;)
	{
		size_t size = xStreamBufferReceive(uart_driver->g.tx_queue, &byte, 1, portMAX_DELAY);
		if (size != 1)
		{
			continue;
		}

		HAL_UART_Transmit(uart_driver->PHY.HAL_UART, &byte, 1u, 100);
	}
}


/*******************************************************************************
Function Name     :     _uart_io_irq_rx
Parameters        :     None
Description       :     UART IRQ handler function
********************************************************************************/
void _uart_io_irq_rx(void* param)
{
	UART_IO_DRIVER_PTR uart_driver = (UART_IO_DRIVER_PTR)param;
	uint8_t data;
	size_t size;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE; /* Initialised to pdFALSE. */
	
	data = uart_driver->g.rx_byte;
	
	/* Restart RX */
	HAL_UART_Receive_IT(uart_driver->PHY.HAL_UART, &uart_driver->g.rx_byte, 1);
	
	/* Push the received byte to the RX queue */
	size = xStreamBufferSendFromISR(uart_driver->g.rx_queue,
									&data,
									1u,
									&xHigherPriorityTaskWoken);
	
	if(size != 1u)
    {
		uint32_t temp = 0;
		UNUSED(temp);
        /* There was not enough free space in the stream buffer for the entire
        string to be written, ut xBytesSent bytes were written. */
    }
	
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
