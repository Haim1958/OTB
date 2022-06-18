/**
 ******************************************************************************
 * @file    optmod.c
 * @brief   OTBA optic modulator algorithm public functions implementations
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
#include <math.h>
#include "UserLib.h"
#include "app.h"
/* Global variables */

#define _OPTMOD_OPER_TASK_STASK_SIZE		256
StackType_t _OPTMOD_OPER_TASK_STACK[_OPTMOD_OPER_TASK_STASK_SIZE];
StaticTask_t _OPTMOD_OPER_TASK_BUFFER;

#define _OPTMOD_CALIB_TASK_STASK_SIZE		256
StackType_t _OPTMOD_CALIB_TASK_STACK[_OPTMOD_CALIB_TASK_STASK_SIZE];
StaticTask_t _OPTMOD_CALIB_TASK_BUFFER;
/**
 *
 * int32_t			(*open_fcn)(IO_HANDLER_PTR, void*);					// Read function pointer
	int32_t			(*read_fcn)(IO_HANDLER_PTR, uint8_t*, uint32_t);	// Read function pointer
	int32_t			(*write_fcn)(IO_HANDLER_PTR, uint8_t*, uint32_t);	// Write function pointer
	int32_t			(*ioctl_fcn)(IO_HANDLER_PTR, uint32_t, void*);


**/

static int32_t _optmod_open(IO_HANDLER_PTR handler, void * init);
static int32_t _optmod_read(IO_HANDLER_PTR handler, uint8_t * parm, uint32_t);
static int32_t _optmod_write(IO_HANDLER_PTR handler, uint8_t * parm, uint32_t);
static int32_t _optmod_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr);


static int32_t optmod_operation_terminate(OPTMOD_STRUCT_PTR OPTMOD);
static int32_t optmod_operation_start(OPTMOD_STRUCT_PTR OPTMOD);


static int32_t optmod_calibration_start(OPTMOD_STRUCT_PTR OPTMOD);
static int32_t optmod_calibration_terminate(OPTMOD_STRUCT_PTR OPTMOD);


static void optmod_calibration_block_until_done(OPTMOD_STRUCT_PTR OPTMOD);
static int32_t optmod_calibration_read_sample(OPTMOD_STRUCT_PTR OPTMOD);
static void optmod_operation_block_until_done(OPTMOD_STRUCT_PTR OPTMOD);


static int32_t optmod_operation_get_settings(OPTMOD_STRUCT_PTR OPTMOD);
static int32_t optmod_operation_set(OPTMOD_STRUCT_PTR OPTMOD);

static OPTMOD_OPER_STATUS_PTR optmod_oper_status_duration(OPTMOD_STRUCT_PTR OPTMOD);
static OPTMOD_OPER_STATUS_PTR optmod_oper_status_state(OPTMOD_STRUCT_PTR OPTMOD);
//static int32_t optmod_init_created(OPTMOD_STRUCT_PTR OPTMOD);
static OPTMOD_STRUCT_PTR optmod_init_created(OPTMOD_STRUCT_PTR OPTMOD);

static int32_t optmod_init(OPTMOD_STRUCT_PTR OPTMOD);

static OPTMOD_CALIB_SAMPLE _CALIB_SAMPLES[4096];

static char buf[100] = "";

static const OPTMOD_OPER_PARAMS OPTMOD_SETTINGS_DEFAULT = {
		.trgt_pwr = 0,
		.dac_start = 2048,
		.direction = 1,
		.if_print = 0
};

/* Functions definitions */




/************************ START_OPTMOD_INITIALIZING***************************************/

int32_t install_optmod_func (OPTMOD_INIT_STRUCT_PTR OPTMod_Func_init)

{
	OPTMOD_STRUCT_PTR driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_optmod_open,
											_optmod_read,
											_optmod_write,
											_optmod_ioctl,
											0u
										};
	int32_t err;


	driver_bundle = (OPTMOD_STRUCT_PTR) pvPortMalloc(sizeof(OPTMOD_STRUCT));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_OTBBCOM__OUT_OF_HEAP;
	}
	OPTMOD_GLB  = (OPTMOD_STRUCT_PTR)driver_bundle;

	ulaser_off_flg = uflgLASER_OFF_ON;
	memset(driver_bundle, 0, sizeof(OPTMOD_STRUCT));

	init.bundle = driver_bundle;

	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__OPTMOD_FUNC, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}

	return 0;
}

