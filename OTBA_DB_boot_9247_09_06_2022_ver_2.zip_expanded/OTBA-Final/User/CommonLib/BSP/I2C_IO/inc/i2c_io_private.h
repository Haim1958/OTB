/**
 ******************************************************************************
 * @file    i2c_io_private.h
 * @brief   I2C IO private declarations and definitions
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
 
#ifndef __I2C_IO_PRIVATE_H__
#define __I2C_IO_PRIVATE_H__

/* Includes */

/* Definitions */

#define I2C_IO__MAX_NUM_OF_DRIVERS		(3u)


/* Typedefs */

typedef struct i2c_io_driver_phy {
	I2C_HandleTypeDef*				HAL_I2C;						// ST I2C driver handler
} I2C_IO_DRIVER_PHY, * I2C_IO_DRIVER_PHY_PTR;

typedef struct i2c_io_driver {
	uint32_t 						index;							// Driver index
	
	I2C_IO_DRIVER_PHY				PHY;
} I2C_IO_DRIVER, * I2C_IO_DRIVER_PTR;

typedef struct i2c_io_driver_bundle {
	I2C_IO_DRIVER_PTR				driver[I2C_IO__MAX_NUM_OF_DRIVERS];
} I2C_IO_DRIVER_BUNDLE, * I2C_IO_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif
	 
int32_t _i2c_io_open(IO_HANDLER_PTR, void*);
int32_t _i2c_io_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t _i2c_io_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t _i2c_io_ioctl(IO_HANDLER_PTR, uint32_t, void*);

int32_t _i2c_io_init_hardware(IO_HANDLER_PTR);
int32_t _i2c_io_init_logic(IO_HANDLER_PTR, I2C_IO_INIT_STRUCT_PTR);

void _i2c_io_irq(void* param);
void _i2c_io_handle_read_write(I2C_IO_DRIVER_BUNDLE_PTR);

//int32_t _i2c_io_pec_initialize_lut(I2C_IO_DRIVER_BUNDLE_PTR);
//uint8_t _i2c_io_pec_calculate(I2C_IO_DRIVER_BUNDLE_PTR, I2C_IO_TRANSACTION_PTR, uint8_t);

#ifdef __cplusplus
}
#endif

#endif //__I2C_IO_PRIVATE_H__
