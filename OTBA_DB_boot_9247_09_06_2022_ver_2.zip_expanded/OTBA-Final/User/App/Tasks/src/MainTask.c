/**
 ******************************************************************************
 * @file    MainTask.c
 * @brief   OTS Main State Machine execution.
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
int32_t loop=0;
static TimerHandle_t main_timer;




static void main_wakeup_timer(TimerHandle_t timer)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) pvTimerGetTimerID(timer);

	xTaskNotifyGive(OTS_APP->TASKS.mainTaskHandler);
}


#define HANDLE_TEC_PERIOD_TICKS (1000u)


static int32_t handle_TEC(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	loop++;
	//printf("Main_loop %ld | ", loop);

	IO_HANDLER_PTR handler = (IO_HANDLER_PTR)OTS_APP->DRIVERS.CPU_TEMP;
	int32_t err;
	FPGA_OTBA_RW_REG request;


	request.address = FPGA_OTBA_REG__SFP_TEC_CONT;
	uint8_t start_temp, stop_temp;

	err = io_ioctl(OTS_APP->DRIVERS.CPU_TEMP, IO_IOCTL_CPU_TEMP__GET_HIGH, &start_temp);
	if (err != IO_ERROR_CPU_TEMP__OK) return err;

	err = io_ioctl(OTS_APP->DRIVERS.CPU_TEMP, IO_IOCTL_CPU_TEMP__GET_LOW, &stop_temp);
	if (err != IO_ERROR_CPU_TEMP__OK) return err;

	// read Temp. Sensor through ADC
	static float temperature = 0.0f;
	io_read(handler, (uint8_t*)&temperature, 1);
//	printf(":::TEC::: Temperature is %ld | ", (int32_t)temperature);

	// cooling operation logic - operate >35, off <15
	// write to FPGA reg 22 - FPGA_OTBA_REG__SFP_TEC_CONT
	// d7 - cooling - always 1
	// d0 - d6 PWM; 0000000 to stop
	if(temperature > start_temp)
	{
		request.value = 0xC0;

		xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
		err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &request);
		xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);

		if (IO_ERROR_FPGA_OTBA__OK != err) return err;
	}

	if(temperature < stop_temp)
	{
		request.value = 0x00;

		xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
		err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &request);
		xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);

		if (IO_ERROR_FPGA_OTBA__OK != err) return err;
	}

	xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__READ_REG, &request);
	xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);

	//err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__READ_REG, &request);
			if (IO_ERROR_FPGA_OTBA__OK != err) return err;

//	printf("Status: %s\n\r", request.value == 0 ? "Not Cooling" : "Cooling");
	return APP_ERROR_OK;
}


#define HANDLE_FPGA_REG23_PERIOD_TICKS (500u)
#define HANDLE_FPGA_REG23_PENDING_TICKS (50u)


/*****************************SM_logic_app *****************************************/

#define SM_LOGIC_TASK_STASK_SIZE		256
StackType_t SM_LOGIC_TASK_STACK[SM_LOGIC_TASK_STASK_SIZE];
StaticTask_t SM_LOGIC_TASK_BUFFER;

TaskHandle_t SM_Logic_task_handler;
TimerHandle_t A_Delay_Update_Logic_ST;

TaskHandle_t temp_task_handler;

void pfunA_Logic_ST_Update( TimerHandle_t xtimer )
 {
	OTS_APPLICATION_BUNDLE_PTR OTS_APP =
				(OTS_APPLICATION_BUNDLE_PTR) pvTimerGetTimerID(xtimer);

	xTaskNotifyGive(SM_Logic_task_handler);

 }


void SM_Logic_task (void  * argument)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) argument;



	for(;;)
	{
		ulTaskNotifyTake(pdTRUE, (TickType_t) portMAX_DELAY);
		SM_logic_app(OTS_APP);

	}
}

/*****************************SM_logic_app *****************************************/


#define OTBBCOM_EXAM_RX_TASK_STACK_SIZE		512
StaticTask_t OTBBCOM_EXAM_RX_TASK_STACK_BUFFER;
StackType_t OTBBCOM_EXAM_RX_TASK_STACK[OTBBCOM_EXAM_RX_TASK_STACK_SIZE];

