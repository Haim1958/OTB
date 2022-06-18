/**
 ******************************************************************************
 * @file    settings_private.h
 * @brief   SETTINGS Private driver public declarations and definitions
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


#ifndef __SETTINGS_PRIVATE_H__
#define __SETTINGS_PRIVATE_H__

/* Includes */


/* Definitions */


#define ENTRY_HEADER_VALUE								(0xA581)

#define SETTINGS_PAGE_SIZE_BYTES						(M24512_PAGE_SIZE_BYTES)
#define SETTINGS_NUMBER_PAGES							(M24512_NUMBER_PAGES)

/* Typedefs */

#pragma pack(push, 0)
typedef struct settings_io_entry_header
{
    uint16_t	HEADER;
	uint16_t	entry_size;
	uint16_t	crc;
} SETTINGS_IO_ENTRY_HEADER, * SETTINGS_IO_ENTRY_HEADER_PTR;
#pragma pack(pop)


typedef struct settings_io_entry
{
	SETTINGS_IO_ENTRY_HEADER	header;
	uint8_t*					data;
} SETTINGS_IO_ENTRY, * SETTINGS_IO_ENTRY_PTR;


typedef struct settings_driver_phy
{
    IO_HANDLER_PTR				eeprom_driver;
} SETTINGS_DRIVER_GENERIC, * SETTINGS_DRIVER_GENERIC_PTR;


typedef struct settings_file
{
	uint32_t					first_page;
	uint32_t					size_number_pages;
	uint32_t					size_bytes;
	uint8_t						is_valid;

	uint8_t						*BUFFER;
} SETTINGS_FILE, * SETTINGS_FILE_PTR;


typedef struct settings_driver_bundle
{
	SETTINGS_DRIVER_GENERIC		G;

	uint32_t					number_files;
	SETTINGS_FILE_PTR			*FILES;
} SETTINGS_DRIVER_BUNDLE, * SETTINGS_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif


int32_t _settings_pull(SETTINGS_DRIVER_BUNDLE_PTR settings_bundle, SETTINGS_FILE_PTR settings_file);
int32_t _settings_push(SETTINGS_DRIVER_BUNDLE_PTR settings_bundle, SETTINGS_FILE_PTR settings_file);
int32_t _settings_write_data(SETTINGS_DRIVER_BUNDLE_PTR settings_bundle, SETTINGS_FILE_PTR settings_file, uint8_t *ptr, uint32_t length);


#ifdef __cplusplus
}
#endif


#endif	//__SETTINGS_PRIVATE_H__
