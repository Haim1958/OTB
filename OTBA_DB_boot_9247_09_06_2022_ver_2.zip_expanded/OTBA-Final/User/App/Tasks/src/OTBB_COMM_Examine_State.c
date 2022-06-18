/*
 * OTBB_COMM_Examine_State.c
 *
 *  Created on: 28 Mar 2022
 *      Author: skh
 */


#include <string.h>
#include "cmsis_os.h"
#include "UserLib.h"
#include "app.h"
#include "FreeRTOS.h"
#include "task.h"



#define OTBB_COMM_OK		0
#define test_telnt   		0

#define TASK_ENBL  			1
#define TASK_NOT_ENBL  		0

#define ENBL_UPDATE_TASK	1
#define DISBL_UPDATE_TASK	0

#define IN_WAIT				0
#define IN_PROCESS			1

#define OTBB_SWITCH_IDLE	1
#define OTBB_SWITCH_BUSY	0

#define BUFF_CAP 	43

#define LASER_OFF	(uint8_t)0x04
#define ANY_STATE	(uint8_t)0x00

TimerHandle_t temp_handler_timer;

static uint32_t OTBBcomExam_task_timer =0;

extern TaskHandle_t Gl_handle_rx ;
char buf[BUFF_CAP];
uint32_t cnt_get_reg_2 = 0;

enum{ST_CREATE,ST_DELETE,ST_DELAY} ST_EXM_TEST,PREV_ST_EXM_TEST;
static int16_t cnt_test_delay = 0;

static int8_t start_entry = 1;



/*** TIMER TASK  ***/

static void pfunB_B_Delay_State_task_timer(TimerHandle_t timer)
{

	OTS_APPLICATION_BUNDLE_PTR driver_bundle = (OTS_APPLICATION_BUNDLE_PTR) pvTimerGetTimerID(timer);

	if(++OTBBcomExam_task_timer >= 5)
	{
		OTBBcomExam_task_timer = 0;
		xTaskNotifyGive(driver_bundle->B_ETHR.examine_task_rx);
	}

}


/** TASK **/

static uint8_t enable_update_task = DISBL_UPDATE_TASK;
void OTBB_COM_examine_task(void  * argument)