static int32_t _optmod_open(IO_HANDLER_PTR handler, void * init)
{
	return 0;
}
static int32_t _optmod_read(IO_HANDLER_PTR handler, uint8_t * parm, uint32_t temp )
{
	return 0;
}
static int32_t _optmod_write(IO_HANDLER_PTR handler, uint8_t * parm, uint32_t temp)
{
	return 0;
}


/*******************************************************************************
* Function Name: _otbbcom_ioctl
********************************************************************************
#define IO_IOCTL_OPTMOD_EXAMINE								1
#define IO_IOCTL_OPTMOD_IS_RE_CREATED						2
#define IO_IOCTL_OPTMOD_IS_RE_DELETED                      	3

#define IO_IOCTL_OPTMOD_IS_CALIB_START						4
#define IO_IOCTL_OPTMOD_IS_CALIB_DONE                      	5

#define IO_ERROR_OPTMOD_OK									0x5A
#define IO_ERROR_OPTMOD_UNKNOWN_IOCTL						0x7F

#define IO_IOCTL_OPTMOD_CALIB_OPER_UNTIL_DONE				5
#define IO_IOCTL_OPTMOD_CALIB_READ_SAMPLE                   6
#define IO_IOCTL_OPTMOD_CALIB_UNTIL_DONE                    7


*******************************************************************************/

static int32_t _optmod_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	//OPTMOD_STRUCT_PTR driver_bundle = (OPTMOD_STRUCT_PTR) handler->bundle;
	OPTMOD_STRUCT_PTR driver_bundle = (OPTMOD_STRUCT_PTR)OPTMOD_GLB;
	int32_t err = IO_ERROR_OPTMOD_OK;

	switch (ctl)
	{
	  case IO_IOCTL_OPTMOD_EXAMINE:
		  //handler->bundle = NULL;
	  break;
	   case IO_IOCTL_OPTMOD_INIT:
		   err = optmod_init(driver_bundle);
	  break;
	  case IO_IOCTL_OPTMOD_INIT_ADC:
		  driver_bundle->OPTIC_ADC  = (IO_HANDLER_PTR)ptr;
		  OPTMOD_OPTIC_ADC_GLB = driver_bundle->OPTIC_ADC;
	  	  break;
	  case IO_IOCTL_OPTMOD_INIT_DAC:
		  driver_bundle->OPTIC_DAC = (IO_HANDLER_PTR)ptr;
		  OPTMOD_OPTIC_DAC_GLB = driver_bundle->OPTIC_DAC;
	  break;

	  case IO_IOCTL_OPTMOD_OPER_START_RE_CREATED:
		       //handler->bundle = driver_bundle->SETTINGS_FILE;
	 		  err = optmod_operation_start(driver_bundle);
	 		  ptr = (uint16_t *)&err;
	 		  break;

	  case IO_IOCTL_OPTMOD_OPER_TERMNT_RE_DELETED:

	  	 	  err = optmod_operation_terminate(driver_bundle);
	  	 	  break;

	  case IO_IOCTL_OPTMOD_IS_CALIB_START:
	 	  	 err = optmod_calibration_start(driver_bundle);
	  break;

	  case IO_IOCTL_OPTMOD_IS_CALIB_TERMINATE:
	  	 	  err = optmod_calibration_terminate(driver_bundle);
	  break;

	  case IO_IOCTL_OPTMOD_OPER_UNTIL_DONE:
	  	  	  optmod_operation_block_until_done(driver_bundle);

	  break;

	  case IO_IOCTL_OPTMOD_CALIB_READ_SAMPLE:

	  	  	  	err = optmod_calibration_read_sample(driver_bundle);
	  	  	//driver_bundle->CALIB.samples_count = (OPTMOD_STRUCT_TEMP_CAILB_PTR)ptr->samples_count;
	  	     //driver_bundle->CALIB.index = (OPTMOD_STRUCT_TEMP_CAILB_PTR)ptr->index;
	  break;

	  case IO_IOCTL_OPTMOD_CALIB_UNTIL_DONE:
		    optmod_calibration_block_until_done(driver_bundle);
	  break;

	  case IO_IOCTL_OPTMOD_OPER_PARAMS_SET:
		  driver_bundle = OPTMOD_GLB;
		  err = optmod_operation_set(driver_bundle);
	  break;

	  case IO_IOCTL_OPTMOD_GET_SETTINGS_EEPROM:
		  err = optmod_operation_set(driver_bundle);

	  break;
	  case IO_IOCTL_OPER_STATUS_DURATION:
		  ptr = (OPTMOD_OPER_STATUS_PTR)optmod_oper_status_duration(driver_bundle);
	  break;
	  case IO_IOCTL_OPER_STATUS_STATE:
		  ptr = (OPTMOD_OPER_STATUS_PTR )optmod_oper_status_state(driver_bundle);
	  break;
	  case IO_IOCTL_OPTMOD_INIT_CREATED:
	     //err = optmod_init_created(driver_bundle);
	  break;
	  default:
		err = IO_ERROR_OPTMOD_UNKNOWN_IOCTL;
		break;
	}

	return err;
}

