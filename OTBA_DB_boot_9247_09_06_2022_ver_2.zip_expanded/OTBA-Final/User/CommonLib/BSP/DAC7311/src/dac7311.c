/**
 ******************************************************************************
 * @file    dac7311.c
 * @brief   DAC7311 driver functions implementations
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
#include "../inc/dac7311.h"
#include "../inc/dac7311_private.h"

/* Global variables */


/* Functions declarations */


int32_t _dac7311_open(IO_HANDLER_PTR, void*);
int32_t _dac7311_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t _dac7311_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t _dac7311_ioctl(IO_HANDLER_PTR, uint32_t, void*);


/* Functions definitions */

/*******************************************************************************
* Function Name: install_dac7311
********************************************************************************
Description: Installs the CPU Temperature driver
Inputs:
DAC7311_INSTALL_STRUCT_PTR - CPU Temperature install parameters
Outputs:
IO_HANDLER_PTR - installed handler
NULL - the instal process failed
*******************************************************************************/
int32_t install_dac7311
	(
		DAC7311_INSTALL_STRUCT_PTR dac7311_init
	)
{
	DAC7311_DRIVER_BUNDLE_PTR	driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_dac7311_open,
											_dac7311_read,
											_dac7311_write,
											_dac7311_ioctl
										};
	int32_t err;
	
	/* Allocate driver bundle */
	driver_bundle = (DAC7311_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(DAC7311_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_DAC7311__OUT_OF_HEAP;
	}
	
	memset(driver_bundle, 0, sizeof(DAC7311_DRIVER_BUNDLE));
	
	init.bundle = driver_bundle;
	
	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__DAC7311, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}
	
	return IO_ERROR_DAC7311__OK;
}


/*******************************************************************************
* Function Name: _dac7311_open
********************************************************************************
Description: Opens and initializes the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
void * - CPU Temperature init parameters (DAC7311_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_DAC7311__OK - initialization OK
*******************************************************************************/
int32_t _dac7311_open(IO_HANDLER_PTR handler, void * init)
{
	DAC7311_DRIVER_BUNDLE_PTR driver_bundle = (DAC7311_DRIVER_BUNDLE_PTR) handler->bundle;
	DAC7311_INIT_STRUCT_PTR dac7311_init = (DAC7311_INIT_STRUCT_PTR)init;

	/* Allocate driver */

	driver_bundle->PHY.SPI			= dac7311_init->SPI;

	return dac7311_initialize(driver_bundle);
}


/*******************************************************************************
* Function Name: _dac7311_read
********************************************************************************
Description: Reads the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_DAC7311__OK - read OK
*******************************************************************************/
int32_t _dac7311_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	return IO_ERROR_DAC7311__OK;
}


/*******************************************************************************
* Function Name: _dac7311_write
********************************************************************************
Description: Writes to the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_DAC7311__OK - write OK
*******************************************************************************/
int32_t _dac7311_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	DAC7311_DRIVER_BUNDLE_PTR driver_bundle = (DAC7311_DRIVER_BUNDLE_PTR) handler->bundle;

	return dac7311_write_value(driver_bundle, *(DAC7311_SAMPLE_PTR)ptr);
}


/*******************************************************************************
* Function Name: _dac7311_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_DAC7311__OK - command execution OK
IO_ERROR_DAC7311__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
int32_t _dac7311_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	DAC7311_DRIVER_BUNDLE_PTR driver_bundle = (DAC7311_DRIVER_BUNDLE_PTR)handler->bundle;
	int32_t err = IO_ERROR_DAC7311__OK;

	switch (ctl)
	{
		case IO_IOCTL_DAC7311__POWER_DOWN:
			err = dac7311_power_down(driver_bundle);
			break;

		default:
			err = IO_ERROR_DAC7311__UNKNOWN_IOCTL;
			break;
	}

	return err;
}

