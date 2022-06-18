/**
 ******************************************************************************
 * @file    app_cli_handler.c
 * @brief   OTS CLI handler.
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
#include <stdlib.h>
#include "cmsis_os.h"
#include "UserLib.h"
#include "app.h"

#include "stm32f7xx_hal_cortex.h"
#include "netif/ethernet.h"
#include "lwip.h"
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "ethernetif.h"


extern uint8_t CBIT_OPERATE_state;
#define RETifERROR {if (err != IO_ERROR_CLI__OK){return err;}}
extern float  dac_temperature;
#define FLASH_GO_TO_BOOTLOADER_VALUE (0x0)
/* Function declarations */
static int32_t cli_handle_command__version(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler);
static int32_t cli_handle_command__print_commands(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler);
static int32_t cli_handle_command__fpga(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler);
static int32_t cli_handle_command__cpu_temp(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler);
static int32_t cli_handle_command__set_temp(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler);
static int32_t cli_handle_command__rf_table(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler);
static int32_t cli_handle_command__rf(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler);
static int32_t cli_handle_command__optics(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler);
static void cli_handle_command_rf_table_print(IO_HANDLER_PTR output_handler, RF_CALIB_TABLE rf_table);
static int32_t cli_handle_command__LD(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler);
void cli_handle_command_read_DACs(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler);
static int32_t cli_handle_command_EMAC_ADDRESS(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler);
static int32_t cli_handle_command__bootloader(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler);

struct netif gnetif;
SemaphoreHandle_t xSemaphore_RF_ALGO_UPDATE = NULL;
extern MAC_ADD_STRUCT MAC_ADD;
/* Global parameters */

typedef int32_t (*cli_handler)(OTS_APPLICATION_BUNDLE_PTR, IO_HANDLER_PTR, IO_HANDLER_PTR);

typedef struct {
	const char	*command;
	cli_handler	handler;
	const char	*description;
} CLI_COMMAND, * CLI_COMMAND_PTR;

//{	"EMAC", cli_handle_command_EMAC_ADDRESS,	    "<write/<read>"},
static const CLI_COMMAND cli_commands[] = {
//			Command			Handler									Description
		{	"?",			cli_handle_command__print_commands, 	""},
		{	"ld",			cli_handle_command__LD,			"Prints laser ADCs"},
		{	"version",		cli_handle_command__version,			"Prints application version"},
		{	"fpga",			cli_handle_command__fpga,				"FPGA commands | fpga <read/write> <register(decimal)> <value(hexa)>"},
		{	"cpu-temp",		cli_handle_command__cpu_temp,			"CPU temperature"},
		{	"set-temp",		cli_handle_command__set_temp,			"Set start / stop cooling temperature | set-temp <start/stop> <temperature>"},
		{	"rf-table",		cli_handle_command__rf_table,			"RF tables commands | rf-table <read/read-all/write/write-many/calib-start/calib-stop/is-in-calib> <row> <temperature> <value(mV)>"},
		{	"rf",			cli_handle_command__rf,					"RF commands"},
		{	"optics",		cli_handle_command__optics,				"Optical modulator commands"},
		{	"dac",		    cli_handle_command_read_DACs,				"read_DACs"},
		{	"EMAC", cli_handle_command_EMAC_ADDRESS,	    "<write/<read>"},
		{   "bootloader",   cli_handle_command__bootloader,  		"Restart In Bootloader mode" }
};

static const uint32_t cli_number_commands = sizeof(cli_commands) / sizeof(CLI_COMMAND);
const char nl[] = "\r\n";

static uint32_t is_cursor_printed = 0u;
static const char cli_cursor[] = "> ";

static char buf[100];

/* Function definitions */

static int32_t cli_handle_command__bootloader(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler)
{


	FLASH_OBProgramInitTypeDef ob = {0,};
	ob.OptionType = OPTIONBYTE_BOOTADDR_1;
	ob.BootAddr1 = FLASH_GO_TO_BOOTLOADER_VALUE;

	/* unlock flash and option bytes */
	if(HAL_FLASH_Unlock() != HAL_OK)
	{
		return 1;
	}
	if(HAL_FLASH_OB_Unlock() != HAL_OK)
	{
		return 1;
	}

	/* program user address option byte */
	if(HAL_FLASHEx_OBProgram(&ob) != HAL_OK)
	{
		return 1;
	}

	/* launch configuration - may result in system reset */
	if(HAL_FLASH_OB_Launch() != HAL_OK)
	{
		return 1;
	}

	/* locking back fash and option bytes */
	if(HAL_FLASH_OB_Lock() != HAL_OK)
	{
		return 1;
	}
	if(HAL_FLASH_Lock() != HAL_OK)
	{
		return 1;
	}
 snprintf(buf, 100-1, "OTBa reset and boot  from IP 192.168.3.20\n\r");

	io_write(output_handler, (uint8_t*)buf, strlen(buf));
	osDelay(1000);
	/* performing reset */
	NVIC_SystemReset ();
}


