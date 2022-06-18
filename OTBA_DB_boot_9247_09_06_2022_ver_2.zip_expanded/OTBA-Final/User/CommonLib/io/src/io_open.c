/**
 ******************************************************************************
 * @file    io_open.c
 * @brief   IO Open implementation
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
#include "../inc/io.h"

IO_HANDLER_PTR io_open
	(
	 	IO_HANDLER_TYPE						type,
		void*								init
	)
{
	IO_HANDLER_PTR new_handler;
	int32_t err = IO_ERROR_HANDLER__OK;

	if (IO_HANDLER_TYPE_COUNT <= type)
	{
		return NULL;
	}
	
	/* Allocate a new handler */
	new_handler = (IO_HANDLER_PTR) pvPortMalloc(sizeof(IO_HANDLER));
	if (NULL == new_handler)
	{
		return NULL;
	}
	
	/* Copy saved install information */
	memcpy(new_handler, &_io_handlers[type], sizeof(IO_HANDLER));

	err = new_handler->open_fcn(new_handler, init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		/*
		*	SET_GLOBAL_ERROR(err)
		*/
		
		return NULL;
	}
	else
	{
//		new_handler->lock = IO_UNLOCK;
		return new_handler;
	}
}
