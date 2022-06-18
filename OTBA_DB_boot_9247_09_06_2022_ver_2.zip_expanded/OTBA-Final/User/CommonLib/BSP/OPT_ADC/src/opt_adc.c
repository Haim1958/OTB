/**
 ******************************************************************************
 * @file    opt_adc.c
 * @brief   Optical ADC driver functions implementations
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
#include "../inc/opt_adc.h"
#include "../inc/opt_adc_private.h"

/* Global variables */

/* Functions definitions */

static int32_t _opt_adc_open(IO_HANDLER_PTR, void*);
static int32_t _opt_adc_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _opt_adc_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _opt_adc_ioctl(IO_HANDLER_PTR, uint32_t, void*);

/* Functions definitions */

/*******************************************************************************
* Function Name: install_opt_adc
********************************************************************************
Description: Installs the CPU Temperature driver
Inputs:
OPT_ADC_INSTALL_STRUCT_PTR - CPU Temperature install parameters
Outputs:
IO_HANDLER_PTR - installed handler
NULL - the instal process failed
*******************************************************************************/
int32_t install_opt_adc
	(
		OPT_ADC_INSTALL_STRUCT_PTR opt_adc_init
	)
{
	OPT_ADC_DRIVER_BUNDLE_PTR	driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_opt_adc_open,
											_opt_adc_read,
											_opt_adc_write,
											_opt_adc_ioctl
										};
	int32_t err;
	
	/* Allocate driver bundle */
	driver_bundle = (OPT_ADC_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(OPT_ADC_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_OPT_ADC__OUT_OF_HEAP;
	}
	
	memset(driver_bundle, 0, sizeof(OPT_ADC_DRIVER_BUNDLE));
	
	init.bundle = driver_bundle;
	
	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__OPT_ADC, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}
	
	return IO_ERROR_OPT_ADC__OK;
}


/*******************************************************************************
* Function Name: _opt_adc_open
********************************************************************************
Description: Opens and initializes the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
void * - CPU Temperature init parameters (OPT_ADC_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_OPT_ADC__OK - initialization OK
*******************************************************************************/
static int32_t _opt_adc_open(IO_HANDLER_PTR handler, void * init)
{
	OPT_ADC_DRIVER_BUNDLE_PTR driver_bundle = (OPT_ADC_DRIVER_BUNDLE_PTR) handler->bundle;
	OPT_ADC_INIT_STRUCT_PTR opt_adc_init = (OPT_ADC_INIT_STRUCT_PTR)init;

	/* Allocate driver */

	driver_bundle->PHY.HAL_ADC			= opt_adc_init->HAL_ADC;
	driver_bundle->PHY.CH_CONFIG		= opt_adc_init->CH_CONFIG;
	driver_bundle->PHY.VREF_MV			= opt_adc_init->VREF_MV;

	return _opt_adc_init(driver_bundle);
}


/*******************************************************************************
* Function Name: _opt_adc_read
********************************************************************************
Description: Reads the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_OPT_ADC__OK - read OK
*******************************************************************************/
static int32_t _opt_adc_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	OPT_ADC_DRIVER_BUNDLE_PTR driver_bundle = (OPT_ADC_DRIVER_BUNDLE_PTR) handler->bundle;

	return _opt_adc_sample(driver_bundle, (int32_t*)ptr, length);
}


/*******************************************************************************
* Function Name: _opt_adc_write
********************************************************************************
Description: Writes to the CPU Temperature driver
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_OPT_ADC__OK - write OK
*******************************************************************************/
static int32_t _opt_adc_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	return IO_ERROR_OPT_ADC__OK;
}


/*******************************************************************************
* Function Name: _opt_adc_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - CPU Temperature handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_OPT_ADC__OK - command execution OK
IO_ERROR_OPT_ADC__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
static int32_t _opt_adc_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
//	OPT_ADC_DRIVER_BUNDLE_PTR driver_bundle = (OPT_ADC_DRIVER_BUNDLE_PTR)handler->bundle;
	int32_t err = IO_ERROR_OPT_ADC__OK;

	switch (ctl)
	{
	  default:
		err = IO_ERROR_OPT_ADC__UNKNOWN_IOCTL;
		break;
	}

	return err;
}

