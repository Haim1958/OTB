/**
 ******************************************************************************
 * @file    rf.h
 * @brief   Application RF declarations and definitions
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


#ifndef _RF_H_
#define _RF_H_


/* Definitions */

#define RF_ERROR__OK							ERROR_OK
#define RF_ERROR__INVALID_RF_TABLE_INDEX		RF_ERROR(0x01)
#define RF_ERROR__NOT_IN_CALIB_MODE				RF_ERROR(0x02)
#define RF_ERROR__ALREADY_RUNNING				RF_ERROR(0x03)
#define RF_ERROR__INVALID_VALUE					RF_ERROR(0x04)
#define RF_ERROR__DAC_MISSING					RF_ERROR(0x05)

#define RF_ERROR__NOK							0xF0

#define IO_IOCTL_RF_OPER_START_RE_CREATED			1
#define IO_IOCTL_RF_OPER_TERMNT_RE_DELETED			2
#define IO_IOCTL_RF_SET_TABLE             		 	3

#define IO_IOCTL_RF_GET_TABLE						4
#define IO_IOCTL_RF_UPDATE_TABLE                  	5

#define IO_IOCTL_RF_DAC_HANDLER 					6
#define IO_IOCTL_TEMP_SENSOR_HANDLER				7


#define IO_IOCTL_RF_EXAMINE							8
#define IO_IOCTL_RF_INIT_HANDLER					9
#define IO_IOCTL_RF_SPARE_HANDLER					10

#define IO_IOCTL_RF_READ_TABLE_ALL					0x0C
#define IO_IOCTL_RF_TABLE_CALIB_START				0x1C
#define IO_IOCTL_RF_TABLE_CALIB_FINAL				0x2C
#define IO_IOCTL_RF_TABLE_IS_IN_CALIB				0x3C

#define IO_IOCTL_RF_READ_TEMP_DAC					0x4C

#define RF_ALGO_INTERVAL_MS		(3000) //3 SEC

#define RF_TABLE_NUMBER_COLUMNS	(3u)

#define RF_TABLE_MAX_NUMBER_ROWS (10u)

extern const char* const RF_TABLE_ROW_LUT[];
extern const char* const RF_TABLE_CHANNEL_LUT[];
extern const char* const RF_TABLE_COLUMN_LUT[RF_TABLE_NUMBER_COLUMNS];
extern const DAC61408_CHANNEL DAC_CHANNEL_LUT[];

typedef int16_t RF_CALIB_TABLE[RF_TABLE_MAX_NUMBER_ROWS][RF_TABLE_NUMBER_COLUMNS];

typedef struct rf_table_struct {
	uint8_t					start_calib;
	RF_CALIB_TABLE			rf_table;
} RF_TABLE_STRUCT;

typedef struct {

	//TimerHandle_t 			rf_oper_task_timer;

	RF_TABLE_STRUCT			_RF_TABLE_STR;

	int32_t dac_temperature_COUNTER;
	int32_t table_index;
	int32_t row_index;
	int32_t column_index;

	float 	test_rf_dac_temptr;

	QueueHandle_t 			rf_mailbox_RX;
	EventGroupHandle_t      rf_EventGrp;

	IO_HANDLER_PTR			RF_DAC;
	IO_HANDLER_PTR			TEMP_SENSOR;

	IO_HANDLER_PTR			*TABLE_FILE;

	TaskHandle_t			oper_task;
} RF_STRUCT, * RF_STRUCT_PTR;



IO_HANDLER_PTR			RF_DAC_GLB;
IO_HANDLER_PTR			TEMP_SENSOR_GLB;
IO_HANDLER_PTR			*TABLE_FILE_GLB;


typedef struct {

	char 							*char_temp;

	uint32_t						u32_temp_0;
	uint32_t						u32_temp_1;
	uint32_t						u32_temp_2;
} RF_INIT_STRUCT, * RF_INIT_STRUCT_PTR;


#ifdef __cplusplus
 extern "C" {
#endif


 int32_t app_rf_table_set_default(RF_STRUCT_PTR RF);
 float app_rf_get_temp_column_value_0(void);
 float app_rf_get_temp_column_value_1(void);
 float app_rf_get_temp_column_value_2(void);
 int32_t app_rf_get_number_of_tables(void);


 int32_t install_rf_func(RF_INIT_STRUCT_PTR RF_Func_init);

//int32_t rf_init(RF_STRUCT_PTR RF, IO_HANDLER_PTR RF_DAC, IO_HANDLER_PTR TEMP_SENSOR);
//int32_t rf_table_get(RF_STRUCT_PTR RF, int32_t table_index, int32_t row_index, int32_t column_index, int16_t *value);

//int32_t rf_table_set(RF_STRUCT_PTR RF, int32_t table_index, int32_t row_index, int32_t column_index, int16_t value);
//int32_t rf_table_calib_start(RF_STRUCT_PTR RF, int32_t table_index);
//int32_t rf_table_calib_finalize(RF_STRUCT_PTR RF, int32_t table_index);
//int32_t rf_table_is_in_calib(RF_STRUCT_PTR RF, int32_t table_index, uint8_t *bool);
//int32_t rf_table_read_all(RF_STRUCT_PTR RF, int32_t table_index, RF_CALIB_TABLE* rf_table);
//int32_t rf_algo_update(RF_STRUCT_PTR RF, float temp_current);
//int32_t rf_start(RF_STRUCT_PTR RF);
//int32_t rf_terminate(RF_STRUCT_PTR RF);


#ifdef __cplusplus
 }
#endif
#endif /* _RF_H_ */
