/**
 ******************************************************************************
 * @file    algo.h
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



#ifndef __ALGO_H__
#define __ALGO_H__


#define OPTMOD_ERROR(n)										(ALGO_ERROR(0x00000100) | n)
#define RF_ERROR(n)											(ALGO_ERROR(0x00000200) | n)


#include "../OPTMOD/inc/optmod.h"
#include "../RF/inc/rf.h"



#endif // __ALGO_H__
