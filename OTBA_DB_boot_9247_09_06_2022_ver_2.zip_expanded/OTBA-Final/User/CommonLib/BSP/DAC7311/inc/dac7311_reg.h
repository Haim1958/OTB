/**
 ******************************************************************************
 * @file    dac7311_reg.h
 * @brief   DAC7311 registers declarations and definitions
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

 
#ifndef __DAC7311_REG_H__
#define __DAC7311_REG_H__

/* Includes */


/* Definitions */




#define DAC7311_RESOLUTION			(4096)		// 12 bit
#define DAC7311_VREF_MV				(2500)		// +2.5V
#define DAC7311_MV_TO_COUNT(mv)		(int16_t)((mv * DAC7311_RESOLUTION) / DAC7311_VREF_MV)



/* Typedefs */


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif //__DAC7311_REG_H__
