/**
 ******************************************************************************
 * @file    app_rf.h
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


#ifndef _APP_RF_H_
#define _APP_RF_H_


/* Definitions */

#define RF_TABLE_NUMBER_TABLES	(1u)

#define RF_TABLE_NUMBER_ROWS	(6u)
#if (RF_TABLE_NUMBER_ROWS > RF_TABLE_MAX_NUMBER_ROWS)
#error "RF_TABLE_NUMBER_ROWS shall be less than or equal to RF_TABLE_MAX_NUMBER_ROWS"
#endif

// Tables names
#define TABLE_1					"T1"

// Rows names
#define ROW_VG1_A1				"VG1_A1"
#define ROW_VC_A1				"VC_A1"
#define ROW_VG1_A2				"VG1_A2"
#define ROW_VC_A2				"VC_A2"
#define ROW_VG1_A3				"VG1_A3"
#define ROW_VC_A3				"VC_A3"

extern const char* const RF_TABLE_ROW_LUT[RF_TABLE_NUMBER_ROWS];

// DAC Channels
#define CHANNEL_VG1_A1			"3"
#define CHANNEL_VC_A1			"2"
#define CHANNEL_VG1_A2			"0"
#define CHANNEL_VC_A2			"1"
#define CHANNEL_VG1_A3			"4"
#define CHANNEL_VC_A3			"5"

extern const char* const RF_TABLE_CHANNEL_LUT[RF_TABLE_NUMBER_ROWS];

// Columns names
#define COLUMN_40_DEG			"-40"
#define COLUMN_25_DEG			"25"
#define COLUMN_85_DEG			"85"

extern const char* const RF_TABLE_COLUMN_LUT[RF_TABLE_NUMBER_COLUMNS];

// Temperatures
#define TEMP_COLUMN_0			(-40.0f)
#define TEMP_COLUMN_1			(25.0f)
#define TEMP_COLUMN_2			(85.0f)


#ifdef __cplusplus
 extern "C" {
#endif


int32_t app_rf_init(OTS_APPLICATION_BUNDLE_PTR OTS_APP, IO_HANDLER_PTR RF_DAC, IO_HANDLER_PTR TEMP_SENSOR);
int32_t app_rf_table_find_table(const char* table);
int32_t app_rf_table_find_row(const char* row);
int32_t app_rf_table_find_column(const char* row);
int32_t CLI_Read_temperature_from_DAC(OTS_APPLICATION_BUNDLE_PTR OTS_APP);



#ifdef __cplusplus
 }
#endif
#endif /* _APP_RF_H_ */
