/**
 ******************************************************************************
 * @file    fpga_otba.h
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

#ifndef __FPGA_OTBA_H__
#define __FPGA_OTBA_H__

/* Includes */

#include "cmsis_os.h"
#include "fpga_otba_reg.h"

/* Definitions */

// Error definitions
#define IO_ERROR_FPGA_OTBA__OK									IO_ERROR__OK
#define IO_ERROR_FPGA_OTBA__OUT_OF_HEAP							IO_ERROR_FPGA_OTBA(0x01)
#define IO_ERROR_FPGA_OTBA__UNKNOWN_IOCTL						IO_ERROR_FPGA_OTBA(0x02)
#define IO_ERROR_FPGA_OTBA__NOT_IMPLEMENTED						IO_ERROR_FPGA_OTBA(0x03)
#define IO_ERROR_FPGA_OTBA__HW_TEST_FAILED						IO_ERROR_FPGA_OTBA(0x04)
#define IO_ERROR_FPGA_OTBA__STATE_CHANGE_FAILED					IO_ERROR_FPGA_OTBA(0x05)

// IOCTL definitions				
#define IO_IOCTL_FPGA_OTBA__WRITE_REG							IO_IOCTL_FPGA_OTBA(0x01)
#define IO_IOCTL_FPGA_OTBA__READ_REG							IO_IOCTL_FPGA_OTBA(0x02)
#define IO_IOCTL_FPGA_OTBA__TEST								IO_IOCTL_FPGA_OTBA(0x03)
#define IO_IOCTL_FPGA_OTBA__GET_HW_REV							IO_IOCTL_FPGA_OTBA(0x04)
#define IO_IOCTL_FPGA_OTBA__WRITE_OUTPUT_DISCRETES				IO_IOCTL_FPGA_OTBA(0x05)
#define IO_IOCTL_FPGA_OTBA__READ_INPUT_DISCRETES				IO_IOCTL_FPGA_OTBA(0x06)
#define IO_IOCTL_FPGA_OTBA__SET_STATE							IO_IOCTL_FPGA_OTBA(0x07)
#define IO_IOCTL_FPGA_OTBA__ENABLE_DIS_SIG						IO_IOCTL_FPGA_OTBA(0x08)
#define IO_IOCTL_FPGA_OTBA__DISABLE_DIS_SIG						IO_IOCTL_FPGA_OTBA(0x09)
#define IO_IOCTL_FPGA_OTBA__READ_INPUT_DISCRETES_TEST			IO_IOCTL_FPGA_OTBA(0x0A)

/* Typedefs */


typedef struct {
	uint32_t temp;
} FPGA_OTBA_INSTALL_STRUCT, * FPGA_OTBA_INSTALL_STRUCT_PTR;


typedef struct {
	IO_HANDLER_PTR			driver;				// Low layer driver
} FPGA_OTBA_INIT_STRUCT, * FPGA_OTBA_INIT_STRUCT_PTR;


typedef struct {
	uint8_t address;
	uint8_t value;
} FPGA_OTBA_RW_REG, * FPGA_OTBA_RW_REG_PTR;

typedef enum
{
	FPGA_State__INIT = 0,
	FPGA_State__IDLE = 1,
	FPGA_State__LASER_OFF = 2,
	FPGA_State__IBIT = 3,
	FPGA_State__OPERATE = 4
}FPGA_State;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_fpga_otba(FPGA_OTBA_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif


#endif	//__FPGA_OTBA_H__
