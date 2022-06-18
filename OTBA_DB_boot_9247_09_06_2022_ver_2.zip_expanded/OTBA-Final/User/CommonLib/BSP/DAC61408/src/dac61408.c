/**
 ******************************************************************************
 * @file    dac61408.c
 * @brief   DAC61408 driver functions implementations
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
#include "../inc/dac61408.h"
#include "../inc/dac61408_private.h"

/* Global variables */


/* Functions declarations */


int32_t _dac61408_open(IO_HANDLER_PTR, void*);
int32_t _dac61408_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t _dac61408_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t _dac61408_ioctl(IO_HANDLER_PTR, uint32_t, void*);


/* Functions definitions */

/*******************************************************************************
* Function Name: install_dac61408
********************************************************************************
Description: Installs the CPU Temperature driver
Inputs:
DAC61408_INSTALL_STRUCT_PTR - CPU Temperature install parameters
Outputs:
IO_HANDLER_PTR - installed handler
NULL - the instal process failed
*******************************************************************************/
int32_t install_dac61408
	(
		DAC61408_INSTALL_STRUCT_PTR dac61408_init
	)
{
	DAC61408_DRIVER_BUNDLE_PTR	driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_dac61408_open,
											_dac61408_read,
											_dac61408_write,
											_dac61408_ioctl
										};
	int32_t err;
	
	/* Allocate driver bundle */
	driver_bundle = (DAC61408_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(DAC61408_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_DAC61408__OUT_OF_HEAP;
	}
	
	memset(driver_bundle, 0, sizeof(DAC61408_DRIVER_BUNDLE));
	
	init.bundle = driver_bundle;
	
	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__DAC61408, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}
	
	return err;
}


/*******************************************************************************
* Function Name: _dac61408_open
********************************************************************************
Description: Opens and initializes the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
void * - CPU Temperature init parameters (DAC61408_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_DAC61408__OK - initialization OK
*******************************************************************************/
int32_t _dac61408_open(IO_HANDLER_PTR handler, void * init)
{
	DAC61408_DRIVER_BUNDLE_PTR driver_bundle = (DAC61408_DRIVER_BUNDLE_PTR) handler->bundle;
	DAC61408_INIT_STRUCT_PTR dac61408_init = (DAC61408_INIT_STRUCT_PTR)init;

	/* Allocate driver */

	driver_bundle->PHY.SPI			= dac61408_init->SPI;
	driver_bundle->PHY.RESET_PORT	= dac61408_init->RESET_PORT;
	driver_bundle->PHY.RESET_PIN	= dac61408_init->RESET_PIN;
	driver_bundle->PHY.LDAC_PORT	= dac61408_init->LDAC_PORT;
	driver_bundle->PHY.LDAC_PIN		= dac61408_init->LDAC_PIN;
	driver_bundle->g.enabled_ch		= dac61408_init->enabled_ch;
	driver_bundle->g.internal_ref	= dac61408_init->internal_ref;

	return dac61408_initialize(driver_bundle);
}


/*******************************************************************************
* Function Name: _dac61408_read
********************************************************************************
Description: Reads the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_DAC61408__OK - read OK
*******************************************************************************/
int32_t _dac61408_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	return IO_ERROR_DAC61408__OK;
}


/*******************************************************************************
* Function Name: _dac61408_write
********************************************************************************
Description: Writes to the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_DAC61408__OK - write OK
*******************************************************************************/
int32_t _dac61408_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	DAC61408_DRIVER_BUNDLE_PTR driver_bundle = (DAC61408_DRIVER_BUNDLE_PTR) handler->bundle;

	return dac61408_write_value(driver_bundle, (DAC61408_SAMPLE_PTR) ptr);
}


/*******************************************************************************
* Function Name: _dac61408_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_DAC61408__OK - command execution OK
IO_ERROR_DAC61408__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
int32_t _dac61408_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	DAC61408_DRIVER_BUNDLE_PTR driver_bundle = (DAC61408_DRIVER_BUNDLE_PTR)handler->bundle;
	int32_t err = IO_ERROR_DAC61408__OK;

	switch (ctl)
	{
		case IO_IOCTL_DAC61408__POWER_UP:
			err = dac61408_power_up(driver_bundle);
			break;

		case IO_IOCTL_DAC61408__POWER_DOWN:
			err = dac61408_power_down(driver_bundle);
			break;

		default:
			err = IO_ERROR_DAC61408__UNKNOWN_IOCTL;
			break;
	}

	return err;
}

