/**
 ******************************************************************************
 * @file    tcpcom_task_rx.c
 * @brief   TCP Communication receive task.
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

#include "cmsis_os.h"

#include "../../SPECTRA/inc/tcpcom_icd.h"
#include "../../SPECTRA/inc/tcpcom_private.h"

static TimerHandle_t tcpcom_task_rx_timer;
static void pfunA_tcpcom_task_rx_timer(TimerHandle_t timer) {
	TCPCOM_DRIVER_BUNDLE_PTR driver_bundle=
			(TCPCOM_DRIVER_BUNDLE_PTR) pvTimerGetTimerID(timer);


       if(driver_bundle->time_delay_task_rx++ >= 2) // 30 msec
       {
    	   driver_bundle->time_delay_task_rx = 0;
    	   xTaskNotifyGive(driver_bundle->task__RX);

       }

}





void _tcpcom_task_rx(void * argument)
{
	TCPCOM_DRIVER_BUNDLE_PTR driver_bundle = (TCPCOM_DRIVER_BUNDLE_PTR) argument;
	TCPCOM_PACKET packet;
	int32_t read_len;

	driver_bundle->last_received_phasis_msg_counter = driver_bundle->last_handled_phasis_messge = -1; // initialization
	driver_bundle->last_received_sig_test_msg_counter = driver_bundle->last_handled_sig_test_messge = -1; // initialization

	driver_bundle->time_delay_task_rx = 0;


	tcpcom_task_rx_timer = xTimerCreate("TCP_TASK_RX_Timer", pdMS_TO_TICKS(5),
			                                     pdTRUE, driver_bundle, pfunA_tcpcom_task_rx_timer);

	xTimerStart(tcpcom_task_rx_timer, 0);



	for(;;)
	{

		ulTaskNotifyTake(pdTRUE, (TickType_t) portMAX_DELAY);

		if (driver_bundle->is_connected__AC)
		{

			read_len = lwip_recv(driver_bundle->socket__AC, &packet, sizeof(packet), 0);

			if (read_len <= 0)
			{
				// Socket closed
				//xSemaphoreTake( xSemaphore_OTBB_CONNECT_1, ( TickType_t ) pdMS_TO_TICKS (0));
				driver_bundle->last_received_phasis_msg_counter = UINT32_MAX_MSG_COUNTER;
				driver_bundle->last_received_sig_test_msg_counter = UINT32_MAX_MSG_COUNTER;
				close(driver_bundle->socket__AC);
				driver_bundle->socket__AC = -1;
				driver_bundle->is_connected__AC = 0;
				//xSemaphoreGive( xSemaphore_OTBB_CONNECT_1);
				//printf(":::TCPCOM::: Disconnected from TCP server\r\n");
			}
			else if (read_len > sizeof(TCPCOM_HEADER))
			{
				QueueHandle_t dest_queue;
				BaseType_t bad_message = pdFALSE;

				_tcpcom_icd_header_ntoh(&packet);

				// Check the header:
				if (packet.HDR.SOURCE != TCPCOM_ICD_ID__SPECTRA)
					bad_message = pdTRUE;
				if (packet.HDR.DESTINATION != TCPCOM_ICD_ID__OTBA)
					bad_message = pdTRUE;

				switch (packet.HDR.OPCODE)
				{
					case TCPCOM_ICD_OPCODE__PHASIS_CONTROL:
						if (packet.HDR.SIZE != sizeof(packet.HDR) - sizeof(packet.HDR.SIZE) + sizeof(TCPCOM_PACKET_BODY__PHASIS_CONTROL))
						{
							bad_message = pdTRUE;
							break;
						}
						if (!((packet.HDR.MSGCOUNTER > driver_bundle->last_received_phasis_msg_counter) ||
								(10 > packet.HDR.MSGCOUNTER && (UINT32_MAX_MSG_COUNTER - driver_bundle->last_received_phasis_msg_counter < 10)))) // allow some tolerance here
						{
							bad_message = pdTRUE;
							break;
						}
						dest_queue = driver_bundle->mailbox__PHASIS_CONTROL;
						driver_bundle->last_received_phasis_msg_counter = packet.HDR.MSGCOUNTER;
						break;

					case TCPCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_TEST:
						if (packet.HDR.SIZE != sizeof(packet.HDR) - sizeof(packet.HDR.SIZE) + sizeof(TCPCOM_PACKET_BODY__DISCRETE_RF_SIGNAL_TEST))
						{
							bad_message = pdTRUE;
							break;
						}
						if (!((packet.HDR.MSGCOUNTER > driver_bundle->last_received_sig_test_msg_counter) ||
								(10 > packet.HDR.MSGCOUNTER && (UINT32_MAX_MSG_COUNTER - driver_bundle->last_received_sig_test_msg_counter < 10)))) // allow some tolerance here
						{
							bad_message = pdTRUE;
							break;
						}
						dest_queue = driver_bundle->mailbox__DESCRITE_SIGNAL_TEST;
						driver_bundle->last_received_sig_test_msg_counter = packet.HDR.MSGCOUNTER;
						break;

					default:
						bad_message = pdTRUE;
						break;
				}

				if (bad_message)
				{
					/*
					 * -#42549007-V2-OTS SOFTWARE REQUIREMENTS.DOCX, page 20, and also section "Header check in TCP/IP" on page 21.
					 *
					 * ignore message
					 * close connection
					 */
					driver_bundle->last_received_phasis_msg_counter = -1;
					driver_bundle->last_received_sig_test_msg_counter = -1;
					close(driver_bundle->socket__AC);
					driver_bundle->is_connected__AC = 0;
					printf(":::TCPCOM::: Disconnected to TCP server\r\n");
				}
				else
				{
					_tcpcom_icd_body_ntoh(&packet);
					_tcpcom_icd_print_packet(&packet, pdTRUE);
					xQueueOverwrite(dest_queue, &packet);
				}
			}
		}
		else
		{
			//osDelay(1);
		}
	}
}