/****************** ****************************************************/
static  OPTMOD_OPER_STATUS_PTR  optmod_oper_status_duration(OPTMOD_STRUCT_PTR OPTMOD)
{
	 void * duration = (OPTMOD_OPER_STATUS_PTR)&OPTMOD->OPER.STATUS;
	 return  duration ;
}

static OPTMOD_OPER_STATUS_PTR optmod_oper_status_state(OPTMOD_STRUCT_PTR OPTMOD)
{
	void * duration = (OPTMOD_OPER_STATUS_PTR)&OPTMOD->OPER.STATUS;
		 return  duration ;
}

/*******************   OPTMOD_OPERATION_TASK  ********************************************/


static uint32_t count_delay_oper_task =0;

static void pfunB_Delay_oper_task_timer(TimerHandle_t timer)
{

	OPTMOD_STRUCT_PTR OPTMOD  = (OPTMOD_STRUCT_PTR) pvTimerGetTimerID(timer);

	if(++count_delay_oper_task >= 2)
	{
		count_delay_oper_task = 0;
		xTaskNotifyGive(OPTMOD->OPER.task);
	}

}




void _optmod_operation_task(void * argument)
{
	OPTMOD_STRUCT_PTR OPTMOD = (OPTMOD_STRUCT_PTR) argument;
	OPTMOD_OPER_STATE state = OPTMOD_OPER_STATE__SEEK, prev_state = OPTMOD_OPER_STATE__SEEK;
	int16_t dac = OPTMOD->OPER.PARAMS.dac_start;
	int16_t direction = OPTMOD->OPER.PARAMS.direction;
	int32_t curr_pwr = 0;
	int32_t old_error, curr_error;
	int32_t err;
	float Error_float;


	OPTMOD->OPER.oper_task_timer = xTimerCreate("Delay_oper_task_timer",
							pdMS_TO_TICKS(5),
							pdTRUE,
							OPTMOD,
							pfunB_Delay_oper_task_timer);

	xTimerStart(OPTMOD->OPER.oper_task_timer, 0);



	OPTMOD->OPER.operation_start_ticks = xTaskGetTickCount();
	OPTMOD->OPER.seek_start_ticks = xTaskGetTickCount();

	old_error = curr_error = OPTMOD->OPER.PARAMS.trgt_pwr - curr_pwr;

	for (;;)
	{

		if(state != OPTMOD_OPER_STATE__OFF)
			ulTaskNotifyTake(pdTRUE, ( TickType_t )/** pdMS_TO_TICKS**/ (portMAX_DELAY));



		if (state == OPTMOD_OPER_STATE__OFF)
		{
			vTaskDelay(OPTMOD_OPER__INTERVAL_MS__SEEK);
			continue;
		}

		if (OPTMOD->OPER.PARAMS.if_print && OPTMOD->STDOUT != NULL)
		{
			if (state == OPTMOD_OPER_STATE__SEEK)
			{
				memset(buf, 0, 100);
				snprintf(buf, 100-1, "OPTMOD SEEK ::: ADC=%ld; curr_error=%ld, DAC=%d, direction=%d\r\n", curr_pwr, curr_error, dac, direction);
				io_write(OPTMOD->STDOUT, (uint8_t*)buf, strlen(buf));
			}
			else if (state == OPTMOD_OPER_STATE__LOCK)
			{
				memset(buf, 0, 100);
				snprintf(buf, 100-1, "OPTMOD LOCK ::: ADC=%ld; curr_error=%ld, DAC=%d, direction=%d\r\n", curr_pwr, curr_error, dac, direction);
				io_write(OPTMOD->STDOUT, (uint8_t*)buf, strlen(buf));
			}
		}

		// Write initial DAC value
		err = io_write(OPTMOD->OPTIC_DAC, (uint8_t*)&dac, 1);
		if (err != IO_ERROR_OPT_ADC__OK)
		{
			continue;
		}

		if (curr_error < OPTMOD_OPER__LOCK_THRESH)
		{
			prev_state = state;
			state = OPTMOD_OPER_STATE__LOCK;
		}

		if (curr_error > OPTMOD_OPER__SEEK_THRESH)
		{
			prev_state = state;
			state = OPTMOD_OPER_STATE__SEEK;
		}

		// print status to UART if state changed
		if (prev_state == OPTMOD_OPER_STATE__LOCK && state == OPTMOD_OPER_STATE__SEEK)
		{
			printf("System in calibration process\r\n");
			OPTMOD->OPER.seek_start_ticks = xTaskGetTickCount();
		}
		else if (prev_state == OPTMOD_OPER_STATE__SEEK && state == OPTMOD_OPER_STATE__LOCK)
		{
			printf("System is locked\r\n");
		}

		// Delay, depending on the state
		if (state == OPTMOD_OPER_STATE__SEEK)
		{
			vTaskDelay(OPTMOD_OPER__INTERVAL_MS__SEEK);
		}
		else
		{
			vTaskDelay(OPTMOD_OPER__INTERVAL_MS__LOCK);
		}

		err = io_read(OPTMOD->OPTIC_ADC, (uint8_t*)&curr_pwr, 1);
		if (err != IO_ERROR_OPT_ADC__OK)
		{
			continue;
		}

		old_error = curr_error;
		curr_error = abs(OPTMOD->OPER.PARAMS.trgt_pwr - curr_pwr);

		if (curr_error > old_error)
		{
			direction = -direction;
		}
		//haim 18/11
		Error_float= ( float) direction * ((float) curr_error) *((float)  OPTMOD_OPER__ADC_DAC_AMP) * 0.5;
			dac = dac +  (int16_t)Error_float;
			int16_t Upper_limit= 4096;
			int16_t Low_limit= 0;

		// Rollover
		while (dac < 0) { dac += 4096; }
		while (dac > 4095) { dac -= 4096; }

		printf( "OPTMOD ::: ADC=%ld; diff=%ld, DAC=%d, dir=%d\r\n", curr_pwr, curr_error, dac, direction);

		// Update status
		//taskENTER_CRITICAL(); 07.12.2021
		OPTMOD->OPER.STATUS.state = state;
		OPTMOD->OPER.STATUS.curr_pwr = curr_pwr;
		OPTMOD->OPER.STATUS.curr_error = curr_error;
		if (state == OPTMOD_OPER_STATE__SEEK)
		{
			OPTMOD->OPER.STATUS.seek_duration = xTaskGetTickCount() - OPTMOD->OPER.seek_start_ticks;
		}
		OPTMOD->OPER.STATUS.duration = xTaskGetTickCount() - OPTMOD->OPER.operation_start_ticks;
		//taskEXIT_CRITICAL();  07.12.2021
	}
}


 static int32_t optmod_operation_start(OPTMOD_STRUCT_PTR OPTMOD)
{

	OPTMOD_OPER_PARAMS_PTR params = &OPTMOD_GLB->OPER.PARAMS;
	int32_t err;
	uint8_t is_valid;

	if (OPTMOD->OPER.task != NULL)
	{
		/**
		if((ST_LSR_PREV ==  ST_LASER_OFF_RUN) && (ST_LSR_CURR == ST_IDLE_RUN))
		{

			return APP_ERROR_OK;
		}
        **/
		return OPTMOD_ERROR__ALREADY_RUNNING;
	}



	//if(OPTMOD_GLB == NULL)
	//{

		if(OPTMOD != NULL)
			vPortFree(OPTMOD);
	    OPTMOD = OPTMOD_GLB = NULL;

		  OPTMOD = (OPTMOD_STRUCT_PTR) pvPortMalloc(sizeof(OPTMOD_STRUCT));
		  OPTMOD_GLB = OPTMOD;
		  OPTMOD->SETTINGS_FILE = (IO_HANDLER_PTR)OPTMOD_SETTING_GLB;
		  OPTMOD->OPTIC_DAC = OPTMOD_OPTIC_DAC_GLB;
		  OPTMOD->OPTIC_ADC = OPTMOD_OPTIC_ADC_GLB;
		  ulaser_off_flg = uflgLASER_OFF_ON;
	//}


	err = io_read(OPTMOD->SETTINGS_FILE, (uint8_t*)&params, 1);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	memcpy(&OPTMOD->OPER.PARAMS, params, sizeof(OPTMOD_OPER_PARAMS));

	OPTMOD->OPER.type = OPTMOD_OPER_TYPE__NEW;

	OPTMOD->OPER.STATUS.state = OPTMOD_OPER_STATE__OFF;
	OPTMOD->OPER.STATUS.duration = 0;
	OPTMOD->OPER.STATUS.curr_pwr = 0;
	OPTMOD->OPER.STATUS.curr_error = 0;


  /*
	err = xTaskCreate(_optmod_operation_task, "OPTMOD_OPER_TASK", 512,
			          OPTMOD, osPriorityNormal + 1, &OPTMOD->OPER.task );
	if (err != pdPASS)
	{
		return IO_ERROR_OTBBCOM__OUT_OF_HEAP;
	}
   */

	/* Create the task without using any dynamic memory allocation. */
	OPTMOD->OPER.task = xTaskCreateStatic(
			_optmod_operation_task,       /* Function that implements the task. */
			"OPTMOD_OPER_TASK",   /* Text name for the task. */
			_OPTMOD_OPER_TASK_STASK_SIZE,      /* Number of indexes in the xStack array. */
		     OPTMOD,    /* Parameter passed into the task. */
			 //tskIDLE_PRIORITY + 1 ,/*  was 3 Priority at which the task is created. */
			 osPriorityNormal  + 1 ,
			 _OPTMOD_OPER_TASK_STACK,          /* Array to use as the task's stack. */
			&_OPTMOD_OPER_TASK_BUFFER );  /* Variable to hold the task's data structure. */

	return OPTMOD_ERROR__OK;
}