static int32_t cli_handle_command__rf_table(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler)
{
	char *params;
	int32_t err;
	char* row_name;
	char* column_name;
	int16_t value;
	uint8_t bool;
	//RF_CALIB_TABLE rf_table;

	RF_STRUCT_PTR   RF_PTR = (RF_STRUCT_PTR)RF_STRCT_GLB;
			//rf_table = RF_STRCT_GLB->_RF_TABLE_STR;

	err = io_read(cli_handler, (uint8_t*)&params, 1);
	if (err != IO_ERROR_CLI__OK)
	{
		return err;
	}

	if (strcmp(params, "write") == 0)
	{
		// read name of row
		err = io_read(cli_handler, (uint8_t*)&row_name, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		// read name of col (temp)
		err = io_read(cli_handler, (uint8_t*)&column_name, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		// read value
		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		value = (int16_t)strtol(params, NULL, 10);

		// convert vars to upper case
		for(int i=0;i<strlen(row_name);i++)
			row_name[i] = toupper(row_name[i]);
		for(int i=0;i<strlen(column_name);i++)
			column_name[i] = toupper(column_name[i]);

		//err = rf_table_set(&OTS_APP->RF, 0,
				//app_rf_table_find_row(row_name),
				//app_rf_table_find_column(column_name), value);
		RF_PTR->table_index = 0;
		RF_PTR->row_index = app_rf_table_find_row(row_name);
		RF_PTR->column_index = app_rf_table_find_column(column_name);
		err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_SET_TABLE,&value);

		if(err == APP_ERROR_OK)
		{
			snprintf(buf, 100-1, "The value of %s in temp: %s was set to %dmV\n\r", row_name, column_name, value);
		}
		else if (err == RF_ERROR__NOT_IN_CALIB_MODE)
		{
			snprintf(buf, 100-1, "RF table not in calibration mode. To start calibration run: rf-table calib-start\n\r");
		}
		else
		{
			 snprintf(buf, 100-1, "Invalid RF table index\n\r");
		}

		io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else if (strcmp(params, "write-many") == 0)
	{
		int16_t values[RF_TABLE_NUMBER_ROWS];

		// read name of col (temp)
		err = io_read(cli_handler, (uint8_t*)&column_name, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		for(int i=0;i<RF_TABLE_NUMBER_ROWS;i++)
		{
			// read value
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}

			values[i] = (int16_t)strtol(params, NULL, 10);
		}

		for(int i=0;i<RF_TABLE_NUMBER_ROWS;i++)
		{
			if(i%2 == 0)
			{
				if(values[i] < -2000 || values[i] > 0)
				{
					snprintf(buf, 100-1, "Value no. %d out of range [-2000..0]\n\r", i + 1);
					return io_write(output_handler, (uint8_t*)buf, strlen(buf));
				}
			}
			else
			{
				if(values[i] < -2000 || values[i] > 2000)
				{
					snprintf(buf, 100-1, "Value no. %d out of range [-2000..2000]\n\r", i + 1);
					return io_write(output_handler, (uint8_t*)buf, strlen(buf));
				}
			}
		}

		for(int i=0;i<RF_TABLE_NUMBER_ROWS;i++)
		{
			row_name = (char*)RF_TABLE_ROW_LUT[i];

			//err = rf_table_set(&OTS_APP->RF, 0,
					//app_rf_table_find_row(row_name),
					//app_rf_table_find_column(column_name), values[i]);

			RF_PTR->table_index = 0;
			RF_PTR->row_index = app_rf_table_find_row(row_name);
			RF_PTR->column_index = app_rf_table_find_column(column_name);
			err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_SET_TABLE,&values[i]);
			if (err == APP_ERROR_OK)
			{
				snprintf(buf, 100-1, "The value of %s in temp: %s was set to %dmV\n\r", row_name, column_name, values[i]);
			}
			else if (err == RF_ERROR__NOT_IN_CALIB_MODE)
			{
				snprintf(buf, 100-1, "RF table not in calibration mode. To start calibration run: rf-table calib-start\n\r");
			}
			else
			{
				 snprintf(buf, 100-1, "Invalid RF table index\n\r");
			}

			io_write(output_handler, (uint8_t*)buf, strlen(buf));
		}
	}
	else if (strcmp(params, "read") == 0)
	{
		// read name of row
		err = io_read(cli_handler, (uint8_t*)&row_name, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		// read name of col (temp)
		err = io_read(cli_handler, (uint8_t*)&column_name, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		// convert vars to upper case
		for(int i=0;i<strlen(row_name);i++)
			row_name[i] = toupper(row_name[i]);
		for(int i=0;i<strlen(column_name);i++)
			column_name[i] = toupper(column_name[i]);

		value = 0;

		//err = rf_table_get(&OTS_APP->RF, 0,
				//app_rf_table_find_row(row_name),
				//app_rf_table_find_column(column_name), &value);

		RF_PTR->table_index = 0;
		RF_PTR->row_index = app_rf_table_find_row(row_name);
		RF_PTR->column_index = app_rf_table_find_column(column_name);

		err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_GET_TABLE,&value);

		snprintf(buf, 100-1, "The value of %s in temp: %s is %d mV\n\r", row_name, column_name, value);

		if(err != APP_ERROR_OK)
			snprintf(buf, 100-1, "Invalid RF table index\n\r");

		io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else if (strcmp(params, "read-all") == 0)
	{
		///...err = rf_table_read_all(&OTS_APP->RF, 0,&RF_PTR->_RF_TABLE_STR.rf_table );//&rf_table);//RF_PTR->_RF_TABLE_STR.rf_table
		err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_READ_TABLE_ALL,&RF_PTR->_RF_TABLE_STR.rf_table);
		if(err != APP_ERROR_OK)
		{
			snprintf(buf, 100-1, "Failed! Error code=0x%lx\n\r", err);
			io_write(output_handler, (uint8_t*)buf, strlen(buf));
		}
		else
		{
			cli_handle_command_rf_table_print(output_handler,RF_PTR->_RF_TABLE_STR.rf_table );//;rf_table);
		}

	}
	else if (strcmp(params, "calib-start") == 0)
	{


		//////err = rf_table_calib_start(&OTS_APP->RF, 0);


		RF_PTR->table_index = 0;

		err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_TABLE_CALIB_START,&RF_PTR->table_index);

		if(err == APP_ERROR_OK)
		{
			snprintf(buf, 100-1, "RF table is now in calibration mode\n\r");
		}
		else
		{
			snprintf(buf, 100-1, "Invalid RF table calib set\n\r");
		}

		io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else if (strcmp(params, "calib-stop") == 0)
	{

		//err = rf_table_calib_finalize(&OTS_APP->RF, 0);


		RF_PTR->table_index = 0;

		err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_TABLE_CALIB_FINAL,&RF_PTR->table_index);

		if(err == APP_ERROR_OK)
		{
			snprintf(buf, 100-1, "RF table calibration finalized\n\r");
		}
		else
		{
			snprintf(buf, 100-1, "Invalid RF table calib stop\n\r");
		}

		io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else if (strcmp(params, "is-in-calib") == 0)
	{
		//err = rf_table_is_in_calib(&OTS_APP->RF, 0, &bool);

		//RF_PTR->table_index = 0;

		err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_TABLE_IS_IN_CALIB,&bool);

		if(err == APP_ERROR_OK)
		{
			snprintf(buf, 100-1, "RF table is %s calibration mode\n\r", bool==0 ? "not in" : "in");
		}
		else
		{
			snprintf(buf, 100-1, "Invalid RF table read\n\r\n");
		}

		io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else
	{
		snprintf(buf, 100-1, "Bad Command...\n\r\n");
			io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}

	return APP_ERROR_OK;
}


static int32_t cli_handle_command__rf(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler)
{
	char *params, *channel;
	int32_t err;

	err = io_read(cli_handler, (uint8_t*)&params, 1);
	if (err != IO_ERROR_CLI__OK)
	{
		return err;
	}

	else if (!strcmp(params, "power"))
	{
		char *device;
		GPIO_PinState onoff;

		// read device name all/3/5
		err = io_read(cli_handler, (uint8_t*)&device, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		// read ON/OFF
		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		if (!strcmp(strupr(params), "ON"))
		{
			onoff = GPIO_PIN_SET;
		}
		else if (!strcmp(strupr(params), "OFF"))
		{
			onoff = GPIO_PIN_RESET;
		}
		else
		{
			snprintf(buf, 100-1, "Invalid ON/OFF designator\r\n\n");
			io_write(output_handler, (uint8_t*)buf, strlen(buf));
			return IO_ERROR_CLI__OK;
		}

		if (!strcmp(device, "all"))
		{
			if (OTS_APP->DRIVERS.RF_DAC == NULL)
			{
				return IO_ERROR_CLI__OK;
			}

			io_ioctl(OTS_APP->DRIVERS.RF_DAC, IO_IOCTL_LTC2666__POWER_DOWN, NULL);

			HAL_GPIO_WritePin(EN_3_3_GPIO_Port, EN_3_3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(EN_5_GPIO_Port, EN_5_Pin, GPIO_PIN_RESET);

			snprintf(buf, 100-1, "DAC, 3.3V and 5V are off...\r\n\n");
			io_write(output_handler, (uint8_t*)buf, strlen(buf));
		}
		else if (!strcmp(device, "3"))
		{
			HAL_GPIO_WritePin(EN_3_3_GPIO_Port, EN_3_3_Pin, onoff);
			if (onoff)
			{
				snprintf(buf, 100-1, "3.3V is on...\r\n\n");
			}
			else
			{
				snprintf(buf, 100-1, "3.3V is off...\r\n\n");
			}
			io_write(output_handler, (uint8_t*)buf, strlen(buf));
		}
		else if (!strcmp(device, "5"))
		{
			HAL_GPIO_WritePin(EN_5_GPIO_Port, EN_5_Pin, onoff);
			if (onoff)
			{
				snprintf(buf, 100-1, "5V is on...\r\n\n");
			}
			else
			{
				snprintf(buf, 100-1, "5V is off...\r\n\n");
			}
			io_write(output_handler, (uint8_t*)buf, strlen(buf));
		}
		else
		{
			snprintf(buf, 100-1, "Invalid all/3/5 RF device\r\n\n");
			io_write(output_handler, (uint8_t*)buf, strlen(buf));
		}
	}
	else if (!strcmp(params, "start-algo"))
	{
		// START RF OPERATION ALGORITHM

		//err = rf_start(&OTS_APP->RF);
		err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_OPER_START_RE_CREATED,NULL);
		if (err != RF_ERROR__OK)
		{
			snprintf(buf, 100-1, "RF Algo start Failed\r\n\n");
			return err;
		}
		else
		{
			snprintf(buf, 100-1, "RF Algo started...\r\n\n");
		}

		io_write(output_handler, (uint8_t*)buf, strlen(buf));

	}
	else if (!strcmp(params, "stop-algo"))
	{
		// STOP RF OPERATION ALGORITHM

		//err = rf_terminate(&OTS_APP->RF);
		err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_OPER_TERMNT_RE_DELETED,NULL);
		if (err != RF_ERROR__OK)
		{
			snprintf(buf, 100-1, "RF Algo stop Failed\r\n\n");
			return err;
		}
		else
		{
			snprintf(buf, 100-1, "RF Algo stopped...\r\n\n");
		}

		io_write(output_handler, (uint8_t*)buf, strlen(buf));

	}
	else if (!strcmp(params, "man-algo"))
	{
		// START MANUAL RF OPERATION ALGORITHM
		char* str_temperature;
		float flt_temperature;

		err = io_read(cli_handler, (uint8_t*)&str_temperature, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		flt_temperature = strtof(str_temperature, NULL);

		//xSemaphoreTake( xSemaphore_RF_ALGO_UPDATE, ( TickType_t ) pdMS_TO_TICKS (2));
		///....err = rf_algo_update(&OTS_APP->RF, flt_temperature);
		err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_UPDATE_TABLE,&flt_temperature);
		//xSemaphoreGive( xSemaphore_RF_ALGO_UPDATE );

		//err = rf_algo_update(&OTS_APP->RF, flt_temperature);
		if (err != RF_ERROR__OK)
		{
			snprintf(buf, 100-1, "RF Algo Failed\r\n\n");
			return err;
		}
		else
		{
			snprintf(buf, 100-1, "Done.\r\n\n");
		}

		io_write(output_handler, (uint8_t*)buf, strlen(buf));

	}
	else if (!strcmp(params, "dac-write"))
	{
		LTC2666_SAMPLE sample;

		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		// convert to upper case
		for(int i=0;i<strlen(params);i++)
			params[i] = toupper(params[i]);

		channel = "NA";

		for(int i=0;i<RF_TABLE_NUMBER_ROWS;i++)
		{
			if (strcmp(params, RF_TABLE_ROW_LUT[i]) == 0)
			{
				channel = RF_TABLE_CHANNEL_LUT[i];
				break;
			}
		}


		if (strcmp(channel, "NA") == 0)
		{
			snprintf(buf, 100-1, "Channel not Found...\r\n\n");
			io_write(output_handler, (uint8_t*)buf, strlen(buf));

			return APP_ERROR_OK;
		}

		sample.CH = (DAC61408_CHANNEL)strtol(channel, NULL, 10);

		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		sample.VAL_MV = (int16_t)strtol(params, NULL, 10);

		if (OTS_APP->DRIVERS.RF_DAC == NULL)
		{
			return IO_ERROR_CLI__OK;
		}

		err = io_write(OTS_APP->DRIVERS.RF_DAC, (uint8_t*)&sample, 1);
		if(err!=IO_ERROR_CLI__OK)
		{
			snprintf(buf, 100-1, "Failed...\r\n\n");
		}

		snprintf(buf, 100-1, "%d was written to DAC channel %s\r\n\n", sample.VAL_MV, channel);
		io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}

	return err;
}


static int32_t cli_handle_command__optics(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler)
{
	/**
	char *params;
	DAC7311_SAMPLE sample;
	int32_t err;
    **/
	//OTS_APP->OPER_PARAMS ;
	//OTS_APP->CALIB_PARAMS;
	//OTS_APP->OPER_PARAMS_OLD;
	//OTS_APP->CALIB_SAMPLE;

	OPTMOD_STRUCT_PTR OPTMOD = OPTMOD_GLB;

	OPTMOD_OPER_PARAMS_PTR oper_params = &OPTMOD->OPER.PARAMS;
	OPTMOD_CALIB_PARAMS_PTR calib_params = &OPTMOD->CALIB.PARAMS;


	OPTMOD_OPER_PARAMS_OLD_PTR oper_params_old = &OPTMOD->OPER.PARAMS_OLD;

	OPTMOD_CALIB_SAMPLE_PTR DOT_MIDDLE  = &OPTMOD->OPER.PARAMS_OLD.DOT_MIDDLE;
	OPTMOD_CALIB_SAMPLE_PTR calib_sample =  &OPTMOD->OPER.PARAMS_OLD.Temp;


	OPTMOD_STRUCT_TEMP_CAILB_PTR Calib_data = &OPTMOD->TEMP_DATA;




	char *params;
	DAC7311_SAMPLE sample;
    int32_t err;


	uint32_t        calib_time;


	err = io_read(cli_handler, (uint8_t*)&params, 1);
	if (err != IO_ERROR_CLI__OK)
	{
		return err;
	}

	optmod_set_stdout(OPTMOD, output_handler);

	if (!strcmp(params, "dac"))
	{
		// WRITE OPTIC DAC

		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		sample = (DAC7311_SAMPLE)strtol(params, NULL, 10);

		err = io_write(OTS_APP->DRIVERS.OPTIC_DAC, (uint8_t*)&sample, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		snprintf(buf, 100-1, "Writing to Optical DAC. value= %d\n\r", sample);
		return io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else if (!strcmp(params, "adc"))
	{
		// READ OPTIC ADC

		err = io_read(OTS_APP->DRIVERS.OPTIC_ADC, (uint8_t*)&sample, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		snprintf(buf, 100-1, "Optical ADC sample %d\n\r", sample);
		return io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else if (!strcmp(params, "stop"))
	{
		// STOP OPTIC OPERATION ALGORITHM

		err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_IS_CALIB_TERMINATE,NULL);

		err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_OPER_UNTIL_DONE,NULL);
		//optmod_operation_terminate(&OTS_APP->OPTMOD);  ///***
		//optmod_operation_block_until_done(&OTS_APP->OPTMOD);//***


		////OTS_APP->OPTMOD.OPER.task = NULL; //***?
//		return IO_ERROR_CLI__OK;

		snprintf(buf, 100-1, "Optic algorithm stopped\n\r");
		return io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else if (!strcmp(params, "start"))
	{
		// START OPTIC OPERATION ALGORITHM
		err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_OPER_START_RE_CREATED,	NULL);
		//err = optmod_operation_start(&OTS_APP->OPTMOD); //***

		//if (err != OPTMOD_ERROR__OK)
		//{
			//return err;
		//}

		snprintf(buf, 100-1, "Optic algorithm started\n\r");
		return io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else if (!strcmp(params, "set"))
	{
		// SET OPTIC OPERATION ALGORITHM PARAMS


		// read TRGT_PWR
		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}
		oper_params->trgt_pwr = (uint32_t)strtol(params, NULL, 10);

		// read DAC_START
		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}
		oper_params->dac_start = (uint32_t)strtol(params, NULL, 10);

		// read direction
		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}
		oper_params->direction = (uint32_t)strtol(params, NULL, 10);
		oper_params->direction = (oper_params->direction > 0 ? 1 : -1);

		// read if_print
		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}
		oper_params->if_print = (uint32_t)strtol(params, NULL, 10);


		snprintf(buf, 100-1, "OPTMOD PARAMS ::: PWR_TRGT=%ld; DAC_START=%d, direction=%d\r\n",
				oper_params->trgt_pwr, oper_params->dac_start, oper_params->direction);
		io_write(output_handler, (uint8_t*)buf, strlen(buf));
		err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_OPER_PARAMS_SET,NULL);
		//err = optmod_operation_set(OPTMOD, oper_params);//***
		if (err != OPTMOD_ERROR__OK)
		{
			return err;
		}
	}
	else if (!strcmp(params, "read"))
	{
		// READ OPTIC OPERATION ALGORITHM PARAMS

		err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_GET_SETTINGS_EEPROM,NULL);
		//err = optmod_operation_get_settings(OPTMOD, oper_params); ///***

		memset(buf, 0, 100);
		snprintf(buf, 100-1, "OPTMOD PARAMS ::: PWR_TRGT=%ld; DAC_START=%d, direction=%d\r\n",
				oper_params->trgt_pwr, oper_params->dac_start, oper_params->direction);
		io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else if (!strcmp(params, "status"))
	{
		char* state;
		if (OPTMOD->OPER.STATUS.state == OPTMOD_OPER_STATE__LOCK)
		{
			state = "LOCK";
		}
		else if (OPTMOD->OPER.STATUS.state == OPTMOD_OPER_STATE__SEEK)
		{
			state = "SEEK";
		}
		else
		{
			state = "OFF";
		}

		memset(buf, 0, 100);
		snprintf(buf, 100-1, "OPTMOD STATUS ::: state=%s; duration=%lu[s], power=%ld, error=%ld, seek_duration=%lu[s],DAC=%ld,\r\n",
					state,
					OPTMOD->OPER.STATUS.duration / 1000,
					OPTMOD->OPER.STATUS.curr_pwr,
					OPTMOD->OPER.STATUS.curr_error,
					OPTMOD->OPER.STATUS.seek_duration / 1000,
					OPTMOD->OPER.PARAMS.dac_start);
		io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else if (!strcmp(params, "old-calib"))
	{
		// OLD OPTICS ALGORITHM + CALIBRATION

	//	if (!strcmp(params, "calib"))
		//{
			// START OLD ALGORITHM CALIBRATION


			// read interval
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			calib_params->interval_ms = (uint32_t)strtol(params, NULL, 10);

			// read dac_min
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			calib_params->dac_count_lowest = (uint16_t)strtol(params, NULL, 10);

			// read dac_max
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			calib_params->dac_count_highest = (uint16_t)strtol(params, NULL, 10);

			// read dac_step
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			calib_params->dac_count_step = (uint16_t)strtol(params, NULL, 10);

			// read if_print
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			calib_params->if_print_output = (uint8_t)strtol(params, NULL, 10);
		//	err = optmod_operation_Suspend(&OTS_APP->OPTMOD);//haim
			//		if (err != OPTMOD_ERROR__OK)
		//			{
			//			return err;
			//		}


			err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_IS_CALIB_START,NULL);
			//err = optmod_calibration_start(&OTS_APP->OPTMOD, &calib_params);//***

			if (err != OPTMOD_ERROR__OK)
			{
//				return err;
			}
             calib_time =(calib_params->dac_count_highest
		                    -(calib_params->dac_count_lowest)/calib_params->dac_count_step)*calib_params->interval_ms;

			snprintf(buf, 100-1, " \t The calibration results will be printed in=%d  Sec \r\n", calib_time/1000);
			io_write(OTS_APP->DRIVERS.CLI_OUTPUT, (uint8_t*)buf, strlen(buf));


			err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_CALIB_UNTIL_DONE,NULL);
			//optmod_calibration_block_until_done(&OTS_APP->OPTMOD);//***

			if (calib_params->if_print_output)
			{
				for (uint32_t i = 0; i < Calib_data->samples_count; i++)
				{

					Calib_data->index = i;
					err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_CALIB_READ_SAMPLE,
							(OPTMOD_STRUCT_TEMP_CAILB_PTR)&Calib_data);
					//err = optmod_calibration_read_sample(&OTS_APP->OPTMOD, i, &calib_sample);//**
					snprintf(buf, 100-1, "Optic modulator calibration :::\tDAC=%d\t\tADC=%ld\r\n",
							                 calib_sample->dac, calib_sample->adc);
					io_write(OTS_APP->DRIVERS.CLI_OUTPUT, (uint8_t*)buf, strlen(buf));
				}
			//}

			io_write(OTS_APP->DRIVERS.CLI_OUTPUT, (uint8_t*)"Done.\r\n", strlen("Done.\r\n"));
		//	err = optmod_operation_Resume(&OTS_APP->OPTMOD);//haim
					if (err != OPTMOD_ERROR__OK)
					{
						return err;
					}

		}
		else if (!strcmp(params, "start"))
		{
			// START OLD OPERATION ALGO

			// read dot START dac
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			oper_params_old->interval_ms = (uint32_t)strtol(params, NULL, 10);

			// read dot START dac
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			oper_params_old->DOT_START.dac = (DAC7311_SAMPLE)strtol(params, NULL, 10);

			// read dot START adc
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			oper_params_old->DOT_START.adc = (uint32_t)strtol(params, NULL, 10);

			// read dot END dac
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			oper_params_old->DOT_END.dac = (DAC7311_SAMPLE)strtol(params, NULL, 10);

			// read dot END adc
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			oper_params_old->DOT_END.adc = (uint32_t)strtol(params, NULL, 10);

			// read feedback gain percent
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			oper_params_old->gain_percent = (int16_t)strtol(params, NULL, 10);
			if (oper_params_old->gain_percent <= 0)
			{
				oper_params_old->gain_percent = 100;
				io_write(OTS_APP->DRIVERS.CLI_OUTPUT, (uint8_t*)"Invalid gain, using 100%.\r\n", strlen("Invalid gain, using 100%.\r\n"));
			}

			// read dot MIDDLE dac
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			oper_params_old->DOT_MIDDLE.dac = (DAC7311_SAMPLE)strtol(params, NULL, 10);

			// read dot MIDDLE adc
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			oper_params_old->DOT_MIDDLE.adc = (uint32_t)strtol(params, NULL, 10);

			// read if_print
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			oper_params_old->if_print = (uint8_t)strtol(params, NULL, 10);

			/////.......err = optmod_operation_start_old(&OTS_APP->OPTMOD, &oper_params);///***
			if (err != OPTMOD_ERROR__OK)
			{
				return err;
			}
		}
		else if (!strcmp(params, "set"))
		{
			// SET OLD OPERATION ALGO PARAMS

			// read dot MIDDLE dac
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			DOT_MIDDLE->dac = (DAC7311_SAMPLE)strtol(params, NULL, 10);

			// read dot MIDDLE adc
			err = io_read(cli_handler, (uint8_t*)&params, 1);
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}
			DOT_MIDDLE->adc = (uint32_t)strtol(params, NULL, 10);

			err = optmod_operation_set_old(OPTMOD, (OPTMOD_CALIB_SAMPLE_PTR)DOT_MIDDLE); ///???????????????????
			if (err != OPTMOD_ERROR__OK)
			{
				return err;
			}
		}
	}
	else
	{
		return IO_ERROR_CLI__BAD_COMMAND;
	}

	return APP_ERROR_OK;
}


