/**
 ******************************************************************************
 * @file    otbbcom_icd.c
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

#include "../inc/otbbcom_private.h"

int32_t _otbbcom_create_configure_socket_server(OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle)
{
	int32_t optval;
	int32_t sock_err;
	struct sockaddr_in servAddr;

	// New socket
	driver_bundle->server_conn_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (driver_bundle->server_conn_socket < 0)
	{
	   return (driver_bundle->server_conn_socket );
	}

	// In order to reuse previously badly closed socket
	optval = 1;
	sock_err = setsockopt(driver_bundle->server_conn_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(uint32_t));
	if (sock_err < 0)
	{
		lwip_close(driver_bundle->server_conn_socket);
	    driver_bundle->server_conn_socket = -1;
	    return driver_bundle->server_conn_socket;
	}

	// In order to deliver messages immediately (no buffering)
	optval = 1;
	sock_err = setsockopt(driver_bundle->server_conn_socket, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(uint32_t));
	if (sock_err < 0)
	{
		lwip_close(driver_bundle->server_conn_socket);
		driver_bundle->server_conn_socket = -1;
		return driver_bundle->server_conn_socket;
	}

	memset(&servAddr, 0, sizeof(struct sockaddr_in));
	servAddr.sin_len = sizeof(servAddr);
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = PP_HTONS(OTBBCOM_SERVER_PORT_OTBA_n);
	servAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS_OTBA);

	sock_err = bind(driver_bundle->server_conn_socket, (struct sockaddr*)&servAddr, sizeof(servAddr));
	if (sock_err < 0)
	{
		lwip_close(driver_bundle->server_conn_socket);
		driver_bundle->server_conn_socket = -1;
		return driver_bundle->server_conn_socket;
	}

	sock_err = listen(driver_bundle->server_conn_socket, 2);//1

	if (sock_err < 0)
	{
		lwip_close(driver_bundle->server_conn_socket);
		driver_bundle->server_conn_socket = -1;
		return driver_bundle->server_conn_socket;
	}

	//printf("*** OTBBCOM: *** listening on socket %ld\r\n", driver_bundle->server_conn_socket);


	return driver_bundle->server_conn_socket ;//IO_ERROR_OTBBCOM__OK;

}
extern SemaphoreHandle_t xSemaphore_IP_Alloct;
int32_t _otbbcom_accept_connection(OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle)
{
	struct sockaddr_in clientAddr;
	socklen_t addr_size;

	memset(&clientAddr, 0, sizeof(struct sockaddr_in));
	addr_size = sizeof(clientAddr);
	// Blocks until a connection is accepted:


	driver_bundle->server_accept = accept(driver_bundle->server_conn_socket, (struct sockaddr*)&clientAddr, &addr_size);

	if ((clientAddr.sin_addr.s_addr == inet_addr(IP_ADDRESS_LEFT_OTBB))
			|| (clientAddr.sin_addr.s_addr == inet_addr(IP_ADDRESS_RIGHT_OTBB)))
		return driver_bundle->server_accept;

	if (driver_bundle->server_accept < 0)
	{
		//printf("*** OTBBCOM: *** Error accepting connection, errno: %d\r\n", errno);
		return -1;
	}
	//printf("*** OTBBCOM: *** Got connection from %s on socket %ld\r\n", ip4addr_ntoa((ip4_addr_t *)&(clientAddr.sin_addr.s_addr)), driver_bundle->socket);



	//lwip_close(driver_bundle->server_accept);
	return IO_ERROR_OTBBCOM__OK;
}




int32_t _otbbcom_connect(OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle)
{
	struct sockaddr_in addr;
	int32_t lwip_err;
	int32_t err;

	err = _otbbcom_create_configure_socket_client(driver_bundle);
	if (err != IO_ERROR_OTBBCOM__OK)
	{
		return err;
	}
	printf(":::OTBBCOM::: Created socket %ld\r\n", driver_bundle->socket);
	addr.sin_len = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = PP_HTONS(OTBBCOM_SERVER_PORT_OTBA);
	addr.sin_addr.s_addr = inet_addr(driver_bundle->tcp_config.remote_ip);

	lwip_err = connect(driver_bundle->socket, (struct sockaddr*)&addr, sizeof(addr));
	if (lwip_err != 0)
	{
		close(driver_bundle->socket);
		driver_bundle->socket = -1;
		printf(":::OTBBCOM::: Connect failed\r\n");
		return IO_ERROR_OTBBCOM__FAILED_CONNECTING_SOCKET;
	}

	printf(":::OTBBCOM::: Connected to TCP server\r\n");

//	driver_bundle->is_connected = 1u;

	return IO_ERROR_OTBBCOM__OK;
}

/**  REOPEN and DELETE	**/