/****************  TERMINATE OPER., TASK **************************/
static int32_t optmod_operation_terminate(OPTMOD_STRUCT_PTR OPTMOD)
{
	if (OPTMOD->OPER.task != NULL)
	{

		vTaskDelete(OPTMOD->OPER.task);


		 optmod_operation_block_until_done(OPTMOD);

		 return OPTMOD_ERROR__NOK;
	}

	return OPTMOD_ERROR__OK;
}

static void optmod_operation_block_until_done(OPTMOD_STRUCT_PTR OPTMOD)
{

	xTimerDelete(OPTMOD->OPER.oper_task_timer, 0);
	while (eTaskGetState(OPTMOD->OPER.task) != eDeleted)
	 {};

	OPTMOD->OPER.task = NULL;

	OPTMOD->OPER.STATUS.state = OPTMOD_OPER_STATE__OFF;

	ulaser_off_flg = uflgLASER_OFF_OFF;
   /**
	if(OPTMOD != NULL)
		  vPortFree(OPTMOD);
		OPTMOD = OPTMOD_GLB = NULL;
	**/
}

/****************  END OF TERMINATE OPER., TASK **************************/






static int32_t optmod_operation_Suspend(OPTMOD_STRUCT_PTR OPTMOD)
{
	if (OPTMOD->OPER.task != NULL)
	{
		vTaskSuspend(OPTMOD->OPER.task);
	}

	return OPTMOD_ERROR__OK;
}
static int32_t optmod_operation_Resume(OPTMOD_STRUCT_PTR OPTMOD)
{
	if (OPTMOD->OPER.task != NULL)
	{
		vTaskResume(OPTMOD->OPER.task);
	}

	return OPTMOD_ERROR__OK;
}


