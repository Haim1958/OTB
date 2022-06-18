/**
 ******************************************************************************
 * @file    CliTask.c
 * @brief   OTS CLI task execution.
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
//int32_t loop2=0;

//static TimerHandle_t cli_timer;

//static void cli_handle_rx(TimerHandle_t timer)
//{
//	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) pvTimerGetTimerID(timer);
//
////	xTaskNotifyGive(OTS_APP->TASKS.cliTaskHandler);
//}

void CliTask(void const * argument)
{

	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) argument;
//	uint32_t counter = 0;
	
	io_ioctl(OTS_APP->DRIVERS.TELNET, IO_IOCTL_TELNET__START_SERVER, NULL);

//	cli_timer = xTimerCreate(
//			"CliTimer",
//			10u,
//	        1u,
//	        OTS_APP,
//			cli_handle_rx);
//
//	xTimerStart(cli_timer, 0);

	for(;;)
	{
		// Wait for timer

		///////////////////////////////////////////////////////////
		// 				NOTE
		//				NOTE
		// 				NOTE
		//
		//				CLI TASK MUST NOT BE BLOCKED BY ulTaskNotifyTake
		//				AND NOT CALL xTaskNotifyGive
		//				BECAUSE THE UART STREAMBUFFER IS BUILT UPON THE SAME
		//				FREERTOS MECHANISM AND CALLING xTaskNotifyGive RELEASES
		//				THE STREAMBUFFER xStreamBufferReceive INTERNAL LOCK
		///////////////////////////////////////////////////////////
//		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		app_cli_handle_rx(OTS_APP);

		osDelay(100);
//	if (loop2&1023==0)	{printf("CLI %ld | ", loop2);}

//		if (100 == counter++)
//		{
//			counter = 0;
//		}

	}
}