static int32_t cli_handle_command__fpga(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler)
{
	char *params;
	int32_t err;

	err = io_read(cli_handler, (uint8_t*)&params, 1);
	if (err != IO_ERROR_CLI__OK)
	{
		return err;
	}

	if (strcmp(params, "write") == 0)
	{
		FPGA_OTBA_RW_REG reg;

		// read address
		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		reg.address = (uint8_t)strtol(params, NULL, 10);

		// read value
		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		reg.value = (uint8_t)strtol(params, NULL, 16);

		xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
		err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &reg);
		xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);
		if (err != IO_ERROR_FPGA_OTBA__OK)
		{
			return err;
		}

		snprintf(buf, 100-1, "FPGA register %d written with value 0x%02X\n\r", reg.address, reg.value);
		io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else if (strcmp(params, "read") == 0)
	{
		FPGA_OTBA_RW_REG reg;

		// read address
		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		reg.address = (uint8_t)strtol(params, NULL, 10);
		reg.value = 0;

		xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
		err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__READ_REG, &reg);
		xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);

		//err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__READ_REG, &reg);


		if (err != IO_ERROR_FPGA_OTBA__OK)
		{
			return err;
		}

		snprintf(buf, 100-1, "FPGA register %d was read and got value 0x%02X\n\r", reg.address, reg.value);
		io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else
	{
		snprintf(buf, 100-1, "Bad Command...\n\r");
			io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}

	return APP_ERROR_OK;
}


