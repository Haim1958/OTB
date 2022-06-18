/**
 ******************************************************************************
 * @file    ADS8681_otba.c
 * @brief   ADS8681 IO functions implementations
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

#include "../inc/ADS8681_otba.h"
#include "../inc/ADS8681_otba_private.h"
#include "../inc/ADS8681_otba_reg.h"

/* Global variables */

/* Functions definitions */

static int32_t _ADS8681_otba_open(IO_HANDLER_PTR, void*);
static int32_t _ADS8681_otba_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _ADS8681_otba_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
static int32_t _ADS8681_otba_ioctl(IO_HANDLER_PTR, uint32_t, void*);


/*! \brief Installs the ADS8681 driver
 *
 ###  Detailed description:
 *
 *  @param ADS8681_init - pointer to the ADS8681 installation struct.
 *  @return **IO_ERROR_ADS8681__OK** if the installation was OK
 *  @return **IO_ERROR_ADS8681__OUT_OF_HEAP** not enough space in heap for allocations
 */
int32_t install_ADS8681_otba
	(
		ADS8681_OTBA_INSTALL_STRUCT_PTR ADS8681_init
	)
{
	ADS8681_OTBA_DRIVER_BUNDLE_PTR driver_bundle;
	IO_HANDLER_INSTALL_STRUCT init =	{
											_ADS8681_otba_open,
											_ADS8681_otba_read,
											_ADS8681_otba_write,
											_ADS8681_otba_ioctl
										};
	int32_t err;
	
	/* Allocate driver bundle */
	driver_bundle = (ADS8681_OTBA_DRIVER_BUNDLE_PTR) pvPortMalloc(sizeof(ADS8681_OTBA_DRIVER_BUNDLE));
	if (NULL == driver_bundle)
	{
		return IO_ERROR_ADS8681_OTBA__OUT_OF_HEAP;
	}
	
	memset(driver_bundle, 0, sizeof(ADS8681_OTBA_DRIVER_BUNDLE));
	
	init.bundle = driver_bundle;

	/* Install the IO handler */
	err = io_install(IO_HANDLER_TYPE__ADS8681_OTBA, &init);
	if (IO_ERROR_HANDLER__OK != err)
	{
		return err;
	}

	return IO_ERROR_ADS8681_OTBA__OK;
}


/*! \brief Opens and initializes the ADS8681 driver
 *
 ###  Detailed description:
 *
 *  @param handler - ADS8681 driver handler.
 *  @param init - initialization struct pointer.
 *  @return **IO_ERROR_ADS8681__OK** if the initialization was OK
 *  @return **Error** if another error
 */
static int32_t _ADS8681_otba_open(IO_HANDLER_PTR handler, void * init)
{
	ADS8681_OTBA_DRIVER_BUNDLE_PTR driver_bundle = (ADS8681_OTBA_DRIVER_BUNDLE_PTR)handler->bundle;
	ADS8681_OTBA_INIT_STRUCT_PTR ADS8681_init = (ADS8681_OTBA_INIT_STRUCT_PTR) init;
	  HAL_GPIO_WritePin(GPIOB, LAser_RESET_Pin|SPI3__NSS_Pin, GPIO_PIN_SET);
	driver_bundle->PHY.driver = ADS8681_init->driver;
	
	return IO_ERROR_ADS8681_OTBA__OK;
}



/*! \brief Reads a sample from ADS8681 driver
 *
 ###  Detailed description:
 *
 *  @param handler - ADS8681 driver handler.
 *  @param ptr - Destination buffer for the sample.
 *  @param samples - Number of samples to read. Not used currently. Set to 1.
 *  @return **IO_ERROR_ADS8681__OK** if the initialization was OK
 *  @return **SPI Error** if SPI error occured
 *  @return **IO_ERROR_ADS8681__INVALID_SAMPLE_OUTPUT_MODE** if invalid output mode is set
 */
static int32_t _ADS8681_otba_read(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t samples)
{
//	ADS8681_DRIVER_BUNDLE_PTR driver_bundle = (ADS8681_DRIVER_BUNDLE_PTR)handler->bundle;
	
	return IO_ERROR_ADS8681_OTBA__NOT_IMPLEMENTED;
}


