/**
 ******************************************************************************
 * @file    ADS8681_otba_private.h
 * @brief   ADS8681 private declarations and definitions
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
 
#ifndef __ADS8681_OTBA_PRIVATE_H__
#define __ADS8681_OTBA_PRIVATE_H__

/* Includes */

#include "../../ADS8681/inc/ADS8681_otba_reg.h"


/* Definitions */
// binary commands
#define CMDBITS_CLEAR_HWORD 	0xc0000000		// 11000_xx_<9-bit address>_<16-bit data>
#define CMDBITS_READ_HWORD 		0xc8000000		// hex(int(11001_xx_<9-bit address>_00000000_00000000 , 2)) in python
#define CMDBITS_READ 			0x48000000 		// 01001_xx_<9-bit address>_00000000_00000000
#define CMDBITS_SET_HWORD		0xd8000000		// 11011_xx_<9-bit address>_<16-bit data>


/* Typedefs */

typedef struct ADS8681_otba_driver_phy {
	IO_HANDLER_PTR					driver;											// SPI driver
} ADS8681_OTBA_DRIVER_PHY, * ADS8681_OTBA_DRIVER_PHY_PTR;

typedef struct ADS8681_otba_driver_bundle {
	ADS8681_OTBA_DRIVER_PHY					PHY;									// Physical parameters (ports, interrupts)
} ADS8681_OTBA_DRIVER_BUNDLE, * ADS8681_OTBA_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t _ADS8681_otba_read_register(ADS8681_OTBA_DRIVER_BUNDLE_PTR driver_bundle, uint8_t reg_addr, uint8_t *reg_val);
int32_t _ADS8681_otba_write_register(ADS8681_OTBA_DRIVER_BUNDLE_PTR driver_bundle, uint8_t reg_addr, uint8_t reg_val);

#ifdef __cplusplus
}
#endif

#endif //__ADS8681_OTBA_PRIVATE_H__
