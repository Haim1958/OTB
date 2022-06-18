/**
 ******************************************************************************
 * @file    spi_io_irq.c
 * @brief   SPI IO driver Interrupt functions implementations
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
#include "../inc/spi_io.h"
#include "../inc/spi_io_private.h"

/* Global variables */


/* Functions definitions */

/*******************************************************************************
Function Name     :     _spi_io_irq
Parameters        :     None
Description       :     SPI1 IRQ handler function
********************************************************************************/
//void _spi_io_irq(void* param)
//{
//	SPI_IO_DRIVER_PTR spi_driver = (SPI_IO_DRIVER_PTR)param;
//	size_t size;
//	BaseType_t xHigherPriorityTaskWoken = pdFALSE; /* Initialised to pdFALSE. */
//
//	if (spi_driver->g.spi_mode == SPI_IO_MODE__DATA)
//	{
//		/* Release CS pin */
//		// Perfomed in HW
////		HAL_GPIO_WritePin(spi_driver->PHY.CS_PORT, spi_driver->PHY.CS_PIN, GPIO_PIN_SET);
//
//		/* Push the received byte to the RX queue */
//		size = xStreamBufferSendFromISR(spi_driver->g.rx_queue,
//										spi_driver->g.read_buffer,
//										spi_driver->g.read_length,
//										&xHigherPriorityTaskWoken);
//
//		if(size != spi_driver->g.read_length)
//		{
//			uint32_t temp = 0;
//			UNUSED(temp);
//			/* There was not enough free space in the stream buffer for the entire
//			string to be written, ut xBytesSent bytes were written. */
//		}
//
//		spi_driver->g.done_flag = 1;
//	}
//
//    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
//}


void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	HAL_SPI_Abort(hspi);
//	spi_driver->g.done_flag = 1;
}
