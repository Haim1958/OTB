/**
 ******************************************************************************
 * @file    i2c_io.c
 * @brief   I2C IO driver functions implementations
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
#include "../inc/i2c_io.h"
#include "../inc/i2c_io_private.h"

/* Global variables */


/* Functions definitions */

/*******************************************************************************
* Function Name: install_i2c_io
********************************************************************************
Description: Installs the I2C driver
Inputs:
I2C_IO_INSTALL_STRUCT_PTR - I2C install parameters
Outputs:
IO_HANDLER_PTR - installed handler
NULL - the instal process failed
*******************************************************************************/
int32_t install_i2c_io
	(
		I2C_IO_INSTALL_STRUCT_PTR i2c_io_init
	)
{
	I2C_IO_DRIVER_BUNDLE_PTR	driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_i2c_io_open,
											_i2c_io_read,
											_i2c_io_write,
											_i2c_io_ioctl
										};
	int32_t err;
	
	/* Allocate driver bundle */
	driver_bundle = (I2C_IO_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(I2C_IO_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_I2C_IO__OUT_OF_HEAP;
	}
	
	memset(driver_bundle, 0, sizeof(I2C_IO_DRIVER_BUNDLE));
	
	/* Initialize PEC LUT */
//	err = _i2c_io_pec_initialize_lut(driver_bundle);
//	if (err != IO_ERROR_I2C_IO__OK)
//	{
//		return err;
//	}
	
	init.bundle = driver_bundle;
	
	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__I2C_IO, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}
	
	return IO_ERROR_I2C_IO__OK;
}


/*******************************************************************************
* Function Name: _i2c_io_open
********************************************************************************
Description: Opens and initializes the I2C driver
Inputs:
IO_HANDLER_PTR - I2C handler
void * - I2C init parameters (I2C_IO_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_I2C_IO__OK - initialization OK
*******************************************************************************/
int32_t _i2c_io_open(IO_HANDLER_PTR handler, void * init)
{
	I2C_IO_DRIVER_BUNDLE_PTR driver_bundle = (I2C_IO_DRIVER_BUNDLE_PTR) handler->bundle;
	I2C_IO_INIT_STRUCT_PTR i2c_init = (I2C_IO_INIT_STRUCT_PTR)init;
	I2C_IO_DRIVER_PTR i2c_driver;
	uint32_t index;
	
	if (I2C1 == i2c_init->HAL_I2C->Instance)
	{
		index = 0;
	}
	else if (I2C2 == i2c_init->HAL_I2C->Instance)
	{
		index = 1;
	}
	else if (I2C3 == i2c_init->HAL_I2C->Instance)
	{
		index = 2;
	}
	else
	{
		return IO_ERROR_I2C_IO__UNKNOWN_HAL_DRIVER;
	}
	
	/* Allocate driver */
	i2c_driver = (I2C_IO_DRIVER_PTR) pvPortMalloc(sizeof(I2C_IO_DRIVER));
	if (NULL == i2c_driver)
	{
		return IO_ERROR_I2C_IO__OUT_OF_HEAP;
	}
	
	memset(i2c_driver, 0, sizeof(I2C_IO_DRIVER));
	
	i2c_driver->index			= index;
	i2c_driver->PHY.HAL_I2C		= i2c_init->HAL_I2C;
	
	driver_bundle->driver[i2c_init->index] = i2c_driver;
	handler->instance = i2c_driver;
	
	return IO_ERROR_I2C_IO__OK;
}


/*******************************************************************************
* Function Name: _i2c_io_read
********************************************************************************
Description: Reads the I2C driver
Inputs:
IO_HANDLER_PTR - I2C handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_I2C_IO__OK - read OK
*******************************************************************************/
int32_t _i2c_io_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	I2C_IO_DRIVER_PTR i2c_driver = (I2C_IO_DRIVER_PTR) handler->instance;
	I2C_IO_TRANSACTION_PTR transaction = (I2C_IO_TRANSACTION_PTR)ptr;
	HAL_StatusTypeDef status = HAL_OK;
	
	UNUSED(length);
	
	/* Initiate the transmission */
	status = HAL_I2C_Mem_Read(
				i2c_driver->PHY.HAL_I2C,
				transaction->DEV << 1,
				transaction->ADD,
				transaction->ADD_SIZE,
				transaction->VAL_PTR,
				transaction->LEN,
				100);
	
	switch (status)
	{
	  case HAL_BUSY: return IO_ERROR_I2C_IO__BUSY;
	  case HAL_TIMEOUT: return IO_ERROR_I2C_IO__TIMEOUT;
	  case HAL_OK: return IO_ERROR_I2C_IO__OK;
	  default: return IO_ERROR_I2C_IO__HAL_ERROR;
	}
}


