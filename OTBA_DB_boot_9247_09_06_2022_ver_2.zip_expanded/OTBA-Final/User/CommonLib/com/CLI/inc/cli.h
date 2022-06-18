/**
 ******************************************************************************
 * @file    cli.h
 * @brief   CLI public declarations and definitions
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



#ifndef __CLI_H__
#define __CLI_H__


/* Includes */

#include "cmsis_os.h"

/* Definitions */
#define IO_ERROR_CLI__OK										ERROR_OK
#define IO_ERROR_CLI__NULL										IO_ERROR_CLI(0x01)
#define IO_ERROR_CLI__UNDEFINED_TYPE							IO_ERROR_CLI(0x02)
#define IO_ERROR_CLI__OUT_OF_HEAP								IO_ERROR_CLI(0x03)
#define IO_ERROR_CLI__UNKNOWN_IOCTL								IO_ERROR_CLI(0x04)
#define IO_ERROR_CLI__INITIALIZATION_ERROR						IO_ERROR_CLI(0x05)
#define IO_ERROR_CLI__COMMAND_ALREADY_READ						IO_ERROR_CLI(0x06)
#define IO_ERROR_CLI__COMMAND_NOT_READY							IO_ERROR_CLI(0x07)
#define IO_ERROR_CLI__NO_MORE_TOKENS_FOUND						IO_ERROR_CLI(0x08)
#define IO_ERROR_CLI__BAD_COMMAND								IO_ERROR_CLI(0x09)


#define IO_IOCTL_CLI__PURGE_COMMAND								IO_IOCTL_CLI(1)

#define CLI_LINE_MAX_LENGTH		(80u)


/* Typedefs */

typedef struct {
	uint32_t	total_number_instances;
} CLI_INSTALL_STRUCT, * CLI_INSTALL_STRUCT_PTR;


typedef struct {
	uint32_t	temp;
} CLI_INIT_STRUCT, * CLI_INIT_STRUCT_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_cli(CLI_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif // __CLI_H__
