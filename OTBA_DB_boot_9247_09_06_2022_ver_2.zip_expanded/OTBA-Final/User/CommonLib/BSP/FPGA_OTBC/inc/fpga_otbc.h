/**
 ******************************************************************************
 * @file    fpga_otbc.h
 * @brief   FPGA public declarations and definitions
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

#ifndef __FPGA_OTBC_H__
#define __FPGA_OTBC_H__

/* Includes */

#include "cmsis_os.h"
#include "fpga_otbc_reg.h"

/* Definitions */

// Error definitions
#define IO_ERROR_FPGA_OTBC__OK									IO_ERROR__OK
#define IO_ERROR_FPGA_OTBC__OUT_OF_HEAP							IO_ERROR_FPGA_OTBC(0x01)
#define IO_ERROR_FPGA_OTBC__UNKNOWN_IOCTL						IO_ERROR_FPGA_OTBC(0x02)
#define IO_ERROR_FPGA_OTBC__NOT_IMPLEMENTED						IO_ERROR_FPGA_OTBC(0x03)
#define IO_ERROR_FPGA_OTBC__HW_TEST_FAILED						IO_ERROR_FPGA_OTBC(0x04)

// IOCTL definitions				
#define IO_IOCTL_FPGA_OTBC__WRITE_REG							IO_IOCTL_FPGA_OTBC(0x01)
#define IO_IOCTL_FPGA_OTBC__READ_REG							IO_IOCTL_FPGA_OTBC(0x02)
#define IO_IOCTL_FPGA_OTBC__TEST								IO_IOCTL_FPGA_OTBC(0x03)
#define IO_IOCTL_FPGA_OTBC__GET_HW_REV							IO_IOCTL_FPGA_OTBC(0x04)
#define IO_IOCTL_FPGA_OTBC__GET_SFP_STATUS						IO_IOCTL_FPGA_OTBC(0x05)


/* Typedefs */


typedef struct {
	uint32_t temp;
} FPGA_OTBC_INSTALL_STRUCT, * FPGA_OTBC_INSTALL_STRUCT_PTR;


typedef struct {
	IO_HANDLER_PTR			driver;				// Low layer driver
} FPGA_OTBC_INIT_STRUCT, * FPGA_OTBC_INIT_STRUCT_PTR;


typedef struct {
	uint8_t address;
	uint8_t value;
} FPGA_OTBC_RW_REG, * FPGA_OTBC_RW_REG_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_fpga_otbc(FPGA_OTBC_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif


#endif	//__FPGA_OTBC_H__