static int32_t cli_handle_command__set_temp(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler)
{
	char *params;
	int32_t err;
	uint8_t temp;

	err = io_read(cli_handler, (uint8_t*)&params, 1);
	if (err != IO_ERROR_CLI__OK)
	{
		return err;
	}

	if (strcmp(params, "high") == 0)
	{
		// SET HIGH TEMPERATURE LEVEL

		// read temperature
		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		temp = (uint8_t)strtol(params, NULL, 10);

		err = io_ioctl(OTS_APP->DRIVERS.CPU_TEMP, IO_IOCTL_CPU_TEMP__SET_HIGH, &temp);
		if (err != IO_ERROR_CPU_TEMP__OK)
		{
			return err;
		}

		snprintf(buf, 100-1, "TEC cooling will start at %d\n\r", temp);
		io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else if (strcmp(params, "low") == 0)
	{
		// SET LOW TEMPERATURE LEVEL

		// read temperature
		err = io_read(cli_handler, (uint8_t*)&params, 1);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}
		temp = (uint8_t)strtol(params, NULL, 10);

		err = io_ioctl(OTS_APP->DRIVERS.CPU_TEMP, IO_IOCTL_CPU_TEMP__SET_LOW, &temp);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}

		snprintf(buf, 100-1, "TEC cooling will start at %d\n\r", temp);
		io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
	else
	{
		snprintf(buf, 100-1, "Bad Command...\n\r");
			io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}

	return APP_ERROR_OK;
}
RF_STRUCT_PTR     RF_STRCT_GLB;
static uint8_t hex_to_str(char strn_data[3])
{
	   //char *params;
		int32_t len;
		int32_t decimal =0 ,val;
		int8_t i,escape_bit = 0;

	   /*
	     * Iterate over each hex digit
	     */
	    decimal = 0;
	    for(i=0; i< 2 /*strn_data[i]!=':'*/; i++)
	    {

	        /* Find the decimal representation of hex[i] */
	    	if(strn_data[i] == ':')
	    	{
	    	   escape_bit = 1;
	    	}
	    	else
	        if(strn_data[i]>='0' && strn_data[i]<='9')
	        {
	            val = strn_data[i] - 48;
	        }
	        else if(strn_data[i]>='a' && strn_data[i]<='f')
	        {
	            val = strn_data[i] - 97 + 10;
	        }
	        else if(strn_data[i]>='A' && strn_data[i]<='F')
	        {
	            val = strn_data[i] - 65 + 10;
	        }
            if(escape_bit == 1 )
             break;
	        decimal += val*pow(16,(1-i)) ;
	        len--;
	    }

       return decimal;
}




