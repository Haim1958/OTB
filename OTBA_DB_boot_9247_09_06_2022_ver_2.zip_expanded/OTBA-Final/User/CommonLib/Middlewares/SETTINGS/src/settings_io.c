/**
 ******************************************************************************
 * @file    settings_io.c
 * @brief   SETTINGS_IO IO driver functions implementations
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
#include "../inc/settings_private.h"

/* Global variables */


/* Functions declarations */

static int32_t _settings_open(IO_HANDLER_PTR, void*);
static int32_t _settings_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _settings_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _settings_ioctl(IO_HANDLER_PTR, uint32_t, void*);


/* Functions definitions */

/*******************************************************************************
* Function Name: install_settings
********************************************************************************
Description: Installs the SETTINGS driver
Inputs:
SETTINGS_IO_INSTALL_STRUCT_PTR - SETTINGS_IO install parameters
Outputs:
IO_HANDLER_PTR - installed handler
NULL - the instal process failed
*******************************************************************************/
int32_t install_settings
	(
		SETTINGS_INSTALL_STRUCT_PTR settings_init
	)
{
	SETTINGS_DRIVER_BUNDLE_PTR	settings_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_settings_open,
											_settings_read,
											_settings_write,
											_settings_ioctl
										};
	int32_t err;
	
	/* Allocate driver bundle */
	settings_bundle = (SETTINGS_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(SETTINGS_DRIVER_BUNDLE));
	if (NULL == settings_bundle)
	{
		return IO_ERROR_SETTINGS__OUT_OF_HEAP;
	}
	
	memset(settings_bundle, 0, sizeof(SETTINGS_DRIVER_BUNDLE));
	
	settings_bundle->number_files = settings_init->max_number_files;
	settings_bundle->FILES = (SETTINGS_FILE_PTR*) pvPortMalloc(sizeof(SETTINGS_FILE_PTR) * settings_bundle->number_files);
	if (NULL == settings_bundle->FILES)
	{
		return IO_ERROR_SETTINGS__OUT_OF_HEAP;
	}

	init.bundle = settings_bundle;
	
	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__SETTINGS, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}
	
	return IO_ERROR_SETTINGS__OK;
}


/*******************************************************************************
* Function Name: _settings_open
********************************************************************************
Description: Opens and initializes the SETTINGS_IO driver
Inputs:
IO_HANDLER_PTR - SETTINGS_IO handler
void * - SETTINGS_IO init parameters (SETTINGS_IO_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_SETTINGS_IO__OK - initialization OK
*******************************************************************************/
static int32_t _settings_open(IO_HANDLER_PTR handler, void * init)
{
	SETTINGS_DRIVER_BUNDLE_PTR settings_bundle = (SETTINGS_DRIVER_BUNDLE_PTR) handler->bundle;
	SETTINGS_INIT_STRUCT_PTR settings_init = (SETTINGS_INIT_STRUCT_PTR)init;
	
	if (settings_init->DRIVER.if_init)
	{
		// Initialize main driver
		settings_bundle->G.eeprom_driver = settings_init->DRIVER.eeprom_driver;
	}
	else if (settings_init->INSTANCE.if_init)
	{
		uint32_t index;
		uint32_t next_first_page = 0;
		SETTINGS_FILE_PTR new_file;

		// Find the next FILE cell to populate
		for (index = 0;
			index < settings_bundle->number_files && settings_bundle->FILES[index] != NULL;
			index++)
		{
			next_first_page = settings_bundle->FILES[index]->first_page + settings_bundle->FILES[index]->size_number_pages;
		}

		if (index >= settings_bundle->number_files)
		{
			return IO_ERROR_SETTINGS__NUMBER_FILES_EXCEEDED_MAXIMUM;
		}

		if (next_first_page + settings_init->INSTANCE.size_number_pages >= SETTINGS_NUMBER_PAGES)
		{
			return IO_ERROR_SETTINGS__FILE_TOO_BIG;
		}

		new_file = (SETTINGS_FILE_PTR) pvPortMalloc(sizeof(SETTINGS_FILE));
		if (NULL == new_file)
		{
			return IO_ERROR_SETTINGS__OUT_OF_HEAP;
		}

		new_file->first_page			= next_first_page;
		new_file->size_number_pages		= settings_init->INSTANCE.size_number_pages;
		new_file->size_bytes			= new_file->size_number_pages * SETTINGS_PAGE_SIZE_BYTES;
		new_file->is_valid				= 0;

		new_file->BUFFER = pvPortMalloc(new_file->size_bytes);
		if (NULL == new_file->BUFFER)
		{
			return IO_ERROR_SETTINGS__OUT_OF_HEAP;
		}

		// Ignore returned error, make file even if pull fails
		_settings_pull(settings_bundle, new_file);

		settings_bundle->FILES[index] = new_file;
		handler->instance = new_file;
	}
	
	return IO_ERROR_SETTINGS__OK;
}


