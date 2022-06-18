/**
 ******************************************************************************
 * @file    rf.c
 * @brief   RF management algorithm and calibration methods
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
#include <math.h>
#include <assert.h>
#include "UserLib.h"
/*** 21/10/2021 ****/
float  dac_temperature_N1;
#define _RF_TASK_STASK_SIZE		512 //256
StackType_t _RF_TASK_STACK[_RF_TASK_STASK_SIZE];
StaticTask_t _RF_TASK_BUFFER;

static int32_t NUMBER_TABLES = 0;
static int32_t NUMBER_ROWS = 0;

static float TEMP_COLUMN_0 = NAN;
static float TEMP_COLUMN_1 = NAN;
static float TEMP_COLUMN_2 = NAN;

extern SemaphoreHandle_t xSemaphore_RF_READ_DAC_TEMP;

static int32_t _rf_write(IO_HANDLER_PTR handler, uint8_t * parm, uint32_t temp);
static int32_t _rf_read(IO_HANDLER_PTR handler, uint8_t * parm, uint32_t temp );
static int32_t _rf_open(IO_HANDLER_PTR handler, void * init);
static int32_t _rf_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr);


static int32_t rf_start(RF_STRUCT_PTR RF);
static int32_t rf_terminate(RF_STRUCT_PTR RF);
static void rf_block_until_done(RF_STRUCT_PTR RF);
static int32_t rf_init(RF_STRUCT_PTR RF, IO_HANDLER_PTR RF_DAC, IO_HANDLER_PTR TEMP_SENSOR);

static int32_t rf_algo_update(RF_STRUCT_PTR RF, float temp_current);

static int32_t rf_table_get(RF_STRUCT_PTR RF, int16_t *value );
static int32_t rf_table_set(RF_STRUCT_PTR RF, int16_t value );
static int32_t rf_table_read_all(RF_STRUCT_PTR RF,RF_CALIB_TABLE* rf_table);

static int32_t rf_table_calib_start(RF_STRUCT_PTR RF, int32_t table_index);
static int32_t rf_table_calib_finalize(RF_STRUCT_PTR RF, int32_t table_index);

static int32_t rf_table_is_in_calib(RF_STRUCT_PTR RF,uint8_t *bool);

/***************** read temperature ************************/
static float Read_temperature_from_DAC(RF_STRUCT_PTR RF);

int32_t install_rf_func (RF_INIT_STRUCT_PTR RF_Func_init)

{
	RF_STRUCT_PTR driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_rf_open,
											_rf_read,
											_rf_write,
											_rf_ioctl,
											0u
										};
	int32_t err;


	driver_bundle = (RF_STRUCT_PTR) pvPortMalloc(sizeof(RF_STRUCT));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_OTBBCOM__OUT_OF_HEAP;
	}
	RF_STRCT_GLB  = (RF_STRUCT_PTR)driver_bundle;

	driver_bundle->rf_mailbox_RX = NULL;
	driver_bundle->rf_EventGrp = NULL;
	memset(driver_bundle, 0, sizeof(RF_STRUCT));

	init.bundle = driver_bundle;

	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__RF_FUNC, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}

	xSemaphore_RF_READ_DAC_TEMP = xSemaphoreCreateMutex();

	io_ioctl(driver_bundle->RF_DAC, IO_IOCTL_LTC2666__POWER_UP, NULL);
	return 0;
}

static int32_t _rf_open(IO_HANDLER_PTR handler, void * init)
{
	return 0;
}
static int32_t _rf_read(IO_HANDLER_PTR handler, uint8_t * parm, uint32_t temp )
{
	return 0;
}
static int32_t _rf_write(IO_HANDLER_PTR handler, uint8_t * parm, uint32_t temp)
{
	return 0;
}