int32_t optmod_operation_set_old(OPTMOD_STRUCT_PTR OPTMOD, OPTMOD_CALIB_SAMPLE_PTR dot)
{
	OPTMOD->OPER.PARAMS_OLD.DOT_MIDDLE = *dot;

	return OPTMOD_ERROR__OK;
}


static int32_t optmod_operation_set(OPTMOD_STRUCT_PTR OPTMOD)
{


	OPTMOD_OPER_PARAMS_PTR params = &OPTMOD->OPER.PARAMS;
	OPTMOD_OPER_PARAMS_PTR read_params = NULL;
	int32_t err;


	err = io_write(OPTMOD->SETTINGS_FILE, (uint8_t*)&OPTMOD->OPER.PARAMS, sizeof(OPTMOD_OPER_PARAMS));
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	err = io_ioctl(OPTMOD->SETTINGS_FILE, IO_IOCTL_SETTINGS__PULL, NULL);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	err = io_read(OPTMOD->SETTINGS_FILE, (uint8_t*)&params, 1);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	if (read_params != NULL &&
			memcmp(read_params, &OPTMOD->OPER.PARAMS, sizeof(OPTMOD_OPER_PARAMS)))
	{
		return OPTMOD_ERROR__FAILED_WRITING_PARAMS;
	}

	OPTMOD->OPER.operation_start_ticks = xTaskGetTickCount();

	return OPTMOD_ERROR__OK;
}