static int32_t cli_handle_command_EMAC_ADDRESS(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler)
{


	char *params;// = (char *)malloc(sizeof(char)*50);
		   int32_t i,j,cnt,err;
		   uint8_t telnet_connected;
		   char emac_char[7];
		   char *emac_new;
		   char dest_temp[3];
		   uint8_t value_ret;
		   err = io_read(cli_handler, (uint8_t*)&params, 1); // read char EMAC_ADDRESS
			if (err != IO_ERROR_CLI__OK)
			{
				return err;
			}


			if (strcmp(params, "read") == 0) ///EMAC_ADDRESS
			{

					   emac_char[0] = (uint8_t)gnetif.hwaddr[0] ;
					   emac_char[1] = (uint8_t)gnetif.hwaddr[1] ;
					   emac_char[2] = (uint8_t)gnetif.hwaddr[2];
					   emac_char[3] = (uint8_t)gnetif.hwaddr[3];
					   emac_char[4] = (uint8_t)gnetif.hwaddr[4];
					   emac_char[5] = (uint8_t)gnetif.hwaddr[5];
					   emac_char[6] = '\0';

						snprintf(buf,100-1, "EMAC ADDRESS = %02X:%02X:%02X:%02X:%02X:%02X \n\r",(uint8_t)emac_char[0], (uint8_t)emac_char[1],
								(uint8_t)emac_char[2], (uint8_t)emac_char[3], (uint8_t)emac_char[4], (uint8_t)emac_char[5]);
						io_write(output_handler, (uint8_t*)buf, strlen(buf));

			}
			else
	    		if (strcmp(params, "write") == 0)
				{
	    			emac_new = (char *)malloc(sizeof(char)*30);
					err = io_read(cli_handler,(uint8_t*)&emac_new, 1);
					if (err != IO_ERROR_CLI__OK)
					{
									return err;
					}





	               // convert input pramams to HEX hata
					i=0;
					for(j=0;j<6;j++)
					{
					  cnt = 0;
					  for(i;i<3*(j+1);i++)
						dest_temp[cnt++] = emac_new[i];
					  value_ret = hex_to_str(dest_temp);
					  emac_char[j]= (int8_t)value_ret;

					}


					snprintf(buf,15, "NEW EMAC ADDRESS = %02X:%02X:%02X:%02X:%02X:%02X \n\r",
							                              (uint8_t)emac_char[0], (uint8_t)emac_char[1],
														  (uint8_t)emac_char[2], (uint8_t)emac_char[3],
														  (uint8_t)emac_char[4], (uint8_t)emac_char[5]);
					io_write(output_handler, (uint8_t*)buf, sizeof(uint8_t)*15);


					//**************save MAC to EEPROM ****************************

					EEPROM_Write_MAC_ADD(RF_STRCT_GLB,&emac_char[0]);

					io_ioctl(OTS_APP->DRIVERS.TELNET, IO_IOCTL_TELNET__IS_CONNECTED, &telnet_connected);
					if(telnet_connected)
					  {
					   snprintf(buf, 100-1, "MAC ADDRESS UPGRADED...\n\r");
					   io_write(OTS_APP->DRIVERS.TELNET, (uint8_t*)buf, strlen(buf));
					  }

					//MAC_Modify_fnc(OTS_APP);

	              free(emac_new);
			  }

	/*		else
			if (strcmp(params, "write") == 0)
			{
				ser_id_crc = calculate_crc_stm_id(0); //0xXX.XX.XX.XX|XX.XX.XX.XX

				emac_char[5] = (char)ser_id_crc ;
				emac_char[4] = (char)(ser_id_crc = ser_id_crc >> 8);
				emac_char[3] = (char)(ser_id_crc = ser_id_crc >> 8);
				(ser_id_crc = ser_id_crc >> 8); //skip 3 char
				emac_char[2] = (char)(ser_id_crc = ser_id_crc >> 8);
				emac_char[1] = 0x80;
				emac_char[0] = 0x00;

				snprintf(buf,100-1, "NEW EMAC ADDRESS = %02X:%02X:%02X:%02X:%02X:%02X \n\r",(uint8_t)emac_char[0], (uint8_t)emac_char[1],
									(uint8_t)emac_char[2], (uint8_t)emac_char[3], (uint8_t)emac_char[4], (uint8_t)emac_char[5]);
				io_write(output_handler, (uint8_t*)buf, strlen(buf));



				chrEMAC_ADDRESS[0] = MACAddr[0] =  (int8_t)emac_char[0];
				chrEMAC_ADDRESS[1] = MACAddr[1] =  (int8_t)emac_char[1];
				chrEMAC_ADDRESS[2] = MACAddr[2] =  (int8_t)emac_char[2];
				chrEMAC_ADDRESS[3] = MACAddr[3] =  (int8_t)emac_char[3];
				chrEMAC_ADDRESS[4] = MACAddr[4] =  (int8_t)emac_char[4];
				chrEMAC_ADDRESS[5] = MACAddr[5] =  (int8_t)emac_char[5];


				osDelay(1000);

				MAC_Modify_fnc(OTS_APP);

			}
			*/
			else
			{
				snprintf(buf, 100-1, "syntax error in command...\n\r");
				io_write(output_handler, (uint8_t*)buf, strlen(buf));
			}

			return APP_ERROR_OK;
}


