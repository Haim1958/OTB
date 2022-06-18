/**
 ******************************************************************************
 * @file    settings.h
 * @brief   SETTINGS driver public declarations and definitions
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


#ifndef __SETTINGS_H__
#define __SETTINGS_H__

/* Includes */

/* Definitions */

// Error definitions
#define IO_ERROR_SETTINGS__OK									IO_ERROR__OK
#define IO_ERROR_SETTINGS__OUT_OF_HEAP							IO_ERROR_SETTINGS(0x01)	
#define IO_ERROR_SETTINGS__UNKNOWN_IOCTL						IO_ERROR_SETTINGS(0x02)
#define IO_ERROR_SETTINGS__INIT_FAIL							IO_ERROR_SETTINGS(0x03)
#define IO_ERROR_SETTINGS__NUMBER_FILES_EXCEEDED_MAXIMUM		IO_ERROR_SETTINGS(0x04)
#define IO_ERROR_SETTINGS__FILE_TOO_BIG							IO_ERROR_SETTINGS(0x05)
#define IO_ERROR_SETTINGS__FILE_INVALID							IO_ERROR_SETTINGS(0x06)
#define IO_ERROR_SETTINGS__INVALID_PARAMS						IO_ERROR_SETTINGS(0x07)


// IOCTL definitions
#define IO_IOCTL_SETTINGS__IS_FILE_VALID						IO_IOCTL_SETTINGS(0x01)
#define IO_IOCTL_SETTINGS__PULL									IO_IOCTL_SETTINGS(0x02)

/* Typedefs */
typedef struct 
{
	uint32_t				max_number_files;
} SETTINGS_INSTALL_STRUCT, * SETTINGS_INSTALL_STRUCT_PTR;


// Initialization struct when driver is opened.
// Shall include all the required information for component initialization.
// low level drivers, port indexes.
typedef struct settings_init_struct
{
	struct
	{
		uint8_t				if_init;
		IO_HANDLER_PTR		eeprom_driver;
	} DRIVER;

	struct
	{
		uint8_t				if_init;
		uint32_t			size_number_pages;
	} INSTANCE;
} SETTINGS_INIT_STRUCT, * SETTINGS_INIT_STRUCT_PTR;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_settings(SETTINGS_INSTALL_STRUCT_PTR);



#ifdef __cplusplus
}
#endif


#endif	//__SETTINGS_IO_H__
