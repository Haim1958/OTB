/**
 ******************************************************************************
 * @file    tcpcom.c
 * @brief   OTS TCP Communication public functions implementations
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

#include "../../SPECTRA/inc/tcpcom_icd.h"
#include "../../SPECTRA/inc/tcpcom_private.h"

/* Global variables */



/* Functions declarations */

static int32_t _tcpcom_open(IO_HANDLER_PTR h, void*);
static int32_t _tcpcom_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _tcpcom_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _tcpcom_ioctl(IO_HANDLER_PTR, uint32_t, void*);


/* Functions definitions */

/*******************************************************************************
* Function Name: install_tcpcom
********************************************************************************
Description: Installs the TCPCOM driver
Inputs:
TCPCOM_INSTALL_STRUCT_PTR - TCPCOM install parameters
Outputs:
IO_HANDLER_PTR - installed handler
NULL - the instal process failed
*******************************************************************************/
int32_t install_tcpcom ( TCPCOM_INSTALL_STRUCT_PTR tcpcom_init)
{
	TCPCOM_DRIVER_BUNDLE_PTR driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_tcpcom_open,
											_tcpcom_read,
											_tcpcom_write,
											_tcpcom_ioctl,
											0u
										};
	int32_t err;

	/* Allocate TCPCOM bundle */
	driver_bundle = (TCPCOM_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(TCPCOM_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_TCPCOM__OUT_OF_HEAP;
	}

	memset(driver_bundle, 0, sizeof(TCPCOM_DRIVER_BUNDLE));

	init.bundle = driver_bundle;

	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__TCPCOM, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}

	return IO_ERROR_TCPCOM__OK;
}


/*******************************************************************************
* Function Name: _tcpcom_open
********************************************************************************
Description: Opens and initializes the TCPCOM driver
Inputs:
IO_HANDLER_PTR - TCPCOM handler
void * - TCPCOM init parameters (TCPCOM_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_TCPCOM__OK - initialization OK
*******************************************************************************/
static int32_t _tcpcom_open(IO_HANDLER_PTR handler, void * init)
{
	TCPCOM_DRIVER_BUNDLE_PTR	driver_bundle = (TCPCOM_DRIVER_BUNDLE_PTR) handler->bundle;
	TCPCOM_INIT_STRUCT_PTR	tcpcom_init = (TCPCOM_INIT_STRUCT_PTR) init;
	BaseType_t err_os;

	driver_bundle->socket__AC = -1;		// Set to invalid value

	driver_bundle->tcp_config.local_ip = tcpcom_init->local_ip;
	driver_bundle->tcp_config.local_netmask = tcpcom_init->local_netmask;
	driver_bundle->tcp_config.local_gw = tcpcom_init->local_gw;
	driver_bundle->tcp_config.local_port = tcpcom_init->local_port;
	driver_bundle->tcp_config.remote_ip = tcpcom_init->remote_ip;
	driver_bundle->tcp_config.remote_port = tcpcom_init->remote_port;
	driver_bundle->tcp_config.tcp_connect_attempts = tcpcom_init->tcp_connect_attempts;
	driver_bundle->tcp_config.tcp_rx_timeout_ms = tcpcom_init->tcp_rx_timeout_ms;
	driver_bundle->tcp_config.tcp_tx_timeout_ms = tcpcom_init->tcp_tx_timeout_ms;

	driver_bundle->queue__TX = xQueueCreate(10, sizeof(TCPCOM_PACKET));
	driver_bundle->mailbox__PHASIS_CONTROL = xQueueCreate(1, sizeof(TCPCOM_PACKET));
	driver_bundle->mailbox__DESCRITE_SIGNAL_TEST = xQueueCreate(1, sizeof(TCPCOM_PACKET));
																		/**2 **/
	err_os = xTaskCreate(_tcpcom_task_rx, "TCPCOM_RX", 256, driver_bundle, osPriorityNormal + 1, &driver_bundle->task__RX );
	if (err_os != pdPASS)
	{
		return IO_ERROR_TCPCOM__OUT_OF_HEAP;
	}
																			/**2 **/
	err_os = xTaskCreate(_tcpcom_task_conn, "TCPCOM_CONN", 256, driver_bundle, osPriorityNormal + 1 , &driver_bundle->task__CONN );
	if (err_os != pdPASS)
	{
		return IO_ERROR_TCPCOM__OUT_OF_HEAP;
	}

	return IO_ERROR_TCPCOM__OK;
}


