/**
 ******************************************************************************
 * @file    app_init.c
 * @brief   OTSA implementation initialization
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
// 05-Octb-2021
#include <string.h>
#include "UserLib.h"
#include "app.h"
#include "main.h"

extern ip4_addr_t ipaddr;
extern ip4_addr_t netmask;
extern ip4_addr_t gw;


/*! \brief Allocates and initializes all the OTS drivers.
 *
 ###  Detailed description:
 *
 *  @param ots_app_bundle - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the initialization was OK
 *  @return **Error** if the initialization failed
 */
SemaphoreHandle_t xSemaphore_OTBA_RD_WR_FPGA = NULL;

static int32_t main_app_init_drivers(OTS_APPLICATION_BUNDLE_PTR ots_app_bundle)
{
	CLI_INSTALL_STRUCT cli_install;
	SETTINGS_INSTALL_STRUCT settings_install;
	TCPCOM_INIT_STRUCT tcpcom_init;
	UART_IO_INIT_STRUCT uart_init;
	TELNET_INIT_STRUCT telnet_struct;
	SPI_IO_INIT_STRUCT spi_init;
	FPGA_OTBA_INIT_STRUCT fpga_init;
	ADS8681_OTBA_INIT_STRUCT ADS8681_init;
	CPU_TEMP_INIT_STRUCT cpu_temp_init;
	I2C_IO_INIT_STRUCT i2c_init;
	M24512_INIT_STRUCT m24512_init;
	SETTINGS_INIT_STRUCT settings_init;
	LTC2666_INIT_STRUCT ltc2666_init;
	DAC7311_INIT_STRUCT dac7311_init;
	OPT_ADC_INIT_STRUCT opt_adc_init;
	OTBBCOM_INIT_STRUCT otbbcom_init;
	OPTMOD_INIT_STRUCT OPTMod_Func_init;
	RF_INIT_STRUCT_PTR RF_Func_init;
	int32_t err;

	/*
	 * INSTALL DRIVERS
	 */

	/* Install TCPCOM driver */
	err = install_tcpcom(NULL);
	if (err != IO_ERROR_TCPCOM__OK)
	{
		return err;
	}

	/* Install UART IO driver */
	err = install_uart_io(NULL);
	if (err != IO_ERROR_UART_IO__OK)
	{
		return err;
	}

	/* Install TELNET driver */
	err = install_telnet(NULL);
	if (err != IO_ERROR_TELNET__OK)
	{
		return err;
	}

	/* Install CLI driver */
	cli_install.total_number_instances = 2;
	err = install_cli(&cli_install);
	if (err != IO_ERROR_CLI__OK)
	{
		return err;
	}

	/* Install SPI IO driver */
	err = install_spi_io(NULL);
	if (err != IO_ERROR_SPI_IO__OK)
	{
		return err;
	}

	/* Install ADS8681 driver */
	err = install_ADS8681_otba(NULL);
	if (err != IO_ERROR_ADS8681_OTBA__OK)
	{
		return err;
	}


	/* Install FPGA driver */
		err = install_fpga_otba(NULL);
		if (err != IO_ERROR_FPGA_OTBA__OK)
		{
			return err;
		}
	/* Install I2C IO driver */
	err = install_i2c_io(NULL);
	if (err != IO_ERROR_I2C_IO__OK)
	{
		return err;
	}

	/* Install EEPROM driver */
	err = install_m24512(NULL);
	if (err != IO_ERROR_I2C_IO__OK)
	{
		return err;
	}

	/* Install SETTINGS driver */
	settings_install.max_number_files = 4;
	err = install_settings(&settings_install);
	if (err != IO_ERROR_SETTINGS__OK)
	{
		return err;
	}

	/* Install CPU Temperature driver */
	err = install_cpu_temp(NULL);
	if (err != IO_ERROR_CPU_TEMP__OK)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* Install LTC2666 driver */
	err = install_ltc2666(NULL);
	if (err != IO_ERROR_LTC2666__OK)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* Install DAC7311 driver */
	err = install_dac7311(NULL);
	if (err != IO_ERROR_DAC7311__OK)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* Install OPT_ADC driver */
	err = install_opt_adc(NULL);
	if (err != IO_ERROR_OPT_ADC__OK)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* Install OTBBCOM driver */
	err = install_otbbcom(NULL);
	if (err != IO_ERROR_OTBBCOM__OK)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}


	err = install_optmod_func(NULL);

	err = install_rf_func(NULL);

	/*
	 * INITIALIZE AND OPEN DRIVERS
	 */

	/* Open TCP COM driver */
	tcpcom_init.remote_ip = IP_ADDRESS_SPECTRA;
	tcpcom_init.local_ip = ipaddr;
	tcpcom_init.local_netmask = netmask;
	tcpcom_init.local_gw = gw;
	tcpcom_init.local_port = TCP_OTBA_PORT_FOR_SPECTRA;
	tcpcom_init.remote_port = TCP_SPECTRA_PORT_FOR_OTBA;
	tcpcom_init.tcp_tx_timeout_ms = 200;
	tcpcom_init.tcp_rx_timeout_ms = 1;
	tcpcom_init.tcp_connect_attempts = 1;
	ots_app_bundle->DRIVERS.TCPCOM = io_open(IO_HANDLER_TYPE__TCPCOM, &tcpcom_init);
	if (NULL == ots_app_bundle->DRIVERS.TCPCOM)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* Open UART driver */
	uart_init.HAL_UART = &huart1;
	uart_init.rx_buf_size = 100;
	uart_init.tx_buf_size = 100;
	ots_app_bundle->DRIVERS.DBG_UART= io_open(IO_HANDLER_TYPE__UART_IO, &uart_init);
	if (NULL == ots_app_bundle->DRIVERS.DBG_UART)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* Open UART CLI instance */
	ots_app_bundle->DRIVERS.CLI_UART = io_open(IO_HANDLER_TYPE__CLI, NULL);
	if (NULL == ots_app_bundle->DRIVERS.CLI_UART)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* Open TELNET driver */
	telnet_struct.local_port = 23;
	telnet_struct.rx_queue_size = 512;
	telnet_struct.tx_queue_size = 512;
	ots_app_bundle->DRIVERS.TELNET = io_open(IO_HANDLER_TYPE__TELNET, &telnet_struct);
	if (NULL == ots_app_bundle->DRIVERS.TELNET)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* Open Telnet CLI instance */
	ots_app_bundle->DRIVERS.CLI_TELNET = io_open(IO_HANDLER_TYPE__CLI, NULL);
	if (NULL == ots_app_bundle->DRIVERS.CLI_TELNET)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}
	
	/* Open FPGA SPI driver */
	spi_init.HAL_SPI = &hspi1;
	spi_init.SPI_NSS_PORT = SPI1_NSS_GPIO_Port;
	spi_init.SPI_NSS_PIN = SPI1_NSS_Pin;
	spi_init.read_buf_size = 32;
	spi_init.rw_timeout = 100;
	ots_app_bundle->DRIVERS.FPGA_SPI = io_open(IO_HANDLER_TYPE__SPI_IO, &spi_init);
	if (NULL == ots_app_bundle->DRIVERS.FPGA_SPI)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* Open FPGA driver */
	fpga_init.driver = ots_app_bundle->DRIVERS.FPGA_SPI;
	ots_app_bundle->DRIVERS.FPGA = io_open(IO_HANDLER_TYPE__FPGA_OTBA, &fpga_init);
	if (NULL == ots_app_bundle->DRIVERS.FPGA)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	//************************************************************
	/* Open ADS8681 SPI driver */
		spi_init.HAL_SPI = &hspi3;
		spi_init.SPI_NSS_PORT = SPI3__NSS_GPIO_Port;
		spi_init.SPI_NSS_PIN = SPI3__NSS_Pin;
		spi_init.read_buf_size = 32;
		spi_init.rw_timeout = 100;
		ots_app_bundle->DRIVERS.ADS8681_SPI = io_open(IO_HANDLER_TYPE__SPI_IO, &spi_init);
		if (NULL == ots_app_bundle->DRIVERS.ADS8681_SPI)
		{
			return APP_ERROR__OUT_OF_HEAP;
		}

		/* Open ADS8681 driver */

		 HAL_GPIO_WritePin (LAser_RESET_GPIO_Port, LAser_RESET_Pin,GPIO_PIN_SET );//reset not active

		 ADS8681_init.driver = ots_app_bundle->DRIVERS.ADS8681_SPI;
		ots_app_bundle->DRIVERS.ADS8681 = io_open(IO_HANDLER_TYPE__ADS8681_OTBA, &ADS8681_init);
		if (NULL == ots_app_bundle->DRIVERS.ADS8681)
		{
			return APP_ERROR__OUT_OF_HEAP;
		}


	/* Open CPU Temperature driver */
	cpu_temp_init.HAL_ADC = &hadc1;
	cpu_temp_init.VREF_MV = 3300.0f;
	ots_app_bundle->DRIVERS.CPU_TEMP = io_open(IO_HANDLER_TYPE__CPU_TEMP, &cpu_temp_init);
	if (NULL == ots_app_bundle->DRIVERS.CPU_TEMP)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* Open I2C IO driver */
	i2c_init.HAL_I2C = &hi2c1;
	i2c_init.index = 1;
	ots_app_bundle->DRIVERS.I2C = io_open(IO_HANDLER_TYPE__I2C_IO, &i2c_init);
	if (NULL == ots_app_bundle->DRIVERS.I2C)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* set start / stop cooling temperatures */
	uint8_t temp;
	temp = 10;
	err = io_ioctl(ots_app_bundle->DRIVERS.CPU_TEMP, IO_IOCTL_CPU_TEMP__SET_LOW, &temp);
	if (err != IO_ERROR_CPU_TEMP__OK)
		return err;
	temp = 40;
	err = io_ioctl(ots_app_bundle->DRIVERS.CPU_TEMP, IO_IOCTL_CPU_TEMP__SET_HIGH, &temp);
	if (err != IO_ERROR_CPU_TEMP__OK)
		return err;

	/* Open M24512 driver */
	m24512_init.i2c_driver = ots_app_bundle->DRIVERS.I2C;
	ots_app_bundle->DRIVERS.M24512 = io_open(IO_HANDLER_TYPE__M24512, &m24512_init);
	if (NULL == ots_app_bundle->DRIVERS.M24512)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* Open SETTINGS driver */
	settings_init.DRIVER.if_init = 1;
	settings_init.DRIVER.eeprom_driver = ots_app_bundle->DRIVERS.M24512;
	settings_init.INSTANCE.if_init = 0;
	ots_app_bundle->DRIVERS.SETTINGS = io_open(IO_HANDLER_TYPE__SETTINGS, &settings_init);
	if (NULL == ots_app_bundle->DRIVERS.SETTINGS)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}
	
	
	/* Open DAC SPI driver */
	spi_init.HAL_SPI = &hspi2;
	spi_init.SPI_NSS_PORT = DAC_CSn_GPIO_Port;
	spi_init.SPI_NSS_PIN = DAC_CSn_Pin;
	spi_init.read_buf_size = 32;
	spi_init.rw_timeout = 100;
	ots_app_bundle->DRIVERS.RF_DAC_SPI = io_open(IO_HANDLER_TYPE__SPI_IO, &spi_init);
	if (NULL == ots_app_bundle->DRIVERS.RF_DAC_SPI)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}


	/* Open LTC2666 driver */
	ltc2666_init.SPI = ots_app_bundle->DRIVERS.RF_DAC_SPI;
	ltc2666_init.LDAC_PORT = DAC_LDACn_GPIO_Port;
	ltc2666_init.LDAC_PIN = DAC_LDACn_Pin;
	ltc2666_init.MSPAN = LTC2666_SPAN__2v5_2v5_mid_man;
	ltc2666_init.RES = LTC2666_RES__16b;
	ots_app_bundle->DRIVERS.RF_DAC = io_open(IO_HANDLER_TYPE__LTC2666, &ltc2666_init);
	// Test is cancelled on purpose, to enable running without RF card
	// If RF is missing, RF also wont be enabled.
