/**
 ******************************************************************************
 * @file    fpga_otbc.c
 * @brief   FPGA IO functions implementations
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


#include <string.h>
#include "UserLib.h"
#include "../inc/fpga_otbc.h"
#include "../inc/fpga_otbc_private.h"
#include "../inc/fpga_otbc_reg.h"

/* Global variables */

/* Functions definitions */

static int32_t _fpga_otbc_open(IO_HANDLER_PTR, void*);
static int32_t _fpga_otbc_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _fpga_otbc_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _fpga_otbc_ioctl(IO_HANDLER_PTR, uint32_t, void*);


/*! \brief Installs the FPGA driver
 *
 ###  Detailed description:
 *
 *  @param fpga_init - pointer to the FPGA installation struct.
 *  @return **IO_ERROR_FPGA__OK** if the installation was OK
 *  @return **IO_ERROR_FPGA__OUT_OF_HEAP** not enough space in heap for allocations
 */
int32_t install_fpga_otbc
	(
		FPGA_OTBC_INSTALL_STRUCT_PTR fpga_init
	)
{
	FPGA_OTBC_DRIVER_BUNDLE_PTR driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_fpga_otbc_open,
											_fpga_otbc_read,
											_fpga_otbc_write,
											_fpga_otbc_ioctl
										};
	int32_t err;
	
	/* Allocate driver bundle */
	driver_bundle = (FPGA_OTBC_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(FPGA_OTBC_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_FPGA_OTBC__OUT_OF_HEAP;
	}
	
	memset(driver_bundle, 0, sizeof(FPGA_OTBC_DRIVER_BUNDLE));
	
	init.bundle = driver_bundle;

	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__FPGA_OTBC, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}

	return IO_ERROR_FPGA_OTBC__OK;
}


/*! \brief Opens and initializes the FPGA driver
 *
 ###  Detailed description:
 *
 *  @param handler - FPGA driver handler.
 *  @param init - initialization struct pointer.
 *  @return **IO_ERROR_FPGA__OK** if the initialization was OK
 *  @return **Error** if another error
 */
static int32_t _fpga_otbc_open(IO_HANDLER_PTR handler, void * init)
{
	FPGA_OTBC_DRIVER_BUNDLE_PTR driver_bundle = (FPGA_OTBC_DRIVER_BUNDLE_PTR)handler->bundle;
	FPGA_OTBC_INIT_STRUCT_PTR fpga_init = (FPGA_OTBC_INIT_STRUCT_PTR) init;
	
	driver_bundle->PHY.driver = fpga_init->driver;
	
	return IO_ERROR_FPGA_OTBC__OK;
}



/*! \brief Reads a sample from FPGA driver
 *
 ###  Detailed description:
 *
 *  @param handler - FPGA driver handler.
 *  @param ptr - Destination buffer for the sample.
 *  @param samples - Number of samples to read. Not used currently. Set to 1.
 *  @return **IO_ERROR_FPGA__OK** if the initialization was OK
 *  @return **SPI Error** if SPI error occured
 *  @return **IO_ERROR_FPGA__INVALID_SAMPLE_OUTPUT_MODE** if invalid output mode is set
 */
static int32_t _fpga_otbc_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t samples)
{
//	FPGA_OTBC_DRIVER_BUNDLE_PTR driver_bundle = (FPGA_OTBC_DRIVER_BUNDLE_PTR)handler->bundle;
	
	return IO_ERROR_FPGA_OTBC__NOT_IMPLEMENTED;
}


/*! \brief Writes to FPGA driver
 *
 ###  Detailed description:
 *  @remark Currently not in use
 *
 *  @param handler - FPGA driver handler.
 *  @param ptr - Buffer pointer of data to be written.
 *  @param length - Length of the data to be written.
 *  @return **IO_ERROR_FPGA__OK** always
 */
static int32_t _fpga_otbc_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	return IO_ERROR_FPGA_OTBC__NOT_IMPLEMENTED;
}


/*! \brief Executes IOCTL command
 *
 ###  Detailed description:
 *
 *  @param handler - FPGA driver handler.
 *  @param ptr - Buffer pointer of data to be written.
 *  @param length - Length of the data to be written.
 *  @return **IO_ERROR_FPGA__OK** if command executed ok
 *  @return **IO_ERROR_FPGA__UNKNOWN_IOCTL** if requested command is not supported
 */
static int32_t _fpga_otbc_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	FPGA_OTBC_DRIVER_BUNDLE_PTR driver_bundle = (FPGA_OTBC_DRIVER_BUNDLE_PTR)handler->bundle;
	FPGA_OTBC_RW_REG_PTR reg;
	uint8_t temp_u8;
	int32_t err = IO_ERROR_FPGA_OTBC__OK;
	
	switch (ctl)
	{
		case IO_IOCTL_FPGA_OTBC__WRITE_REG:
			reg = (FPGA_OTBC_RW_REG_PTR)ptr;
			err = _fpga_otbc_write_register(driver_bundle, reg->address, reg->value);
			break;

		case IO_IOCTL_FPGA_OTBC__READ_REG:
			reg = (FPGA_OTBC_RW_REG_PTR)ptr;
			err = _fpga_otbc_read_register(driver_bundle, reg->address, &reg->value);
			break;

		case IO_IOCTL_FPGA_OTBC__TEST:
			err = _fpga_otbc_write_register(driver_bundle, FPGA_OTBC_REG__TEST, 0xA1);
			if (err != IO_ERROR__OK)
			{
				return err;
			}

			temp_u8 = 0;
			err = _fpga_otbc_read_register(driver_bundle, FPGA_OTBC_REG__TEST, &temp_u8);
			if (err != IO_ERROR__OK)
			{
				return err;
			}

			err = (temp_u8 == 0xA1 ? IO_ERROR__OK : IO_ERROR_FPGA_OTBC__HW_TEST_FAILED);
			break;

		case IO_IOCTL_FPGA_OTBC__GET_HW_REV:
			temp_u8 = 0;
			err = _fpga_otbc_read_register(driver_bundle, FPGA_OTBC_REG__HW_REV, &temp_u8);
			if (err != IO_ERROR__OK)
			{
				return err;
			}
			*(uint8_t*)ptr = temp_u8;
			break;

		case IO_IOCTL_FPGA_OTBC__GET_SFP_STATUS:
			temp_u8 = 0;
			err = _fpga_otbc_read_register(driver_bundle, FPGA_OTBC_REG__SERDES_SFP_STATUS, &temp_u8);
			if (err != IO_ERROR__OK)
			{
				return err;
			}
			*(uint8_t*)ptr = temp_u8;
			break;

		default:
			err = IO_ERROR_FPGA_OTBC__UNKNOWN_IOCTL;
			break;
	}
	
	return err;
}


