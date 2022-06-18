/**
 ******************************************************************************
 * @file    i2c_io.h
 * @brief   I2C IO public declarations and definitions
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

#ifndef __I2C_IO_H__
#define __I2C_IO_H__

/* Includes */



/* Definitions */

// Error definitions
#define IO_ERROR_I2C_IO__OK							IO_ERROR__OK
#define IO_ERROR_I2C_IO__OUT_OF_HEAP				IO_ERROR_I2C_IO(0x01)
#define IO_ERROR_I2C_IO__UNKNOWN_HAL_DRIVER			IO_ERROR_I2C_IO(0x02)
#define IO_ERROR_I2C_IO__DRV_INDEX_OUT_OF_RANGE		IO_ERROR_I2C_IO(0x03)
#define IO_ERROR_I2C_IO__UNKNOWN_IOCTL				IO_ERROR_I2C_IO(0x04)
#define IO_ERROR_I2C_IO__DATA_TOO_LONG				IO_ERROR_I2C_IO(0x05)
#define IO_ERROR_I2C_IO__BUSY						IO_ERROR_I2C_IO(0x06)
#define IO_ERROR_I2C_IO__FAILED_HAL_INIT			IO_ERROR_I2C_IO(0x07)
#define IO_ERROR_I2C_IO__INVALID_PEC_ACQUIRED		IO_ERROR_I2C_IO(0x08)
#define IO_ERROR_I2C_IO__TIMEOUT					IO_ERROR_I2C_IO(0x09)
#define IO_ERROR_I2C_IO__HAL_ERROR					IO_ERROR_I2C_IO(0x0A)


// IOCTL definitions
//#define IO_IOCTL_I2C_IO__MODIFY_CLOCK				IO_IOCTL_I2C_IO(1)


#define I2C_IO_ADD_SIZE_8_BIT						(I2C_MEMADD_SIZE_8BIT)
#define I2C_IO_ADD_SIZE_16_BIT						(I2C_MEMADD_SIZE_16BIT)


/* Typedefs */

typedef struct {
	uint32_t				temp;
} I2C_IO_INSTALL_STRUCT, * I2C_IO_INSTALL_STRUCT_PTR;


typedef struct {
	uint8_t					index;				// I2C driver index
	I2C_HandleTypeDef*		HAL_I2C;			// ST HAL handler of I2C
} I2C_IO_INIT_STRUCT, * I2C_IO_INIT_STRUCT_PTR;


typedef struct {
	uint8_t					DEV;				// I2C Slave Device address
	uint8_t					ADD_SIZE;			// Address size. Either I2C_IO_ADD_SIZE_8_BIT or I2C_IO_ADD_SIZE_16_BIT
	uint16_t				ADD;				// Memory address to access
	uint8_t					LEN;				// Payload size
	uint8_t					*VAL_PTR;			// RX/TX payload ptr
} I2C_IO_TRANSACTION, * I2C_IO_TRANSACTION_PTR;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_i2c_io(I2C_IO_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif


#endif	//__I2C_IO_H__