//	if (NULL == ots_ app_bundle->DRIVERS.RF_DAC)
//	{
//		return APP_ERROR__OUT_OF_HEAP;
//	}


	/* Open OPTIC SPI driver */
	spi_init.HAL_SPI = &hspi3;
	spi_init.SPI_NSS_PORT = OPT_DAC_SYNC_GPIO_Port;
	spi_init.SPI_NSS_PIN = OPT_DAC_SYNC_Pin;
	spi_init.read_buf_size = 32;
	spi_init.rw_timeout = 100;
	ots_app_bundle->DRIVERS.OPTIC_DAC_SPI = io_open(IO_HANDLER_TYPE__SPI_IO, &spi_init);
	if (NULL == ots_app_bundle->DRIVERS.OPTIC_DAC_SPI)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}


	/* Open DAC7311 driver */
	dac7311_init.SPI = ots_app_bundle->DRIVERS.OPTIC_DAC_SPI;
	ots_app_bundle->DRIVERS.OPTIC_DAC = io_open(IO_HANDLER_TYPE__DAC7311, &dac7311_init);
	if (NULL == ots_app_bundle->DRIVERS.OPTIC_DAC)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}


	/************************* Initializing OPTMOD_FUNC & RF Driver *************************************/

	ots_app_bundle->DRIVERS.OPTMOD_FUNC = io_open(IO_HANDLER_TYPE__OPTMOD_FUNC, &OPTMod_Func_init);
	if (NULL == ots_app_bundle->DRIVERS.OPTMOD_FUNC)
	{
			return APP_ERROR__OUT_OF_HEAP;
	}

	ots_app_bundle->DRIVERS.RF_FUNC = io_open(IO_HANDLER_TYPE__RF_FUNC, &RF_Func_init);
		if (NULL == ots_app_bundle->DRIVERS.RF_FUNC)
		{
				return APP_ERROR__OUT_OF_HEAP;
		}

	/************************* End Initializing OPTMOD_FUNC & RF Driver *************************************/

	/* Open OPT_ADC driver */
	opt_adc_init.HAL_ADC = &hadc3;
	opt_adc_init.CH_CONFIG.Channel = ADC_CHANNEL_6;
	opt_adc_init.CH_CONFIG.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	opt_adc_init.CH_CONFIG.Rank = ADC_REGULAR_RANK_1;
	opt_adc_init.VREF_MV = 3300.0f;
	ots_app_bundle->DRIVERS.OPTIC_ADC = io_open(IO_HANDLER_TYPE__OPT_ADC, &opt_adc_init);
	if (NULL == ots_app_bundle->DRIVERS.OPTIC_ADC)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}


	/* Open OTBB COM driver */
	otbbcom_init.type = OTBBCOM_EDGE_TYPE__OTBA;