#define OTBB_COMM_OK		0
#define test_telnt   1

#define TASK_ENBL  	1
#define TASK_NOT_ENBL  	0

#define BUFF_CAP 	43

/** re create task**/
#define OTBBCOM_RX_TASK_STACK_SIZE		512
StaticTask_t OTBBCOM_RX_TASK_STACK_BUFFER;
StackType_t OTBBCOM_RX_TASK_STACK[OTBBCOM_RX_TASK_STACK_SIZE];

/**
SemaphoreHandle_t xSemaphore_OTBB_Accept_RX  = NULL;
StaticSemaphore_t xSemaphore_OTBB_Accept_RX_Buffer;

SemaphoreHandle_t xSemaphore_OTBB_Receive_RX  = NULL;
StaticSemaphore_t xSemaphore_OTBB_Receive_RX_Buffer;
**/

TimerHandle_t OTBB_COMM_exam_task_timer;

static uint32_t OTBBcomExam_task_timer =0;

extern TaskHandle_t Gl_handle_rx ;
char buf[BUFF_CAP];

enum{ST_CREATE,ST_DELETE,ST_DELAY} ST_EXM_TEST,PREV_ST_EXM_TEST;
static int16_t cnt_test_delay = 0;

static int8_t start_entry_redelete = 1;
static int8_t start_entry_recreate = 0;


int32_t _otbbcom_redelete(OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle )
{

   if(1)
   {
	   start_entry_redelete = 0;
	   start_entry_recreate = 1;

	   if(driver_bundle->socket_accept >= 0)
		  lwip_close(driver_bundle->socket_accept);
	   if(driver_bundle->server_conn_socket >= 0)
		  lwip_close(driver_bundle->server_conn_socket);


	  	   //taskYIELD();
	  	 vTaskDelete(driver_bundle->task__RX);
	  	 //driver_bundle->task__RX = NULL;


	  	 //vQueueDelete( driver_bundle->mailbox__RX );
	  	 //vQueueDelete( driver_bundle->queue__TX );

	  	  //vSemaphoreDelete( xSemaphore_OTBB_Receive_RX );
	  	  //vSemaphoreDelete( xSemaphore_OTBB_Accept_RX );
	  	  xTimerDelete(driver_bundle->OTBBcom_task_rx_timer, ( TickType_t ) pdMS_TO_TICKS (0));

	  	   //vPortFree(driver_bundle);
   }

	  return IO_ERROR_OTBBCOM__OK;

}