static int32_t optmod_operation_get_settings(OPTMOD_STRUCT_PTR OPTMOD)
{
	OPTMOD_OPER_PARAMS_PTR params;
	memcpy(params, &OPTMOD->OPER.PARAMS, sizeof(OPTMOD_OPER_PARAMS));
	return OPTMOD_ERROR__OK;
}


void _optmod_calibration_task(void * argument)
{
	OPTMOD_STRUCT_PTR OPTMOD = (OPTMOD_STRUCT_PTR) argument;
	int32_t dac_val;
	uint32_t i = 0;
	int32_t err;

	dac_val = OPTMOD->CALIB.PARAMS.dac_count_lowest;

	while (dac_val < OPTMOD->CALIB.PARAMS.dac_count_highest)
	{
		int32_t adc_sample;

		err = io_write(OPTMOD->OPTIC_DAC, (uint8_t*)&dac_val, 1);
		if (err != IO_ERROR_DAC7311__OK)
		{
			goto _optmod_calibration_task_complete;
		}

		vTaskDelay(OPTMOD->CALIB.PARAMS.interval_ms);

		err = io_read(OPTMOD->OPTIC_ADC, (uint8_t*)&adc_sample, 1);
		if (err != IO_ERROR_OPT_ADC__OK)
		{
			goto _optmod_calibration_task_complete;
		}

		_CALIB_SAMPLES[i].dac = dac_val;
		_CALIB_SAMPLES[i].adc = adc_sample;
		i++;

		if (OPTMOD->OPER.PARAMS_OLD.if_print && OPTMOD->STDOUT != NULL)
		{
			snprintf(buf, 100-1, "Optic modulator calibration ::: DAC=%ld\tADC=%ld\r\n", dac_val, adc_sample);
			io_write(OPTMOD->STDOUT, (uint8_t*)buf, strlen(buf));
		}

		dac_val += OPTMOD->CALIB.PARAMS.dac_count_step;
	}

_optmod_calibration_task_complete:
	OPTMOD->CALIB.samples_count = i;
	optmod_calibration_terminate(OPTMOD);
	return;
}


static int32_t optmod_calibration_start(OPTMOD_STRUCT_PTR OPTMOD)
{

	OPTMOD_CALIB_PARAMS_PTR params;

	if (OPTMOD->CALIB.task != NULL)
	{
		return OPTMOD_ERROR__ALREADY_RUNNING;
	}

	if (params->dac_count_lowest > params->dac_count_highest)
	{
		return OPTMOD_ERROR__INVALID_PARAMS;
	}

	OPTMOD->CALIB.PARAMS = *params;


	/* Create the task without using any dynamic memory allocation. */
	OPTMOD->CALIB.task = xTaskCreateStatic(
			_optmod_calibration_task,       /* Function that implements the task. */
			"OPTMOD_CALIB_TASK",   /* Text name for the task. */
			_OPTMOD_CALIB_TASK_STASK_SIZE,      /* Number of indexes in the xStack array. */
			OPTMOD,    /* Parameter passed into the task. */
			tskIDLE_PRIORITY + 1 ,/* Priority at which the task is created. */
			_OPTMOD_CALIB_TASK_STACK,          /* Array to use as the task's stack. */
			&_OPTMOD_CALIB_TASK_BUFFER );  /* Variable to hold the task's data structure. */

	return OPTMOD_ERROR__OK;
}


static int32_t optmod_calibration_terminate(OPTMOD_STRUCT_PTR OPTMOD)
{
	if (OPTMOD->CALIB.task != NULL)
	{
		vTaskDelete(OPTMOD->CALIB.task);
		OPTMOD->CALIB.task = NULL;
	}

	return OPTMOD_ERROR__OK;
}


static void optmod_calibration_block_until_done(OPTMOD_STRUCT_PTR OPTMOD)
{
	while (eTaskGetState(OPTMOD->CALIB.task) != eDeleted) {}
}


static int32_t optmod_calibration_read_sample(OPTMOD_STRUCT_PTR OPTMOD)
{
	if (OPTMOD->CALIB.index < OPTMOD->CALIB.samples_count)
	{
		OPTMOD->CALIB_SAMPLE.dac = _CALIB_SAMPLES[OPTMOD->CALIB.index].dac;
		OPTMOD->CALIB_SAMPLE.adc = _CALIB_SAMPLES[OPTMOD->CALIB.index].adc;
		return OPTMOD_ERROR__OK;
	}
	else
	{
		return OPTMOD_ERROR__INVALID_PARAMS;
	}
}


