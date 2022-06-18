/**
 ******************************************************************************
 * @file    m24512.c
 * @brief   M24512 driver functions implementations
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

#include "../inc/m24512_private.h"

/* Global variables */

/* Definitions */

/* Functions declarations */

static int32_t _m24512_open(IO_HANDLER_PTR, void*);
static int32_t _m24512_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _m24512_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _m24512_ioctl(IO_HANDLER_PTR, uint32_t, void*);

/* Functions definitions */


/*******************************************************************************
* Function Name: install_m24512
********************************************************************************
Description: 
Inputs:
M24512_INSTALL_STRUCT_PTR - 
Outputs:
uint32_t - 

*******************************************************************************/
int32_t install_m24512(M24512_INSTALL_STRUCT_PTR m24512_install)
{
	int32_t err = 0;
	M24512_DRIVER_BUNDLE_PTR	driver_bundle = {0};
	IO_HANDLER_INSTALL_STRUCT install =	{
											_m24512_open,
											_m24512_read,
											_m24512_write,
											_m24512_ioctl
										};
	
	/* Allocate driver bundle */
	driver_bundle = (M24512_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(M24512_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_M24512__OUT_OF_HEAP;
	}
	
	memset(driver_bundle, 0, sizeof(M24512_DRIVER_BUNDLE));
	
	install.bundle = driver_bundle;
	
	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__M24512, &install);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}
	
	return IO_ERROR_M24512__OK;
}


/*******************************************************************************
* Function Name: _m24512_open
********************************************************************************
Description: Opens and initializes the m24512 driver
Inputs:
IO_HANDLER_PTR - m24512 handler
void * - m24512 init parameters (M24512_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_M24512__OK - initialization OK
*******************************************************************************/
static int32_t _m24512_open(IO_HANDLER_PTR handler, void * init)
{
    M24512_DRIVER_BUNDLE_PTR driver_bundle = (M24512_DRIVER_BUNDLE_PTR)handler->bundle;
	M24512_INIT_STRUCT_PTR m24512_init = (M24512_INIT_STRUCT_PTR)init;

	driver_bundle->seek_addr							= 0;
	driver_bundle->PHY.i2c_driver						= m24512_init->i2c_driver;

	return IO_ERROR_M24512__OK;
}


/*******************************************************************************
* Function Name: _m24512_read
********************************************************************************
Description: 
Inputs:
IO_HANDLER_PTR - 
uint8_t * - 
uint32_t - 
Outputs:

*******************************************************************************/
static int32_t _m24512_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	M24512_DRIVER_BUNDLE_PTR driver_bundle = handler->bundle;
	I2C_IO_TRANSACTION i2c_read = {0};
	
	// Fill i2c driver transaction struct.
	i2c_read.DEV = M24512_I2C_ADDR;
    i2c_read.ADD = driver_bundle->seek_addr;
    i2c_read.ADD_SIZE = I2C_IO_ADD_SIZE_16_BIT;
	i2c_read.VAL_PTR = ptr;
    i2c_read.LEN = length;

	return io_read(driver_bundle->PHY.i2c_driver, (void *)&i2c_read, 1);
}


/*******************************************************************************
* Function Name: _m24512_write
********************************************************************************
Description:
Inputs:
IO_HANDLER_PTR - 
uint8_t * - 
uint32_t - 
Outputs:

*******************************************************************************/
static int32_t _m24512_write(IO_HANDLER_PTR handler, uint8_t * data, uint32_t length)
{
	uint32_t err = IO_ERROR_M24512__OK;
	M24512_DRIVER_BUNDLE_PTR driver_bundle = handler->bundle;
	uint32_t last_page = M24512_ADDR_TO_PAGE(driver_bundle->seek_addr + length - 1);
	I2C_IO_TRANSACTION i2c_write = {
			.DEV = M24512_I2C_ADDR,
			.ADD_SIZE = I2C_IO_ADD_SIZE_16_BIT
	};

	while (length)
	{
		if (M24512_ADDR_TO_PAGE(driver_bundle->seek_addr) != last_page)
		{
			i2c_write.LEN = M24512_PAGE_SIZE_BYTES - M24512_OFFSET_IN_PAGE(driver_bundle->seek_addr);
		}
		else
		{
			i2c_write.LEN = length;
		}

		i2c_write.ADD = driver_bundle->seek_addr;
		i2c_write.VAL_PTR = data;

		err = io_write(driver_bundle->PHY.i2c_driver, (void *)&i2c_write, 1);
		if(err != IO_ERROR_I2C_IO__OK)
		{
			return err;
		}

		length -= i2c_write.LEN;
		data += i2c_write.LEN;
		driver_bundle->seek_addr += i2c_write.LEN;
	}

	// Write time
	HAL_Delay(4);

	return IO_ERROR_M24512__OK;
}


/*******************************************************************************
* Function Name: _m24512_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - m24512 handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:

*******************************************************************************/
static int32_t _m24512_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	int32_t err = IO_ERROR_M24512__OK;
	M24512_DRIVER_BUNDLE_PTR driver_bundle = (M24512_DRIVER_BUNDLE_PTR)handler->bundle;
	
	switch (ctl)
	{
		case IO_IOCTL_M24512__LOCK_AND_SEEK:
			driver_bundle->seek_addr = *(uint16_t*)ptr;
			break;

		default:
			err = IO_ERROR_M24512__UNKNOWN_IOCTL;
			break;
	}
	
	return err;
}

