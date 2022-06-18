/**
 ******************************************************************************
 * @file    OTSTasks.h
 * @brief   OTS application tasks
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


#ifndef __OTS_TASKS_H__
#define __OTS_TASKS_H__



void MainTask(void const * argument);
void CliTask(void const * argument);
void IPAllocationTask(void const * argument);


void OTBA_B_Req_Task(void const * argument);
void OTBA_Get_B_Answ_Task(void const * argument);
///void OTBB_COM_examine_task(void const * argument);

#endif // __OTS_TASKS_H__
