/**
 ******************************************************************************
 * @file    cli.c
 * @brief   OTS CLI public functions implementations
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
#include "../inc/cli_private.h"

/* Global variables */

/* Functions declarations */

static int32_t _cli_open(IO_HANDLER_PTR h, void*);
static int32_t _cli_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _cli_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _cli_ioctl(IO_HANDLER_PTR, uint32_t, void*);


/* Functions definitions */

/*******************************************************************************
* Function Name: install_cli
********************************************************************************
Description: Installs the CLI driver
Inputs:
CLI_INSTALL_STRUCT_PTR - CLI install parameters
Outputs:
IO_HANDLER_PTR - installed handler
*******************************************************************************/
int32_t install_cli
	(
		CLI_INSTALL_STRUCT_PTR cli_init
	)
{
	CLI_DRIVER_BUNDLE_PTR driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_cli_open,
											_cli_read,
											_cli_write,
											_cli_ioctl,
											0u
										};
	int32_t err;

	/* Allocate CLI bundle */
	driver_bundle = (CLI_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(CLI_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_CLI__OUT_OF_HEAP;
	}

	memset(driver_bundle, 0, sizeof(CLI_DRIVER_BUNDLE));

	driver_bundle->total_number_instances = cli_init->total_number_instances;
	driver_bundle->next_instance_index = 0;
	driver_bundle->INSTANCES = (CLI_INSTANCE_PTR*) pvPortMalloc(sizeof(CLI_INSTANCE_PTR) * driver_bundle->total_number_instances);
	if (NULL == driver_bundle->INSTANCES)
	{
		return IO_ERROR_CLI__OUT_OF_HEAP;
	}

	memset(driver_bundle->INSTANCES, 0, sizeof(CLI_INSTANCE_PTR) * driver_bundle->total_number_instances);

	init.bundle = driver_bundle;

	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__CLI, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}

	return IO_ERROR_CLI__OK;
}


/*******************************************************************************
* Function Name: _cli_open
********************************************************************************
Description: Opens and initializes the CLI driver
Inputs:
IO_HANDLER_PTR - CLI handler
void * - CLI init parameters (CLI_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_CLI__OK - initialization OK
*******************************************************************************/
static int32_t _cli_open(IO_HANDLER_PTR handler, void * init)
{
	CLI_DRIVER_BUNDLE_PTR driver_bundle = (CLI_DRIVER_BUNDLE_PTR) handler->bundle;
	CLI_INSTANCE_PTR cli_instance;
//	CLI_INIT_STRUCT_PTR	cli_init = (CLI_INIT_STRUCT_PTR) init;

	if (driver_bundle->next_instance_index >= driver_bundle->total_number_instances)
	{
		return IO_ERROR_CLI__OUT_OF_HEAP;
	}

	cli_instance = (CLI_INSTANCE_PTR) pvPortMalloc(sizeof(CLI_INSTANCE));
	if (NULL == cli_instance)
	{
		return IO_ERROR_CLI__OUT_OF_HEAP;
	}

	memset(cli_instance, 0, sizeof(CLI_INSTANCE));

	cli_instance->instance_index = driver_bundle->next_instance_index;
	driver_bundle->INSTANCES[driver_bundle->next_instance_index] = cli_instance;
	driver_bundle->next_instance_index++;

	handler->instance = cli_instance;

	return IO_ERROR_CLI__OK;
}


/*******************************************************************************
* Function Name: _cli_read
********************************************************************************
Description: Reads the OTS CLI driver
Inputs:
IO_HANDLER_PTR - CLI handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_CLI__OK - read OK
*******************************************************************************/
static int32_t _cli_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
//	CLI_DRIVER_BUNDLE_PTR driver_bundle = (CLI_DRIVER_BUNDLE_PTR) handler->bundle;
	CLI_INSTANCE_PTR cli_instance = (CLI_INSTANCE_PTR) handler->instance;
	int32_t err;

	if (cli_instance->g.command == NULL)
	{
		err = cli_read_command(cli_instance);
		if (err == IO_ERROR_CLI__OK)
		{
			*(char**)ptr = cli_instance->g.command;
		}
	}
	else
	{
		err = cli_read_token(cli_instance);
		if (err == IO_ERROR_CLI__OK)
		{
			*(char**)ptr = cli_instance->g.token;
		}
	}

	return err;
}


/*******************************************************************************
* Function Name: _cli_write
********************************************************************************
Description: Writes to the CLI driver
Inputs:
IO_HANDLER_PTR - CLI handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_CLI__OK - write OK
*******************************************************************************/
static int32_t _cli_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
//	CLI_DRIVER_BUNDLE_PTR driver_bundle = (CLI_DRIVER_BUNDLE_PTR) handler->bundle;
	CLI_INSTANCE_PTR cli_instance = (CLI_INSTANCE_PTR) handler->instance;

	return cli_write_data(cli_instance, ptr, length);
}


/*******************************************************************************
* Function Name: _cli_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - CLI handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_CLI__OK - command execution OK
IO_ERROR_CLI__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
static int32_t _cli_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
//	CLI_DRIVER_BUNDLE_PTR driver_bundle = (CLI_DRIVER_BUNDLE_PTR) handler->bundle;
	CLI_INSTANCE_PTR cli_instance = (CLI_INSTANCE_PTR) handler->instance;
	int32_t err = IO_ERROR_CLI__OK;

	switch (ctl)
	{
		case IO_IOCTL_CLI__PURGE_COMMAND:
			err = cli_purge_command(cli_instance);
			break;

		default:
			err = IO_ERROR_CLI__UNKNOWN_IOCTL;
			break;
	}

	return err;
}