static int32_t cli_handle_command__cpu_temp(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler)
{
	extern uint8_t CBIT_OPERATE_state;
	static float temperature = 0.0f;
	int32_t err;
	float ret_temprt_value;

	err = io_read(OTS_APP->DRIVERS.CPU_TEMP, (uint8_t*)&temperature, 1);
	if (err != IO_ERROR_CPU_TEMP__OK)
	{
		return err;
	}

	snprintf(buf, 25-1, "CPU Temperature is %ld\n\r", (int32_t)temperature);
	io_write(output_handler, (uint8_t*)buf, strlen(buf));


	ret_temprt_value = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_READ_TEMP_DAC,&ret_temprt_value);

if (ret_temprt_value==9999)
	{
		snprintf(buf, 25-1, "DAC error \n\r");
	}
	else
	{
	snprintf(buf, 25-1, "DAC Temperature is %ld\n\r",(int32_t)ret_temprt_value);//(int32_t)(dac_temperature));
	}
	io_write(output_handler, (uint8_t*)buf, strlen(buf));

	return APP_ERROR_OK;
}


static int32_t cli_handle_command__print_commands(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler)
{
	uint32_t i;
	const char delim[] = " - ";
	int32_t err;

	for (i = 1; i < cli_number_commands; i++)
	{
		err = io_write(output_handler, (uint8_t*)cli_commands[i].command, strlen(cli_commands[i].command));
		if (err != IO_ERROR_TELNET__OK)
		{
			return err;
		}

		err = io_write(output_handler, (uint8_t*)delim, strlen(delim));
		if (err != IO_ERROR_TELNET__OK)
		{
			return err;
		}

		err = io_write(output_handler, (uint8_t*)cli_commands[i].description, strlen(cli_commands[i].description));
		if (err != IO_ERROR_TELNET__OK)
		{
			return err;
		}

		err = io_write(output_handler, (uint8_t*)nl, strlen(nl));
		if (err != IO_ERROR_TELNET__OK)
		{
			return err;
		}
	}

	return io_write(output_handler, (uint8_t*)nl, strlen(nl));
}


