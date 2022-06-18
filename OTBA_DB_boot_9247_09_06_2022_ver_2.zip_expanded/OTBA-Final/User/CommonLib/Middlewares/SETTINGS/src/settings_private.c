/**
 ******************************************************************************
 * @file    settings_private.c
 * @brief   SETTINGS Private driver functions implementations
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
#include <stdlib.h>
#include <string.h>
#include "UserLib.h"
#include "../inc/settings_private.h"

/* Global variables */



/* Functions definitions */

static uint16_t _settings_crc16_compute(uint8_t * p_data, uint32_t size)
{
    uint16_t crc = 0;

    for (uint32_t i = 0; i < size; i++)    
	{
        crc  = (uint8_t)(crc >> 8) | (crc << 8);
        crc ^= p_data[i];
        crc ^= (uint8_t)(crc & 0xFF) >> 4;
        crc ^= (crc << 8) << 4;
        crc ^= ((crc & 0xFF) << 4) << 1;
    }

    return crc;
}


static int32_t _settings_file_validate(SETTINGS_FILE_PTR settings_file)
{
	SETTINGS_IO_ENTRY_HEADER_PTR header = (SETTINGS_IO_ENTRY_HEADER_PTR) settings_file->BUFFER;
	uint8_t *entry = settings_file->BUFFER + sizeof(SETTINGS_IO_ENTRY_HEADER);
	uint16_t crc = 0;

	settings_file->is_valid = 0;

	if (header == NULL)
	{
		return IO_ERROR_SETTINGS__FILE_INVALID;
	}

	if (header->HEADER != ENTRY_HEADER_VALUE)
	{
		return IO_ERROR_SETTINGS__FILE_INVALID;
	}

	crc = _settings_crc16_compute(entry, header->entry_size);
	if (crc != header->crc)
	{
		return IO_ERROR_SETTINGS__FILE_INVALID;
	}

	settings_file->is_valid = 1u;

	return IO_ERROR_SETTINGS__OK;
}


int32_t _settings_pull(SETTINGS_DRIVER_BUNDLE_PTR settings_bundle, SETTINGS_FILE_PTR settings_file)
{
	uint32_t seek_address;
	int32_t err = 0;

	memset(settings_file->BUFFER, 0, settings_file->size_bytes);
	settings_file->is_valid = 0;

	seek_address = SETTINGS_PAGE_SIZE_BYTES * settings_file->first_page;
	err = io_ioctl(settings_bundle->G.eeprom_driver, IO_IOCTL_M24512__LOCK_AND_SEEK, &seek_address);
	if(err != IO_ERROR_M24512__OK)
	{
		return err;
	}

	err = io_read(settings_bundle->G.eeprom_driver, settings_file->BUFFER, settings_file->size_bytes);
	if(err != IO_ERROR_M24512__OK)
	{
		return err;
	}
	
	io_ioctl(settings_bundle->G.eeprom_driver, IO_IOCTL_M24512__UNLOCK, NULL);

	return _settings_file_validate(settings_file);
}


int32_t _settings_push(SETTINGS_DRIVER_BUNDLE_PTR settings_bundle, SETTINGS_FILE_PTR settings_file)
{
	SETTINGS_IO_ENTRY_HEADER_PTR header = (SETTINGS_IO_ENTRY_HEADER_PTR) settings_file->BUFFER;
	uint8_t *entry = settings_file->BUFFER + sizeof(SETTINGS_IO_ENTRY_HEADER);
	uint32_t seek_address;
	int32_t err = 0;

	// Make sure header is OK. The length shall be set ahead.
	header->HEADER = ENTRY_HEADER_VALUE;
	header->crc = _settings_crc16_compute(entry, header->entry_size);

	seek_address = SETTINGS_PAGE_SIZE_BYTES * settings_file->first_page;
	err = io_ioctl(settings_bundle->G.eeprom_driver, IO_IOCTL_M24512__LOCK_AND_SEEK, &seek_address);
	if(err != IO_ERROR_M24512__OK)
	{
		return err;
	}

	err = io_write(settings_bundle->G.eeprom_driver, settings_file->BUFFER, settings_file->size_bytes);
	if(err != IO_ERROR_M24512__OK)
	{
		return err;
	}

	io_ioctl(settings_bundle->G.eeprom_driver, IO_IOCTL_M24512__UNLOCK, NULL);

	return IO_ERROR_SETTINGS__OK;
}


int32_t _settings_write_data(SETTINGS_DRIVER_BUNDLE_PTR settings_bundle, SETTINGS_FILE_PTR settings_file, uint8_t *ptr, uint32_t length)
{
	SETTINGS_IO_ENTRY_HEADER_PTR header = (SETTINGS_IO_ENTRY_HEADER_PTR) settings_file->BUFFER;
	uint8_t *entry = settings_file->BUFFER + sizeof(SETTINGS_IO_ENTRY_HEADER);

	header->entry_size = length;
	memcpy(entry, ptr, length);

	return IO_ERROR_SETTINGS__OK;
}