int32_t  _otbbcom_reopen(void )
{
	OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle = gl_otbb_bundle;
	BaseType_t err_os;

		 if(start_entry_recreate == 1)
		 {

			 vPortFree(driver_bundle);


			 //xSemaphore_OTBB_Accept_RX = NULL;
			 //xSemaphore_OTBB_Receive_RX = NULL;

			 driver_bundle = (OTBBCOM_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(OTBBCOM_DRIVER_BUNDLE));
			 memset(driver_bundle, 0, sizeof(OTBBCOM_DRIVER_BUNDLE));

			 driver_bundle->queue__TX = queue_TX_GLB;
			 driver_bundle->mailbox__RX = queue_mailbox_RX;

			 driver_bundle->socket_accept = -1;

			 driver_bundle->OTBB_is_connected = 0;

			 driver_bundle->server_conn_socket = -1;


			 driver_bundle->release_sem_receive = 1;
			 driver_bundle->task_give_notify = 0;


			 driver_bundle->tcp_config.remote_ip = gl_tcp_remote_ip;
			 driver_bundle->tcp_config.tcp_connect_attempts = gl_tcp_connect_attempts;
			 driver_bundle->tcp_config.tcp_rx_timeout_ms = gl_tcp_rx_timeout_ms;
			 driver_bundle->tcp_config.tcp_tx_timeout_ms = gl_tcp_tx_timeout_ms;


			 //driver_bundle->queue__TX = xQueueCreate(10, sizeof(OTBBCOM_PACKET));
			 //driver_bundle->mailbox__RX = xQueueCreate(1, sizeof(OTBBCOM_PACKET));

			 //xSemaphore_OTBB_Accept_RX = xSemaphoreCreateBinaryStatic(&xSemaphore_OTBB_Accept_RX_Buffer);
			// xSemaphore_OTBB_Receive_RX = xSemaphoreCreateBinaryStatic(&xSemaphore_OTBB_Receive_RX_Buffer);

			 if(driver_bundle->server_conn_socket < 0)
			 {

				// osDelay(2); /** do delay before re-supply OTBB board**/

				xSemaphoreTake( xSemaphore_OTBB_CONNECT_1, ( TickType_t ) pdMS_TO_TICKS (0));
			 	{
			 	   _otbbcom_create_configure_socket_server(driver_bundle);
			 	}
			 	xSemaphoreGive( xSemaphore_OTBB_CONNECT_1);
			  }

			 //driver_bundle->task__RX = NULL;

			 driver_bundle->task__RX  = (TaskHandle_t) xTaskCreateStatic( _otbbcom_task_rx,
			 									"OTBBCOM_RX",
			 									OTBBCOM_RX_TASK_STACK_SIZE,
			 									(void *) (driver_bundle),
			 									osPriorityNormal + 1,
			 									OTBBCOM_RX_TASK_STACK,
			 									&OTBBCOM_RX_TASK_STACK_BUFFER );

         	 //err_os = xTaskCreate(_otbbcom_task_rx, "OTBBCOM_RX", 512, driver_bundle, osPriorityNormal + 1, &driver_bundle->task__RX );
			 	//if (err_os != pdPASS)
			 	//{
			 		//return IO_ERROR_OTBBCOM__OUT_OF_HEAP;
			 	//}

			 //xTaskNotifyGive((TaskHandle_t)driver_bundle->task__RX);
		    //osDelay(2);
		 }


		 gl_otbb_task_rx = (TaskHandle_t)driver_bundle->task__RX ;


	    return err_os;
}







void _otbbcom_task_conn(void * argument)
{
	OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle = (OTBBCOM_DRIVER_BUNDLE_PTR) argument;
	int32_t err;

	if (driver_bundle->tcp_config.type == OTBBCOM_EDGE_TYPE__OTBA)
	{
		_otbbcom_create_configure_socket_server(driver_bundle);
	}

	for (;;)
	{
		if (driver_bundle->tcp_config.type == OTBBCOM_EDGE_TYPE__OTBA)
		{
			if (driver_bundle->socket < 0)
			{
				err = _otbbcom_accept_connection(driver_bundle);
				if (err == IO_ERROR_OTBBCOM__FAILED_CREATING_SOCKET)
				{
//					printf(":::OTBBCOM::: Failed creating socket, retrying...\n");
				}
				else if (err != IO_ERROR_OTBBCOM__OK)
				{
//					printf(":::OTBBCOM::: Failed connecting to socket, unexpected error 0x%08lX\n", err);
				}
			}
		}
		else if (driver_bundle->tcp_config.type == OTBBCOM_EDGE_TYPE__OTBB)
		{
//			if (!driver_bundle->is_connected)
			if (driver_bundle->socket < 0)
			{
				// Not connected, connect now
				err = _otbbcom_connect(driver_bundle);
				if (err == IO_ERROR_OTBBCOM__FAILED_CREATING_SOCKET)
				{
//					printf(":::OTBBCOM::: Failed creating socket, retrying...\n");
				}
				else if (err == IO_ERROR_OTBBCOM__FAILED_CONNECTING_SOCKET)
				{
//					printf(":::OTBBCOM::: Failed connecting to socket, retrying...\n");
				}
				else if (err != IO_ERROR_OTBBCOM__OK)
				{
//					printf(":::OTBBCOM::: Failed connecting to socket, unexpected error 0x%08lX\n", err);
				}
			}
		}

		osDelay(10);
	}
}


