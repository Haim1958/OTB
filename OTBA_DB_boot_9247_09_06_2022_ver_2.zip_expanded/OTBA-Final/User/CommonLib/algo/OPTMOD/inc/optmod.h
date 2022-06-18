/**
 ******************************************************************************
 * @file    optmod.h
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

#ifndef __OPTMOD_H__
#define __OPTMOD_H__

/* Includes */


/* Definitions */
#define OPTMOD_ERROR__OK							OPTMOD_ERROR(0x00)
//#define OPTMOD_ERROR__OK							OPTMOD_ERROR(0x01)
#define OPTMOD_ERROR__INVALID_PARAMS				OPTMOD_ERROR(0x02)
#define OPTMOD_ERROR__ALREADY_RUNNING				OPTMOD_ERROR(0x03)
#define OPTMOD_ERROR__FAILED_WRITING_PARAMS			OPTMOD_ERROR(0x04)
#define OPTMOD_ERROR__NOK							OPTMOD_ERROR(0x05)

// OLD Algorithm definitions
#define OPTMOD_OPER__ADC_OVERSAMPLING				(16u)
#define OPTMOD_OPER__MAX_DAC_STEP					(50)


// NEW Algorithm definitions
#define OPTMOD_OPER__INTERVAL_MS__SEEK				(100)		// interval of the algorithm operation when in SEEK state
#define OPTMOD_OPER__INTERVAL_MS__LOCK				(1000)		// interval of the algorithm operation when in LOCK state

#define OPTMOD_OPER__LOCK_THRESH					(10)
#define OPTMOD_OPER__SEEK_THRESH					(20)
#define OPTMOD_OPER__ADC_DAC_AMP					(1)



#define IO_IOCTL_OPTMOD_EXAMINE								1
#define IO_IOCTL_OPTMOD_OPER_START_RE_CREATED				2
#define IO_IOCTL_OPTMOD_OPER_TERMNT_RE_DELETED              3

#define IO_IOCTL_OPTMOD_IS_CALIB_START						4
#define IO_IOCTL_OPTMOD_IS_CALIB_TERMINATE                  5

#define IO_IOCTL_OPTMOD_OPER_UNTIL_DONE						6
#define IO_IOCTL_OPTMOD_CALIB_READ_SAMPLE                   7
#define IO_IOCTL_OPTMOD_CALIB_UNTIL_DONE                    8
#define IO_IOCTL_OPTMOD_OPER_PARAMS_SET						9
#define IO_IOCTL_OPTMOD_GET_SETTINGS_EEPROM					0x0A
#define IO_IOCTL_OPER_STATUS_DURATION						0x0B
#define IO_IOCTL_OPER_STATUS_STATE							0x0C
#define IO_IOCTL_OPTMOD_INIT_CREATED						0x0D

#define IO_IOCTL_OPTMOD_INIT								0x1D
#define IO_IOCTL_OPTMOD_INIT_ADC							0x2D
#define IO_IOCTL_OPTMOD_INIT_DAC							0x3D

#define IO_ERROR_OPTMOD_OK									0x00//0x5A
#define IO_ERROR_OPTMOD_UNKNOWN_IOCTL						0x7F

/* Typedefs */

// OLD ALGORITHM

typedef struct {
	DAC7311_SAMPLE			dac;
	int32_t					adc;
} OPTMOD_CALIB_SAMPLE, * OPTMOD_CALIB_SAMPLE_PTR;

typedef struct {
	uint32_t				interval_ms;
	uint16_t				dac_count_lowest;
	uint16_t				dac_count_highest;
	uint16_t				dac_count_step;

	uint8_t					if_print_output;
} OPTMOD_CALIB_PARAMS, * OPTMOD_CALIB_PARAMS_PTR;

typedef struct {
	OPTMOD_CALIB_PARAMS		PARAMS;

	TaskHandle_t			task;
	uint32_t				samples_count;
	uint32_t				index;
} OPTMOD_CALIB, * OPTMOD_CALIB_PTR;

typedef struct {
	uint32_t				interval_ms;

	OPTMOD_CALIB_SAMPLE		DOT_START;
	OPTMOD_CALIB_SAMPLE		DOT_MIDDLE;
	OPTMOD_CALIB_SAMPLE		DOT_END;
	OPTMOD_CALIB_SAMPLE		Temp;

	int16_t					gain_percent;

	uint8_t					if_print;
} OPTMOD_OPER_PARAMS_OLD, * OPTMOD_OPER_PARAMS_OLD_PTR;


// NEW ALGORITHM

