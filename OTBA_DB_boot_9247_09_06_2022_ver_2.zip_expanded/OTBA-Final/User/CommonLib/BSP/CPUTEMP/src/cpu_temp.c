/**
 ******************************************************************************
 * @file    cpu_temp.c
 * @brief   CPU Temperature driver functions implementations
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
#include "../inc/cpu_temp.h"
#include "../inc/cpu_temp_private.h"

/* Global variables */


/* Functions definitions */

/*******************************************************************************
* Function Name: install_cpu_temp
********************************************************************************
Description: Installs the CPU Temperature driver
Inputs:
CPU_TEMP_INSTALL_STRUCT_PTR - CPU Temperature install parameters
Outputs:
IO_HANDLER_PTR - installed handler
NULL - the instal process failed
*******************************************************************************/
int32_t install_cpu_temp
	(
		CPU_TEMP_INSTALL_STRUCT_PTR cpu_temp_init
	)
{
	CPU_TEMP_DRIVER_BUNDLE_PTR	driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_cpu_temp_open,
											_cpu_temp_read,
											_cpu_temp_write,
											_cpu_temp_ioctl
										};
	int32_t err;
	
	/* Allocate driver bundle */
	driver_bundle = (CPU_TEMP_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(CPU_TEMP_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_CPU_TEMP__OUT_OF_HEAP;
	}
	
	memset(driver_bundle, 0, sizeof(CPU_TEMP_DRIVER_BUNDLE));
	
	init.bundle = driver_bundle;
	
	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__CPU_TEMP, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}
	
	return IO_ERROR_CPU_TEMP__OK;
}


/*******************************************************************************
* Function Name: _cpu_temp_open
********************************************************************************
Description: Opens and initializes the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
void * - CPU Temperature init parameters (CPU_TEMP_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_CPU_TEMP__OK - initialization OK
*******************************************************************************/
int32_t _cpu_temp_open(IO_HANDLER_PTR handler, void * init)
{
	CPU_TEMP_DRIVER_BUNDLE_PTR driver_bundle = (CPU_TEMP_DRIVER_BUNDLE_PTR) handler->bundle;
	CPU_TEMP_INIT_STRUCT_PTR cpu_temp_init = (CPU_TEMP_INIT_STRUCT_PTR)init;

	/* Allocate driver */

	driver_bundle->PHY.HAL_ADC			= cpu_temp_init->HAL_ADC;
	driver_bundle->PHY.VREF_MV			= cpu_temp_init->VREF_MV;

	return _cpu_temp_init(driver_bundle);
}


/*******************************************************************************
* Function Name: _cpu_temp_read
********************************************************************************
Description: Reads the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_CPU_TEMP__OK - read OK
*******************************************************************************/
int32_t _cpu_temp_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	CPU_TEMP_DRIVER_BUNDLE_PTR driver_bundle = (CPU_TEMP_DRIVER_BUNDLE_PTR) handler->bundle;
	int32_t err;

	err = _cpu_temp_sample_temperature(driver_bundle);
	if (err != IO_ERROR_CPU_TEMP__OK)
	{
		return err;
	}

	*(float*)ptr = driver_bundle->g.calibrated_temperature;
//	printf("Temperature Sensor read is 0x%04x\n", driver_bundle->g.raw_reading);

	return IO_ERROR_CPU_TEMP__OK;
}


/*******************************************************************************
* Function Name: _cpu_temp_write
********************************************************************************
Description: Writes to the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_CPU_TEMP__OK - write OK
*******************************************************************************/
int32_t _cpu_temp_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	return IO_ERROR_CPU_TEMP__OK;
}


/*******************************************************************************
* Function Name: _cpu_temp_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_CPU_TEMP__OK - command execution OK
IO_ERROR_CPU_TEMP__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
int32_t _cpu_temp_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	CPU_TEMP_DRIVER_BUNDLE_PTR driver_bundle = (CPU_TEMP_DRIVER_BUNDLE_PTR)handler->bundle;
	uint8_t temp;
	int32_t err = IO_ERROR_CPU_TEMP__OK;

	switch (ctl)
	{
	case IO_IOCTL_CPU_TEMP__SET_LOW:
		temp = *(uint8_t*)ptr;
		driver_bundle->g.cool_stop_temp = temp;
		break;

	case IO_IOCTL_CPU_TEMP__SET_HIGH:
		temp = *(uint8_t*)ptr;
		driver_bundle->g.cool_start_temp = temp;
		break;

	case IO_IOCTL_CPU_TEMP__GET_LOW:
		temp = driver_bundle->g.cool_stop_temp;
		*(uint8_t*)ptr = temp;
		break;

	case IO_IOCTL_CPU_TEMP__GET_HIGH:
		temp = driver_bundle->g.cool_start_temp;
		*(uint8_t*)ptr = temp;
		break;

	  default:
		err = IO_ERROR_CPU_TEMP__UNKNOWN_IOCTL;
		break;
	}

	return err;
}

