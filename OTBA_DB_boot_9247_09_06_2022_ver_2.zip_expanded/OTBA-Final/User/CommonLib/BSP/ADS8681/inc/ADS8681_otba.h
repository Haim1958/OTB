/**
 ******************************************************************************
 * @file    ADS8681_otba.h
 * @brief   ADS8681 public declarations and definitions
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

#ifndef __ADS8681_OTBA_H__
#define __ADS8681_OTBA_H__

/* Includes */

#include "cmsis_os.h"
#include "ADS8681_otba_reg.h"

/* Definitions */

// Error definitions
#define IO_ERROR_ADS8681_OTBA__OK									IO_ERROR__OK
#define IO_ERROR_ADS8681_OTBA__OUT_OF_HEAP							IO_ERROR_ADS8681_OTBA(0x01)
#define IO_ERROR_ADS8681_OTBA__UNKNOWN_IOCTL						IO_ERROR_ADS8681_OTBA(0x02)
#define IO_ERROR_ADS8681_OTBA__NOT_IMPLEMENTED						IO_ERROR_ADS8681_OTBA(0x03)
#define IO_ERROR_ADS8681_OTBA__HW_TEST_FAILED						IO_ERROR_ADS8681_OTBA(0x04)
#define IO_ERROR_ADS8681_OTBA__STATE_CHANGE_FAILED					IO_ERROR_ADS8681_OTBA(0x05)

// IOCTL definitions				
#define IO_IOCTL_ADS8681_OTBA__WRITE_REG							IO_IOCTL_ADS8681_OTBA(0x01)
#define IO_IOCTL_ADS8681_OTBA__READ_REG							IO_IOCTL_ADS8681_OTBA(0x02)
#define IO_IOCTL_ADS8681_OTBA__TEST								IO_IOCTL_ADS8681_OTBA(0x03)
#define IO_IOCTL_ADS8681_OTBA__GET_HW_REV							IO_IOCTL_ADS8681_OTBA(0x04)
#define IO_IOCTL_ADS8681_OTBA__WRITE_OUTPUT_DISCRETES				IO_IOCTL_ADS8681_OTBA(0x05)
#define IO_IOCTL_ADS8681_OTBA__READ_INPUT_DISCRETES				IO_IOCTL_ADS8681_OTBA(0x06)
#define IO_IOCTL_ADS8681_OTBA__SET_STATE							IO_IOCTL_ADS8681_OTBA(0x07)
#define IO_IOCTL_ADS8681_OTBA__ENABLE_DIS_SIG						IO_IOCTL_ADS8681_OTBA(0x08)
#define IO_IOCTL_ADS8681_OTBA__DISABLE_DIS_SIG						IO_IOCTL_ADS8681_OTBA(0x09)
#define IO_IOCTL_ADS8681_OTBA__READ_INPUT_DISCRETES_TEST			IO_IOCTL_ADS8681_OTBA(0x0A)

/* Typedefs */


typedef struct {
	uint32_t temp;
} ADS8681_OTBA_INSTALL_STRUCT, * ADS8681_OTBA_INSTALL_STRUCT_PTR;


typedef struct {
	IO_HANDLER_PTR			driver;				// Low layer driver
} ADS8681_OTBA_INIT_STRUCT, * ADS8681_OTBA_INIT_STRUCT_PTR;


typedef struct {
	uint8_t address;
	uint32_t value;
} ADS8681_OTBA_RW_REG, * ADS8681_OTBA_RW_REG_PTR;

typedef enum
{
	ADS8681_State__INIT = 0,
	ADS8681_State__IDLE = 1,
	ADS8681_State__LASER_OFF = 2,
	ADS8681_State__IBIT = 3,
	ADS8681_State__OPERATE = 4
}ADS8681_State;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_ADS8681_otba(ADS8681_OTBA_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif


#endif	//__ADS8681_OTBA_H__
