/**
 ******************************************************************************
 * @file    printf.c
 * @brief   OTS printf implementation.
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
#include "cmsis_os.h"
#include "UserLib.h"


static IO_HANDLER_PTR DST[MAX_PRINTF_DST] = {NULL};


int32_t printf_register_io(IO_HANDLER_PTR io)
{
	uint32_t i, null_pos = MAX_PRINTF_DST;

	for (i = 0; i < MAX_PRINTF_DST; i++)
	{
		// Find first NULL position
		if (DST[i] == NULL && null_pos >= MAX_PRINTF_DST)
		{
			null_pos = i;
		}

		if (DST[i] == io)
		{
			// already registered
			return PRINTF_ERROR__OK;
		}
	}

	if (null_pos >= MAX_PRINTF_DST)
	{
		return PRINTF_ERROR__CANT_REGISTER_IO;
	}

	DST[null_pos] = io;

	return PRINTF_ERROR__OK;
}


int _write(int file, char *ptr, int len)
{
	uint32_t io_index;
	IO_HANDLER_PTR io;

	for (io_index = 0; io_index < MAX_PRINTF_DST; io_index++)
	{
		io = DST[io_index];
		io_write(io, (uint8_t*)ptr, len);
	}

  return len;
}
