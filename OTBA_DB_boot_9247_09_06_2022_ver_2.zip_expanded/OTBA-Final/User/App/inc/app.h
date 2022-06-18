/**
 ******************************************************************************
 * @file    app.h
 * @brief   OTS Application public declarations and definitions
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


#ifndef _APP_H_
#define _APP_H_

/* Includes */

#include "App_Inc.h"

/* Definitions */

// Application Errors
#define APP_ERROR_OK										ERROR_OK
#define APP_ERROR__OUT_OF_HEAP								APP_ERROR(0x01)
#define APP_ERROR__INVALID_STATE							APP_ERROR(0x02)
#define APP_ERROR__IBIT_STATE_ERROR							APP_ERROR(0x03)	// Unexpected error with the IBIT internal state.
#define APP_ERROR__INVALID_CLI_OPCODE						APP_ERROR(0x04)
#define APP_ERROR__INVALID_CLI_PACKET						APP_ERROR(0x05)


struct _OTS_APPLICATION_BUNDLE
{
	/* Drivers bundle */
	struct {
		IO_HANDLER_PTR		TCPCOM;
		IO_HANDLER_PTR		DBG_UART;
		IO_HANDLER_PTR		CLI_UART;
		IO_HANDLER_PTR		TELNET;
		IO_HANDLER_PTR		CLI_TELNET;
		IO_HANDLER_PTR		FPGA_SPI;
		IO_HANDLER_PTR		ADS8681_SPI;
		IO_HANDLER_PTR		ADS8681;
		IO_HANDLER_PTR		FPGA;
		IO_HANDLER_PTR		CPU_TEMP;
		IO_HANDLER_PTR		I2C;
		IO_HANDLER_PTR		M24512;
		IO_HANDLER_PTR		SETTINGS;
//		IO_HANDLER_PTR		RF_SETTINGS;
		IO_HANDLER_PTR		RF_DAC_SPI;
		IO_HANDLER_PTR		RF_DAC;
		IO_HANDLER_PTR		OPTIC_DAC_SPI;
		IO_HANDLER_PTR		OPTIC_DAC;
		IO_HANDLER_PTR		OPTIC_ADC;
		IO_HANDLER_PTR		OPTIC_SETTINGS;
		IO_HANDLER_PTR		OTBBCOM;

		IO_HANDLER_PTR		OPTMOD_FUNC;
		IO_HANDLER_PTR		RF_FUNC;

		IO_HANDLER_PTR		CLI_OUTPUT;				// Latest CLI output handler
	} DRIVERS;
	
	OTS_APP_SM_BUNDLE		FSM;
	
	struct {
		osThreadId			mainTaskHandler;
		osThreadId			cliTaskHandler;
		osThreadId			ipAllocCTaskHandler;
		osThreadId          OtbaBreqTaskHandler;
		osThreadId          OtbaGetbAnswTaskHandler;
		osThreadId          OTBBExamETHRStateTaskHandler;
		osThreadId          IP_Allocation_task_handler;
	} TASKS;

	   struct
		  {
			int32_t sock;
			int32_t conn;
			int32_t accept;
			uint16_t port;
			uint8_t type;
			uint8_t adr_status;
		 } struct_conn;

		struct
		    {
				int32_t sock;
				uint32_t tick_count;
			}struct_recv_sock;





	struct {
		uint8_t				is_connected;

		uint32_t			last_msgcounter__PHASIS_CONTROL;
		uint32_t			last_msgcounter__DESCRITE_SIGNAL_TEST;

		OTB_STATE			phasis_control_state;
		uint8_t				state_init_succeeded;
		uint8_t				if_send_response_status;
	} TCP;

	struct {
		uint8_t				is_connected;
		uint32_t			last_msgcounter;
		uint8_t				otbb_err_counter;
		uint8_t				otbb_disc_rx_err;
		uint8_t				otbb_reg23_value;
		uint8_t				otbb_reg23_value_recv_last;
		uint8_t				otbb_reg23_value_received;
	} OTBBCOM;

	struct {
		TCPCOM_PACKET_BODY__DISCRETE_RF_SIGNAL_TEST		test_request;
		TCPCOM_PACKET_BODY__DISCRETE_TEST_REPORT		test_report;
		IBIT_STATE										ibit_state;
		uint8_t											is_test_in_progress;
	} IBIT;
	
	struct {
		uint8_t 			cbit_counter;

