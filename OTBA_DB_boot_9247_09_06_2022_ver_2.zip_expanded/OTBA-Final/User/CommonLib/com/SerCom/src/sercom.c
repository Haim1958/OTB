/**
 ******************************************************************************
 * @file    sercom.c
 * @brief   Serial Communication Protocol public functions implementations
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
#include "../inc/sercom.h"
#include "../inc/sercom_private.h"

/* Global variables */

/* Functions declarations */

static int32_t _sercom_open(IO_HANDLER_PTR, void*);
static int32_t _sercom_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _sercom_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _sercom_ioctl(IO_HANDLER_PTR, uint32_t, void*);


/* Functions definitions */

/*******************************************************************************
* Function Name: install_sercom
********************************************************************************
Description: Installs the MSBT driver
Inputs:
SERCOM_INSTALL_STRUCT_PTR - SERCOM install parameters
Outputs:
IO_HANDLER_PTR - installed handler
NULL - the instal process failed
*******************************************************************************/
int32_t install_sercom
	(
		SERCOM_INSTALL_STRUCT_PTR sercom_init
	)
{
	SERCOM_DRIVER_BUNDLE_PTR driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_sercom_open,
											_sercom_read,
											_sercom_write,
											_sercom_ioctl,
											0u
										};
	int32_t err;
	
	/* Allocate SERCOM bundle */
	driver_bundle = (SERCOM_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(SERCOM_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_SERCOM__OUT_OF_HEAP;
	}
	
	memset(driver_bundle, 0, sizeof(SERCOM_DRIVER_BUNDLE));
	
	/* Allocate SERCOM instances array */
	driver_bundle->instance = pvPortMalloc(sizeof(SERCOM_INSTANCE_PTR) * sercom_init->max_num_instances);
	if (NULL == driver_bundle->instance)
	{
		return IO_ERROR_SERCOM__OUT_OF_HEAP;
	}
	
	memset(driver_bundle->instance, 0, sizeof(SERCOM_INSTANCE_PTR) * sercom_init->max_num_instances);
	
	driver_bundle->num_instances = sercom_init->max_num_instances;
	
	init.bundle = driver_bundle;

	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__SERCOM, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}
	
	return IO_ERROR_SERCOM__OK;
}


/*******************************************************************************
* Function Name: _sercom_open
********************************************************************************
Description: Opens and initializes the SERCOM driver
Inputs:
IO_HANDLER_PTR - SERCOM handler
void * - SERCOM init parameters (SERCOM_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_SERCOM__OK - initialization OK
*******************************************************************************/
static int32_t _sercom_open(IO_HANDLER_PTR handler, void * init)
{
	SERCOM_DRIVER_BUNDLE_PTR	driver_bundle = (SERCOM_DRIVER_BUNDLE_PTR) handler->bundle;
	SERCOM_INIT_STRUCT_PTR	sercom_init = (SERCOM_INIT_STRUCT_PTR) init;
	SERCOM_INSTANCE_PTR instance;
	uint32_t i;
	BaseType_t os_err;
	
	if (sercom_init->if_new == 0)
	{
		/* Open a previously opened queue */
		// Check index
		if (sercom_init->index >= driver_bundle->num_instances)
		{
			return IO_ERROR_SERCOM__INDEX_OUT_RANGE;
		}
		
		instance = driver_bundle->instance[sercom_init->index];
		
		// Check if already allocated
		if (NULL == instance)
		{
			return IO_ERROR_SERCOM__INSTANCE_NOT_ALLOCATED;
		}
	}
	else
	{
		for (i = 0; i < driver_bundle->num_instances && driver_bundle->instance[i] != NULL; i++) {}
		if (i < driver_bundle->num_instances && driver_bundle->instance[i] == NULL)
		{
			/* Found a free instance, allocate */
			instance = (SERCOM_INSTANCE_PTR) pvPortMalloc(sizeof(SERCOM_INSTANCE));
			if (NULL == instance)
			{
				return IO_ERROR_SERCOM__OUT_OF_HEAP;
			}
			
			memset(instance, 0, sizeof(SERCOM_INSTANCE));
			
			instance->driver = sercom_init->driver;

			os_err = xTaskCreate(_sercom_task_tx, "SERCOM_TX", 128, instance, 5, &instance->tx_task);
			if (os_err != pdPASS)
			{
				return IO_ERROR_UART_IO__OUT_OF_HEAP;
			}

			os_err = xTaskCreate(_sercom_task_rx, "SERCOM_RX", 128, instance, 5, &instance->rx_task);
			if (os_err != pdPASS)
			{
				return IO_ERROR_UART_IO__OUT_OF_HEAP;
			}
		
			instance->tx_queue = xQueueCreate(sercom_init->tx_queue_size, sizeof(SERCOM_PACKET));
			if (NULL == instance->tx_queue)
			{
				return IO_ERROR_UART_IO__OUT_OF_HEAP;
			}
			
			instance->rx_queue = xQueueCreate(sercom_init->rx_queue_size, sizeof(SERCOM_PACKET));
			if (NULL == instance->rx_queue)
			{
				return IO_ERROR_UART_IO__OUT_OF_HEAP;
			}
			
			instance->tx_timeout_ms = sercom_init->tx_timeout_ms;
			instance->rx_timeout_ms = sercom_init->rx_timeout_ms;

			driver_bundle->instance[i] = instance;
		}
	}

	handler->instance = instance;
	
	return IO_ERROR_SERCOM__OK;
}


/*******************************************************************************
* Function Name: _sercom_read
********************************************************************************
Description: Reads the SERCOM driver
Inputs:
IO_HANDLER_PTR - SERCOM handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_SERCOM__OK - read OK
*******************************************************************************/
static int32_t _sercom_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	SERCOM_INSTANCE_PTR	driver_instance = (SERCOM_INSTANCE_PTR) handler->instance;
	BaseType_t os_err;
	
	os_err = xQueueReceive(driver_instance->rx_queue, ptr, driver_instance->rx_timeout_ms / portTICK_PERIOD_MS);
	if (os_err != pdPASS)
	{
		return IO_ERROR_SERCOM__PACKET_NOT_READY;
	}

	return IO_ERROR_SERCOM__OK;
}


/*******************************************************************************
* Function Name: _sercom_write
********************************************************************************
Description: Writes to the SERCOM driver
Inputs:
IO_HANDLER_PTR - SERCOM handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_SERCOM__OK - write OK
*******************************************************************************/
static int32_t _sercom_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	SERCOM_INSTANCE_PTR	driver_instance = (SERCOM_INSTANCE_PTR) handler->instance;
	BaseType_t os_err;
	
	os_err = xQueueSend(driver_instance->tx_queue, ptr, driver_instance->tx_timeout_ms / portTICK_PERIOD_MS);
	if (os_err != pdPASS)
	{
		return IO_ERROR_SERCOM__TIMEOUT;
	}
	
	return IO_ERROR_SERCOM__OK;
}


/*******************************************************************************
* Function Name: _sercom_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - SERCOM handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_SERCOM__OK - command execution OK
IO_ERROR_SERCOM__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
static int32_t _sercom_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
//	SERCOM_INSTANCE_PTR	driver_instance = (SERCOM_INSTANCE_PTR) handler->instance;
	int32_t err = IO_ERROR_SERCOM__OK;
	
	switch (ctl)
	{
	  default:
		err = IO_ERROR_SERCOM__UNKNOWN_IOCTL;
		break;
	}
	
	return err;
}


