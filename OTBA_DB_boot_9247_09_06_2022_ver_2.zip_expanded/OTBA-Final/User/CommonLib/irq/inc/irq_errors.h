/**
 ******************************************************************************
 * @file    irq_errors.h
 * @brief   Interrupt Handler public declarations and definitions
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
	 
	 
#ifndef __IRQ_ERRORS_H__
#define __IRQ_ERRORS_H__

/* Includes */

/* Typedefs */

#define IRQ_ERROR__OK						ERROR_OK
#define IRQ_ERROR__UNKNOWN_TYPE				IRQ_ERROR(0x01)
#define IRQ_ERROR__INDEX_OUT_OF_RANGE		IRQ_ERROR(0x02)
#define IRQ_ERROR__ALREADY_REGISTERED		IRQ_ERROR(0x03)
#define IRQ_ERROR__INVALID_CALLBACK			IRQ_ERROR(0x04)

/* Declarations */


#endif //__IRQ_ERRORS_H__