static int32_t cli_handle_command__version(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler)
{
	snprintf(buf, 100-1, "%s\r\n\n", APPLICATION_SOFTWARE_VERSION);

	return io_write(output_handler, (uint8_t*)buf, strlen(buf));
}


int32_t cli_handle_rx(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler, uint8_t support_cursor, uint8_t *cursor_state)
{
	// TODO - add UART
	// Read from both Cli endpoints and try to parse the commands
	char *command;
	int32_t err;
	uint8_t byte;

	if (support_cursor && *cursor_state == 0)
	{
		*cursor_state = 1u;

		err = io_write(output_handler, (uint8_t*)cli_cursor, strlen(cli_cursor));
		if (err != IO_ERROR_TELNET__OK)
		{
			return err;
		}
	}

	do {
		err = io_read(output_handler, &byte, 1);
		if (err != IO_ERROR__OK)
		{
			return err;
		}

		err = io_write(cli_handler, &byte, 1);
		if (err != IO_ERROR__OK)
		{
			return err;
		}

		command = NULL;
		err = io_read(cli_handler, (uint8_t*)&command, 1);
	} while (err != IO_ERROR__OK);

	is_cursor_printed = 0;

	uint32_t i;

	if (command == NULL)
	{
		return err;
	}

	for (i = 0; i < cli_number_commands; i++)
	{
		if (strcmp(command, cli_commands[i].command) == 0)
		{
			if (cli_commands[i].handler != NULL)
			{
				OTS_APP->DRIVERS.CLI_OUTPUT = output_handler;
				cli_commands[i].handler(OTS_APP, cli_handler, output_handler);
			}
		}
	}

	return io_ioctl(cli_handler, IO_IOCTL_CLI__PURGE_COMMAND, NULL);
}

int32_t app_cli_handle_rx(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	uint8_t telnet_connected;
	static uint8_t telnet_cursor_state = 0;

	// Handle UART CLI
	cli_handle_rx(OTS_APP, OTS_APP->DRIVERS.CLI_UART, OTS_APP->DRIVERS.DBG_UART, 0, NULL);

	// Handle Telnet CLI
	io_ioctl(OTS_APP->DRIVERS.TELNET, IO_IOCTL_TELNET__IS_CONNECTED, &telnet_connected);
	if (!telnet_connected)
	{
		return APP_ERROR_OK;
	}

	cli_handle_rx(OTS_APP, OTS_APP->DRIVERS.CLI_TELNET, OTS_APP->DRIVERS.TELNET, 1, &telnet_cursor_state);

	return APP_ERROR_OK;
}


