/*
 * B_ExamRegTask.c
 *
 *  Created on: 06_Octb 2021
 *      Author: SKH
 */

#include <string.h>
#include "cmsis_os.h"
#include "UserLib.h"
#include "app.h"
//#include "../inc/app_sm.h"


#define test_telnt   0

static TimerHandle_t A_Delay_Get_Answ_timer;

static TimerHandle_t A_Send_Req_Update_timer;


static int32_t fsm_exam_test(OTS_APPLICATION_BUNDLE_PTR OTS_SM_TEST,OTB_STATE current_state,OTB_STATE previous_state);
static int32_t send_OTBA_request_to_OTBB(OTS_APPLICATION_BUNDLE_PTR OTS);
static int32_t Proccessing_Answ_OTBB(OTS_APPLICATION_BUNDLE_PTR OTS);

static int32_t complete_handle_FPGA_reg31_write(
		OTS_APPLICATION_BUNDLE_PTR OTS_APP, int8_t);
//static int32_t read_FPGA_reg31(OTS_APPLICATION_BUNDLE_PTR OTS_APP);
static int32_t  cnt_get_reg_2 = 0;
SemaphoreHandle_t xSemaphore_OTBB_A_COM_CONNECT = NULL;
extern SemaphoreHandle_t  xSemaphore_OTBA_RD_WR_FPGA;
extern SemaphoreHandle_t  xSemaphore_OTBB_A_STATE_EXM;
FPGA_OTBA_RW_REG FPGA_REG;

//void init_freertos_timers(OTS_APPLICATION_BUNDLE_PTR OTS_APP);
/*** ============CBIT REG EXAM _timer==========================   ****/

char buf[25];

static void pfunA_Delay_Get_Answ_timer(TimerHandle_t timer) {
	OTS_APPLICATION_BUNDLE_PTR OTS_APP =
			(OTS_APPLICATION_BUNDLE_PTR) pvTimerGetTimerID(timer);



	switch (OTS_APP->ST_OTBA_B_COMM)
	{
	  case ST_GET_ANWS_B:
		  if (++OTS_APP->Count_Reg31_B_10ms_Timer >= 1) /** each 3 milliseconds will be notify **/
		  {
			  xTaskNotifyGive(OTS_APP->TASKS.OtbaGetbAnswTaskHandler);
			  /** OTS_APP->ST_OTBA_B_COMM = ST_WAIT_TIMER_OUT; /** 17 November **/
		  }
	 break;
	  case ST_WAIT_TIMER_OUT:
		OTS_APP->Count_Reg31_B_10ms_Timer = 0;
		OTS_APP->ST_OTBA_B_COMM = ST_GET_ANWS_B; /** 17 November **/

		//++OTS_APP->Count_Reg31_B_10ms_Timer;
		//xTimerStop(A_Send_Req_Update_timer,1);
		// OTS_APP->ST_OTBA_B_COMM = ST_WAIT_TIMER_OUT;
		break;
	}

}
/*** ======================================   ****/

/////////////////////////  callback pfunB_Wait_Reg_Update_timer /* xTimerReset() */
static void pfunA_Send_Req_Update_timer(TimerHandle_t timer) {

	OTS_APPLICATION_BUNDLE_PTR OTS_APP =
			(OTS_APPLICATION_BUNDLE_PTR) pvTimerGetTimerID(timer);

	//int32_t err;
	//FPGA_OTBA_RW_REG fpga_reg;

	//ST_SEND_REQ_B,ST_WAIT_AFTER_SEND,ST_GET_ANWS_B   OTS_APP->Count_Reg31_Timer
	switch (OTS_APP->ST_OTBA_B_COMM) {
	case ST_CNT_BEFORE_SEND:
		if (++OTS_APP->Count_Reg31_Timer >= 10) /***expired 100 millsec --> send request to OTBB ***/
		{

			OTS_APP->Count_Reg31_Timer = 0;
			// OTS_APP->ST_OTBA_B_COMM = ST_GET_ANWS_B;
			xTaskNotifyGive(OTS_APP->TASKS.OtbaBreqTaskHandler);

		}
		break;

	case ST_OTBB_NOT_COMMUNC:
		//OTS_APP->Count_Reg31_Timer = 0;
		++OTS_APP->Count_Reg31_Timer;
		OTS_APP->ST_OTBA_B_COMM = ST_CNT_BEFORE_SEND;
		break;
	case ST_OTBB_FAULT_COMMUNC:
		//OTS_APP->Count_Reg31_Timer = 0;
		++OTS_APP->Count_Reg31_Timer;
		OTS_APP->ST_OTBA_B_COMM = ST_CNT_BEFORE_SEND;
		break;
	default:
		break;

	}

}

