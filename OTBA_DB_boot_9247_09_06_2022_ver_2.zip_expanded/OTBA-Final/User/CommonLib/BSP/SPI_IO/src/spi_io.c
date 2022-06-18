/**
 ******************************************************************************
 * @file    spi_io.c
 * @brief   SPI IO driver functions implementations
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
#include "../inc/spi_io.h"
#include "../inc/spi_io_private.h"

/* Global variables */


/* Functions definitions */

/*******************************************************************************
* Function Name: install_spi_io
********************************************************************************
Description: Installs the SPI driver
Inputs:
SPI_IO_INSTALL_STRUCT_PTR - SPI install parameters
Outputs:
IO_HANDLER_PTR - installed handler
NULL - the instal process failed
*******************************************************************************/
int32_t install_spi_io
	(
		SPI_IO_INSTALL_STRUCT_PTR spi_io_init
	)
{
	SPI_IO_DRIVER_BUNDLE_PTR	driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_spi_io_open,
											_spi_io_read,
											_spi_io_write,
											_spi_io_ioctl
										};
	int32_t err;
	
	/* Allocate driver bundle */
	driver_bundle = (SPI_IO_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(SPI_IO_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_SPI_IO__OUT_OF_HEAP;
	}
	
	memset(driver_bundle, 0, sizeof(SPI_IO_DRIVER_BUNDLE));
	
	init.bundle = driver_bundle;
	
	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__SPI_IO, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}
	
	return IO_ERROR_SPI_IO__OK;
}


/*******************************************************************************
* Function Name: _spi_io_open
********************************************************************************
Description: Opens and initializes the SPI driver
Inputs:
IO_HANDLER_PTR - SPI handler
void * - SPI init parameters (SPI_IO_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_SPI_IO__OK - initialization OK
*******************************************************************************/
int32_t _spi_io_open(IO_HANDLER_PTR handler, void * init)
{
	SPI_IO_DRIVER_BUNDLE_PTR driver_bundle = (SPI_IO_DRIVER_BUNDLE_PTR) handler->bundle;
	SPI_IO_INIT_STRUCT_PTR spi_init = (SPI_IO_INIT_STRUCT_PTR)init;
	SPI_IO_DRIVER_PTR spi_driver;
	uint32_t index;

	if (SPI1 == spi_init->HAL_SPI->Instance)
	{
		index = 0;
	}
	else if (SPI2 == spi_init->HAL_SPI->Instance)
	{
		index = 1;
	}
	else if (SPI3 == spi_init->HAL_SPI->Instance)
	{
		index = 2;
	}
	else
	{
		return IO_ERROR_SPI_IO__UNKNOWN_HAL_DRIVER;
	}

	if (index >= SPI_IO__MAX_NUM_OF_DRIVERS)
	{
		return IO_ERROR_SPI_IO__DRV_INDEX_OUT_OF_RANGE;
	}

	/* Allocate driver */
	spi_driver = (SPI_IO_DRIVER_PTR) pvPortMalloc(sizeof(SPI_IO_DRIVER));
	if (NULL == spi_driver)
	{
		return IO_ERROR_SPI_IO__OUT_OF_HEAP;
	}

	memset(spi_driver, 0, sizeof(SPI_IO_DRIVER));

	spi_driver->index				= index;
	spi_driver->PHY.HAL_SPI			= spi_init->HAL_SPI;
	spi_driver->PHY.SPI_NSS_PORT	= spi_init->SPI_NSS_PORT;
	spi_driver->PHY.SPI_NSS_PIN		= spi_init->SPI_NSS_PIN;

	/* Allocate READ buffer */
	spi_driver->g.read_buffer = pvPortMalloc(spi_init->read_buf_size);
	if (NULL == spi_driver->g.read_buffer)
	{
		return IO_ERROR_SPI_IO__OUT_OF_HEAP;
	}

	memset(spi_driver->g.read_buffer, 0, spi_init->read_buf_size);

	spi_driver->g.read_buffer_size	= spi_init->read_buf_size;
	spi_driver->g.received_size = 0;
	spi_driver->g.rw_timeout = spi_init->rw_timeout;

	driver_bundle->driver[spi_driver->index] = spi_driver;
	handler->instance = spi_driver;

	/* Register interrupt callback */
//	irq_register(IRQ_TYPE__SPI,
//				 irq_index,
//				 _spi_io_irq,
//				 spi_driver);


	// Unselect CS
	HAL_GPIO_WritePin(spi_driver->PHY.SPI_NSS_PORT, spi_driver->PHY.SPI_NSS_PIN, GPIO_PIN_SET);

	return IO_ERROR_SPI_IO__OK;
}