/*******************************************************************************
* Function Name: _tcpcom_read
********************************************************************************
Description: Reads the OTS TCPCOM driver
Inputs:
IO_HANDLER_PTR - TCPCOM handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_TCPCOM__OK - read OK
*******************************************************************************/
static int32_t _tcpcom_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	// TODO: implement with queue set - block on both queues
	// Meanwhile, use PEEK IOCTL
	return IO_ERROR_TCPCOM__OK;
}


/*******************************************************************************
* Function Name: _tcpcom_write
********************************************************************************
Description: Writes to the TCPCOM driver
Inputs:
IO_HANDLER_PTR - TCPCOM handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_TCPCOM__OK - write OK
*******************************************************************************/
static int32_t _tcpcom_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	TCPCOM_DRIVER_BUNDLE_PTR driver_bundle = (TCPCOM_DRIVER_BUNDLE_PTR) handler->bundle;

	return _tcpcom_icd_send_message(driver_bundle, (TCPCOM_PACKET_PTR)ptr);
}


/*******************************************************************************
* Function Name: _tcpcom_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - TCPCOM handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_TCPCOM__OK - command execution OK
IO_ERROR_TCPCOM__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
static int32_t _tcpcom_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	TCPCOM_DRIVER_BUNDLE_PTR driver_bundle = (TCPCOM_DRIVER_BUNDLE_PTR) handler->bundle;
	BaseType_t err_os;
	int32_t err = IO_ERROR_TCPCOM__OK;
	TCPCOM_PACKET_PTR packet = (TCPCOM_PACKET_PTR)ptr;

	switch (ctl)
	{
	  case IO_IOCTL_TCPCOM__CONNECT_AC:
		  if(!driver_bundle->is_connected__AC)
		  {
			//xSemaphoreTake( xSemaphore_OTBB_CONNECT_1, ( TickType_t ) pdMS_TO_TICKS (0));
			err = _tcpcom_connect(driver_bundle);
			//xSemaphoreGive( xSemaphore_OTBB_CONNECT_1);
		  }
		  break;

	  case IO_IOCTL_TCPCOM__IS_CONNECTED_AC:
		  *(uint8_t*)ptr = driver_bundle->is_connected__AC;
		  //if(!driver_bundle->is_connected__AC)
			 // err = _tcpcom_connect(driver_bundle);
		  break;

	  case IO_IOCTL_TCPCOM__PEEK_INPUT_MESSAGE_DESCRITE_SIGNAL_TEST:
		  err_os = xQueuePeek( driver_bundle->mailbox__DESCRITE_SIGNAL_TEST, ptr, 0);
		  if (err_os != pdTRUE || packet->HDR.MSGCOUNTER == driver_bundle->last_handled_sig_test_messge)
			  err = IO_ERROR_TCPCOM__NO_AVAILABLE_MESSAGES;
		  else
			  driver_bundle->last_handled_sig_test_messge = packet->HDR.MSGCOUNTER;
		  break;

	  case IO_IOCTL_TCPCOM__PEEK_INPUT_MESSAGE_PHASIS_CONTROL:
		  err_os = xQueuePeek( driver_bundle->mailbox__PHASIS_CONTROL, ptr, 0);
		  if (err_os != pdTRUE || packet->HDR.MSGCOUNTER == driver_bundle->last_handled_phasis_messge)
			  err = IO_ERROR_TCPCOM__NO_AVAILABLE_MESSAGES;
		  else
			  driver_bundle->last_handled_phasis_messge = packet->HDR.MSGCOUNTER;
		  break;


	  default:
		err = IO_ERROR_TCPCOM__UNKNOWN_IOCTL;
		break;
	}

	return err;
}