void OTBA_B_Req_Task(void const *argument)

{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) argument;
	uint32_t err;
	OTB_STATE current_state = OTB_STATE__IDLE,previous_state = OTB_STATE__IDLE;

	OTS_APP->Count_Reg31_Timer = 0;
   /**
	A_Send_Req_Update_timer = xTimerCreate("A_SendReqTimer", pdMS_TO_TICKS(10),
	pdTRUE, OTS_APP, pfunA_Send_Req_Update_timer);

	xTimerStart(A_Send_Req_Update_timer, 0);
	OTS_APP->ST_OTBA_B_COMM = ST_CNT_BEFORE_SEND;
    **/
	printf("OTBA send request to OTBB  task started\n");

	for (;;) {


		vTaskSuspend( NULL );
		 osDelay(15);
		 err = 1;//fsm_exam_test(OTS_APP,current_state,previous_state);
		//vTaskSuspend( NULL );
		//ulTaskNotifyTake(pdTRUE, portMAX_DELAY);//( TickType_t ) pdMS_TO_TICKS (150));
		//send_OTBA_request_to_OTBB(OTS_APP);

	}

}


static int32_t fsm_exam_test(OTS_APPLICATION_BUNDLE_PTR OTS_SM_TEST,OTB_STATE current_state,
		OTB_STATE previous_state)
{

			//void *active_state;
			//OTB_STATE current_state = OTB_STATE__IDLE,previous_state = OTB_STATE__IDLE;
			uint32_t rtrn_status = 0;


			//active_state = fsm_get_active_state(OTS_SM_TEST->fsm);

			void *active_state_test;

			//xSemaphoreTake( xSemaphore_OTBB_A_STATE_EXM, ( TickType_t ) pdMS_TO_TICKS (0));
			//active_state_test =  fsm_get_active_state(OTS_SM_TEST->FSM.fsm);
			//xSemaphoreGive(xSemaphore_OTBB_A_STATE_EXM);

			/****************************start TEST********************************************/

			/********************************LASER_OFF**********************************************/
			if((previous_state == OTB_STATE__IDLE) && (current_state == OTB_STATE__IDLE))
			 {
		    	if(active_state_test == OTS_SM_TEST->FSM.state__LASEROFF->obj )
				{
		    		previous_state = OTB_STATE__IDLE;
					current_state = OTB_STATE__LASER_OFF;
					rtrn_status = 2;
				}
			 }


			if((current_state == OTB_STATE__LASER_OFF) && (previous_state == OTB_STATE__IDLE))
				 {
					if(active_state_test == OTS_SM_TEST->FSM.state__IDLE->obj)
					 {
						current_state = OTB_STATE__IDLE;
						previous_state = OTB_STATE__LASER_OFF;
						rtrn_status = 1;
					 }
				 }

			/********************************end of LASER_OFF******************************************/

			/********************************OTB_STATE__OPERATE**********************************************/
			if((previous_state == OTB_STATE__IDLE) && (current_state == OTB_STATE__IDLE))
				 {
			    	if(active_state_test == OTS_SM_TEST->FSM.state__OPERATE->obj )
					{
			    		previous_state = OTB_STATE__IDLE;
						current_state = OTB_STATE__OPERATE;
						rtrn_status = 3;
					}
				 }

			 if((current_state == OTB_STATE__OPERATE) && (previous_state == OTB_STATE__IDLE))
					 {
						if(active_state_test == OTS_SM_TEST->FSM.state__IDLE->obj)
						 {
							previous_state = OTB_STATE__OPERATE;
							current_state = OTB_STATE__IDLE;
							rtrn_status = 1;
						 }
					 }
			 /********************************end of OTB_STATE__OPERATE******************************************/

			/****************************END OF TEST********************************************/
		     return rtrn_status;
}


