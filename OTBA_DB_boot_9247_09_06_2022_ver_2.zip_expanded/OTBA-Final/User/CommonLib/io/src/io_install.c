/**
 ******************************************************************************
 * @file    io_install.c
 * @brief   IO Install implementation
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



#include "../inc/io.h"

int32_t io_install
	(
	 	IO_HANDLER_TYPE						type,
		IO_HANDLER_INSTALL_STRUCT_PTR		init
	)
{
	if (IO_HANDLER_TYPE_COUNT <= type)
	{
		return IO_ERROR_HANDLER__UNDEFINED_TYPE;
	}
	
	_io_handlers[type].type			= type;
//	_io_handlers[type].lock			= IO_UNLOCK;
	_io_handlers[type].open_fcn		= init->open_fcn;
	_io_handlers[type].read_fcn		= init->read_fcn;
	_io_handlers[type].write_fcn	= init->write_fcn;
	_io_handlers[type].ioctl_fcn	= init->ioctl_fcn;
	_io_handlers[type].bundle		= init->bundle;

	return IO_ERROR_HANDLER__OK;
}
