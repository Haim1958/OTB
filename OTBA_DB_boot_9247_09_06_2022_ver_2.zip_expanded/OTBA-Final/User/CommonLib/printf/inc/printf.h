/**
 ******************************************************************************
 * @file    printf.h
 * @brief   Printf
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



#ifndef __PRINTF_H__
#define __PRINTF_H__


/* Includes */

#include "cmsis_os.h"

/* Definitions */
// Errors
#define PRINTF_ERROR__OK					ERROR_OK
#define PRINTF_ERROR__CANT_REGISTER_IO		PRINTF_ERROR(0x01)

#define MAX_PRINTF_DST						(2u)

/* Typedefs */


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t printf_register_io(IO_HANDLER_PTR io);

#ifdef __cplusplus
}
#endif

#endif // __PRINTF_H__