static OTBB_CONNECTED_STATUS = 0;
void OTBA_Get_B_Answ_Task(void const *argument) {

	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) argument;
	uint32_t err;
    OTBBCOM_PACKET packet;
	size_t NumBytesBeReceived;					   
	//xSemaphore_OTBB_COM = xSemaphoreCreateMutex();
	xSemaphore_OTBB_A_COM_CONNECT = xSemaphoreCreateMutex();
	//OTS_APP->ST_OTBA_B_COMM = ST_CNT_BEFORE_SEND; /*** initialized ST_REG6_EXAM  ***/

	OTS_APP->ST_OTBA_B_COMM = ST_WAIT_TIMER_OUT; /** ST_GET_ANWS_B;  **/  /** 17 November**/
	OTS_APP->Count_Reg31_B_10ms_Timer = 0;
	OTS_APP->OTBBCOM.otbb_reg23_value_recv_last = 0;

	A_Delay_Get_Answ_timer = xTimerCreate("A_GetBanswtimer", pdMS_TO_TICKS(3),
	pdTRUE, OTS_APP, pfunA_Delay_Get_Answ_timer);

	//B_Exam_Event_Bits= xEventGroupCreate();

	xTimerStart(A_Delay_Get_Answ_timer, 1);  /*** 17 November ***/

	printf("OTB_B Register Error Examine Task started\n");

	for (;;) {

		ulTaskNotifyTake(pdTRUE, (TickType_t) portMAX_DELAY);

		//xSemaphoreTake( xSemaphore_OTBB_A_COM_CONNECT, ( TickType_t ) pdMS_TO_TICKS (0)); //07_03_2022
		err = io_ioctl(OTS_APP->DRIVERS.OTBBCOM, IO_IOCTL_OTBBCOM__IS_CONNECTED,
						                       &OTS_APP->OTBBCOM.is_connected);
		//xSemaphoreGive(xSemaphore_OTBB_A_COM_CONNECT); //07_03_2022

		//osDelay(1);
	/**																		
	NumBytesBeReceived = xMessageBufferReceive( xOTBBMessageBuffer,
	  						                        (void *)&packet,
	  												sizeof( packet ) +5 ,
	  												pdMS_TO_TICKS(0));

	  //xSemaphoreGive( xSemaphore_IP_Queue);

	  app_otbbcom_handle_comm_2(OTS_APP, &packet);
      **/														
		if(OTS_APP->OTBBCOM.is_connected==0)//not conncted
		{
			OTS_APP->OTBBCOM.otbb_err_counter=0;
		}
   //  if(OTS_APP->OTBBCOM.is_connected)
		else
      if(packet.HDR.OTBB_IS_CONNECTED)
      {
    	 OTS_APP->OTBBCOM.is_connected = 1;
    	if ((OTS_APP->OTBBCOM.otbb_reg23_value_received != 0) /*&& (OTS_APP->OTBBCOM.is_connected)*/)// flag for new data
		{
			Proccessing_Answ_OTBB(OTS_APP);
			OTS_APP->OTBBCOM.otbb_reg23_value_received = 0;
			OTS_APP->OTBBCOM.otbb_disc_rx_err=1;
		}
	  }
	}

}
/***
static int32_t read_FPGA_reg31(OTS_APPLICATION_BUNDLE_PTR OTS_APP) {

	FPGA_OTBA_RW_REG reg;

	reg.address = 31;
	reg.value = 0;	//OTS_APP->OTBBCOM.otbb_reg23_value;

	io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__READ_REG, &reg);

	OTS_APP->OTBBCOM.otbb_reg23_value = reg.value;

	return OTS_APP->OTBBCOM.otbb_reg23_value;
}
***/
static int32_t complete_handle_FPGA_reg31_write(
		OTS_APPLICATION_BUNDLE_PTR OTS_APP, int8_t bit) {

	FPGA_OTBA_RW_REG reg;
	int32_t err;
	//xSemaphoreTake( xSemaphore_OTBA_RD_WR_FPGA, ( TickType_t ) pdMS_TO_TICKS (0));
	reg.address = 31;
	//if (bit == 1)
//		reg.value = OTS_APP->OTBBCOM.otbb_reg23_value_recv_last | bit;
	//else
		//...reg.value = OTS_APP->OTBBCOM.otbb_reg23_value_received;
	//...FPGA_REG.value = reg.value;

	xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
	reg.address = 31;
	reg.value = OTS_APP->OTBBCOM.otbb_reg23_value_received;
    FPGA_REG.value = reg.value;
	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &reg);
	xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);

	return err;

}

