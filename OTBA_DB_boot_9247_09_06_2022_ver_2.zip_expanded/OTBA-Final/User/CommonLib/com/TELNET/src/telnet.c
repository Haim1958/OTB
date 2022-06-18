/**
 ******************************************************************************
 * @file    telnet.c
 * @brief   OTS Telnet public functions implementations
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
#include "../inc/telnet_private.h"
#include "../inc/telnet_server.h"

/* Global variables */

/* Functions declarations */

static int32_t _telnet_open(IO_HANDLER_PTR h, void*);
static int32_t _telnet_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _telnet_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _telnet_ioctl(IO_HANDLER_PTR, uint32_t, void*);


/* Functions definitions */

/*******************************************************************************
* Function Name: install_telnet
********************************************************************************
Description: Installs the TELNET driver
Inputs:
TELNET_INSTALL_STRUCT_PTR - TELNET install parameters
Outputs:
IO_HANDLER_PTR - installed handler
*******************************************************************************/
int32_t install_telnet
	(
		TELNET_INSTALL_STRUCT_PTR telnet_init
	)
{
	TELNET_DRIVER_BUNDLE_PTR driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_telnet_open,
											_telnet_read,
											_telnet_write,
											_telnet_ioctl,
											0u
										};
	int32_t err;

	/* Allocate TELNET bundle */
	driver_bundle = (TELNET_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(TELNET_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_TELNET__OUT_OF_HEAP;
	}

	memset(driver_bundle, 0, sizeof(TELNET_DRIVER_BUNDLE));

	init.bundle = driver_bundle;

	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__TELNET, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}

	return IO_ERROR_TELNET__OK;
}


/*******************************************************************************
* Function Name: _telnet_open
********************************************************************************
Description: Opens and initializes the TELNET driver
Inputs:
IO_HANDLER_PTR - TELNET handler
void * - TELNET init parameters (TELNET_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_TELNET__OK - initialization OK
*******************************************************************************/
static int32_t _telnet_open(IO_HANDLER_PTR handler, void * init)
{
	TELNET_DRIVER_BUNDLE_PTR	driver_bundle = (TELNET_DRIVER_BUNDLE_PTR) handler->bundle;
	TELNET_INIT_STRUCT_PTR	telnet_init = (TELNET_INIT_STRUCT_PTR) init;

	driver_bundle->rx_queue = xStreamBufferCreate(telnet_init->rx_queue_size, 1u);
	if (driver_bundle->rx_queue == NULL)
	{
		return IO_ERROR_TELNET__OUT_OF_HEAP;
	}

	driver_bundle->tx_queue = xStreamBufferCreate(telnet_init->tx_queue_size, 1u);
	if (driver_bundle->tx_queue == NULL)
	{
		return IO_ERROR_TELNET__OUT_OF_HEAP;
	}

	driver_bundle->tx_mutex = xSemaphoreCreateMutex();
	if (driver_bundle->tx_mutex == NULL)
	{
		return IO_ERROR_TELNET__OUT_OF_HEAP;
	}

	driver_bundle->tx_double_buf = pvPortMalloc(telnet_init->tx_queue_size);
	if (driver_bundle->tx_double_buf == NULL)
	{
		return IO_ERROR_TELNET__OUT_OF_HEAP;
	}

	InitializeTelnetServer(driver_bundle->rx_queue, driver_bundle->tx_queue, driver_bundle->tx_double_buf);

	return IO_ERROR_TELNET__OK;
}


/*******************************************************************************
* Function Name: _telnet_read
********************************************************************************
Description: Reads the OTS TELNET driver
Inputs:
IO_HANDLER_PTR - TELNET handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_TELNET__OK - read OK
*******************************************************************************/
static int32_t _telnet_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	TELNET_DRIVER_BUNDLE_PTR driver_bundle = (TELNET_DRIVER_BUNDLE_PTR) handler->bundle;
	int32_t read_len;

	read_len = xStreamBufferReceive(driver_bundle->rx_queue, ptr, length, 0);
	if (read_len < length)
	{
		return IO_ERROR_TELNET__FAILED_TO_READ_REQUESTED_AMOUNT;
	}

	return IO_ERROR_TELNET__OK;
}


/*******************************************************************************
* Function Name: _telnet_write
********************************************************************************
Description: Writes to the TELNET driver
Inputs:
IO_HANDLER_PTR - TELNET handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_TELNET__OK - write OK
*******************************************************************************/
static int32_t _telnet_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	TELNET_DRIVER_BUNDLE_PTR driver_bundle = (TELNET_DRIVER_BUNDLE_PTR) handler->bundle;
	int32_t err = IO_ERROR_TELNET__OK;

	xSemaphoreTake(driver_bundle->tx_mutex, portMAX_DELAY);
	if (!TelnetIsConnected())
	{
		err = IO_ERROR_TELNET__NOT_CONNECTED;
		goto _telnet_write_complete;
	}

	xStreamBufferSend(driver_bundle->tx_queue, ptr, length, portMAX_DELAY);

_telnet_write_complete:
	xSemaphoreGive(driver_bundle->tx_mutex);
	return err;
}


/*******************************************************************************
* Function Name: _telnet_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - TELNET handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_TELNET__OK - command execution OK
IO_ERROR_TELNET__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
static int32_t _telnet_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
//	TELNET_DRIVER_BUNDLE_PTR driver_bundle = (TELNET_DRIVER_BUNDLE_PTR) handler->bundle;
	TelnetStatus_t telnet_err;
	int32_t err = IO_ERROR_TELNET__OK;

	switch (ctl)
	{
		case IO_IOCTL_TELNET__START_SERVER:
			telnet_err = StartTelnetServer();
			if (telnet_err != TELNET_OK)
			{
				return IO_ERROR_TELNET__INITIALIZATION_ERROR;
			}
			break;

		case IO_IOCTL_TELNET__IS_CONNECTED:
			*(uint8_t*)ptr = TelnetIsConnected();
			break;

		default:
			err = IO_ERROR_TELNET__UNKNOWN_IOCTL;
			break;
	}

	return err;
}