typedef struct {
	int32_t					trgt_pwr;			// target ADC value
	DAC7311_SAMPLE			dac_start;			// initial DAC value
	int16_t					direction;			// initial direction, either +1 or -1

	uint8_t					if_print;
} OPTMOD_OPER_PARAMS, * OPTMOD_OPER_PARAMS_PTR;

typedef enum {
	OPTMOD_OPER_STATE__OFF,
	OPTMOD_OPER_STATE__SEEK,
	OPTMOD_OPER_STATE__LOCK,
} OPTMOD_OPER_STATE;

typedef struct {
	OPTMOD_OPER_STATE 		state;
	int32_t					curr_pwr;
	int32_t					curr_error;
	TickType_t 				duration;					// duration of operation since start or set
	TickType_t 				seek_duration;				// duration of current SEEK state
} OPTMOD_OPER_STATUS, * OPTMOD_OPER_STATUS_PTR;

typedef enum {
	OPTMOD_OPER_TYPE__NONE,
	OPTMOD_OPER_TYPE__OLD,
	OPTMOD_OPER_TYPE__NEW
} OPTMOD_OPER_TYPE;

typedef struct {
	OPTMOD_OPER_TYPE		type;

	OPTMOD_OPER_PARAMS_OLD	PARAMS_OLD;
	OPTMOD_OPER_PARAMS		PARAMS;

	OPTMOD_OPER_STATUS		STATUS;

	TimerHandle_t 			oper_task_timer;
	TaskHandle_t			task;

	TickType_t				operation_start_ticks;		// operation start or set timestamp
	TickType_t				seek_start_ticks;			// last transition to SEEK timestamp


} OPTMOD_OPER, * OPTMOD_OPER_PTR;

typedef struct {
	uint32_t				samples_count;
	uint32_t				index;
}OPTMOD_STRUCT_TEMP_CAILB, *OPTMOD_STRUCT_TEMP_CAILB_PTR;

typedef struct {
	OPTMOD_CALIB			CALIB;
	OPTMOD_OPER				OPER;
	OPTMOD_CALIB_SAMPLE		CALIB_SAMPLE;

	OPTMOD_STRUCT_TEMP_CAILB TEMP_DATA;


	IO_HANDLER_PTR			OPTIC_DAC;
	IO_HANDLER_PTR			OPTIC_ADC;
	IO_HANDLER_PTR			SETTINGS_FILE;
	IO_HANDLER_PTR			STDOUT;
} OPTMOD_STRUCT, * OPTMOD_STRUCT_PTR;


IO_HANDLER_PTR			OPTMOD_SETTING_GLB;
IO_HANDLER_PTR			OPTMOD_OPTIC_DAC_GLB;
IO_HANDLER_PTR			OPTMOD_OPTIC_ADC_GLB;
IO_HANDLER_PTR			OPTMOD_STDOUT_GLB;
typedef struct {

	char 							*pchar_spare;

	uint32_t						u32_spare_0;
	uint32_t						u32_spare_1;
	uint32_t						u32_spare_2;
} OPTMOD_INIT_STRUCT, * OPTMOD_INIT_STRUCT_PTR;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif
 int32_t  install_optmod_func(OPTMOD_INIT_STRUCT_PTR OPTMod_Func_init);
//int32_t optmod_init(OPTMOD_STRUCT_PTR OPTMOD_GLB,IO_HANDLER_PTR OPTIC_ADC, IO_HANDLER_PTR OPTIC_DAC);
//int32_t optmod_calibration_start(OPTMOD_STRUCT_PTR OPTMOD, OPTMOD_CALIB_PARAMS_PTR params);
//int32_t optmod_calibration_terminate(OPTMOD_STRUCT_PTR OPTMOD);

int32_t optmod_operation_start_old(OPTMOD_STRUCT_PTR OPTMOD, OPTMOD_OPER_PARAMS_OLD_PTR params);
int32_t optmod_operation_set_old(OPTMOD_STRUCT_PTR OPTMOD, OPTMOD_CALIB_SAMPLE_PTR dot);
//int32_t optmod_operation_set(OPTMOD_STRUCT_PTR OPTMOD, OPTMOD_OPER_PARAMS_PTR params);
//int32_t optmod_operation_get_settings(OPTMOD_STRUCT_PTR OPTMOD, OPTMOD_OPER_PARAMS_PTR params);
int32_t optmod_set_stdout(OPTMOD_STRUCT_PTR OPTMOD, IO_HANDLER_PTR STDOUT);

#ifdef __cplusplus
}
#endif

#endif	// __OPTMOD_H__