static int32_t send_OTBA_request_to_OTBB(OTS_APPLICATION_BUNDLE_PTR OTS_APP) {
	///uint8_t is_connected;

	 //FPGA_REG.value
/*
	uint8_t telnet_connected;
	int32_t err;
	OTBBCOM_PACKET packet;


	xSemaphoreTake( xSemaphore_OTBB_A_COM_CONNECT, ( TickType_t ) pdMS_TO_TICKS (0));
	err = io_ioctl(OTS_APP->DRIVERS.OTBBCOM, IO_IOCTL_OTBBCOM__IS_CONNECTED,
			                       &OTS_APP->OTBBCOM.is_connected);
	xSemaphoreGive(xSemaphore_OTBB_A_COM_CONNECT);

	//OTS_APP->OTBBCOM.otbb_reg23_value = read_FPGA_reg31(OTS_APP);

	if (err != IO_ERROR_OTBBCOM__OK) {

		OTS_APP->ST_OTBA_B_COMM = ST_OTBB_NOT_COMMUNC;
		//////printf("OTBB get error from io_ioctl Fault to send packet\r\n");
		//OTS_APP->OTBBCOM.otbb_reg23_value |= 0x01;
		complete_handle_FPGA_reg31_write(OTS_APP, 1);
		//xTimerReset(A_Send_Req_Update_timer, 2);// re-start A send req timer

		return err;
	}

	if (!OTS_APP->OTBBCOM.is_connected) {
		// Disconnected - write 1 to reg 23
		OTS_APP->ST_OTBA_B_COMM = ST_OTBB_NOT_COMMUNC;
		//OTS_APP->OTBBCOM.otbb_reg23_value |= 0x01;
		//////printf("OTBB connection fault Fault to send packet\r\n");
		complete_handle_FPGA_reg31_write(OTS_APP, 1);
		//xTimerReset(A_Send_Req_Update_timer, 2); // re-start A send req timer

		return err;
	} else
	 {
		// Connected, ask OTBB for reg23 value
		OTS_APP->OTBBCOM.otbb_reg23_value_received = 0;


		packet.HDR.DESTINATION = OTBBCOM_ICD_ID__OTBB;
		packet.HDR.SOURCE = OTBBCOM_ICD_ID__OTBA;
		packet.HDR.OPCODE = OTBBCOM_OPCODE__FPGA_REG_23_VALUE_REQ;
		packet.HDR.LEN = 0;

		err = io_write(OTS_APP->DRIVERS.OTBBCOM, (uint8_t*) &packet, 1);

		if (err != IO_ERROR_OTBBCOM__OK) {
			OTS_APP->ST_OTBA_B_COMM = ST_OTBB_FAULT_COMMUNC;
			//OTS_APP->OTBBCOM.otbb_reg23_value |= 0x01;
			/////// printf("Communication Fault to send packet\r\n");
			complete_handle_FPGA_reg31_write(OTS_APP, 1);//xTimerStop(A_Send_Req_Update_timer,4);
			//xTimerReset(A_Send_Req_Update_timer, 2);// re-start A send req timer

			return err;

		}

		//ST_OTBA_B_COMM = ST_CNT_BEFORE_SEND;  ST_OTBA_B_COMM = ST_SEND_A_REQ_B;
		//xTimerStop(A_Send_Req_Update_timer,2);


		OTS_APP->ST_OTBA_B_COMM = ST_GET_ANWS_B;
		OTS_APP->Count_Reg31_B_10ms_Timer = 0;
		xTimerStart(A_Delay_Get_Answ_timer, 1); // start timer to waiting for an answer from OTBB;

	}
	*/
}

