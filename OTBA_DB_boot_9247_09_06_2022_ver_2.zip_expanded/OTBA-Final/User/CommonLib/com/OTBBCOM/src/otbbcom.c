/**
 ******************************************************************************
 * @file    otbbcom.c
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

#include "../inc/otbbcom_icd.h"
#include "../inc/otbbcom_private.h"

/* Global variables */



/* Functions declarations */

static int32_t _otbbcom_open(IO_HANDLER_PTR h, void*);
static int32_t _otbbcom_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _otbbcom_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _otbbcom_ioctl(IO_HANDLER_PTR, uint32_t, void*);



#define OTBBCOM_EXAM_RX_TASK_STACK_SIZE		256
StaticTask_t OTBBCOM_EXAM_RX_TASK_STACK_BUFFER;
StackType_t OTBBCOM_EXAM_RX_TASK_STACK[OTBBCOM_EXAM_RX_TASK_STACK_SIZE];

/* Functions definitions */

/*******************************************************************************
* Function Name: install_otbbcom
********************************************************************************
Description: Installs the OTBBCOM driver
Inputs:
OTBBCOM_INSTALL_STRUCT_PTR - OTBBCOM install parameters
Outputs:
IO_HANDLER_PTR - installed handler
NULL - the instal process failed
*******************************************************************************/
int32_t install_otbbcom (OTBBCOM_INSTALL_STRUCT_PTR otbbcom_init)

{
	OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_otbbcom_open,
											_otbbcom_read,
											_otbbcom_write,
											_otbbcom_ioctl,
											0u
										};
	int32_t err;

	/* Allocate OTBBCOM bundle */
	driver_bundle = (OTBBCOM_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(OTBBCOM_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_OTBBCOM__OUT_OF_HEAP;
	}


	gl_otbb_bundle = (OTBBCOM_DRIVER_BUNDLE_PTR) driver_bundle;
	memset(driver_bundle, 0, sizeof(OTBBCOM_DRIVER_BUNDLE));

	init.bundle = driver_bundle;

	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__OTBBCOM, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}

	return IO_ERROR_OTBBCOM__OK;
}


/*******************************************************************************
* Function Name: _otbbcom_open
********************************************************************************
Description: Opens and initializes the OTBBCOM driver
Inputs:
IO_HANDLER_PTR - OTBBCOM handler
void * - OTBBCOM init parameters (OTBBCOM_INIT_STRUCT_PTR)
Outputs:
IO_ERROR_OTBBCOM__OK - initialization OK
*******************************************************************************/
static int32_t _otbbcom_open(IO_HANDLER_PTR handler, void * init)
{
	OTBBCOM_DRIVER_BUNDLE_PTR	driver_bundle = (OTBBCOM_DRIVER_BUNDLE_PTR) handler->bundle;
	OTBBCOM_INIT_STRUCT_PTR	otbbcom_init = (OTBBCOM_INIT_STRUCT_PTR) init;

	BaseType_t err_os;

	driver_bundle->socket = -1;		// Set to invalid value
	driver_bundle->socket_accept = -1;

	driver_bundle->OTBB_is_connected = 0;

	driver_bundle->server_conn_socket = -1;

	driver_bundle->release_sem_receive = 1;
	driver_bundle->task_give_notify = 0;

	driver_bundle->tcp_config.type = otbbcom_init->type;
	driver_bundle->tcp_config.remote_ip = otbbcom_init->remote_ip;
	driver_bundle->tcp_config.tcp_connect_attempts = otbbcom_init->tcp_connect_attempts;
	driver_bundle->tcp_config.tcp_rx_timeout_ms = otbbcom_init->tcp_rx_timeout_ms;
	driver_bundle->tcp_config.tcp_tx_timeout_ms = otbbcom_init->tcp_tx_timeout_ms;

	driver_bundle->queue__TX = xQueueCreate(10, sizeof(OTBBCOM_PACKET));
	driver_bundle->mailbox__RX = xQueueCreate(1, sizeof(OTBBCOM_PACKET));

	queue_TX_GLB = driver_bundle->queue__TX;
	queue_mailbox_RX = driver_bundle->mailbox__RX;

	    gl_tcp_remote_ip = driver_bundle->tcp_config.remote_ip;
	    gl_tcp_connect_attempts = driver_bundle->tcp_config.tcp_connect_attempts;
	    gl_tcp_rx_timeout_ms = driver_bundle->tcp_config.tcp_rx_timeout_ms;
	    gl_tcp_tx_timeout_ms = driver_bundle->tcp_config.tcp_tx_timeout_ms;

	    gl_release_sem_receive = driver_bundle->release_sem_receive;
	    gl_task_give_notify = driver_bundle->task_give_notify;





	err_os = xTaskCreate(_otbbcom_task_rx, "OTBBCOM_RX", 512, driver_bundle, osPriorityNormal + 1, &driver_bundle->task__RX );
	if (err_os != pdPASS)
	{
		return IO_ERROR_OTBBCOM__OUT_OF_HEAP;
	}

	PACKET_RECEIVE_FROM_OTBB = (QueueHandle_t)driver_bundle->mailbox__RX;
	Handle_OTBB_task_RX = (TaskHandle_t )driver_bundle->task__RX;
	gl_otbb_task_rx = (TaskHandle_t)driver_bundle->task__RX ;
	vTaskSuspend( driver_bundle->task__RX );


    // start a task for handling connection
//	err_os = xTaskCreate(_otbbcom_task_conn, "OTBBCOM_CONN", 256, driver_bundle, 1, &driver_bundle->task__CONN);
//	if (err_os != pdPASS)
//	{
//		return IO_ERROR_OTBBCOM__OUT_OF_HEAP;
//	}

	return IO_ERROR_OTBBCOM__OK;
}