/*! \brief Writes to ADS8681 driver
 *
 ###  Detailed description:
 *  @remark Currently not in use
 *
 *  @param handler - ADS8681 driver handler.
 *  @param ptr - Buffer pointer of data to be written.
 *  @param length - Length of the data to be written.
 *  @return **IO_ERROR_ADS8681__OK** always
 */
static int32_t _ADS8681_otba_write(IO_HANDLER_PTR handler, uint8_t * ptr, uint32_t length)
{
	return IO_ERROR_ADS8681_OTBA__NOT_IMPLEMENTED;
}


/*! \brief Executes IOCTL command
 *
 ###  Detailed description:
 *
 *  @param handler - ADS8681 driver handler.
 *  @param ptr - Buffer pointer of data to be written.
 *  @param length - Length of the data to be written.
 *  @return **IO_ERROR_ADS8681__OK** if command executed ok
 *  @return **IO_ERROR_ADS8681__UNKNOWN_IOCTL** if requested command is not supported
 */
static int32_t _ADS8681_otba_ioctl(IO_HANDLER_PTR handler, uint32_t ctl, void * ptr)
{
	ADS8681_OTBA_DRIVER_BUNDLE_PTR driver_bundle = (ADS8681_OTBA_DRIVER_BUNDLE_PTR)handler->bundle;
	ADS8681_OTBA_RW_REG_PTR reg;
	uint8_t temp_u8;
	int32_t all_32;
	int32_t err = IO_ERROR_ADS8681_OTBA__OK;
	
	switch (ctl)
	{

			reg = (ADS8681_OTBA_RW_REG_PTR)ptr;
			err = _ADS8681_otba_write_register(driver_bundle, reg->address, reg->value);
			break;

		case IO_IOCTL_ADS8681_OTBA__READ_REG:
			reg = (ADS8681_OTBA_RW_REG_PTR)ptr;
			err = _ADS8681_otba_read_register(driver_bundle, reg->address, &reg->value);
			break;

		case IO_IOCTL_ADS8681_OTBA__TEST:
			err = _ADS8681_otba_write_register(driver_bundle, ADS8681_OTBA_REG__TEST, 0xA1);
			if (err != IO_ERROR_ADS8681_OTBA__OK)
			{
				return err;
			}

			temp_u8 = 0;
			err = _ADS8681_otba_read_register(driver_bundle, ADS8681_OTBA_REG__TEST, &temp_u8);
			if (err != IO_ERROR_ADS8681_OTBA__OK)
			{
				return err;
			}

			err = (temp_u8 == 0xA1 ? IO_ERROR__OK : IO_ERROR_ADS8681_OTBA__HW_TEST_FAILED);
			break;

		case IO_IOCTL_ADS8681_OTBA__GET_HW_REV:
			temp_u8 = 0;
			err = _ADS8681_otba_read_register(driver_bundle, ADS8681_OTBA_REG__HW_REV, &temp_u8);
			if (err != IO_ERROR_ADS8681_OTBA__OK)
			{
				return err;
			}
			*(uint8_t*)ptr = temp_u8;
			break;

		case IO_IOCTL_ADS8681_OTBA__WRITE_OUTPUT_DISCRETES:
			//GM TODO: We should use the internal mapping of the registers' addresses and use the driver_bundle->PHY.driver
			// to write to the relevant registers according to ptr(ADS8681_DISCRETES_TO_WRITE_PTR).
			break;
			
		case IO_IOCTL_ADS8681_OTBA__READ_INPUT_DISCRETES:
			all_32 = 0;
			temp_u8 = 0;
			err = _ADS8681_otba_read_register(driver_bundle, ADS8681_OTBA_REG__DISC_AC_2_OTBB_REG2, &temp_u8);
			if (err != IO_ERROR_ADS8681_OTBA__OK)
			{
				return err;
			}

			all_32 |= temp_u8;
			all_32 = all_32 << 8;

			temp_u8 = 0;
			err = _ADS8681_otba_read_register(driver_bundle, ADS8681_OTBA_REG__DISC_AC_2_OTBB_REG1, &temp_u8);
			if (err != IO_ERROR_ADS8681_OTBA__OK)
			{
				return err;
			}

			all_32 |= temp_u8;
			all_32 = all_32 << 8;

			temp_u8 = 0;
			err = _ADS8681_otba_read_register(driver_bundle, ADS8681_OTBA_REG__DISC_AC_2_OTBC_REG, &temp_u8);
			if (err != IO_ERROR_ADS8681_OTBA__OK)
			{
				return err;
			}

			all_32 |= temp_u8;

			*(int32_t*)ptr = all_32;
			
			break;

		case IO_IOCTL_ADS8681_OTBA__READ_INPUT_DISCRETES_TEST:
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
		case IO_IOCTL_ADS8681_OTBA__SET_STATE:
			temp_u8 = *(uint8_t*)ptr;
			err = _ADS8681_otba_write_register(driver_bundle, ADS8681_OTBA_REG__SYSTEM_STATE, temp_u8);
			if (err != IO_ERROR_ADS8681_OTBA__OK)
			{
				return err;
			}

 * Read Back to verify:	TODO: do we need this?
			temp_u8 = 0;
			err = _ADS8681_otba_read_register(driver_bundle, ADS8681_OTBA_REG__SYSTEM_STATE, &temp_u8);
			if (err != IO_ERROR_ADS8681_OTBA__OK)
			{
				return err;
			}

			err = (temp_u8 == val ? IO_ERROR__OK : IO_ERROR_ADS8681_OTBA__STATE_CHANGE_FAILED);

			break;
*/

	  	case IO_IOCTL_ADS8681_OTBA__ENABLE_DIS_SIG:
	  		temp_u8 = 0;
	  		err = _ADS8681_otba_read_register(driver_bundle, ADS8681_OTBA_REG__SER1_CONTROL, &temp_u8);
	  		if (err != IO_ERROR_ADS8681_OTBA__OK)
	  		{
	  			return err;
	  		}

	  		temp_u8 |= 1;

	  		err = _ADS8681_otba_write_register(driver_bundle, ADS8681_OTBA_REG__SER1_CONTROL, temp_u8);
	  		if (err != IO_ERROR_ADS8681_OTBA__OK)
	  		{
	  			return err;
	  		}

	  	   	temp_u8 = 0;
	  	   	err = _ADS8681_otba_read_register(driver_bundle, ADS8681_OTBA_REG__SER2_CONTROL, &temp_u8);
	  	   	if (err != IO_ERROR_ADS8681_OTBA__OK)
	  	   	{
	  	   		return err;
	  	   	}

	  	   	temp_u8 |= 1;

	  	   	err = _ADS8681_otba_write_register(driver_bundle, ADS8681_OTBA_REG__SER2_CONTROL, temp_u8);
	  	   	if (err != IO_ERROR_ADS8681_OTBA__OK)
	  	   	{
	  	   		return err;
	  	   	}

	  	   	break;

	  case IO_IOCTL_ADS8681_OTBA__DISABLE_DIS_SIG:
		  	  temp_u8 = 0;
		  	  err = _ADS8681_otba_read_register(driver_bundle, ADS8681_OTBA_REG__SER1_CONTROL, &temp_u8);
		  	  if (err != IO_ERROR_ADS8681_OTBA__OK)
		  	  {
		  		  return err;
		  	  }

		  	  temp_u8 &= 0xFE;

		  	  err = _ADS8681_otba_write_register(driver_bundle, ADS8681_OTBA_REG__SER1_CONTROL, temp_u8);
		  	  if (err != IO_ERROR_ADS8681_OTBA__OK)
		  	  {
		  	  	return err;
		  	  }

		  	  temp_u8 = 0;
		  	  err = _ADS8681_otba_read_register(driver_bundle, ADS8681_OTBA_REG__SER2_CONTROL, &temp_u8);
		  	  if (err != IO_ERROR_ADS8681_OTBA__OK)
		  	  {
		  		  return err;
		  	  }

		  	  temp_u8 &= 0xFE;

		  	  err = _ADS8681_otba_write_register(driver_bundle, ADS8681_OTBA_REG__SER2_CONTROL, temp_u8);
		  	  if (err != IO_ERROR_ADS8681_OTBA__OK)
		  	  {
		  		  return err;
		  	  }

		  	  break;

			
		default:
			err = IO_ERROR_ADS8681_OTBA__UNKNOWN_IOCTL;
			break;
	}
	
	return err;
}


