/**
 ******************************************************************************
 * @file    tcpcom_icd.c
 * @brief   TCP Communication ICD utility functions.
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

#include "../../SPECTRA/inc/tcpcom_private.h"

extern SemaphoreHandle_t xSemaphore_OTBB_CONNECT_1;

static int32_t _tcpcom_create_configure_socket(TCPCOM_DRIVER_BUNDLE_PTR driver_bundle)
{
	struct sockaddr_in clientAddr;
	int32_t optval;
	int32_t sock_err;

	// New socket
	driver_bundle->socket__AC = socket(AF_INET, SOCK_STREAM, 0);
	if (driver_bundle->socket__AC == -1)
	{
		printf(":::TCPCOM::: socket failed\r\n");
		return IO_ERROR_TCPCOM__FAILED_CREATING_SOCKET;
	}


	memset(&clientAddr, 0, sizeof(struct sockaddr_in));
	clientAddr.sin_len = sizeof(clientAddr);
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = PP_HTONS(driver_bundle->tcp_config.local_port);
	clientAddr.sin_addr.s_addr = INADDR_ANY;//inet_addr(IP_ADDRESS_OTBA);//INADDR_ANY;

	sock_err = bind(driver_bundle->socket__AC, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
	if (-1 == sock_err)
	{
		close(driver_bundle->socket__AC);
		driver_bundle->socket__AC = -1;
		printf(":::TCPCOM::: bind socket to local port failed\r\n");
		return IO_ERROR_TCPCOM__FAILED_CREATING_SOCKET;
	}

	// In order to reuse previously badly closed socket
	optval = 1;
	sock_err = setsockopt(driver_bundle->socket__AC, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(uint32_t));
	if (sock_err < 0)
	{
		close(driver_bundle->socket__AC);
		driver_bundle->socket__AC = -1;
		printf(":::TCPCOM::: setsockopt SO_REUSEADDR failed\r\n");
		return IO_ERROR_TCPCOM__FAILED_CREATING_SOCKET;
	}

	// In order to detect a dead connection
	optval = 1;
	sock_err = setsockopt(driver_bundle->socket__AC, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(uint32_t));
	if (sock_err < 0)
	{
		close(driver_bundle->socket__AC);
		driver_bundle->socket__AC = -1;
		printf(":::TCPCOM::: setsockopt SO_KEEPALIVE failed\r\n");
		return IO_ERROR_TCPCOM__FAILED_CREATING_SOCKET;
	}

	// Time before the first keep alive probe is sent
	optval = 2;
	sock_err = setsockopt(driver_bundle->socket__AC, IPPROTO_TCP, TCP_KEEPIDLE, &optval, sizeof(uint32_t));
	if (sock_err < 0)
	{
		close(driver_bundle->socket__AC);
		driver_bundle->socket__AC = -1;
		printf(":::TCPCOM::: setsockopt TCP_KEEPIDLE failed\r\n");
		return IO_ERROR_TCPCOM__FAILED_CREATING_SOCKET;
	}

	// Time between keep alive probes
	optval = 2;
	sock_err = setsockopt(driver_bundle->socket__AC, IPPROTO_TCP, TCP_KEEPINTVL, &optval, sizeof(uint32_t));
	if (sock_err < 0)
	{
		close(driver_bundle->socket__AC);
		driver_bundle->socket__AC = -1;
		printf(":::TCPCOM::: setsockopt TCP_KEEPINTVL failed\r\n");
		return IO_ERROR_TCPCOM__FAILED_CREATING_SOCKET;
	}

	// The number of unacknowledged probes to send before considering the connection dead
	// and notifying the application layer
	optval = 2;
	sock_err = setsockopt(driver_bundle->socket__AC, IPPROTO_TCP, TCP_KEEPCNT, &optval, sizeof(uint32_t));
	if (sock_err < 0)
	{
		close(driver_bundle->socket__AC);
		driver_bundle->socket__AC = -1;
		printf(":::TCPCOM::: setsockopt TCP_KEEPCNT failed\r\n");
		return IO_ERROR_TCPCOM__FAILED_CREATING_SOCKET;
	}

	// In order to deliver messages immediately (no buffering)
	optval = 1;
	sock_err = setsockopt(driver_bundle->socket__AC, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(uint32_t));
	if (sock_err < 0)
	{
		close(driver_bundle->socket__AC);
		driver_bundle->socket__AC = -1;
		printf(":::TCPCOM::: setsockopt TCP_NODELAY failed\r\n");
		return IO_ERROR_TCPCOM__FAILED_CREATING_SOCKET;
	}

	// Send buffer size. Should be configurable
	// Warning - unimplemented by LwIP
//	optval = 1;
//	sock_err = setsockopt(driver_bundle->socket__AC, SOL_SOCKET, SO_SNDBUF, &optval, sizeof(uint32_t));
//	if (sock_err != 0)
//	{
//		return IO_ERROR_TCPCOM__FAILED_CREATING_SOCKET;
//	}

	// Receive buffer size. Should be cofigurable
	optval = 65536;
	sock_err = setsockopt(driver_bundle->socket__AC, SOL_SOCKET, SO_RCVBUF, &optval, sizeof(uint32_t));
	if (sock_err < 0)
	{
		close(driver_bundle->socket__AC);
		driver_bundle->socket__AC = -1;
		printf(":::TCPCOM::: setsockopt SO_RCVBUF failed\r\n");
		return IO_ERROR_TCPCOM__FAILED_CREATING_SOCKET;
	}

	return IO_ERROR_TCPCOM__OK;
}

int32_t _tcpcom_connect(TCPCOM_DRIVER_BUNDLE_PTR driver_bundle)
{
	struct sockaddr_in Serveraddr;
	int32_t lwip_err;
	int32_t err;

	err = _tcpcom_create_configure_socket(driver_bundle);
	if (err != IO_ERROR_TCPCOM__OK)
	{
		return err;
	}
//	printf(":::TCPCOM::: Created socket %ld\r\n", driver_bundle->socket__AC);
	memset(&Serveraddr, 0, sizeof(struct sockaddr_in));
	Serveraddr.sin_len = sizeof(Serveraddr);
	Serveraddr.sin_family = AF_INET;
	Serveraddr.sin_port = PP_HTONS(driver_bundle->tcp_config.remote_port);
	Serveraddr.sin_addr.s_addr = inet_addr(driver_bundle->tcp_config.remote_ip);


	//vTaskSuspend(Gl_handle_rx);
	lwip_err = connect(driver_bundle->socket__AC, (struct sockaddr*)&Serveraddr, sizeof(Serveraddr));
	//vTaskResume(Gl_handle_rx);

	if (lwip_err < 0)
	{
		close(driver_bundle->socket__AC);
		driver_bundle->socket__AC = -1;
//		printf(":::TCPCOM::: Connect failed\r\n");
		return IO_ERROR_TCPCOM__FAILED_CONNECTING_SOCKET;
	}

	//printf(":::TCPCOM::: Connected to TCP server\r\n");

	driver_bundle->is_connected__AC = 1u;

	return IO_ERROR_TCPCOM__OK;
}



static TimerHandle_t tcpcom_task_conn_timer;
static void pfunA_tcpcom_task_conn_timer(TimerHandle_t timer) {
	TCPCOM_DRIVER_BUNDLE_PTR driver_bundle=
			(TCPCOM_DRIVER_BUNDLE_PTR) pvTimerGetTimerID(timer);


       if(driver_bundle->time_delay_task_conn++ >= 1)
       {
    	   driver_bundle->time_delay_task_conn = 0;
    	   xTaskNotifyGive(driver_bundle->task__CONN);

       }

}


void _tcpcom_task_conn(void * argument)
{
	TCPCOM_DRIVER_BUNDLE_PTR driver_bundle = (TCPCOM_DRIVER_BUNDLE_PTR) argument;
	int32_t err;

	driver_bundle->time_delay_task_conn = 0;

	tcpcom_task_conn_timer = xTimerCreate("TCP_TASK_CONN_Timer", pdMS_TO_TICKS(4),
	                                     pdTRUE, driver_bundle, pfunA_tcpcom_task_conn_timer);

	xTimerStart(tcpcom_task_conn_timer, 0);


	if (!driver_bundle->is_connected__AC)
    {
	  xSemaphoreTake( xSemaphore_OTBB_CONNECT_1, ( TickType_t ) pdMS_TO_TICKS (0));
	  err = _tcpcom_connect(driver_bundle);
	  xSemaphoreGive( xSemaphore_OTBB_CONNECT_1);
    }

	for (;;)
	{

		ulTaskNotifyTake(pdTRUE, (TickType_t) portMAX_DELAY);

		if (!driver_bundle->is_connected__AC)
		{
			// Not connected, connect now
			//xSemaphoreTake( xSemaphore_OTBB_CONNECT_1, ( TickType_t ) pdMS_TO_TICKS (0));
			/***********************************************/

			//vTaskSuspend();
			err = _tcpcom_connect(driver_bundle);
			/***********************************************/

			//xSemaphoreGive( xSemaphore_OTBB_CONNECT_1);

			if (err == IO_ERROR_TCPCOM__FAILED_CREATING_SOCKET)
			{
//				printf(":::TCPCOM::: Failed creating socket, retrying...\n");
			}
			else if (err == IO_ERROR_TCPCOM__FAILED_CONNECTING_SOCKET)
			{
//				printf(":::TCPCOM::: Failed connecting to socket, retrying...\n");
			}
			else if (err != IO_ERROR_TCPCOM__OK)
			{
//				printf(":::TCPCOM::: Failed connecting to socket, unexpected error 0x%08lX\n", err);
			}
		}

		//....osDelay(1); //07_03_2022
	}
}