/*******************************************************************************
* Function Name: _i2c_io_write
********************************************************************************
Description: Writes to the I2C driver
Inputs:
IO_HANDLER_PTR - I2C handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_I2C_IO__OK - write OK
*******************************************************************************/
int32_t _i2c_io_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	I2C_IO_DRIVER_PTR i2c_driver = (I2C_IO_DRIVER_PTR) handler->instance;
	I2C_IO_TRANSACTION_PTR transaction = (I2C_IO_TRANSACTION_PTR)ptr;
	HAL_StatusTypeDef status = HAL_OK;
	
	UNUSED(length);
	
	/* Initiate the transmission */
	status = HAL_I2C_Mem_Write(
				i2c_driver->PHY.HAL_I2C,
				transaction->DEV << 1,
				transaction->ADD,
				transaction->ADD_SIZE,
				transaction->VAL_PTR,
				transaction->LEN,
				100);
	
	switch (status)
	{
	  case HAL_BUSY: return IO_ERROR_I2C_IO__BUSY;
	  case HAL_TIMEOUT: return IO_ERROR_I2C_IO__TIMEOUT;
	  case HAL_OK: return IO_ERROR_I2C_IO__OK;
	  default: return IO_ERROR_I2C_IO__HAL_ERROR;
	}
}


/*******************************************************************************
* Function Name: _i2c_io_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - I2C handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_I2C_IO__OK - command execution OK
IO_ERROR_I2C_IO__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
int32_t _i2c_io_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	I2C_IO_DRIVER_PTR i2c_driver = (I2C_IO_DRIVER_PTR)handler->instance;
	int32_t err = IO_ERROR_I2C_IO__OK;
	
	UNUSED(i2c_driver);
	
	switch (ctl)
	{
	  default:
		err = IO_ERROR_I2C_IO__UNKNOWN_IOCTL;
		break;
	}
	
	return err;
}





/*******************************************************************************
* Function Name: _i2c_io_pec_initialize_lut
********************************************************************************
Description: Initializes auxilary PEC LUT
Inputs:
I2C_IO_DRIVER_BUNDLE_PTR - I2C driver bundle
Outputs:
IO_ERROR_I2C_IO__OK - init OK
*******************************************************************************/
//int32_t _i2c_io_pec_initialize_lut(I2C_IO_DRIVER_BUNDLE_PTR driver_bundle)
//{
//	uint32_t i, j;
//	uint8_t crc;
//	
//	for (i = 0; i < 256; i++)
//	{
//		crc = i;
//		for (j = 0; j < 8; j++)
//		{
//			crc = (crc << 1) ^ ((crc & 0x80) ? 0x07 : 0);		/* crc-8 poly: x^8 + x^2 + x + 1 */
//			driver_bundle->PEC_LUT[i] = crc;
//		}
//	}
//	
//	return IO_ERROR_I2C_IO__OK;
//}


/*******************************************************************************
* Function Name: _i2c_io_pec_calculate
********************************************************************************
Description: Calculates PEC of the input I2C transaction
Inputs:
I2C_IO_DRIVER_BUNDLE_PTR - I2C driver bundle
I2C_IO_TRANSACTION_PTR - transaction
uint8_t - TRUE if read transaction; FALSE if write transaction
Outputs:
uint8_t - calculated PEC value
*******************************************************************************/
//uint8_t _i2c_io_pec_calculate(I2C_IO_DRIVER_BUNDLE_PTR driver_bundle, I2C_IO_TRANSACTION_PTR i2c_transaction, uint8_t if_read)
//{
//	uint32_t i;
//	uint8_t pec = 0;
//	
//	if (i2c_transaction->if_pec == 0)
//	{
//		return 0;
//	}
//	
//	pec = driver_bundle->PEC_LUT[pec ^ (i2c_transaction->DEV << 1)];
//	pec = driver_bundle->PEC_LUT[pec ^ (i2c_transaction->ADD)];
//	pec = driver_bundle->PEC_LUT[pec ^ ((i2c_transaction->DEV << 1) + (if_read ? 1 : 0))];
//	
//	for (i = 0; i < i2c_transaction->LEN; i++)
//	{
//		pec = driver_bundle->PEC_LUT[pec ^ i2c_transaction->VAL[i]];
//	}
//
//	return pec;
//}