		uint8_t				optics_init_error_sent;
		uint8_t				optics_long_seek_error_sent;
		uint8_t				optics_low_power_error_sent;
	} CBIT;

	struct {
		uint8_t 			bLastConnectedOTBBIsLeft;
		uint8_t 			bLastConnectedOTBCIsWing;
	} OPTICAL_CONNECTION;



	struct {
		uint8_t				is_connected;
	} HOST;



	struct _OTBB_COMM_exam_state{
				uint8_t							OTBB_COMM_task_enable;
				uint8_t							OTBB_COMM_task_disable;
				enum{ST_OTBB_rx_task_START,ST_OTBB_rx_task_WAIT_OFF,ST_OTBB_rx_task_WAIT_ON,
					             ST_OTBB_rx_task_ON,ST_OTBB_rx_task_OFF,
								 ST_OTBB_rx_task_wait_LASEROFF,ST_OTBB_rx_task_wait_LASERON,
								 ST_OTBB_rx_task_LASERSTATE_ON,ST_OTBB_rx_task_LASERSTATE_OFF,
								 ST_OTBB_rx_task_LASERSTATE_IDLE}
						        ST_OTBB_COMM_PRESENT,ST_OTBB_COMM_PRESENT_prev;


						        TaskHandle_t					examine_task_rx;
						        TimerHandle_t					OTBB_eaxm_task_rx_timer;
						        uint8_t						    release_sem_receive;
						        uint8_t						    task_give_notify;
	}
	B_ETHR;



	struct _OTBB_COMM_laser_state{
       void   *pstate;
       uint8_t  uint_state;
	}
	B_laser_state;

	QueueHandle_t   mailbox_laser_state;
	uint8_t			Handle_Laser_OFF;
	uint8_t			Handle_OTBB_SWITCH;

	enum{ST_SEND_A_REQ_B,ST_CNT_BEFORE_SEND,ST_GET_ANWS_B,ST_WAIT_TIMER_OUT,
		ST_OTBB_FAULT_COMMUNC,ST_OTBB_NOT_COMMUNC} ST_OTBA_B_COMM;

	enum{Left_Start_IP_Config,Right_Start_IP_Config,Center_IP,Any_IP,Exam_Left_COMM,Exam_Right_COMM}ST_IP_ALLOCT,ST_IP_ALLOCT_prv;

	enum{Los_Los,Is_Los,Los_Is,Is_Is}ST_LOS_PINS,ST_LOS_PINS_NEXT ;



	uint32_t Count_Reg31_Timer;
	uint32_t Count_Reg31_B_10ms_Timer;
	uint32_t Count_IP_ALLOC_Timer;







	//.........OPTMOD_STRUCT			OPTMOD;
	//RF_STRUCT				RF;



	//RF_STRUCT_PTR     RF_STRCT_GLB

	void *OPER_PARAMS;
	void *CALIB_PARAMS;
	void *OPER_PARAMS_OLD;
	void *CALIB_SAMPLE;
};

typedef struct _OTBB_COMM_exam_state OTBB_COMM_exam_state,*OTBBCOM_EXAM_BUNDLE_PTR ;
/*******************************************************/
typedef enum{ST_LASER_OFF_RUN,ST_IDLE_RUN} OTBA_ST_LSR;
SemaphoreHandle_t xSemaphore_OTBA_SPECTRA_CONNECT;
SemaphoreHandle_t xSemaphore_OTBA_FPGA_READ;




/******************************************************/

#define MAC_address_size 6
typedef struct MAC_ADD_struct {
	uint8_t MAC_address[MAC_address_size];
	uint8_t eeprom_is_valid;

} MAC_ADD_STRUCT,*MAC_ADD_STRC_PTR;

int8_t MAC_INIT(OTS_APPLICATION_BUNDLE_PTR OTS_APP);
int32_t EEPROM_Write_MAC_ADD(RF_STRUCT_PTR RF, char  *pemac_char);

void OTBB_COM_examine_task_rx(void *);

uint8_t getFwVersion(uint8_t* rxBuffer, uint8_t* txBuffer);


int32_t main_app_init(OTS_APPLICATION_BUNDLE_PTR *ots_app_bundle);

int32_t app_tcpcom_handle_comm(OTS_APPLICATION_BUNDLE_PTR OTS_APP);
int32_t app_otbbcom_handle_comm(OTS_APPLICATION_BUNDLE_PTR OTS_APP);



int32_t app_cli_handle_rx(OTS_APPLICATION_BUNDLE_PTR OTS_APP);


#endif //_APP_H_