/*******************************************************************************
* Function Name: _spi_io_read
********************************************************************************
Description: Reads the SPI driver
Inputs:
IO_HANDLER_PTR - SPI handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_SPI_IO__OK - read OK
*******************************************************************************/
int32_t _spi_io_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	SPI_IO_DRIVER_PTR spi_driver = (SPI_IO_DRIVER_PTR) handler->instance;

	if (length > spi_driver->g.received_size)
	{
		return IO_ERROR_SPI_IO__DATA_TOO_LONG;
	}

	memcpy(ptr, spi_driver->g.read_buffer, length);

	return IO_ERROR_SPI_IO__OK;
}


/*******************************************************************************
* Function Name: _spi_io_write
********************************************************************************
Description: Writes to the SPI driver
Inputs:
IO_HANDLER_PTR - SPI handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_SPI_IO__OK - write OK
*******************************************************************************/
int32_t _spi_io_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	SPI_IO_DRIVER_PTR spi_driver = (SPI_IO_DRIVER_PTR) handler->instance;
	HAL_StatusTypeDef errHAL = HAL_OK;

	if (HAL_SPI_GetState(spi_driver->PHY.HAL_SPI) != HAL_SPI_STATE_READY)
	{
		return IO_ERROR_SPI_IO__BUSY;
	}

	if (length > spi_driver->g.read_buffer_size)
	{
		return IO_ERROR_SPI_IO__DATA_TOO_LONG;
	}

	spi_driver->g.received_size = 0;


	/* Blocking WRITE */
	HAL_GPIO_WritePin(spi_driver->PHY.SPI_NSS_PORT, spi_driver->PHY.SPI_NSS_PIN, GPIO_PIN_RESET);
//	for (uint32_t i = 0; i < 50; i++);	// delay
	errHAL = HAL_SPI_TransmitReceive(spi_driver->PHY.HAL_SPI, ptr, spi_driver->g.read_buffer, length, spi_driver->g.rw_timeout);
//	for (uint32_t i = 0; i < 50; i++);	// delay
	HAL_GPIO_WritePin(spi_driver->PHY.SPI_NSS_PORT, spi_driver->PHY.SPI_NSS_PIN, GPIO_PIN_SET);

	if (errHAL != HAL_OK)
	{
		return IO_ERROR_SPI_IO__HAL_WRITE_FAILED;
	}

	spi_driver->g.received_size = length;

	return IO_ERROR_SPI_IO__OK;
}


/*******************************************************************************
* Function Name: _spi_io_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - SPI handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_SPI_IO__OK - command execution OK
IO_ERROR_SPI_IO__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
int32_t _spi_io_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
//	SPI_IO_DRIVER_PTR spi_driver = (SPI_IO_DRIVER_PTR)handler->instance;
	int32_t err = IO_ERROR_SPI_IO__OK;

	switch (ctl)
	{
//	  case IO_IOCTL_SPI_IO__CS_LOW:
//		HAL_SPI_Abort(spi_driver->PHY.HAL_SPI);
//		HAL_GPIO_WritePin(spi_driver->PHY.CS_PORT, spi_driver->PHY.CS_PIN, GPIO_PIN_RESET);
//		break;
//
//	  case IO_IOCTL_SPI_IO__CS_HIGH:
//		HAL_SPI_Abort(spi_driver->PHY.HAL_SPI);
//		HAL_GPIO_WritePin(spi_driver->PHY.CS_PORT, spi_driver->PHY.CS_PIN, GPIO_PIN_SET);
//		break;

	  default:
		err = IO_ERROR_SPI_IO__UNKNOWN_IOCTL;
		break;
	}

	return err;
}