static int32_t _optmod_settings_set_default(OPTMOD_STRUCT_PTR OPTMOD)
{
	int32_t err;

	err = io_write(OPTMOD->SETTINGS_FILE, (uint8_t*)&OPTMOD_SETTINGS_DEFAULT, sizeof(OPTMOD_OPER_PARAMS));
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	err = io_ioctl(OPTMOD->SETTINGS_FILE, IO_IOCTL_SETTINGS__PULL, NULL);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	return OPTMOD_ERROR__OK;
}

static OPTMOD_STRUCT_PTR optmod_init_created(OPTMOD_STRUCT_PTR OPTMOD)

{
	OPTMOD_STRUCT_PTR RET_OPTMOD;
	int32_t err;

	/*******************************************************/
	if(OPTMOD != NULL)
		RET_OPTMOD = (OPTMOD_STRUCT_PTR) pvPortMalloc(sizeof(OPTMOD_STRUCT));
	/******************************************************/
	else
		RET_OPTMOD = OPTMOD;
	return (OPTMOD_STRUCT_PTR)RET_OPTMOD;//OPTMOD_ERROR__OK;
}

//int32_t optmod_init(OPTMOD_STRUCT_PTR OPTMOD, IO_HANDLER_PTR OPTIC_ADC, IO_HANDLER_PTR OPTIC_DAC)
static int32_t optmod_init(OPTMOD_STRUCT_PTR OPTMOD)
{
	SETTINGS_INIT_STRUCT settings_init;
	OPTMOD_OPER_PARAMS_PTR params;
	uint8_t is_valid;
	int32_t err;

   /**
	OPTMOD->OPTIC_ADC = (IO_HANDLER_PTR)OPTIC_ADC;
	OPTMOD->OPTIC_DAC = (IO_HANDLER_PTR)OPTIC_DAC;

	OPTMOD_OPTIC_DAC_GLB = (IO_HANDLER_PTR)OPTMOD->OPTIC_DAC;
	OPTMOD_OPTIC_ADC_GLB = (IO_HANDLER_PTR)OPTMOD->OPTIC_ADC;
   **/
	/* Open SETTINGS file */
	settings_init.DRIVER.if_init = 0;
	settings_init.INSTANCE.if_init = 1;
	settings_init.INSTANCE.size_number_pages = 1;
	OPTMOD->SETTINGS_FILE = io_open(IO_HANDLER_TYPE__SETTINGS, &settings_init);
	if (OPTMOD->SETTINGS_FILE == NULL)
	{
		printf("Failed to read RF settings from EEPROM");
		return IO_ERROR_SETTINGS__OUT_OF_HEAP;
	}

	OPTMOD_SETTING_GLB   = (IO_HANDLER_PTR)OPTMOD->SETTINGS_FILE;

	is_valid = 0;
	err = io_ioctl(OPTMOD->SETTINGS_FILE, IO_IOCTL_SETTINGS__IS_FILE_VALID, &is_valid);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	if (!is_valid)
	{
		err = _optmod_settings_set_default(OPTMOD);
		if (err != OPTMOD_ERROR__OK)
		{
			return err;
		}
	}

	err = io_read(OPTMOD->SETTINGS_FILE, (uint8_t*)&params, 1);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	memcpy(&OPTMOD->OPER.PARAMS, params, sizeof(OPTMOD_OPER_PARAMS));

	OPTMOD_GLB = (OPTMOD_STRUCT_PTR)OPTMOD;


	return OPTMOD_ERROR__OK;
}


int32_t optmod_set_stdout(OPTMOD_STRUCT_PTR OPTMOD, IO_HANDLER_PTR STDOUT)
{
	OPTMOD->STDOUT = STDOUT;

	return OPTMOD_ERROR__OK;
}


/***************  VVVVVVVVVVVVVVVVVV OLD ALGORITHM   VVVVVVVVV *************/