static int32_t _rf_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	//OPTMOD_STRUCT_PTR driver_bundle = (OPTMOD_STRUCT_PTR) handler->bundle;
	RF_STRUCT_PTR driver_bundle = (RF_STRUCT_PTR)RF_STRCT_GLB;
	uint16_t err = IO_ERROR_OPTMOD_OK;
	uint32_t table_index;
	int16_t value;
	float *ret_temprt_value,ftemp;

	switch (ctl)
	{
	  case IO_IOCTL_RF_EXAMINE:
		  //handler->bundle = NULL;
		  break;


	  case IO_IOCTL_RF_OPER_START_RE_CREATED:
		       //handler->bundle = driver_bundle->SETTINGS_FILE;
		  err = rf_start(driver_bundle);
	  break;

	  case IO_IOCTL_RF_OPER_TERMNT_RE_DELETED:
		  err = rf_terminate(driver_bundle);

	  break;

	  case IO_IOCTL_RF_SET_TABLE:
		  value = *(int16_t *)ptr;
		  err = rf_table_set(driver_bundle,  value);
	  break;

	  case IO_IOCTL_RF_GET_TABLE:
		  err = rf_table_get(driver_bundle,  ptr);
		  //ptr = (int16_t *)value;
	  break;
	  case IO_IOCTL_RF_READ_TABLE_ALL:         /*(driver_bundle->_RF_TABLE_STR.rf_table)*/
		  err = rf_table_read_all(driver_bundle,ptr);
		  break;



	  case IO_IOCTL_RF_TABLE_CALIB_START:
	  		 table_index = *(uint32_t *)ptr;
	  		 err =   rf_table_calib_start(driver_bundle, table_index);
	  break;

	  case 	  IO_IOCTL_RF_TABLE_CALIB_FINAL:
		  err = rf_table_calib_finalize(driver_bundle,0);
	  break;
	  case IO_IOCTL_RF_TABLE_IS_IN_CALIB:
		  err = rf_table_is_in_calib(driver_bundle,ptr);
	  break;
	  case IO_IOCTL_RF_UPDATE_TABLE:

	  break;

	  case IO_IOCTL_RF_DAC_HANDLER:
		  RF_DAC_GLB = (IO_HANDLER_PTR)ptr;
	  break;

	  case IO_IOCTL_TEMP_SENSOR_HANDLER:
		  TEMP_SENSOR_GLB = (IO_HANDLER_PTR)ptr;
	  break;

	  case IO_IOCTL_RF_INIT_HANDLER:
		  err = rf_init(driver_bundle,(IO_HANDLER_PTR)RF_DAC_GLB,(IO_HANDLER_PTR)TEMP_SENSOR_GLB);
	  break;

	  case IO_IOCTL_RF_READ_TEMP_DAC:
		  xSemaphoreTake( xSemaphore_RF_READ_DAC_TEMP, ( TickType_t ) pdMS_TO_TICKS (1));
		  ftemp = Read_temperature_from_DAC(driver_bundle);
		  xSemaphoreGive( xSemaphore_RF_READ_DAC_TEMP);
		  ret_temprt_value = &ftemp;//test
		  ptr = (float *)ret_temprt_value;//test
		  ftemp = *(float *)ptr;//test
		  return (int32_t)ftemp;
	  break;
	  default:
		err = RF_ERROR__NOK;
	  break;
	}

	return err;
}