void MainTask(void const * argument)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) argument;
	//OTBBCOM_EXAM_BUNDLE_PTR driver_exam_bundle = NULL;
	int32_t err;
	int16_t adc_data;
	TickType_t start_time, now_time;

	uint32_t rtrn_status = 0;

	printf("OTBA main task started\n");

	xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__TEST, NULL);
	xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);

	if (err == IO_ERROR_FPGA_OTBA__OK)
		printf("FPGA HW test passed\r\n");
	else printf("FPGA HW test failed\r\n");

	uint8_t fpga_rev = 0;
	xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
	io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__GET_HW_REV, &fpga_rev);
	xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);
	printf("FPGA HW revision: 0x%X\r\n", fpga_rev);

	main_timer = xTimerCreate(
					"MainTimer",
					pdMS_TO_TICKS(10),
					pdTRUE,
					OTS_APP,
					main_wakeup_timer);

	xTimerStart(main_timer, 0);


	SM_Logic_task_handler = xTaskCreateStatic(
						SM_Logic_task,
						"SM_Logic",
						SM_LOGIC_TASK_STASK_SIZE,
						OTS_APP,//OTS_APP,
						osPriorityNormal - 2 ,
						SM_LOGIC_TASK_STACK,
						&SM_LOGIC_TASK_BUFFER );


	A_Delay_Update_Logic_ST = xTimerCreate("SM_Logic_timer", pdMS_TO_TICKS(25),
				                     pdTRUE, OTS_APP, pfunA_Logic_ST_Update);

	xTimerStart(A_Delay_Update_Logic_ST, 0);




    /* Allocate OTBBCOM bundle */
 	 /**
	   driver_exam_bundle = (OTBBCOM_EXAM_BUNDLE_PTR) pvPortMalloc(sizeof(OTBBCOM_EXAM_BUNDLE_PTR));
 	   if (NULL == driver_exam_bundle)
 	   {
 	    	return IO_ERROR_OTBBCOM__OUT_OF_HEAP;
 	    }
 	   memset(driver_exam_bundle, 0, sizeof(OTBBCOM_EXAM_BUNDLE_PTR));

      **/

 	  //driver_exam_bundle->release_sem_receive = Gl_driver_bundle->release_sem_receive;
 	  //driver_exam_bundle->task_give_notify =	Gl_driver_bundle->task_give_notify;


   /**
 	  err_os = xTaskCreate(OTBB_COM_examine_task_rx, "OTBBCOM_EXAMINE_RX", 128, driver_exam_bundle, osPriorityNormal,
 			                                      &driver_exam_bundle->examine_task_rx );
 	  	if (err_os != pdPASS)
 	  	{
 	  		return IO_ERROR_OTBBCOM__OUT_OF_HEAP;
 	  	}

    **/

	OTS_APP->mailbox_laser_state = xQueueCreate(1, sizeof(OTS_APP->B_laser_state));

	temp_task_handler  = xTaskCreateStatic(
											OTBB_COM_examine_task,
 										    "OTBBCOM_EXAMINE_RX",
 											OTBBCOM_EXAM_RX_TASK_STACK_SIZE,
 											OTS_APP,
 											osPriorityNormal - 1,
 											OTBBCOM_EXAM_RX_TASK_STACK,
 											&OTBBCOM_EXAM_RX_TASK_STACK_BUFFER );



	OTS_APP->B_ETHR.examine_task_rx = temp_task_handler;



	OTS_APP->CBIT.cbit_counter = 0;

	start_time = xTaskGetTickCount();

	for(;;)
	{
		// Wait for timer
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		now_time = xTaskGetTickCount();
		//OTS_APP->CBIT.cbit_counter = 0;



		if ((now_time - start_time) % HANDLE_TEC_PERIOD_TICKS < 10)
		{
			OTS_APP->CBIT.cbit_counter = 100;
			handle_TEC(OTS_APP);
	//		 adc_data = Read_ADS8681(OTS_APP);
	//		 perform_CBIT_if_necessary(OTS_APP);
		}

	//*******************************************************************
		//handle_FPGA_reg23(OTS_APP, 1u); /*** Register 31 processing   ***/

		fsm_sm_run(OTS_APP->FSM.fsm);
		//SM_logic_app(OTS_APP);
	}

}

/********************************end of main task***************************************************************/

