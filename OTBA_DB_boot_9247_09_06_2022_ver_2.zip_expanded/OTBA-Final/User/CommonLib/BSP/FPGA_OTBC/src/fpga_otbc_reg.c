/**
 ******************************************************************************
 * @file    fpga_reg.c
 * @brief   FPGA Registers access implementations
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
#include "UserLib.h"
#include "../inc/fpga_otbc.h"
#include "../inc/fpga_otbc_private.h"
#include "../inc/fpga_otbc_reg.h"

/* Global variables */

/**
 * TODO
 *	@brief Initial register values
*/

		
		

/* Functions definitions */


/*! \brief Reads single register from the device
 *
 ###  Detailed description:
 *
 *  @param driver_bundle - FPGA driver bundle structure pointer.
 *  @param reg_addr - Register address.
 *  @param reg_val - Register value pointer
 *  @return **IO_ERROR_FPGA__OK** if the registers where read successfully
 *  @return **SPI Error** if SPI error occured
 */
int32_t _fpga_otbc_read_register(FPGA_OTBC_DRIVER_BUNDLE_PTR driver_bundle, uint8_t reg_addr, uint8_t *reg_val)
{
	uint8_t temp[2];
	int32_t err;

	temp[0] = reg_addr | FPGA_COMMAND__RREG;
	temp[1] = 0;		// dummy

	err = io_write(driver_bundle->PHY.driver, temp, 2);
	if (IO_ERROR_SPI_IO__OK != err)
	{
		return err;
	}

	// Read the SPI data
	err = io_read(driver_bundle->PHY.driver, temp, 2);

	*reg_val = temp[1];

	return err;
}


/*! \brief Writes single register to the device
 *
 ###  Detailed description:
 *
 *  @param driver_bundle - FPGA driver bundle structure pointer.
 *  @param reg_addr - Register address.
 *  @param reg_val - Register value
 *  @return **IO_ERROR_FPGA__OK** if the registers where written successfully
 *  @return **SPI Error** if SPI error occured
 */
int32_t _fpga_otbc_write_register(FPGA_OTBC_DRIVER_BUNDLE_PTR driver_bundle, uint8_t reg_addr, uint8_t reg_val)
{
	uint8_t temp[2];

	temp[0] = reg_addr | FPGA_COMMAND__WREG;
	temp[1] = reg_val;

	return io_write(driver_bundle->PHY.driver, temp, 2);
}
