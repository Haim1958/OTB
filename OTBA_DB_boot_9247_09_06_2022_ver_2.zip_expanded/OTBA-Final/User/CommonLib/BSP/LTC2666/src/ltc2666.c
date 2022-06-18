/**
 ******************************************************************************
 * @file    ltc2666.c
 * @brief   LTC2666 driver functions implementations
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
#include "../inc/ltc2666.h"
#include "../inc/ltc2666_private.h"

/* Global variables */


/* Functions declarations */


int32_t _ltc2666_open(IO_HANDLER_PTR, void*);
int32_t _ltc2666_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t _ltc2666_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t _ltc2666_ioctl(IO_HANDLER_PTR, uint32_t, void*);


/* Functions definitions */

/*******************************************************************************
* Function Name: install_ltc2666
********************************************************************************
Description: Installs the CPU Temperature driver
Inputs:
LTC2666_INSTALL_STRUCT_PTR - CPU Temperature install parameters
Outputs:
IO_HANDLER_PTR - installed handler
NULL - the instal process failed
*******************************************************************************/
int32_t install_ltc2666
	(
		LTC2666_INSTALL_STRUCT_PTR ltc2666_init
	)
{
	LTC2666_DRIVER_BUNDLE_PTR	driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_ltc2666_open,
											_ltc2666_read,
											_ltc2666_write,
											_ltc2666_ioctl
										};
	int32_t err;
	
	/* Allocate driver bundle */
	driver_bundle = (LTC2666_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(LTC2666_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_LTC2666__OUT_OF_HEAP;
	}
	
	memset(driver_bundle, 0, sizeof(LTC2666_DRIVER_BUNDLE));
	
	init.bundle = driver_bundle;
	
	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__LTC2666, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}
	
	return err;
}


/*******************************************************************************
* Function Name: _ltc2666_open
********************************************************************************
Description: Opens and initializes the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
void * - CPU Temperature init parameters (LTC2666_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_LTC2666__OK - initialization OK
*******************************************************************************/
int32_t _ltc2666_open(IO_HANDLER_PTR handler, void * init)
{
	LTC2666_DRIVER_BUNDLE_PTR driver_bundle = (LTC2666_DRIVER_BUNDLE_PTR) handler->bundle;
	LTC2666_INIT_STRUCT_PTR ltc2666_init = (LTC2666_INIT_STRUCT_PTR)init;

	/* Allocate driver */

	driver_bundle->PHY.SPI			= ltc2666_init->SPI;
	driver_bundle->PHY.LDAC_PORT	= ltc2666_init->LDAC_PORT;
	driver_bundle->PHY.LDAC_PIN		= ltc2666_init->LDAC_PIN;
	driver_bundle->g.MSPAN			= ltc2666_init->MSPAN;
	driver_bundle->g.RES			= ltc2666_init->RES;

	return ltc2666_initialize(driver_bundle);
}


/*******************************************************************************
* Function Name: _ltc2666_read
********************************************************************************
Description: Reads the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_LTC2666__OK - read OK
*******************************************************************************/
int32_t _ltc2666_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	return IO_ERROR_LTC2666__OK;
}


/*******************************************************************************
* Function Name: _ltc2666_write
********************************************************************************
Description: Writes to the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_LTC2666__OK - write OK
*******************************************************************************/
int32_t _ltc2666_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	LTC2666_DRIVER_BUNDLE_PTR driver_bundle = (LTC2666_DRIVER_BUNDLE_PTR) handler->bundle;

	return ltc2666_write_channel_value(driver_bundle, (LTC2666_SAMPLE_PTR) ptr);
}


/*******************************************************************************
* Function Name: _ltc2666_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_LTC2666__OK - command execution OK
IO_ERROR_LTC2666__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
int32_t _ltc2666_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	LTC2666_DRIVER_BUNDLE_PTR driver_bundle = (LTC2666_DRIVER_BUNDLE_PTR)handler->bundle;
	int32_t err = IO_ERROR_LTC2666__OK;

	switch (ctl)
	{
		case IO_IOCTL_LTC2666__POWER_UP:
			err = ltc2666_write_all_channels_value(driver_bundle, 0);
			break;

		case IO_IOCTL_LTC2666__POWER_DOWN:
			err = ltc2666_power_down(driver_bundle);
			break;
		case IO_IOCTL_LTC2666__SET_MUX:
					err = ltc2666_set_MUX(driver_bundle);
					break;
		default:
			err = IO_ERROR_LTC2666__UNKNOWN_IOCTL;
			break;
	}

	return err;
}