{

	OTS_APPLICATION_BUNDLE_PTR driver_bundle = (OTS_APPLICATION_BUNDLE_PTR) argument;

	//int32_t read_len;
	int32_t err;
	BaseType_t err_os;
	void *current_state;
	uint8_t  logic_state;
	uint8_t telnet_connected;

	int32_t cnt_dly;

	ST_EXM_TEST = PREV_ST_EXM_TEST = ST_DELAY;

	driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT = ST_OTBB_rx_task_START;
	driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev = ST_OTBB_rx_task_START;

	driver_bundle->B_ETHR.OTBB_COMM_task_enable = TASK_NOT_ENBL;

	driver_bundle->Handle_Laser_OFF = IN_WAIT;
	driver_bundle->Handle_OTBB_SWITCH = OTBB_SWITCH_IDLE;

	temp_handler_timer = xTimerCreate("Delay_State_task_timer",
							pdMS_TO_TICKS(5),
							pdTRUE,
							driver_bundle,
							pfunB_B_Delay_State_task_timer);

	 driver_bundle->B_ETHR.OTBB_eaxm_task_rx_timer = temp_handler_timer;

	xTimerStart(temp_handler_timer, 1);


	for(;;)
	{


			ulTaskNotifyTake(pdTRUE, ( TickType_t )/** pdMS_TO_TICKS**/ (portMAX_DELAY));




			 /***************************Handle Laser_OFF State**********************/
			/**
			if(driver_bundle->Handle_Laser_OFF == IN_WAIT)
			  {


				  xSemaphoreTake( xSemaphore_OTBB_Queue_Laser_Off, ( TickType_t ) pdMS_TO_TICKS (0));
			      err_os = xQueuePeek(driver_bundle->mailbox_laser_state,  &driver_bundle->B_laser_state, 0);
			      xSemaphoreGive( xSemaphore_OTBB_Queue_Laser_Off);
			      if (err_os == pdTRUE)
			      {
			    	  current_state  =  driver_bundle->B_laser_state.pstate ;

			  	  	  if((current_state == driver_bundle->FSM.state__LASEROFF->obj)
			  	  		  && (driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT != ST_OTBB_rx_task_wait_LASERON))
			  	  	      {
			  	  		    logic_state  =  driver_bundle->B_laser_state.uint_state = LASER_OFF;
			  	  		    driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT = ST_OTBB_rx_task_wait_LASEROFF;
			  	  			driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev = ST_OTBB_rx_task_wait_LASEROFF;
			  	  		    driver_bundle->Handle_OTBB_SWITCH = OTBB_SWITCH_BUSY;
			  	  	      }
			  	  	  else
			  	  		if((current_state != driver_bundle->FSM.state__LASEROFF->obj)
			  	  		  && (driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT == ST_OTBB_rx_task_wait_LASERON))
			  	  		  logic_state  =  driver_bundle->B_laser_state.uint_state = ANY_STATE;
	              }
              **/
			  /************************************************************************/


			   if(driver_bundle->Handle_OTBB_SWITCH == OTBB_SWITCH_IDLE)
			   {

				   /********OTBB1_Ethernet_glb*********OTBB2_Ethernet_glb*****************/
				   	OTBB1_Ethernet_glb  = HAL_GPIO_ReadPin(P9_1__LOS_GPIO_Port, P9_1__LOS_Pin);
				   	OTBB2_Ethernet_glb  = HAL_GPIO_ReadPin(P9_2__LOS_GPIO_Port, P9_2__LOS_Pin);
				   	OTBC_Ethernet_glb  =  HAL_GPIO_ReadPin(P9_0__LOS_GPIO_Port, P9_0__LOS_Pin);

				   	/**********************************************************************/

			   }


				switch(driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT)

					{
				       /**
				        //...............   LASER_OFF LASER_ON
				        case ST_OTBB_rx_task_wait_LASEROFF:
				        	driver_bundle->Handle_Laser_OFF = IN_PROCESS;
				        	//driver_bundle->Handle_OTBB_SWITCH = OTBB_SWITCH_BUSY;
				        	driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT = ST_OTBB_rx_task_LASERSTATE_OFF;//go to switch OFF

				         break;

				        case ST_OTBB_rx_task_LASERSTATE_OFF:

				        	if(driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev == ST_OTBB_rx_task_wait_LASEROFF)
				        	  enable_update_task  = ENBL_UPDATE_TASK; //go to delete
				        	else
                             if(driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev == ST_OTBB_rx_task_LASERSTATE_OFF)
                             {
				       		    driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT = ST_OTBB_rx_task_wait_LASERON;
				       		    driver_bundle->Handle_Laser_OFF = IN_WAIT;
                             }
				       		 break;

				        case ST_OTBB_rx_task_wait_LASERON:
				        	driver_bundle->Handle_Laser_OFF = IN_WAIT;
				        	if(logic_state == ANY_STATE) //wait to switch ON
				        	{
				        		driver_bundle->Handle_Laser_OFF = IN_PROCESS;
				        		driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT = ST_OTBB_rx_task_LASERSTATE_ON;//go to switch ON
				        	  driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev = ST_OTBB_rx_task_wait_LASERON;
				        	}
				        break;

				        case ST_OTBB_rx_task_LASERSTATE_ON:
                             if(driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev == ST_OTBB_rx_task_wait_LASERON)
                            	 enable_update_task  = ENBL_UPDATE_TASK; //go to create
                             else
                               if(driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev == ST_OTBB_rx_task_LASERSTATE_ON)
                               	 driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT = ST_OTBB_rx_task_LASERSTATE_IDLE;
                        break;

				        case ST_OTBB_rx_task_LASERSTATE_IDLE:
				        	driver_bundle->Handle_Laser_OFF = IN_WAIT;
				        	driver_bundle->Handle_OTBB_SWITCH = OTBB_SWITCH_IDLE;
				        break;
				        //...............  END OF SWITCH LASER_OFF LASER_ON
                       **/
				        //...............  START OF SWITCH  OTBB_OFF OTBB_ON
				         case ST_OTBB_rx_task_WAIT_ON:

				        	 if((OTBB1_Ethernet_glb == OTBB_COMM_OK) || (OTBB2_Ethernet_glb == OTBB_COMM_OK))
				        	 {
				        	    driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT = ST_OTBB_rx_task_ON;
				        	    driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev = ST_OTBB_rx_task_WAIT_ON;
				        	 }
				         break;
				         case ST_OTBB_rx_task_ON:
				        	 if(driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev == ST_OTBB_rx_task_WAIT_ON)
				        	 	enable_update_task  = ENBL_UPDATE_TASK; //go to create
				        	 else
				        	   if(driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev == ST_OTBB_rx_task_ON)
				        	     driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT = ST_OTBB_rx_task_WAIT_OFF;
						break;

				         case ST_OTBB_rx_task_WAIT_OFF:
				          if((OTBB1_Ethernet_glb != OTBB_COMM_OK) && (OTBB2_Ethernet_glb != OTBB_COMM_OK))
				        	{
				        		driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT = ST_OTBB_rx_task_OFF;
				        		driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev = ST_OTBB_rx_task_WAIT_OFF;
				        	}
				        break;

						case ST_OTBB_rx_task_OFF:

                             if(driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev == ST_OTBB_rx_task_WAIT_OFF)
							   enable_update_task  = ENBL_UPDATE_TASK; //go to delete
                             else
                              if(driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev == ST_OTBB_rx_task_OFF)
                            	  driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT = ST_OTBB_rx_task_WAIT_ON;
					  break;


						case ST_OTBB_rx_task_START:

                           	if((OTBB1_Ethernet_glb == OTBB_COMM_OK) || (OTBB2_Ethernet_glb == OTBB_COMM_OK))
						        {
                           		  driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT = ST_OTBB_rx_task_WAIT_ON;
                           		  driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev = ST_OTBB_rx_task_WAIT_OFF;
						        }

						break;
						//...............  END OF SWITCH  OTBB_OFF OTBB_ON
				  }				//...............  END OF SWITCH


				      if((enable_update_task  == ENBL_UPDATE_TASK)
				    	  && (driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT != driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev))
					   {



				    	  if((driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT == ST_OTBB_rx_task_ON)
				    		  /**|| (driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT == ST_OTBB_rx_task_LASERSTATE_ON)**/)
						   {

		                        driver_bundle->B_ETHR.OTBB_COMM_task_enable = TASK_ENBL;


		                          err = io_ioctl(driver_bundle->DRIVERS.OTBBCOM, IO_IOCTL_OTBBCOM_IS_RE_CONNECTED,
					                       &driver_bundle->OTBBCOM.is_connected);


						  }
						  else
							if((driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT == ST_OTBB_rx_task_OFF)
								   /**|| (driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT == ST_OTBB_rx_task_LASERSTATE_OFF)**/)
							{
								  driver_bundle->B_ETHR.OTBB_COMM_task_enable = TASK_NOT_ENBL;
								  err = io_ioctl(driver_bundle->DRIVERS.OTBBCOM, IO_IOCTL_OTBBCOM_IS_RE_DELETED,
								  			 		&driver_bundle->OTBBCOM.is_connected);
							}



				    	  /**********TEL_NET   *****************************/

				    	  #if test_telnt

				    	  if((cnt_get_reg_2++% 250 == 0) && (cnt_get_reg_2 >= 250))
				    	  {
				    		  cnt_get_reg_2 = 0;
				    		  io_ioctl(driver_bundle->DRIVERS.TELNET, IO_IOCTL_TELNET__IS_CONNECTED, &telnet_connected);
				    		  if (telnet_connected)
				    		  {
				    			  if(driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT == ST_OTBB_rx_task_ON)
				    				  snprintf(buf, BUFF_CAP-1, "OTBB_ETHR_STATE= %s \r\n","NOW_IS_ON");
				    			  else
				    				  if(driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT == ST_OTBB_rx_task_OFF)
				    					  snprintf(buf, BUFF_CAP-1, "OTBB_ETHR_STATE= %s \n","NOW_IS_OFF");
				    			  io_write(driver_bundle->DRIVERS.TELNET, (uint8_t*)buf, strlen(buf));


				    			  if(driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev == ST_OTBB_rx_task_ON)
				    				  snprintf(buf, BUFF_CAP-1, "PREV_OTBB_ETHR_STATE= %s \n","NOW_IS_ON");
				    			  else
				    				  if(driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev == ST_OTBB_rx_task_OFF)
				    					  snprintf(buf, BUFF_CAP-1, "PREV_OTBB_ETHR_STATE= %s \n\r","NOW_IS_OFF");
				    			  io_write(driver_bundle->DRIVERS.TELNET, (uint8_t*)buf, strlen(buf));

				    		  }
				    	  }
							#endif
				    	  enable_update_task  = DISBL_UPDATE_TASK;
				    	  driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT_prev = driver_bundle->B_ETHR.ST_OTBB_COMM_PRESENT;

				}//..END OF if((enable_update_task  == ENBL_UPDATE_TASK)





	    } /** end of for(;;) **/
}