/**** =========================================CLIENT =======================*********/
int32_t _otbbcom_create_configure_socket_client(OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle)
{
	struct sockaddr_in clientAddr;
	int32_t optval;
	int32_t sock_err;

	// New socket
	driver_bundle->socket = socket(AF_INET, SOCK_STREAM, 0);
	if (driver_bundle->socket == -1)
	{
		printf(":::OTBBCOM::: socket failed\r\n");
		return IO_ERROR_OTBBCOM__FAILED_CREATING_SOCKET;
	}

	memset(&clientAddr, 0, sizeof(struct sockaddr_in));
	clientAddr.sin_len = sizeof(clientAddr);
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = PP_HTONS(OTBBCOM_CLIENT_PORT_OTBB);
	clientAddr.sin_addr.s_addr = INADDR_ANY;

	sock_err = bind(driver_bundle->socket, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
	if (-1 == sock_err)
	{
		printf(":::OTBBCOM::: bind socket to local port failed\r\n");
		goto complete_otbbcom_create_configure_socket_client;
	}

	// In order to reuse previously badly closed socket
	optval = 1;
	sock_err = setsockopt(driver_bundle->socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(uint32_t));
	if (sock_err != 0)
	{
		printf(":::OTBBCOM::: setsockopt SO_REUSEADDR failed\r\n");
		goto complete_otbbcom_create_configure_socket_client;
	}

	// In order to detect a dead connection
	optval = 1;
	sock_err = setsockopt(driver_bundle->socket, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(uint32_t));
	if (sock_err != 0)
	{
		printf(":::OTBBCOM::: setsockopt SO_KEEPALIVE failed\r\n");
		goto complete_otbbcom_create_configure_socket_client;
	}

	// Time before the first keep alive probe is sent
	optval = 2;
	sock_err = setsockopt(driver_bundle->socket, IPPROTO_TCP, TCP_KEEPIDLE, &optval, sizeof(uint32_t));
	if (sock_err != 0)
	{
		printf(":::OTBBCOM::: setsockopt TCP_KEEPIDLE failed\r\n");
		goto complete_otbbcom_create_configure_socket_client;
	}

	// Time between keep alive probes
	optval = 2;
	sock_err = setsockopt(driver_bundle->socket, IPPROTO_TCP, TCP_KEEPINTVL, &optval, sizeof(uint32_t));
	if (sock_err != 0)
	{
		printf(":::OTBBCOM::: setsockopt TCP_KEEPINTVL failed\r\n");
		goto complete_otbbcom_create_configure_socket_client;
	}

	// The number of unacknowledged probes to send before considering the connection dead
	// and notifying the application layer
	optval = 2;
	sock_err = setsockopt(driver_bundle->socket, IPPROTO_TCP, TCP_KEEPCNT, &optval, sizeof(uint32_t));
	if (sock_err != 0)
	{
		printf(":::OTBBCOM::: setsockopt TCP_KEEPCNT failed\r\n");
		goto complete_otbbcom_create_configure_socket_client;
	}

	// In order to deliver messages immediately (no buffering)
	optval = 1;
	sock_err = setsockopt(driver_bundle->socket, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(uint32_t));
	if (sock_err != 0)
	{
		printf(":::OTBBCOM::: setsockopt TCP_NODELAY failed\r\n");
		goto complete_otbbcom_create_configure_socket_client;
	}

	// Send buffer size. Should be configurable
	// Warning - unimplemented by LwIP
//	optval = 1;
//	sock_err = setsockopt(driver_bundle->socket__AC, SOL_SOCKET, SO_SNDBUF, &optval, sizeof(uint32_t));
//	if (sock_err != 0)
//	{
//		return IO_ERROR_OTBBCOM__FAILED_CREATING_SOCKET;
//	}

	// Receive buffer size. Should be cofigurable
	optval = 65536;
	sock_err = setsockopt(driver_bundle->socket, SOL_SOCKET, SO_RCVBUF, &optval, sizeof(uint32_t));
	if (sock_err != 0)
	{
		printf(":::OTBBCOM::: setsockopt SO_RCVBUF failed\r\n");
		goto complete_otbbcom_create_configure_socket_client;
	}




	complete_otbbcom_create_configure_socket_client:
		if (sock_err)
		{
			close(driver_bundle->socket);
			driver_bundle->socket = -1;
			return IO_ERROR_OTBBCOM__FAILED_CREATING_SOCKET;
		}
		else
		{
			return IO_ERROR_OTBBCOM__OK;
		}
	}
