/**
 ******************************************************************************
 * @file    io_read.c
 * @brief   IO Read implementation
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

int32_t io_read
	(
	 	IO_HANDLER_PTR	handler,
		uint8_t*		ptr,
		uint32_t		length
	)
{
	int32_t err;
	
	if (NULL == handler)
	{
		return IO_ERROR_HANDLER__NULL;
	}
	
	/* Wait for handler to be unlocked */
//	while (handler->lock != IO_UNLOCK) {}

	/* Execute the function */
//	handler->lock = IO_LOCK;
	err = handler->read_fcn(handler, ptr, length);
//	handler->lock = IO_UNLOCK;
	
	return err;
}
