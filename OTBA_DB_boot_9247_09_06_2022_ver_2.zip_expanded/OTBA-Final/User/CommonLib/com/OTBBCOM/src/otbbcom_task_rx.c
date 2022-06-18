/**
 ******************************************************************************
 * @file    otbbcom_task_rx.c
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

#include "../inc/otbbcom_icd.h"
#include "../inc/otbbcom_private.h"


//TimerHandle_t OTBBcom_task_rx_timer;

extern SemaphoreHandle_t xSemaphore_IP_Alloct;
extern SemaphoreHandle_t xSemaphore_IP_Alloct_Accept;
									   
extern SemaphoreHandle_t xSemaphore_OTBB_CONNECT_1;
int32_t test_num = -1000;



static uint32_t count_delay_task_rx =0;
extern SemaphoreHandle_t xSemaphore_OTBB_CONNECT_1;
static void pfunB_B_Delay_task_rx_timer(TimerHandle_t timer)
{

	OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle = (OTBBCOM_DRIVER_BUNDLE_PTR) pvTimerGetTimerID(timer);

	if(++count_delay_task_rx >= 2)
	{
		count_delay_task_rx = 0;
		xTaskNotifyGive(driver_bundle->task__RX);
	}

}




void _otbbcom_task_rx(void * argument)
{
	OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle = (OTBBCOM_DRIVER_BUNDLE_PTR) argument;
	OTBBCOM_PACKET packet;
	int32_t read_len;
	int32_t err;

    size_t NumBytesBeSentPacket;
	char data_buffer[19] ;
	driver_bundle->last_received_msg_counter = -1; // initialization
	driver_bundle->last_handled_msg_counter = -1; // initialization

	if(driver_bundle->task_give_notify == 0)
	{


		count_delay_task_rx =0;
		driver_bundle->OTBBcom_task_rx_timer = xTimerCreate("Delay_task_rx_timer",
						pdMS_TO_TICKS(5),
						pdTRUE,
						driver_bundle,
						pfunB_B_Delay_task_rx_timer);

	         xTimerStart(driver_bundle->OTBBcom_task_rx_timer, 0);


	//if(driver_bundle->task_give_notify == 0)
	//{
		xSemaphoreTake( xSemaphore_IP_Alloct, ( TickType_t ) pdMS_TO_TICKS (0));
		if (driver_bundle->tcp_config.type == OTBBCOM_EDGE_TYPE__OTBA)
		{
			///if(driver_bundle->server_conn_socket < 0)
			_otbbcom_create_configure_socket_server(driver_bundle);
		}
		xSemaphoreGive( xSemaphore_IP_Alloct);
	}


 for(;;)
  {
        /** in the last version 9033 has been omitted **/
		ulTaskNotifyTake(pdTRUE, ( TickType_t )/** pdMS_TO_TICKS**/ (portMAX_DELAY));



	   if(driver_bundle->task_give_notify == 1)
	   {
		 //ulTaskNotifyTake(pdTRUE, ( TickType_t )/** pdMS_TO_TICKS**/ (portMAX_DELAY));
		 driver_bundle->task_give_notify = 0;
	   }

		if (driver_bundle->socket_accept < 0)
		{
			if (driver_bundle->tcp_config.type == OTBBCOM_EDGE_TYPE__OTBA)
			{

				if(driver_bundle->server_conn_socket < 0)
					_otbbcom_create_configure_socket_server(driver_bundle);
				///xSemaphoreTake( xSemaphore_OTBB_Accept_RX, ( TickType_t ) pdMS_TO_TICKS (1));
				driver_bundle->socket_accept = _otbbcom_accept_connection(driver_bundle);
				//xSemaphoreGive( xSemaphore_OTBB_Accept_RX);


				if (err == IO_ERROR_OTBBCOM__FAILED_CREATING_SOCKET)
				{
					printf(":::OTBBCOM::: Failed creating socket, retrying...\n");
				}
				else if (err != IO_ERROR_OTBBCOM__OK)
				{
					printf(":::OTBBCOM::: Failed connecting to socket, unexpected error 0x%08lX\n", err);
				}
			}
			else if (driver_bundle->tcp_config.type == OTBBCOM_EDGE_TYPE__OTBB)
			{
				// Not connected, connect now
				//err = _otbbcom_connect(driver_bundle);
				if (err == IO_ERROR_OTBBCOM__FAILED_CREATING_SOCKET)
				{
					printf(":::OTBBCOM::: Failed creating socket, retrying...\n");
				}
				else if (err == IO_ERROR_OTBBCOM__FAILED_CONNECTING_SOCKET)
				{
					printf(":::OTBBCOM::: Failed connecting to socket, retrying...\n");
				}
				else if (err != IO_ERROR_OTBBCOM__OK)
				{
					printf(":::OTBBCOM::: Failed connecting to socket, unexpected error 0x%08lX\n", err);
				}
			}

			//osDelay(2);//osDelay(10); 06_Oct_2021
		}
		else if (driver_bundle->socket_accept >= 0)
		{

			 //xSemaphoreTake( xSemaphore_OTBB_Receive_RX, ( TickType_t ) pdMS_TO_TICKS (1));
			 read_len = lwip_recv(driver_bundle->socket_accept, &packet, sizeof(packet), 0);
			 //xSemaphoreGive( xSemaphore_OTBB_Receive_RX);
         //if(driver_bundle->release_sem_receive == 1)
          //{
        	  //driver_bundle->release_sem_receive = 0;
               lwip_close(driver_bundle->server_accept);
			if (read_len <= 0)
			{
				// Socket closed
				driver_bundle->last_received_msg_counter = UINT32_MAX_MSG_COUNTER;
				close(driver_bundle->socket_accept);
				driver_bundle->socket_accept = -1;
				driver_bundle->OTBB_is_connected = 0;

				//printf(":::OTBBCOM::: Disconnected from TCP server\r\n");
			}
			else if (read_len >= sizeof(OTBBCOM_HEADER))
			{
				BaseType_t bad_message = pdFALSE;

				_otbbcom_icd_header_ntoh(&packet);

				driver_bundle->OTBB_is_connected = 1;
				packet.HDR.OTBB_IS_CONNECTED = 1;												  
				// Check the header:
				if ((driver_bundle->tcp_config.type == OTBBCOM_EDGE_TYPE__OTBA &&
						packet.HDR.DESTINATION == OTBBCOM_ICD_ID__OTBA &&
						packet.HDR.SOURCE == OTBBCOM_ICD_ID__OTBB) ||
					(driver_bundle->tcp_config.type == OTBBCOM_EDGE_TYPE__OTBB &&
						packet.HDR.DESTINATION == OTBBCOM_ICD_ID__OTBB &&
						packet.HDR.SOURCE == OTBBCOM_ICD_ID__OTBA))
				{
					// OK, NOP
				}
				else
				{
					bad_message = pdTRUE;
				}

//				if (packet.HDR.MSGCOUNTER > driver_bundle->last_received_msg_counter ||
//						(packet.HDR.MSGCOUNTER < 10 &&
//						 driver_bundle->last_received_msg_counter > UINT32_MAX_MSG_COUNTER - 10))
//				{
//					driver_bundle->last_received_msg_counter = packet.HDR.MSGCOUNTER;
//				}
//				else
//				{
//					bad_message = pdTRUE;
//				}

				if (!bad_message)
				{
					_otbbcom_icd_body_ntoh(&packet);
					_otbbcom_icd_print_packet(&packet, pdTRUE);
					xSemaphoreTake( xSemaphore_OTBB_Queue, ( TickType_t ) pdMS_TO_TICKS (0));
					xQueueOverwrite(driver_bundle->mailbox__RX, &packet);
					xSemaphoreGive( xSemaphore_OTBB_Queue);
				}
			}//else if (read_len >= sizeof(OTBBCOM_HEADER))
         //}//if(driver_bundle->release_sem_receive == 1)
       }
		//osDelay(5);
   }
}