//	otbbcom_init.remote_ip = IP_ADDRESS_SPECTRA;		// not required for OTBA (server)
	otbbcom_init.tcp_tx_timeout_ms = 200;
	otbbcom_init.tcp_rx_timeout_ms = 1;
	otbbcom_init.tcp_connect_attempts = 1;
	ots_app_bundle->DRIVERS.OTBBCOM = io_open(IO_HANDLER_TYPE__OTBBCOM, &otbbcom_init);
	if (NULL == ots_app_bundle->DRIVERS.OTBBCOM)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}


	return APP_ERROR_OK;
}


/*! \brief Allocates and initializes all the Operating System handlers.
 *
 ###  Detailed description:
 *
 *  @param ots_app_bundle - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the initialization was OK
 *  @return **Error** if the initialization failed
 */

#define SM_LOGIC_TASK_STASK_SIZE		256
StackType_t SM_LOGIC_TASK_STACK[SM_LOGIC_TASK_STASK_SIZE];
StaticTask_t SM_LOGIC_TASK_BUFFER;

///TaskHandle_t SM_Logic_task_handler;
//void SM_Logic_task (void  * argument);
static int32_t app_init_os(OTS_APPLICATION_BUNDLE_PTR ots_app_bundle)
{

	xSemaphore_OTBA_RD_WR_FPGA = xSemaphoreCreateMutex();
	/* MainStateMachineTask */
	osThreadDef(mainTaskDef, MainTask, osPriorityNormal - 2 , 0, 512);
	ots_app_bundle->TASKS.mainTaskHandler = osThreadCreate(osThread(mainTaskDef), ots_app_bundle);
	if (NULL == ots_app_bundle->TASKS.mainTaskHandler)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* CliTask */
	osThreadDef(cliTaskDef, CliTask, osPriorityNormal - 2, 0, 512);
	ots_app_bundle->TASKS.cliTaskHandler = osThreadCreate(osThread(cliTaskDef), ots_app_bundle);
	if (NULL == ots_app_bundle->TASKS.cliTaskHandler)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/* IP allocation task */

	osThreadDef(ipAllocCTaskDef, IPAllocationTask, osPriorityNormal, 0, 512);
	ots_app_bundle->TASKS.ipAllocCTaskHandler = osThreadCreate(osThread(ipAllocCTaskDef), ots_app_bundle);
	if (NULL == ots_app_bundle->TASKS.ipAllocCTaskHandler)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	/*****   adding new tasks       ***/
   /*
	osThreadDef(OtbaBreqTaskDef, OTBA_B_Req_Task, osPriorityNormal - 2, 0, 512);
	ots_app_bundle->TASKS.OtbaBreqTaskHandler = osThreadCreate(osThread(OtbaBreqTaskDef), ots_app_bundle);
	if (NULL == ots_app_bundle->TASKS.OtbaBreqTaskHandler)
	{
			return APP_ERROR__OUT_OF_HEAP;
	}
     */
	osThreadDef(OtbaGetbAnswTaskDef, OTBA_Get_B_Answ_Task, osPriorityNormal - 1, 0, 512);
	ots_app_bundle->TASKS.OtbaGetbAnswTaskHandler = osThreadCreate(osThread(OtbaGetbAnswTaskDef), ots_app_bundle);
	if (NULL == ots_app_bundle->TASKS.OtbaGetbAnswTaskHandler)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

   /*
	osThreadDef(OTBBExamETHRStateTaskDef, OTBB_COM_examine_task, osPriorityNormal , 0, 512);
	ots_app_bundle->TASKS.OTBBExamETHRStateTaskHandler = osThreadCreate(osThread(OTBBExamETHRStateTaskDef), ots_app_bundle);
	if (NULL == ots_app_bundle->TASKS.OTBBExamETHRStateTaskHandler)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}
   */
/*
	osThreadDef(SMLogicTaskDef, SM_Logic_task, osPriorityNormal - 2, 0, 512);
	ots_app_bundle->TASKS.SM_LogicTaskHandler = osThreadCreate(osThread(SMLogicTaskDef), ots_app_bundle);
	if (NULL == ots_app_bundle->TASKS.SM_LogicTaskHandler)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}
   */
   /*
	SM_Logic_task_handler = xTaskCreateStatic(
					SM_Logic_task,
					"SM_Logic",
					SM_LOGIC_TASK_STASK_SIZE,
					ots_app_bundle,//OTS_APP,
					osPriorityNormal - 2 ,
					SM_LOGIC_TASK_STACK,
					&SM_LOGIC_TASK_BUFFER );
    */

	/*****   END OF adding new tasks       ***/
	return APP_ERROR_OK;
}