static void cli_handle_command_rf_table_print(IO_HANDLER_PTR output_handler, RF_CALIB_TABLE rf_table)
{
	uint8_t i;
	uint8_t j;

	// print table headers
	snprintf(buf, 100-1, "%8s\t", "Name");
	io_write(output_handler, (uint8_t*)buf, sizeof( RF_TABLE_COLUMN_LUT[0])+9);
	snprintf(buf, 100-1, "%8s\t", "DAC_CH#");
	io_write(output_handler, (uint8_t*)buf, sizeof( RF_TABLE_COLUMN_LUT[0])+9);

	for(i = 0 ; i < RF_TABLE_NUMBER_COLUMNS ; i++)
	{
		snprintf(buf, 100-1, "%8s\t", RF_TABLE_COLUMN_LUT[i]);
		io_write(output_handler, (uint8_t*)buf, sizeof( RF_TABLE_COLUMN_LUT[0])+9);
	}

	snprintf(buf, 100-1, "\n\r");
	io_write(output_handler, (uint8_t*)buf, strlen(buf));

	// print underline for table headers
	for(i = 0 ; i < RF_TABLE_NUMBER_COLUMNS + 2 ; i++)
	{
		snprintf(buf, 100-1, "%8s\t", "-----");
		io_write(output_handler, (uint8_t*)buf, sizeof( RF_TABLE_COLUMN_LUT[0])+9);
	}

	snprintf(buf, 100-1, "\n\r");
	io_write(output_handler, (uint8_t*)buf, strlen(buf));

	// print table values
	for(i = 0 ; i < RF_TABLE_NUMBER_ROWS ; i++)
	{
		// colum header
		snprintf(buf, 100-1, "%8s\t|", RF_TABLE_ROW_LUT[i]);
		io_write(output_handler, (uint8_t*)buf, sizeof( RF_TABLE_ROW_LUT[0])+10);

		snprintf(buf, 100-1, "%8s\t|", RF_TABLE_CHANNEL_LUT[i]);
		io_write(output_handler, (uint8_t*)buf, sizeof( RF_TABLE_CHANNEL_LUT[0])+10);

		// column value
		for(j = 0 ; j < RF_TABLE_NUMBER_COLUMNS ; j++)
		{
			snprintf(buf, 100-1, "%8d\t", rf_table[i][j]);
			io_write(output_handler, (uint8_t*)buf, sizeof(rf_table[0][0])+9);
		}

		snprintf(buf, 100-1, "\n\r");
		io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
}
static int32_t cli_handle_command__LD(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler)
{
	int32_t err;
	int16_t ADC_VAL[10];
	ADC_ChannelConfTypeDef sConfig;
	 sConfig.Channel = Photodiode;
	err = io_read(OTS_APP->DRIVERS.OPTIC_ADC, (uint8_t*)&ADC_VAL, 1);
	RETifERROR
	snprintf(buf, 100-1,"photodiode_mVolt =%d" , ADC_VAL[0]);
	io_write(output_handler, (uint8_t*)buf, strlen(buf));

	// define adc channel
	 sConfig.Channel = Laser_Volt;
	err = io_read(OTS_APP->DRIVERS.OPTIC_ADC, (uint8_t*)&ADC_VAL, 1);
	RETifERROR
	snprintf(buf, 100-1, "   laser mVolt =%d", ADC_VAL[0]);
	io_write(output_handler, (uint8_t*)buf, strlen(buf));

	// read Temp. Sensor through ADC
		static float temperature = 0.0f;
		IO_HANDLER_PTR handler = (IO_HANDLER_PTR)OTS_APP->DRIVERS.CPU_TEMP;
		io_read(handler, (uint8_t*)&temperature, 1);
		snprintf(buf, 100-1,"   Temperature is %ld  ", (int32_t)temperature);
		io_write(output_handler, (uint8_t*)buf, strlen(buf));

	snprintf(buf, 100-1, "\n\r");
	io_write(output_handler, (uint8_t*)buf, strlen(buf));

	return err;
}
RF_STRUCT_PTR     RF_STRCT_GLB;
void cli_handle_command_read_DACs(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR cli_handler, IO_HANDLER_PTR output_handler)
{
	int32_t i;
	LTC2666_SAMPLE dac_sample;
	EventBits_t 	RF_bits_get;
	BaseType_t err_os = 0;
if (CBIT_OPERATE_state==0)
{
	snprintf(buf, 100-1,"NOT OPERATE\n\r");
	io_write(output_handler, (uint8_t*)buf, strlen(buf));
	return ;
}

	static int32_t  temp_DAC_Shadow[6]={0,0,0,0,0,0};
	uint8_t new_data_flg = 0;

	RF_bits_get = xEventGroupGetBits(RF_STRCT_GLB->rf_EventGrp);
	RF_bits_get = xEventGroupWaitBits(RF_STRCT_GLB->rf_EventGrp,0x5A,pdTRUE,pdTRUE,pdMS_TO_TICKS(5));
	//wait for 1 seconds then -> if logical state "(RF_bits_get == 0x5A)" is true
	// then is the next step tu update DAC_Shadow either to exit from routine
	if(RF_bits_get == 0x5A)
	{
		new_data_flg = 1;
	}
	else
	{
		new_data_flg = 0;
	}

	if(new_data_flg)
	 {

		err_os = xQueuePeek(RF_STRCT_GLB->rf_mailbox_RX, dac_sample.DAC_Shadow, 0);
		i=0;
		snprintf(buf, 100-1,"NEW DATA IS .....\n\r");
	    io_write(output_handler, (uint8_t*)buf, strlen(buf));

	    snprintf(buf, 100-1,"DAC_VG1=%dmV \n\r",(int)dac_sample.DAC_Shadow[0]);
	  	io_write(output_handler, (uint8_t*)buf, strlen(buf));
	  	temp_DAC_Shadow[0] = dac_sample.DAC_Shadow[0];

	  	snprintf(buf, 100-1,"DAC_VC1=%dmV \n\r",(int) dac_sample.DAC_Shadow[1]);
	  	io_write(output_handler, (uint8_t*)buf, strlen(buf));
	  	temp_DAC_Shadow[1] = dac_sample.DAC_Shadow[1];

	  	snprintf(buf, 100-1,"DAC_VG2=%dmV \n\r",(int) dac_sample.DAC_Shadow[2]);
	  	io_write(output_handler, (uint8_t*)buf, strlen(buf));
	  	temp_DAC_Shadow[2] = dac_sample.DAC_Shadow[2];

	  	snprintf(buf, 100-1,"DAC_VC2=%dmV \n\r",(int) dac_sample.DAC_Shadow[3]);
	  	io_write(output_handler, (uint8_t*)buf, strlen(buf));
	  	temp_DAC_Shadow[3] = dac_sample.DAC_Shadow[3];

	  	snprintf(buf, 100-1,"DAC_VG3=%dmV \n\r",(int) dac_sample.DAC_Shadow[4]);
	  	io_write(output_handler, (uint8_t*)buf, strlen(buf));
	  	temp_DAC_Shadow[4] = dac_sample.DAC_Shadow[5];

	  	snprintf(buf, 100-1,"DAC_VC3=%dmV \n\r",(int) dac_sample.DAC_Shadow[5]);
	  	io_write(output_handler, (uint8_t*)buf, strlen(buf));
	  	temp_DAC_Shadow[4] = dac_sample.DAC_Shadow[5];
	 }
	else
	{
		snprintf(buf, 100-1,"WAITING FOR NEW DATA UPDATE \n\r");
	    io_write(output_handler, (uint8_t*)buf, strlen(buf));
	    i=0;
	    snprintf(buf, 100-1,"DAC_VG1=%dmV \n\r",(int)temp_DAC_Shadow[0]);
	    io_write(output_handler, (uint8_t*)buf, strlen(buf));
	    snprintf(buf, 100-1,"DAC_VC1=%dmV \n\r",(int) temp_DAC_Shadow[1]);
	    io_write(output_handler, (uint8_t*)buf, strlen(buf));
	    snprintf(buf, 100-1,"DAC_VG2=%dmV \n\r",(int) temp_DAC_Shadow[2]);
	    io_write(output_handler, (uint8_t*)buf, strlen(buf));
	    snprintf(buf, 100-1,"DAC_VC2=%dmV \n\r",(int) temp_DAC_Shadow[3]);
	    io_write(output_handler, (uint8_t*)buf, strlen(buf));
	    snprintf(buf, 100-1,"DAC_VG3=%dmV \n\r",(int) temp_DAC_Shadow[4]);
	    io_write(output_handler, (uint8_t*)buf, strlen(buf));
	    snprintf(buf, 100-1,"DAC_VC3=%dmV \n\r",(int) temp_DAC_Shadow[5]);
	    io_write(output_handler, (uint8_t*)buf, strlen(buf));
	}
}


