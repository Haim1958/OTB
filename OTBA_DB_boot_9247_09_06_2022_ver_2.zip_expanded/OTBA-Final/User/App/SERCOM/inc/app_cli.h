/**
 ******************************************************************************
 * @file    app_cli.h
 * @brief   OTS application CLI public declarations and definitions
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


#ifndef __APP_CLI_H__
#define __APP_CLI_H__




/* Includes */


/* Definitions */


#define OTS_APP_CLI_OPCODE__SPI_WRITE		(0x10)
#define OTS_APP_CLI_OPCODE__SPI_READ		(0x11)
#define OTS_APP_CLI_OPCODE__SPI_DATA		(0x12)

#define OTS_APP_CLI_OPCODE__PORT_WRITE		(0x20)
#define OTS_APP_CLI_OPCODE__PORT_READ		(0x21)
#define OTS_APP_CLI_OPCODE__PORT_DATA		(0x22)

#define OTS_APP_CLI_OPCODE__FPGA_REG_WRITE	(0x30)
#define OTS_APP_CLI_OPCODE__FPGA_REG_READ	(0x31)


/* Declarations */

int32_t app_cli_handle_comm(OTS_APPLICATION_BUNDLE_PTR OTS_APP);


#endif // __APP_CLI_H__
