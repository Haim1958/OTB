/**
 ******************************************************************************
 * @file    i2c_io_isr.c
 * @brief   I2C IO driver Interrupt functions implementations
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

/* Includes */
#include <string.h>
#include <assert.h>
#include "UserLib.h"
#include "../inc/i2c_io.h"
#include "../inc/i2c_io_private.h"

/* Global variables */


/* Functions definitions */

/*******************************************************************************
Function Name     :     _i2c_io_irq
Parameters        :     None
Description       :     I2C1 IRQ handler function
********************************************************************************/
//void _i2c_io_irq(void* param)
//{
//	I2C_IO_DRIVER_PTR i2c_driver = (I2C_IO_DRIVER_PTR)param;
//	
//	i2c_driver->g.busy_flag = 0;
//}