static int32_t rf_algo_update(RF_STRUCT_PTR RF, float temp_current)
{
	int32_t err;

	int32_t row, column_low, column_high;
	float temp_low, temp_high;
	RF_TABLE_STRUCT *table_struct = NULL;
	LTC2666_SAMPLE dac_sample;
	EventBits_t rf_Bits;

	if (RF->RF_DAC == NULL)
	{
		return RF_ERROR__DAC_MISSING;
	}

	if (temp_current >= TEMP_COLUMN_0 && temp_current < TEMP_COLUMN_1)
	{
		column_low = 0;
		column_high = 1;
		temp_low = TEMP_COLUMN_0;
		temp_high = TEMP_COLUMN_1;
	}
	else if (temp_current >= TEMP_COLUMN_1 && temp_current <= TEMP_COLUMN_2)
	{
		column_low = 1;
		column_high = 2;
		temp_low = TEMP_COLUMN_1;
		temp_high = TEMP_COLUMN_2;
	}
	else
	{
		return RF_ERROR__INVALID_VALUE;
	}

	///xQueuePeek(RF_STRCT_GLB->rf_mailbox_RX, dac_sample.DAC_Shadow, 0);//delete buffer
	// TODO - decide with which table to work
	err = io_read(RF->TABLE_FILE[0], (uint8_t*)&table_struct, 1);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return RF_ERROR__INVALID_VALUE;
	}

	///...io_ioctl(RF->RF_DAC, IO_IOCTL_LTC2666__POWER_UP, NULL);  //LTC2666__POWER_UP
	HAL_GPIO_WritePin(DAC_LDACn_GPIO_Port, DAC_LDACn_Pin, GPIO_PIN_SET);
	for (row = 0; row < NUMBER_ROWS; row++)
	{
		//LTC2666_SAMPLE dac_sample;
		int16_t val_low = (table_struct->rf_table)[row][column_low];
		int16_t val_high = (table_struct->rf_table)[row][column_high];
		float slope = ((float)(val_high - val_low)) / (temp_high - temp_low);
		int32_t write_err;


		dac_sample.CH = DAC_CHANNEL_LUT[row];
		dac_sample.VAL_MV = val_low + (int16_t)(slope * (temp_current - temp_low));

		dac_sample.DAC_Shadow[row]=dac_sample.VAL_MV;

		write_err = io_write(RF->RF_DAC, (uint8_t*)&dac_sample, 1);
		if (write_err != IO_ERROR_LTC2666__OK)
		{
			err = write_err;
		}
	}
	HAL_GPIO_WritePin(DAC_LDACn_GPIO_Port, DAC_LDACn_Pin, GPIO_PIN_RESET);
	xQueueOverwrite(RF->rf_mailbox_RX, dac_sample.DAC_Shadow);
	rf_Bits = xEventGroupSetBits(RF->rf_EventGrp,0x5A);
	if(rf_Bits != 0x5A)
	{
		return RF_ERROR__INVALID_VALUE;
	}

	return err;
}

/******   RF TASK  ******/
void _rf_task(void * argument)
{
	RF_STRUCT_PTR RF = (RF_STRUCT_PTR) argument;
	int32_t err;

	for (;;)
	{
		static float temperature = 0.0f;

		err = io_read(RF->TEMP_SENSOR, (uint8_t*)&temperature, 1);
		if (err != IO_ERROR_CPU_TEMP__OK)
		{
			goto rf_task_continue;
		}
		//xSemaphoreTake( xSemaphore_RF_ALGO_UPDATE, ( TickType_t ) pdMS_TO_TICKS (2));

		RF->dac_temperature_COUNTER++;
		xSemaphoreTake( xSemaphore_RF_READ_DAC_TEMP, ( TickType_t ) pdMS_TO_TICKS (1));
		temperature = Read_temperature_from_DAC(RF);
		xSemaphoreGive( xSemaphore_RF_READ_DAC_TEMP);
		temperature += 7.0;
		if (RF->dac_temperature_COUNTER >= 10)
		{
			RF->dac_temperature_COUNTER = 0;
			rf_algo_update(RF, temperature);
		}
		//xSemaphoreGive( xSemaphore_RF_ALGO_UPDATE );
		//rf_algo_update(RF, temperature);

       rf_task_continue:
		vTaskDelay(RF_ALGO_INTERVAL_MS);
		continue;
	}
}
/******   END OF RF TASK  ******/