/*******************************************************************************
* Function Name: _otbbcom_read
********************************************************************************
Description: Reads the OTS OTBBCOM driver
Inputs:
IO_HANDLER_PTR - OTBBCOM handler
uint8_t * - data to be read
uint32_t - length of the data
Outputs:
IO_ERROR_OTBBCOM__OK - read OK
*******************************************************************************/
static int32_t _otbbcom_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle = (OTBBCOM_DRIVER_BUNDLE_PTR) handler->bundle;
	OTBBCOM_PACKET_PTR packet = (OTBBCOM_PACKET_PTR)ptr;
	BaseType_t err_os;
	int32_t err = IO_ERROR_OTBBCOM__OK;

	xSemaphoreTake( xSemaphore_OTBB_Queue, ( TickType_t ) pdMS_TO_TICKS (0));
	err_os = xQueuePeek(driver_bundle->mailbox__RX, ptr, 0);
	xSemaphoreGive( xSemaphore_OTBB_Queue);

	if (err_os != pdTRUE || packet->HDR.MSGCOUNTER == driver_bundle->last_handled_msg_counter)
	{
		err = IO_ERROR_OTBBCOM__NO_AVAILABLE_MESSAGES;
	}
	else
	{
		driver_bundle->last_handled_msg_counter = packet->HDR.MSGCOUNTER;
	}

	return err;
}


/*******************************************************************************
* Function Name: _otbbcom_write
********************************************************************************
Description: Writes to the OTBBCOM driver
Inputs:
IO_HANDLER_PTR - OTBBCOM handler
uint8_t * - data to be written
uint32_t - length of the data
Outputs:
IO_ERROR_OTBBCOM__OK - write OK
*******************************************************************************/
static int32_t _otbbcom_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle = (OTBBCOM_DRIVER_BUNDLE_PTR) handler->bundle;

	return _otbbcom_icd_send_message(driver_bundle, (OTBBCOM_PACKET_PTR)ptr);
}


/*******************************************************************************
* Function Name: _otbbcom_ioctl
********************************************************************************
Description: Executes IOCTL command
Inputs:
IO_HANDLER_PTR - OTBBCOM handler
uint32_t - ioctl command to be performed
void * - data to be used/returned upon the command execution
Outputs:
IO_ERROR_OTBBCOM__OK - command execution OK
IO_ERROR_OTBBCOM__UNKNOWN_IOCTL - the received command is unfamiliar
*******************************************************************************/
static int32_t _otbbcom_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle = (OTBBCOM_DRIVER_BUNDLE_PTR) handler->bundle;
	int32_t err = IO_ERROR_OTBBCOM__OK;

	switch (ctl)
	{
	  case IO_IOCTL_OTBBCOM__CONNECT:
		  err = _otbbcom_connect(driver_bundle);
		  break;

	  case IO_IOCTL_OTBBCOM_IS_RE_CONNECTED:
	 		  err = _otbbcom_reopen();
	 		  break;

	  case IO_IOCTL_OTBBCOM_IS_RE_DELETED:
	  	 		  err = _otbbcom_redelete(driver_bundle);
	  	 		  break;
	  case IO_IOCTL_OTBBCOM__IS_CONNECTED:
		  *(uint8_t*)ptr = driver_bundle->OTBB_is_connected;
		 // *(uint8_t*)ptr = (driver_bundle->socket >= 0);
		  break;

	  default:
		err = IO_ERROR_OTBBCOM__UNKNOWN_IOCTL;
		break;
	}

	return err;
}
