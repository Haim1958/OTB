/**
 ******************************************************************************
 * @file    sercom_tasks.c
 * @brief   Serial Communication Protocol OS tasks implementations
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
#include "cmsis_os.h"
#include "UserLib.h"
#include "../inc/sercom.h"
#include "../inc/sercom_private.h"

/* Global variables */

/* Functions declarations */


/* Functions definitions */


void _sercom_task_tx(void * argument)
{
	SERCOM_INSTANCE_PTR driver_instance = (SERCOM_INSTANCE_PTR) argument;
	SERCOM_PACKET tx_packet;
	BaseType_t os_err;

	for (;;)
	{
		os_err = xQueueReceive(driver_instance->tx_queue, &tx_packet, portMAX_DELAY);
		if (os_err == pdPASS)
		{
			_sercom_send_packet(driver_instance, &tx_packet);
		}
	}
}


void _sercom_task_rx(void * argument)
{
	SERCOM_INSTANCE_PTR driver_instance = (SERCOM_INSTANCE_PTR) argument;
	uint8_t input;
	int32_t err;

	for (;;)
	{
		/* Handle RX */
		err = io_read(driver_instance->driver, &input, 1);
		if (err != IO_ERROR__OK)
		{
			continue;
		}

		/* Handle input byte */
		_sercom_protocol_handle_input_byte(driver_instance, input);
	}
}


