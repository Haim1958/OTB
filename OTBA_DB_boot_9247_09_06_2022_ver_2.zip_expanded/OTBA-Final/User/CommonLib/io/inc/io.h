/**
 ******************************************************************************
 * @file    io.h
 * @brief   IO public definitions and includes
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

	 
	 
#ifndef __IO_H__
#define __IO_H__

/* Includes */

#include <stdint.h>
#include "./io_handler.h"
#include "./io_errors.h"
#include "./io_ioctl.h"

/* Typedefs */

#ifndef NULL
	#define NULL 0
#endif //NULL

/* Declarations */

int32_t io_install(IO_HANDLER_TYPE, IO_HANDLER_INSTALL_STRUCT_PTR);
IO_HANDLER_PTR io_open(IO_HANDLER_TYPE, void*);
int32_t io_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t io_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t io_ioctl(IO_HANDLER_PTR, uint32_t, void*);



#endif //__IO_H__
