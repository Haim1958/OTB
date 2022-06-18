/**
 ******************************************************************************
 * @file    m24512_private.h
 * @brief   M24512 private declarations and definitions
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

 
#ifndef __M24512_PRIVATE_H__
#define __M24512_PRIVATE_H__

/* Includes */

#include "m24512.h"

/* Definitions */

#define M24512_ADDR_MSB						(0)
#define M24512_ADDR_LSB						(1)

#define M24512_ADDR_BYTES					(2)

#define M24512_ADDR_TO_PAGE(addr)			(addr / M24512_PAGE_SIZE_BYTES)
#define M24512_OFFSET_IN_PAGE(addr)			(addr % M24512_PAGE_SIZE_BYTES)


/* Typedefs */

typedef struct m24512_driver_phy
{
	IO_HANDLER_PTR							i2c_driver;

} M24512_DRIVER_PHY, * M24512_DRIVER_PHY_PTR;


typedef struct m24512_driver_bundle
{
	M24512_DRIVER_PHY		PHY;
	uint16_t				seek_addr;

} M24512_DRIVER_BUNDLE, * M24512_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif //__M24512_PRIVATE_H__