static int32_t rf_start(RF_STRUCT_PTR RF)
{
	//uint8_t i;
	LTC2666_SAMPLE dac_sample;
	if (RF->oper_task != NULL)
	{
		return RF_ERROR__ALREADY_RUNNING;
	}
	if (RF->RF_DAC == NULL)
		{
			return RF_ERROR__DAC_MISSING;
		}

	if(RF->TABLE_FILE != NULL)
			vPortFree(RF->TABLE_FILE);
		RF->TABLE_FILE = NULL;

	if(RF != NULL)
	{
      if(RF->rf_mailbox_RX != NULL)
		vQueueDelete( RF->rf_mailbox_RX);
      if(RF->rf_EventGrp != NULL)
		vEventGroupDelete(RF->rf_EventGrp);

	    vPortFree(RF);

	}
	 RF = RF_STRCT_GLB = NULL;



	 RF = (RF_STRUCT_PTR) pvPortMalloc(sizeof(RF_STRUCT));
	 RF_STRCT_GLB = RF;

	 RF->RF_DAC = (IO_HANDLER_PTR)RF_DAC_GLB;
	 RF->TEMP_SENSOR = (IO_HANDLER_PTR)TEMP_SENSOR_GLB;

	 RF->dac_temperature_COUNTER = 0;

	 rf_init(RF, RF->RF_DAC, RF->TEMP_SENSOR);

	 ///for (i = 0; i < NUMBER_TABLES; i++)
	 /// {
		/// RF->TABLE_FILE[i] = TABLE_FILE_GLB[i];
	 ////// }
	 //RF->SETTINGS_FILE = (IO_HANDLER_PTR)OPTMOD_SETTING_GLB;
	 RF->rf_mailbox_RX = xQueueCreate(1, sizeof(dac_sample.DAC_Shadow));
	 RF->rf_EventGrp   = xEventGroupCreate();
	 /* Create the task without using any dynamic memory allocation. */
	 RF->oper_task = xTaskCreateStatic(
			_rf_task,       /* Function that implements the task. */
			"RF_TASK",   /* Text name for the task. */
			_RF_TASK_STASK_SIZE,      /* Number of indexes in the xStack array. */
			 RF,    /* Parameter passed into the task. */
			 //tskIDLE_PRIORITY + 1 ,/* Priority at which the task is created. */
			 osPriorityNormal  + 1 ,
			 _RF_TASK_STACK,          /* Array to use as the task's stack. */
			&_RF_TASK_BUFFER );  /* Variable to hold the task's data structure. */

	return RF_ERROR__OK;
}


static int32_t rf_terminate(RF_STRUCT_PTR RF)
{
	if (RF->oper_task != NULL)
	{
		vTaskDelete(RF->oper_task);
		rf_block_until_done( RF);

	}



	return RF_ERROR__OK;
}


static void rf_block_until_done(RF_STRUCT_PTR RF)
{


	while (eTaskGetState(RF->oper_task) != eDeleted)
	 {}
	RF->oper_task = NULL;
}

/**  get table value ****/

static int32_t rf_table_get(RF_STRUCT_PTR RF, int16_t *value )/// probably replace to void
{
	//RF_TABLE_STRUCT *table_struct = NULL;
	int32_t err;
	int32_t table_index = RF->table_index;
	int32_t row_index   = RF->row_index;
	int32_t column_index = RF->column_index;

	if (table_index < 0 || table_index >= NUMBER_TABLES ||
		row_index < 0 || row_index >= NUMBER_ROWS ||
		column_index < 0 || column_index >= RF_TABLE_NUMBER_COLUMNS)
	{
		return RF_ERROR__INVALID_RF_TABLE_INDEX;
	}

	err = io_read(RF->TABLE_FILE[table_index], (uint8_t*)&RF->_RF_TABLE_STR, 1);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	*value = (RF->_RF_TABLE_STR.rf_table)[row_index][column_index];

	return RF_ERROR__OK;
}

/**  set  table value ****/

static int32_t rf_table_set(RF_STRUCT_PTR RF, int16_t value )
{
	//RF_TABLE_STRUCT table_struct, *table_struct_ptr;
	int32_t err;

	int32_t table_index = RF->table_index;
		int32_t row_index   = RF->row_index;
		int32_t column_index = RF->column_index;

	if (table_index < 0 || row_index < 0 || column_index < 0)
	{
		return RF_ERROR__INVALID_RF_TABLE_INDEX;
	}

	err = io_read(RF->TABLE_FILE[table_index], (uint8_t*)&RF->_RF_TABLE_STR, 1);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	if (!RF->_RF_TABLE_STR.start_calib)
	{
		return RF_ERROR__NOT_IN_CALIB_MODE;
	}

	//memcpy(&table_struct, table_struct_ptr, sizeof(RF_TABLE_STRUCT)); ///.....????
	RF->_RF_TABLE_STR.rf_table[row_index][column_index] = value;

	return io_write(RF->TABLE_FILE[table_index], (uint8_t*)&RF->_RF_TABLE_STR, sizeof(RF_TABLE_STRUCT));
}