/*! \brief Allocates and initializes the OTS application structure.
 *
 ###  Detailed description:
 *  Allocates the OTS Main Application structure and initializes it's drivers and internal components.
 *
 *  @param ots_app_bundle - pointer to the pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the initialization was OK
 *  @return **Error** if the initialization failed
 */
int32_t main_app_init(OTS_APPLICATION_BUNDLE_PTR *ots_app_bundle)
{
	int32_t err;

	OTS_APPLICATION_BUNDLE_PTR OTS_APP = NULL;
	/* Allocate the OTS Application Bundle */
	OTS_APP = *ots_app_bundle =  (OTS_APPLICATION_BUNDLE_PTR) pvPortMalloc(sizeof(OTS_APPLICATION_BUNDLE));
	if (NULL == *ots_app_bundle)
	{
		return APP_ERROR__OUT_OF_HEAP;
	}

	memset(*ots_app_bundle, 0, sizeof(OTS_APPLICATION_BUNDLE));
	(*ots_app_bundle)->TCP.phasis_control_state = OTB_STATE_COUNT; // Initialization indicating no state transition was received yet.

	/* Initialize all global drivers */
	err = main_app_init_drivers(*ots_app_bundle);
	if (err != IO_ERROR__OK)
	{
		return err;
	}

	/*****************************/
	err = app_rf_init(OTS_APP, OTS_APP->DRIVERS.RF_DAC, OTS_APP->DRIVERS.CPU_TEMP);

		err = MAC_INIT(OTS_APP);
	/*****************************/


	/* Initialize OTS state machine */
	err = app_sm_init(*ots_app_bundle);
	if (err != FSM_OK)
	{
		return err;
	}



	/* Initialize OTS FreeRTOS operating system */
	err = app_init_os(*ots_app_bundle);
	if (err != APP_ERROR_OK)
	{
		return err;
	}

	//...................printf_register_io((*ots_app_bundle)->DRIVERS.TELNET);




	return APP_ERROR_OK;
}
