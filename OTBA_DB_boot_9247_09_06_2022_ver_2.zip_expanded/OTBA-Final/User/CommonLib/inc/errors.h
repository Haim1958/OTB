/**
 ******************************************************************************
 * @file    errors.h
 * @brief   ADC header
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


 
#ifndef __ERRORS_H__
#define __ERRORS_H__

/* Includes */

/* Definitions */

#define ERROR_OK					(0u)

#define APP_ERROR_BIT				(0x00010000)			// Can be used freely by the app
#define IO_ERROR_BIT				(0x00020000)
#define IRQ_ERROR_BIT				(0x00030000)
#define FSM_ERROR_BIT				(0x00040000)
#define ALGO_ERROR_BIT				(0x00050000)
#define PRINTF_ERROR_BIT			(0x00050000)


#define APP_ERROR(n)				(APP_ERROR_BIT | (n & 0x0000FFFF))
#define IO_ERROR(n)					(IO_ERROR_BIT | (n & 0x0000FFFF))
#define IRQ_ERROR(n)				(IRQ_ERROR_BIT | (n & 0x0000FFFF))
#define FSM_ERROR(n)				(FSM_ERROR_BIT | (n & 0x0000FFFF))
#define ALGO_ERROR(n)				(ALGO_ERROR_BIT | (n & 0x0000FFFF))
#define PRINTF_ERROR(n)				(PRINTF_ERROR_BIT | (n & 0x0000FFFF))


#endif //__ERRORS_H__
