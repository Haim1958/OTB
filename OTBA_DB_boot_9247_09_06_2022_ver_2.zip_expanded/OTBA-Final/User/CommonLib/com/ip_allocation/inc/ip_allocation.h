/**
 ******************************************************************************
 * @file    ip_allocation.h
 * @brief   IP Allocation definition
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

#ifndef __IP_ALLOCATION__
#define __IP_ALLOCATION__

/* Declarations */

/*******************************************************************************
 * GetLocation:
 *   Connects to OTBA to receive the location information and then set the ip
 *   address accordingly.
 *
 * Input:
 *   first_location  - The string that OTBA should send to indicate the location
 *                     for the first ip
 *   second_location - The string that OTBA should send to indicate the location
 *                     for the second ip
 *   first_ip        - Set the ip to this address in case the first_location was
 *                     received
 *   second_ip       - Set the ip to this address in case the second_location was
 *                     received
 *
 *  Returns:
 *    1 - We received confirmation for the first location.
 *    0 - We received confirmation for the second location.
 *
 *******************************************************************************/
uint8_t  GetLocationAndSetIP(const char *first_location, const char* second_location,
		const char* first_ip, const char* second_ip);

#endif /* __IP_ALLOCATION__ */
