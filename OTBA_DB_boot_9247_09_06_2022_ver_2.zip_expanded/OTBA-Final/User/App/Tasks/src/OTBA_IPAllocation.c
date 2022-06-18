/**
 ******************************************************************************
 * @file    IPAllocation-C.c
 * @brief   Task responsible for the ip allocarion of the OTBC
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

#include <string.h>
#include "cmsis_os.h"
#include "UserLib.h"
#include "app.h"
#include "lwip.h"
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/sockets.h"
#include "lwip/priv/sockets_priv.h"
#include "lwip/inet.h"


#define SET_TX_ENABLE   	0
#define SET_TX_DISABLE   	1

#define LOS_OPT_COMM_IN_IS   	0
#define LOS_OPT_COMM_IN_NO   	1

#define IP_CFG_IDLE      0xFF
#define IP_CFG_START     0x01
#define IP_CFG_OK        0x06
#define IP_CFG_NOK        0x0A

#define ADR_CONFIG_LEFT    0x01
#define ADR_CONFIG_RIGHT   0x02
#define ADR_DEFAULT        0x00

extern SemaphoreHandle_t xSemaphore_IP_Alloct;
extern SemaphoreHandle_t xSemaphore_IP_Alloct_Accept;
///extern Pntr_ip_alloc_bundle OTS_APP;
 static int32_t CreateSocketBindAndListen(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	int32_t optval;
	int32_t sock_err;
	//int32_t socket_fd;
	struct sockaddr_in servAddr;


	// New socket
	OTS_APP->struct_conn.sock = lwip_socket(AF_INET, SOCK_STREAM, 0);
	if (OTS_APP->struct_conn.sock == -1)
	{
	//	printf("*** IP Allocation: *** socket failed\r\n");
		return OTS_APP->struct_conn.sock;
	}

	// In order to reuse previously badly closed socket
	optval = 1;
	sock_err = setsockopt(OTS_APP->struct_conn.sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(uint32_t));
	if (sock_err < 0)
	{
	//	printf("*** IP Allocation: *** setsockopt SO_REUSEADDR failed\r\n");
		lwip_close(OTS_APP->struct_conn.sock);
		return -1;
	}

	// In order to deliver messages immediately (no buffering)
	optval = 1;
	sock_err = setsockopt(OTS_APP->struct_conn.sock, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(uint32_t));
	if (sock_err < 0)
	{
	//	printf("*** IP Allocation: *** setsockopt TCP_NODELAY failed\r\n");
		lwip_close(OTS_APP->struct_conn.sock);
		return -1;
	}
   //INADDR_ANY
	memset(&servAddr, 0, sizeof(struct sockaddr_in));
	servAddr.sin_len = sizeof(servAddr);
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = PP_HTONS(OTBBCOM_CLIENT_IP_ALLOC_PORT_OTBB); //TCP_OTBA_PORT_FOR_OTB
	servAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS_OTBA);// inet_addr(IP_ADDRESS_OTBA);//INADDR_ANY;// IP_ADDRESS_DEFAULT_OTBB);//IP_ADDRESS_OTBA


	sock_err = bind(OTS_APP->struct_conn.sock, (struct sockaddr*)&servAddr, sizeof(servAddr));
	if (sock_err < 0)
	{
		//printf("*** IP Allocation: *** Error binding socket\r\n");
		lwip_close(OTS_APP->struct_conn.sock);
		return -1;
	}
	sock_err = listen(OTS_APP->struct_conn.sock,  2);
	if (sock_err < 0)
	{
	//	printf("*** IP Allocation: *** Error listening on socket\r\n");
		lwip_close(OTS_APP->struct_conn.sock);
		return -1;
	}

//	printf("*** IP Allocation: *** listening on socket %ld\r\n", OTS_APP->struct_conn.sock);


	return OTS_APP->struct_conn.sock;
}


 static int32_t IP_accept_connect(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	struct sockaddr_in clientAddr;
	socklen_t addr_size;

	memset(&clientAddr, 0, sizeof(struct sockaddr_in));
	addr_size = sizeof(clientAddr);
	// Blocks until a connection is accepted:


	OTS_APP->struct_conn.accept = accept(OTS_APP->struct_conn.sock , (struct sockaddr*)&clientAddr, &addr_size);


	if (OTS_APP->struct_conn.accept < 0)
	{
	//	printf("*** OTBBCOM: *** Error accepting connection, errno: %d\r\n", errno);
		return -1;
	}

	//printf("*** IP Allocation: *** Got connection from %s on socket %ld\r\n", ip4addr_ntoa((ip4_addr_t *)&(clientAddr.sin_addr.s_addr)), accept_fd);
	if ((clientAddr.sin_addr.s_addr == inet_addr(IP_ADDRESS_DEFAULT_OTBB)) )

	{
		OTS_APP->struct_conn.adr_status = ADR_DEFAULT;
	}
	else
	if (clientAddr.sin_addr.s_addr == inet_addr(IP_ADDRESS_LEFT_OTBB))
    {
	   OTS_APP->struct_conn.adr_status = ADR_CONFIG_LEFT;
	}
	else
	if (clientAddr.sin_addr.s_addr == inet_addr(IP_ADDRESS_RIGHT_OTBB))
	 {
		   OTS_APP->struct_conn.adr_status = ADR_CONFIG_RIGHT;
	}

	//lwip_close(OTS_APP->OTBBCOM_IP_ALLOCT.ip_accept_connect);
	return OTS_APP->struct_conn.accept;//IO_ERROR_OTBBCOM__OK;
}


TimerHandle_t A_Delay_IP_ALLOCT;

void pfunA_IP_ALLOCT_Update( TimerHandle_t timer )
 {
	//Pntr_ip_alloc_bundle OTS_APP  =
				//(Pntr_ip_alloc_bundle) pvTimerGetTimerID(xtimer);

	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) pvTimerGetTimerID(timer);
  if(++OTS_APP->Count_IP_ALLOC_Timer >= 10)
   {
	  OTS_APP->Count_IP_ALLOC_Timer = 0;
	  xTaskNotifyGive( OTS_APP->TASKS.IP_Allocation_task_handler);
   }


 }


//void IPAllocation(void const * argument)
int32_t accept_fd= -1;
//void IPAllocation_task (void  * argument)

static char data_buffer[10] ;
static int8_t cnt_dly =0;

static uint8_t status_config_left = IP_CFG_IDLE;

static uint8_t status_config_right = IP_CFG_IDLE;

static uint8_t status_los_left = LOS_OPT_COMM_IN_NO;

static uint8_t status_los_right = LOS_OPT_COMM_IN_NO;


void IPAllocationTask (void  const * argument)
{

	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) argument;
	static int32_t socket_fd = -1;
	int32_t  sent_data;
	//void *pvdata_buffer = (char *)data_buffer[0];

	struct sockaddr_in clientAddr;
	socklen_t addr_size;
	///GPIO_PinState bitstatus;
	TickType_t start_time, current_time;



	////...A_Delay_IP_ALLOCT = xTimerCreate("IP_ALLOCT_timer", pdMS_TO_TICKS(5),pdTRUE, OTS_APP, pfunA_IP_ALLOCT_Update);
	//xTimerStart(A_Delay_IP_ALLOCT, 0);

	OTS_APP->Count_IP_ALLOC_Timer = 0;
	OTS_APP->ST_IP_ALLOCT = Left_Start_IP_Config;//Left_IP;
	OTS_APP->ST_IP_ALLOCT_prv  = Left_Start_IP_Config;//Left_IP;

	OTS_APP->struct_conn.sock = -1;
	OTS_APP->struct_conn.adr_status = ADR_DEFAULT;
	////...xTimerStart(A_Delay_IP_ALLOCT, 0);
	           /** 01.02.2022 **/
	//socket_fd = CreateSocketBindAndListen(OTS_APP);
	current_time = xTaskGetTickCount();
	start_time = current_time;

	OTS_APP->ST_LOS_PINS = OTS_APP->ST_LOS_PINS_NEXT = Los_Los;

	HAL_GPIO_WritePin (P9_1_TXDIS_GPIO_Port, P9_1_TXDIS_Pin,SET_TX_ENABLE );
	HAL_GPIO_WritePin (P9_2_TXDIS_GPIO_Port, P9_2_TXDIS_Pin,SET_TX_ENABLE );


	xSemaphoreTake( xSemaphore_IP_Alloct, ( TickType_t ) pdMS_TO_TICKS (0));
	socket_fd = CreateSocketBindAndListen(OTS_APP);
	xSemaphoreGive( xSemaphore_IP_Alloct);


	for(;;)
	{


		////...ulTaskNotifyTake(pdTRUE, (TickType_t) portMAX_DELAY);
        osDelay(50);

       	if(OTS_APP->struct_conn.sock < 0)
       	{
       		xSemaphoreTake( xSemaphore_IP_Alloct, ( TickType_t ) pdMS_TO_TICKS (0));
       		socket_fd = CreateSocketBindAndListen(OTS_APP);
       		xSemaphoreGive( xSemaphore_IP_Alloct);
       		if(socket_fd < 0)
       		{
       			lwip_close(socket_fd);
       			OTS_APP->struct_conn.sock = -1;
       		}
       	}


           if(OTS_APP->ST_LOS_PINS == OTS_APP->ST_LOS_PINS_NEXT)
            {
       	      status_los_left = HAL_GPIO_ReadPin (P9_1__LOS_GPIO_Port, P9_1__LOS_Pin);

       	      status_los_right = HAL_GPIO_ReadPin (P9_2__LOS_GPIO_Port, P9_2__LOS_Pin);



               if((status_los_left == LOS_OPT_COMM_IN_NO)
            		 && (status_los_right == LOS_OPT_COMM_IN_NO))
               {
       	         OTS_APP->ST_LOS_PINS = Los_Los;
       	         vTaskSuspend( Handle_OTBB_task_RX );
               }
               else
               if((status_los_left == LOS_OPT_COMM_IN_IS)
                      && (status_los_right == LOS_OPT_COMM_IN_NO))
               {
            	   OTS_APP->ST_LOS_PINS = Is_Los;

               }
               else
               if((status_los_left == LOS_OPT_COMM_IN_NO)
                      && (status_los_right == LOS_OPT_COMM_IN_IS))
               {
            	   OTS_APP->ST_LOS_PINS = Los_Is;
            	   //vTaskSuspend( driver_bundle->task__RX );
               }
               else
               if((status_los_left == LOS_OPT_COMM_IN_IS)
                      && (status_los_right == LOS_OPT_COMM_IN_IS))
               {
            	   OTS_APP->ST_LOS_PINS = Is_Is;

               }
                switch(OTS_APP->ST_LOS_PINS)
                {
                  case Los_Los:
                	     status_config_left = IP_CFG_START;
                	     status_config_right = IP_CFG_START;
                	      OTS_APP->ST_IP_ALLOCT = Exam_Left_COMM;
                  break;

                  case Is_Los:
                	  if(status_config_left != IP_CFG_OK)
                	    {
                	      status_config_left = IP_CFG_START;
                	       OTS_APP->ST_IP_ALLOCT = Left_Start_IP_Config;
                	     }
                	  else
                	  {
                		  status_config_right = IP_CFG_START;
                		  OTS_APP->ST_IP_ALLOCT = Exam_Right_COMM;
                	  }
                  break;

                  case Los_Is:
                	  if(status_config_right != IP_CFG_OK)
                	    {
                		  status_config_right = IP_CFG_START;
                	      OTS_APP->ST_IP_ALLOCT = Right_Start_IP_Config;
                	    }
                	   else
                	    {
                		   status_config_left = IP_CFG_START;
                	       OTS_APP->ST_IP_ALLOCT = Exam_Left_COMM;
                	     }
                  break;

                  case Is_Is:
                	  if(status_config_left != IP_CFG_OK)
                	    {
                	       status_config_left = IP_CFG_START;
                	       OTS_APP->ST_IP_ALLOCT = Left_Start_IP_Config;
                	     }

                	   if(status_config_right != IP_CFG_OK)
                	     {
                	       status_config_right = IP_CFG_START;
                	       if(status_config_left != IP_CFG_START)
                	         OTS_APP->ST_IP_ALLOCT = Right_Start_IP_Config;
                	     }
                	   if((status_config_left == IP_CFG_OK)
                			   && (status_config_right == IP_CFG_OK))
                		   OTS_APP->ST_IP_ALLOCT = Any_IP;
                   break;
              }
         }// if (ST_LOS_PINS == ST_LOS_PINS_NEXT)


         //if(socket_fd >= 0) //new
         // {
           switch(OTS_APP->ST_IP_ALLOCT )
       		{

       	       case Exam_Left_COMM:
       	            status_los_left = HAL_GPIO_ReadPin (P9_1__LOS_GPIO_Port, P9_1__LOS_Pin);
       	            if(status_los_left == LOS_OPT_COMM_IN_IS)
       	              OTS_APP->ST_IP_ALLOCT = Left_Start_IP_Config;
       	            else
       	            {
       	              if(current_time - start_time >= 500) //if time out expired
       	              {
       	            	 start_time = current_time;
       	            	 if(OTS_APP->ST_LOS_PINS == Los_Los)
       	            	 {
       	            	   OTS_APP->ST_IP_ALLOCT = Exam_Right_COMM;
       	            	 }
       	            	 else
       	            		if(OTS_APP->ST_LOS_PINS == Los_Is)
       	            		  OTS_APP->ST_IP_ALLOCT = Any_IP;

       	            	  status_config_left = IP_CFG_NOK; //without success
       	               }
       	             }

                  /**  if(socket_fd < 0)
                    {
       	              lwip_close(socket_fd);
       	              OTS_APP->struct_conn.sock = -1;
                    } **/
       	        break;
       	        case Exam_Right_COMM:

       	            status_los_right = HAL_GPIO_ReadPin (P9_2__LOS_GPIO_Port, P9_2__LOS_Pin);
       	            if(status_los_right == LOS_OPT_COMM_IN_IS)
       	              OTS_APP->ST_IP_ALLOCT = Right_Start_IP_Config;
       	            else
       	             {
       	              if(current_time - start_time >= 500) //if time out expired
       	               {
       	                  start_time = current_time;
       	                  OTS_APP->ST_IP_ALLOCT = Any_IP;
        	              status_config_right = IP_CFG_NOK; //without success
       	                 }
       	              }

       	       /**  if(socket_fd < 0)
       	          {
       	            lwip_close(socket_fd);
       	             OTS_APP->struct_conn.sock = -1;
       	          } **/
       	        break;
       	     }

	    /// }//new

        if(socket_fd >= 0)
         {
            switch(OTS_APP->ST_IP_ALLOCT )
		    {

		    case Left_Start_IP_Config:

		    	if(((status_config_left == IP_CFG_START)
		    				 && (status_config_right == IP_CFG_START))	)
		    	{
		    	  HAL_GPIO_WritePin (P9_2_TXDIS_GPIO_Port, P9_2_TXDIS_Pin,SET_TX_DISABLE );

		    	}
		    	//HAL_GPIO_WritePin (P9_2_TXDIS_GPIO_Port, P9_2_TXDIS_Pin,SET_TX_DISABLE );
			    HAL_GPIO_WritePin (P9_1_TXDIS_GPIO_Port, P9_1_TXDIS_Pin,SET_TX_ENABLE );
			    strcpy(data_buffer,"left");
                for(cnt_dly =0; cnt_dly < 25; ++cnt_dly)
			      sent_data = -1;

                status_los_left = HAL_GPIO_ReadPin (P9_1__LOS_GPIO_Port, P9_1__LOS_Pin);
			    if(status_los_left == LOS_OPT_COMM_IN_IS) // get again the los input
			    {

			    	    ///...xSemaphoreTake( xSemaphore_IP_Alloct_Accept, ( TickType_t ) pdMS_TO_TICKS (0));
			    	    addr_size = sizeof(clientAddr);
			    		if ((accept_fd = IP_accept_connect(OTS_APP)) >= 0)
			    		 {
			    			//if((OTS_APP->struct_conn.adr_status == ADR_DEFAULT)
			    				//|| (OTS_APP->struct_conn.adr_status == ADR_CONFIG_LEFT))
			    			sent_data = lwip_send(accept_fd, data_buffer, strlen(data_buffer) + 1, 0);
			    		 }
			    		lwip_close(accept_fd);
			    		///...xSemaphoreGive( xSemaphore_IP_Alloct_Accept);
			    		if (accept_fd < 0 )
			    		{
			    			sent_data = -1;
			    		}
			      }


			    if((sent_data < 0))
			    {
			       if(current_time - start_time >= 500) //if time out expired
			       {
			    	  start_time = current_time;
			    	  if(status_config_right == IP_CFG_START)
			    		  OTS_APP->ST_IP_ALLOCT = Right_Start_IP_Config;
			    	   else
			    			OTS_APP->ST_IP_ALLOCT = Any_IP;

			    	   status_config_left = IP_CFG_NOK;
			       }

			    	/**lwip_close(socket_fd);
			    	OTS_APP->struct_conn.sock = -1; **/
			    }
			    else
			    {

			    	if(eTaskGetState(Handle_OTBB_task_RX) == eSuspended)
			    	  vTaskResume(Handle_OTBB_task_RX);
			    	if(status_config_right == IP_CFG_START)
			    	  OTS_APP->ST_IP_ALLOCT = Right_Start_IP_Config;
			    	else
			    	  OTS_APP->ST_IP_ALLOCT = Any_IP;

			        //lwip_close(socket_fd);
			    	//OTS_APP->struct_conn.sock = -1;
			    	start_time = current_time;
			    	status_config_left = IP_CFG_OK;

			    }

		  break;
		  case Right_Start_IP_Config:
			  if(((status_config_left == IP_CFG_START)
			  		    				 && (status_config_right == IP_CFG_START))	)
			  		{
				  HAL_GPIO_WritePin (P9_1_TXDIS_GPIO_Port, P9_1_TXDIS_Pin,SET_TX_DISABLE );

			  		}
			    HAL_GPIO_WritePin (P9_2_TXDIS_GPIO_Port, P9_2_TXDIS_Pin,SET_TX_ENABLE );
			    strcpy(data_buffer,"right");
			    for(cnt_dly =0; cnt_dly < 25; ++cnt_dly)
			      sent_data = -1;
			    status_los_right = HAL_GPIO_ReadPin (P9_2__LOS_GPIO_Port, P9_2__LOS_Pin);
			    if(status_los_right == LOS_OPT_COMM_IN_IS)
			     {

			    	///...xSemaphoreTake( xSemaphore_IP_Alloct_Accept, ( TickType_t ) pdMS_TO_TICKS (0));
			    	addr_size = sizeof(clientAddr);
			    	if ((accept_fd = IP_accept_connect(OTS_APP)) >= 0)
			    	{
			    		//if((OTS_APP->struct_conn.adr_status == ADR_DEFAULT)
			    		///|| (OTS_APP->struct_conn.adr_status == ADR_CONFIG_RIGHT))
			    		  sent_data = lwip_send(accept_fd, data_buffer, strlen(data_buffer) + 1, 0);
			    	}

			    	lwip_close(accept_fd);
			    	///...xSemaphoreGive( xSemaphore_IP_Alloct_Accept);
			    	if (accept_fd < 0 )
			    	{
			    		sent_data = -1;
			    	}
			     }


			    if(sent_data < 0)
			    {
			      if(current_time - start_time >= 500)
			       {
			    	  start_time = current_time;
			    	  OTS_APP->ST_IP_ALLOCT = Any_IP;//Any_IP;
			    	  status_config_right = IP_CFG_NOK;
			       }


			    	/**lwip_close(socket_fd);
			    	OTS_APP->struct_conn.sock = -1; **/
			    }
			    else
			    {
			    	if(eTaskGetState(Handle_OTBB_task_RX) == eSuspended)
			    	  vTaskResume(Handle_OTBB_task_RX);
			    	OTS_APP->ST_IP_ALLOCT = Any_IP;
			    	//lwip_close(socket_fd);
			    	//OTS_APP->struct_conn.sock = -1;
			    	start_time = current_time;
			    	status_config_right = IP_CFG_OK;

			    }
			break;
			case Any_IP:
				//status_config_left = IP_CFG_IDLE;
				//status_config_right = IP_CFG_IDLE;
				OTS_APP->ST_LOS_PINS_NEXT = OTS_APP->ST_LOS_PINS;

			break;

			case  Center_IP:

			break;
		 }
       }//end of if sock >= 0


       current_time = xTaskGetTickCount();


	}//end of for(;;)
}



