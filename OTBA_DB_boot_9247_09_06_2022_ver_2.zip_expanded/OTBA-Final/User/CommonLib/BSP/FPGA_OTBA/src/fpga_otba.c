/**
 ******************************************************************************
 * @file    fpga_otba.c
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
#include "../inc/fpga_otba.h"
#include "../inc/fpga_otba_private.h"
#include "../inc/fpga_otba_reg.h"

/* Global variables */

/* Functions definitions */

static int32_t _fpga_otba_open(IO_HANDLER_PTR, void*);
static int32_t _fpga_otba_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _fpga_otba_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _fpga_otba_ioctl(IO_HANDLER_PTR, uint32_t, void*);


/*! \brief Installs the FPGA driver
 *
 ###  Detailed description:
 *
 *  @param fpga_init - pointer to the FPGA installation struct.
 *  @return **IO_ERROR_FPGA__OK** if the installation was OK
 *  @return **IO_ERROR_FPGA__OUT_OF_HEAP** not enough space in heap for allocations
 */
int32_t install_fpga_otba
	(
		FPGA_OTBA_INSTALL_STRUCT_PTR fpga_init
	)
{
	FPGA_OTBA_DRIVER_BUNDLE_PTR driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_fpga_otba_open,
											_fpga_otba_read,
											_fpga_otba_write,
											_fpga_otba_ioctl
										};
	int32_t err;
	
	/* Allocate driver bundle */
	driver_bundle = (FPGA_OTBA_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(FPGA_OTBA_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_FPGA_OTBA__OUT_OF_HEAP;
	}
	
	memset(driver_bundle, 0, sizeof(FPGA_OTBA_DRIVER_BUNDLE));
	
	init.bundle = driver_bundle;

	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__FPGA_OTBA, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}

	return IO_ERROR_FPGA_OTBA__OK;
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
static int32_t _fpga_otba_open(IO_HANDLER_PTR handler, void * init)
{
	FPGA_OTBA_DRIVER_BUNDLE_PTR driver_bundle = (FPGA_OTBA_DRIVER_BUNDLE_PTR)handler->bundle;
	FPGA_OTBA_INIT_STRUCT_PTR fpga_init = (FPGA_OTBA_INIT_STRUCT_PTR) init;
	
	driver_bundle->PHY.driver = fpga_init->driver;
	
	return IO_ERROR_FPGA_OTBA__OK;
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
static int32_t _fpga_otba_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t samples)
{
//	FPGA_DRIVER_BUNDLE_PTR driver_bundle = (FPGA_DRIVER_BUNDLE_PTR)handler->bundle;
	
	return IO_ERROR_FPGA_OTBA__NOT_IMPLEMENTED;
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
static int32_t _fpga_otba_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	return IO_ERROR_FPGA_OTBA__NOT_IMPLEMENTED;
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
static int32_t _fpga_otba_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	FPGA_OTBA_DRIVER_BUNDLE_PTR driver_bundle = (FPGA_OTBA_DRIVER_BUNDLE_PTR)handler->bundle;
	FPGA_OTBA_RW_REG_PTR reg;
	uint8_t temp_u8;
	int32_t all_32;
	int32_t err = IO_ERROR_FPGA_OTBA__OK;
	
	switch (ctl)
	{
		case IO_IOCTL_FPGA_OTBA__WRITE_REG:
			reg = (FPGA_OTBA_RW_REG_PTR)ptr;
			err = _fpga_otba_write_register(driver_bundle, reg->address, reg->value);
			break;

		case IO_IOCTL_FPGA_OTBA__READ_REG:
			reg = (FPGA_OTBA_RW_REG_PTR)ptr;
			err = _fpga_otba_read_register(driver_bundle, reg->address, &reg->value);
			break;

		case IO_IOCTL_FPGA_OTBA__TEST:
			err = _fpga_otba_write_register(driver_bundle, FPGA_OTBA_REG__TEST, 0xA1);
			if (err != IO_ERROR_FPGA_OTBA__OK)
			{
				return err;
			}

			temp_u8 = 0;
			err = _fpga_otba_read_register(driver_bundle, FPGA_OTBA_REG__TEST, &temp_u8);
			if (err != IO_ERROR_FPGA_OTBA__OK)
			{
				return err;
			}

			err = (temp_u8 == 0xA1 ? IO_ERROR__OK : IO_ERROR_FPGA_OTBA__HW_TEST_FAILED);
			break;

		case IO_IOCTL_FPGA_OTBA__GET_HW_REV:
			temp_u8 = 0;
			err = _fpga_otba_read_register(driver_bundle, FPGA_OTBA_REG__HW_REV, &temp_u8);
			if (err != IO_ERROR_FPGA_OTBA__OK)
			{
				return err;
			}
			*(uint8_t*)ptr = temp_u8;
			break;

		case IO_IOCTL_FPGA_OTBA__WRITE_OUTPUT_DISCRETES:
			//GM TODO: We should use the internal mapping of the registers' addresses and use the driver_bundle->PHY.driver
			// to write to the relevant registers according to ptr(FPGA_DISCRETES_TO_WRITE_PTR).
			break;
			
		case IO_IOCTL_FPGA_OTBA__READ_INPUT_DISCRETES:
			all_32 = 0;
			temp_u8 = 0;
			err = _fpga_otba_read_register(driver_bundle, FPGA_OTBA_REG__DISC_AC_2_OTBB_REG2, &temp_u8);
			if (err != IO_ERROR_FPGA_OTBA__OK)
			{
				return err;
			}

			all_32 |= temp_u8;
			all_32 = all_32 << 8;

			temp_u8 = 0;
			err = _fpga_otba_read_register(driver_bundle, FPGA_OTBA_REG__DISC_AC_2_OTBB_REG1, &temp_u8);
			if (err != IO_ERROR_FPGA_OTBA__OK)
			{
				return err;
			}

			all_32 |= temp_u8;
			all_32 = all_32 << 8;

			temp_u8 = 0;
			err = _fpga_otba_read_register(driver_bundle, FPGA_OTBA_REG__DISC_AC_2_OTBC_REG, &temp_u8);
			if (err != IO_ERROR_FPGA_OTBA__OK)
			{
				return err;
			}

			all_32 |= temp_u8;

			*(int32_t*)ptr = all_32;
			
			break;

		case IO_IOCTL_FPGA_OTBA__READ_INPUT_DISCRETES_TEST:
			all_32 = 0;
			temp_u8 = 0x03;

			all_32 |= temp_u8;
			all_32 = all_32 << 8;

			temp_u8 = 0x1F;
			all_32 |= temp_u8;
			all_32 = all_32 << 8;

			temp_u8 = 0x04;

			all_32 |= temp_u8;

			*(int32_t*)ptr = all_32;

			break;
/*
		case IO_IOCTL_FPGA_OTBA__SET_STATE:
			temp_u8 = *(uint8_t*)ptr;
			err = _fpga_otba_write_register(driver_bundle, FPGA_OTBA_REG__SYSTEM_STATE, temp_u8);
			if (err != IO_ERROR_FPGA_OTBA__OK)
			{
				return err;
			}

 * Read Back to verify:	TODO: do we need this?
			temp_u8 = 0;
			err = _fpga_otba_read_register(driver_bundle, FPGA_OTBA_REG__SYSTEM_STATE, &temp_u8);
			if (err != IO_ERROR_FPGA_OTBA__OK)
			{
				return err;
			}

			err = (temp_u8 == val ? IO_ERROR__OK : IO_ERROR_FPGA_OTBA__STATE_CHANGE_FAILED);

			break;
*/

	  	case IO_IOCTL_FPGA_OTBA__ENABLE_DIS_SIG:
	  		temp_u8 = 0;
	  		err = _fpga_otba_read_register(driver_bundle, FPGA_OTBA_REG__SER1_CONTROL, &temp_u8);
	  		if (err != IO_ERROR_FPGA_OTBA__OK)
	  		{
	  			return err;
	  		}

	  		temp_u8 |= 1;

	  		err = _fpga_otba_write_register(driver_bundle, FPGA_OTBA_REG__SER1_CONTROL, temp_u8);
	  		if (err != IO_ERROR_FPGA_OTBA__OK)
	  		{
	  			return err;
	  		}

	  	   	temp_u8 = 0;
	  	   	err = _fpga_otba_read_register(driver_bundle, FPGA_OTBA_REG__SER2_CONTROL, &temp_u8);
	  	   	if (err != IO_ERROR_FPGA_OTBA__OK)
	  	   	{
	  	   		return err;
	  	   	}

	  	   	temp_u8 |= 1;

	  	   	err = _fpga_otba_write_register(driver_bundle, FPGA_OTBA_REG__SER2_CONTROL, temp_u8);
	  	   	if (err != IO_ERROR_FPGA_OTBA__OK)
	  	   	{
	  	   		return err;
	  	   	}

	  	   	break;

	  case IO_IOCTL_FPGA_OTBA__DISABLE_DIS_SIG:
		  	  temp_u8 = 0;
		  	  err = _fpga_otba_read_register(driver_bundle, FPGA_OTBA_REG__SER1_CONTROL, &temp_u8);
		  	  if (err != IO_ERROR_FPGA_OTBA__OK)
		  	  {
		  		  return err;
		  	  }

		  	  temp_u8 &= 0xFE;

		  	  err = _fpga_otba_write_register(driver_bundle, FPGA_OTBA_REG__SER1_CONTROL, temp_u8);
		  	  if (err != IO_ERROR_FPGA_OTBA__OK)
		  	  {
		  	  	return err;
		  	  }

		  	  temp_u8 = 0;
		  	  err = _fpga_otba_read_register(driver_bundle, FPGA_OTBA_REG__SER2_CONTROL, &temp_u8);
		  	  if (err != IO_ERROR_FPGA_OTBA__OK)
		  	  {
		  		  return err;
		  	  }

		  	  temp_u8 &= 0xFE;

		  	  err = _fpga_otba_write_register(driver_bundle, FPGA_OTBA_REG__SER2_CONTROL, temp_u8);
		  	  if (err != IO_ERROR_FPGA_OTBA__OK)
		  	  {
		  		  return err;
		  	  }

		  	  break;

			
		default:
			err = IO_ERROR_FPGA_OTBA__UNKNOWN_IOCTL;
			break;
	}
	
	return err;
}