OTBA_ST_LSR ST_LSR_CURR = ST_IDLE_RUN, ST_LSR_PREV = ST_IDLE_RUN;
void _optmod_operation_old_task(void * argument)
{
	OPTMOD_STRUCT_PTR OPTMOD = (OPTMOD_STRUCT_PTR) argument;
	OPTMOD_CALIB_SAMPLE_PTR dot0 = &OPTMOD->OPER.PARAMS_OLD.DOT_START;
	OPTMOD_CALIB_SAMPLE_PTR dot1 = &OPTMOD->OPER.PARAMS_OLD.DOT_END;
	OPTMOD_CALIB_SAMPLE_PTR dot_middle = &OPTMOD->OPER.PARAMS_OLD.DOT_MIDDLE;
	float slope = (dot1->adc - dot0->adc) / (float)(dot1->dac - dot0->dac);
	int32_t err;

	OPTMOD->OPER.operation_start_ticks = xTaskGetTickCount();

	// Write initial DAC value
	io_write(OPTMOD->OPTIC_DAC, (uint8_t*)&dot_middle->dac, 1);
	vTaskDelay(OPTMOD->OPER.PARAMS_OLD.interval_ms);

	// Start modulator gain control
	for(;;)
	{
		int32_t adc_sample;
		uint32_t i;

		for (i = 0; i < OPTMOD_OPER__ADC_OVERSAMPLING; i++)
		{
			int32_t temp_sample;
			err = io_read(OPTMOD->OPTIC_ADC, (uint8_t*)&temp_sample, 1);
			if (err != IO_ERROR_OPT_ADC__OK)
			{
				break;
			}

			adc_sample += temp_sample;
		}

		if (err != IO_ERROR_OPT_ADC__OK)
		{
			vTaskDelay(OPTMOD->OPER.PARAMS_OLD.interval_ms);
			continue;
		}

		adc_sample /= OPTMOD_OPER__ADC_OVERSAMPLING;

		float adc_delta = (float)adc_sample - (float)dot_middle->adc;
		int16_t dac_delta = (int16_t)(adc_delta / slope);

		if (dac_delta > OPTMOD_OPER__MAX_DAC_STEP)
		{
			dac_delta = OPTMOD_OPER__MAX_DAC_STEP;
		}
		else if (dac_delta < -OPTMOD_OPER__MAX_DAC_STEP)
		{
			dac_delta = -OPTMOD_OPER__MAX_DAC_STEP;
		}

		DAC7311_SAMPLE dac_new = dot_middle->dac - dac_delta;

		// Apply feedback percentage gain
		dac_new = (dac_new * OPTMOD->OPER.PARAMS_OLD.gain_percent) / 100;

		// Stay in predefined DAC range
		if (dac_new < dot0->dac)
		{
			dac_new = dot0->dac;
		}

		if (dac_new > dot1->dac)
		{
			dac_new = dot1->dac;
		}

		io_write(OPTMOD->OPTIC_DAC, (uint8_t*)&dac_new, 1);

		// Remember as last dac value
		dot_middle->dac = dac_new;

		if (OPTMOD->OPER.PARAMS_OLD.if_print)
		{
			memset(buf, 0, 100);
			snprintf(buf, 100-1, "OPTMOD OPER ::: ADC=%ld; ADC_delta=%ld, DAC_delta=%d, DAC=%d\r\n", adc_sample, (int32_t)adc_delta, (int16_t)dac_delta, dac_new);
			io_write(OPTMOD->STDOUT, (uint8_t*)buf, strlen(buf));
		}

		vTaskDelay(OPTMOD->OPER.PARAMS_OLD.interval_ms);
	}
}


int32_t optmod_operation_start_old(OPTMOD_STRUCT_PTR OPTMOD, OPTMOD_OPER_PARAMS_OLD_PTR params)
{
	if (OPTMOD->OPER.task != NULL)
	{


		return OPTMOD_ERROR__ALREADY_RUNNING;
	}

	OPTMOD->OPER.PARAMS_OLD = *params;
	OPTMOD->OPER.type = OPTMOD_OPER_TYPE__OLD;

	/* Create the task without using any dynamic memory allocation. */
	OPTMOD->OPER.task = xTaskCreateStatic(
			_optmod_operation_old_task,       /* Function that implements the task. */
			"OPTMOD_OPER_TASK",   /* Text name for the task. */
			_OPTMOD_OPER_TASK_STASK_SIZE,      /* Number of indexes in the xStack array. */
			OPTMOD,    /* Parameter passed into the task. */
			tskIDLE_PRIORITY + 1 ,/*  was 3  07.12.2021 Priority at which the task is created. */
			_OPTMOD_OPER_TASK_STACK,          /* Array to use as the task's stack. */
			&_OPTMOD_OPER_TASK_BUFFER );  /* Variable to hold the task's data structure. */

	return OPTMOD_ERROR__OK;
}

/*********^^^^^^^^^^^^^^^^^^^  OLD_ALGORITHM        ^^^^^^^^^^^^^^^***********************/