static int32_t rf_table_calib_start(RF_STRUCT_PTR RF, int32_t table_index)
{
	RF_TABLE_STRUCT table_struct, *table_struct_ptr = NULL;
	int32_t err;

	table_index = RF->table_index;

	if (table_index < 0 || table_index >= NUMBER_TABLES)
	{
		return RF_ERROR__INVALID_RF_TABLE_INDEX;
	}

	err = io_read(RF->TABLE_FILE[table_index], (uint8_t*)&table_struct_ptr, 1);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	memcpy(&table_struct, table_struct_ptr, sizeof(RF_TABLE_STRUCT));
	table_struct.start_calib = 1;

	return io_write(RF->TABLE_FILE[table_index], (uint8_t*)&table_struct, sizeof(RF_TABLE_STRUCT));
}


static int32_t rf_table_calib_finalize(RF_STRUCT_PTR RF, int32_t table_index)
{
	RF_TABLE_STRUCT table_struct, *table_struct_ptr = NULL;
	int32_t err;

	if (table_index < 0 || table_index >= NUMBER_TABLES)
	{
		return RF_ERROR__INVALID_RF_TABLE_INDEX;
	}

	err = io_read(RF->TABLE_FILE[table_index], (uint8_t*)&table_struct_ptr, 1);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	memcpy(&table_struct, table_struct_ptr, sizeof(RF_TABLE_STRUCT));
	table_struct.start_calib = 0;

	return io_write(RF->TABLE_FILE[table_index], (uint8_t*)&table_struct, sizeof(RF_TABLE_STRUCT));
}




static int32_t rf_table_is_in_calib(RF_STRUCT_PTR RF, uint8_t *bool)

{
	RF_TABLE_STRUCT *table_struct = NULL;
	int32_t err;
	int32_t table_index = RF->table_index = 0;

	if (table_index < 0 || table_index >= NUMBER_TABLES)
	{
		return RF_ERROR__INVALID_RF_TABLE_INDEX;
	}

	err = io_read(RF->TABLE_FILE[table_index], (uint8_t*)&table_struct, 1);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	*bool = table_struct->start_calib;

	return RF_ERROR__OK;
}


static int32_t rf_table_read_all(RF_STRUCT_PTR RF,RF_CALIB_TABLE* rf_table)

{
	RF_TABLE_STRUCT *table_struct = NULL;
	int32_t err;
	int32_t table_index = RF->table_index;

	if (table_index < 0 || table_index >= NUMBER_TABLES)
	{
		return RF_ERROR__INVALID_RF_TABLE_INDEX;
	}

	err = io_read(RF->TABLE_FILE[table_index], (uint8_t*)&table_struct, 1);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	memcpy(rf_table, table_struct->rf_table, sizeof(RF_CALIB_TABLE));

	return RF_ERROR__OK;
}



static int32_t _rf_table_init(RF_STRUCT_PTR RF)
{
	SETTINGS_INIT_STRUCT settings_init;
	uint8_t is_valid = 1, temp;
	uint32_t i;
	int32_t err;

	/* Open RF SETTINGS file */
	settings_init.DRIVER.if_init = 0;
	settings_init.INSTANCE.if_init = 1;
	settings_init.INSTANCE.size_number_pages = 1;

	for (i = 0; i < NUMBER_TABLES + 2; i++)
	{
		RF->TABLE_FILE[i] = io_open(IO_HANDLER_TYPE__SETTINGS, &settings_init);
		if (RF->TABLE_FILE[i] == NULL)
		{
			printf("Failed to read RF settings from EEPROM");
			return IO_ERROR_SETTINGS__OUT_OF_HEAP;
		}

		temp = 0;
		err = io_ioctl(RF->TABLE_FILE[i], IO_IOCTL_SETTINGS__IS_FILE_VALID, &temp);
		if (err != IO_ERROR_SETTINGS__OK)
		{
			return err;
		}


		TABLE_FILE_GLB[i] = RF->TABLE_FILE[i];
		is_valid = is_valid & temp;
	}

	if (!is_valid)
	{
		err = app_rf_table_set_default(RF);
		if (err != IO_ERROR_SETTINGS__OK)
		{
			return err;
		}
	}

	return RF_ERROR__OK;
}