/*******************************************************************************
* Function Name: _settings_read
********************************************************************************
Description: Reads the internal SETTINGS_IO receive buffer. The buffer is filled during _settings_io_write execution.
Inputs:
IO_HANDLER_PTR - SETTINGS_IO handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_SETTINGS_IO__OK - read OK
*******************************************************************************/
static int32_t _settings_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	SETTINGS_FILE_PTR settings_file = (SETTINGS_FILE_PTR) handler->instance;

	if (!settings_file->is_valid)
	{
		return IO_ERROR_SETTINGS__FILE_INVALID;
	}

	// Return the pointer to the data entry
	*(uint8_t**)ptr = settings_file->BUFFER + sizeof(SETTINGS_IO_ENTRY_HEADER);

	return IO_ERROR_SETTINGS__OK;
}


/*******************************************************************************
* Function Name: _settings_write
********************************************************************************
Description: Writes to the SETTINGS_IO driver
Inputs:
IO_HANDLER_PTR - SETTINGS_IO handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_SETTINGS_IO__OK - write OK
*******************************************************************************/
static int32_t _settings_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	SETTINGS_DRIVER_BUNDLE_PTR settings_bundle = (SETTINGS_DRIVER_BUNDLE_PTR) handler->bundle;
	SETTINGS_FILE_PTR settings_file = (SETTINGS_FILE_PTR) handler->instance;
	int32_t err;
	
	if (length >= settings_file->size_bytes - sizeof(SETTINGS_IO_ENTRY_HEADER) ||
		length == 0 ||
		ptr == NULL)
	{
		return IO_ERROR_SETTINGS__INVALID_PARAMS;
	}
	
	err = _settings_write_data(settings_bundle, settings_file, ptr, length);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	return _settings_push(settings_bundle, settings_file);
}


/*******************************************************************************
* Function Name: _settings_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - SETTINGS_IO handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_SETTINGS_IO__OK - command execution OK
IO_ERROR_SETTINGS_IO__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
static int32_t _settings_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	SETTINGS_DRIVER_BUNDLE_PTR settings_bundle = (SETTINGS_DRIVER_BUNDLE_PTR) handler->bundle;
	SETTINGS_FILE_PTR settings_file = (SETTINGS_FILE_PTR) handler->instance;
	int32_t err = IO_ERROR_SETTINGS__OK;

	switch (ctl)
	{
        case IO_IOCTL_SETTINGS__IS_FILE_VALID:
        	if (settings_file == NULL)
        	{
        		return IO_ERROR_SETTINGS__INVALID_PARAMS;
        	}
        	*(uint8_t*)ptr = settings_file->is_valid;
			break;
			
        case IO_IOCTL_SETTINGS__PULL:
			err = _settings_pull(settings_bundle, settings_file);
			break;
	 
		default:
			err = IO_ERROR_SETTINGS__UNKNOWN_IOCTL;
			break;
	}
	
	return err;
}