static int32_t Proccessing_Answ_OTBB(OTS_APPLICATION_BUNDLE_PTR OTS_APP) {

	uint8_t telnet_connected;
	int32_t err;

	   ///...xSemaphoreTake( xSemaphore_OTBB_A_COM_CONNECT, ( TickType_t ) pdMS_TO_TICKS (0));  //07_03_2022
		////err = io_ioctl(OTS_APP->DRIVERS.OTBBCOM, IO_IOCTL_OTBBCOM__IS_CONNECTED,&OTS_APP->OTBBCOM.is_connected);
		///....xSemaphoreGive(xSemaphore_OTBB_A_COM_CONNECT);  //07_03_2022

	//OTS_APP->OTBBCOM.otbb_reg23_value = read_FPGA_reg31(OTS_APP);

	if (err != IO_ERROR_OTBBCOM__OK) {

		/**if (OTS_APP->ST_OTBA_B_COMM == ST_WAIT_TIMER_OUT)
				  OTS_APP->ST_OTBA_B_COMM = ST_OTBB_NOT_COMMUNC; ***/
				  OTS_APP->ST_OTBA_B_COMM = ST_WAIT_TIMER_OUT; /** 17 November ***/

		   complete_handle_FPGA_reg31_write(OTS_APP, 1);
		   /***  xTimerStop(A_Delay_Get_Answ_timer, 1);  17 November ***/


		return err;
	}

	 if(!OTS_APP->OTBBCOM.is_connected) ///if (!is_connected) {
	  {
		// Disconnected - write 1 to reg 23
		/**if (OTS_APP->ST_OTBA_B_COMM == ST_WAIT_TIMER_OUT)
		  OTS_APP->ST_OTBA_B_COMM = ST_OTBB_NOT_COMMUNC; ***/
		  OTS_APP->ST_OTBA_B_COMM = ST_WAIT_TIMER_OUT; /** 17 November ***/


			complete_handle_FPGA_reg31_write(OTS_APP, 1);
			/***  xTimerStop(A_Delay_Get_Answ_timer, 1); 17 November ***/
			//xTimerReset(A_Send_Req_Update_timer, 2); // start timer to waiting for an answer from OTBB;

		return err;
	  }
	  else
	   {
		/*** get answer from OTBB ***/
		//  if (OTS_APP->OTBBCOM.otbb_reg23_value_received == 1)
		   {

			 OTS_APP->OTBBCOM.otbb_reg23_value_recv_last =
					OTS_APP->OTBBCOM.otbb_reg23_value;  /// store last received value of reg 31

			 complete_handle_FPGA_reg31_write(OTS_APP, 0);
			 /*** OTS_APP->ST_OTBA_B_COMM = ST_CNT_BEFORE_SEND;  17 November***/
			 OTS_APP->ST_OTBA_B_COMM = ST_WAIT_TIMER_OUT;  /*** 17 November***/

             #if test_telnt
			  io_ioctl(OTS_APP->DRIVERS.TELNET, IO_IOCTL_TELNET__IS_CONNECTED, &telnet_connected);
			  if(cnt_get_reg_2++% 300 == 0)
	           {
				  cnt_get_reg_2 = 0;
			      io_ioctl(OTS_APP->DRIVERS.TELNET, IO_IOCTL_TELNET__IS_CONNECTED, &telnet_connected);
			      if (telnet_connected)
			      {
				    snprintf(buf, 25-1, "WRITE_TO_REG31= 0x%x \r\n",OTS_APP->OTBBCOM.otbb_reg23_value);
				    io_write(OTS_APP->DRIVERS.TELNET, (uint8_t*)buf, strlen(buf));
			      }
			   }
             #endif

		  }
		/*** OTS_APP->ST_OTBA_B_COMM = ST_CNT_BEFORE_SEND;  17 November***/
		OTS_APP->ST_OTBA_B_COMM = ST_WAIT_TIMER_OUT;  /*** 17 November***/

		/***  xTimerStop(A_Delay_Get_Answ_timer, 1);  17 November***/
		//xTimerReset(A_Send_Req_Update_timer, 2); // start timer to waiting for an answer from OTBB;
	}

}