/* Override in app rf module */
__weak int32_t app_rf_table_set_default(RF_STRUCT_PTR RF)
{
	UNUSED(RF);
	return RF_ERROR__OK;
}

__weak float app_rf_get_temp_column_value_0(void)
{
	return NAN;
}


__weak float app_rf_get_temp_column_value_1(void)
{
	return NAN;
}


__weak float app_rf_get_temp_column_value_2(void)
{
	return NAN;
}


__weak int32_t app_rf_get_number_of_tables(void)
{
	return 0;
}


__weak int32_t app_rf_get_number_of_rows(void)
{
	return 0;
}




static int32_t rf_init(RF_STRUCT_PTR RF, IO_HANDLER_PTR RF_DAC, IO_HANDLER_PTR TEMP_SENSOR)
{
	RF->RF_DAC = RF_DAC;
	RF->TEMP_SENSOR = TEMP_SENSOR;

	NUMBER_TABLES = app_rf_get_number_of_tables();
	///assert(NUMBER_TABLES > 0);

	NUMBER_ROWS = app_rf_get_number_of_rows();
	///assert(NUMBER_ROWS > 0);

	TEMP_COLUMN_0 = app_rf_get_temp_column_value_0();
	TEMP_COLUMN_1 = app_rf_get_temp_column_value_1();
	TEMP_COLUMN_2 = app_rf_get_temp_column_value_2();
	////assert(!isnanf(TEMP_COLUMN_0));
	////assert(!isnanf(TEMP_COLUMN_1));
	////assert(!isnanf(TEMP_COLUMN_2));

	RF->TABLE_FILE = (IO_HANDLER_PTR*) pvPortMalloc(sizeof(IO_HANDLER_PTR) * NUMBER_TABLES);
	if (NULL == RF->TABLE_FILE)
	{
		return IO_ERROR_CPU_TEMP__OUT_OF_HEAP;
	}
	TABLE_FILE_GLB = RF->TABLE_FILE;
	memset(RF->TABLE_FILE, 0, sizeof(IO_HANDLER_PTR) * NUMBER_TABLES);

	return _rf_table_init(RF);
}


//temperature can be calculated as TJ = 25?C + (1.4V ?VMUXOUT)/(3.7mV/?C).
//*************************************************************************************************************************************
static float Read_temperature_from_DAC(RF_STRUCT_PTR RF)
{
extern	ADC_HandleTypeDef hadc2;
	float temperature_Volt;
	float  dac_temperature=9999;
	uint32_t ADC_temperature;
	if (RF->RF_DAC == NULL)
	{
		return dac_temperature;
	}
	//...io_ioctl(RF->RF_DAC, IO_IOCTL_LTC2666__POWER_UP, NULL);//LTC2666__POWER_UP
	io_ioctl(RF->RF_DAC, IO_IOCTL_LTC2666__SET_MUX, NULL);
	HAL_ADC_Start(&hadc2);
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2,10);
	ADC_temperature=HAL_ADC_GetValue(&hadc2);
	temperature_Volt=((float)ADC_temperature)*3.3/4095;
	dac_temperature=(25.0+((1.4 -temperature_Volt)/0.0037))*0.1;

	//dac_temperature=dac_temperature_N1*0.9+dac_temperature*0.1;//....?????.
	//dac_temperature_N1=dac_temperature;
	return dac_temperature;
	//....?????y[t] = a*y[t-1] + b*x[t] should be  read the previous value
}
