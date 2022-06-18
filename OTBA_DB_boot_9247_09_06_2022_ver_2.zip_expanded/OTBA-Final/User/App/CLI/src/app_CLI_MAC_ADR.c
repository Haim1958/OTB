/**

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
 Created on: 9 Dec 2021
 */


#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static uint32_t  stm_id_CRC_value32(uint32_t CRC_value, const char *buf, size_t buf_length)
{
	static uint32_t table[256];
	static int is_table = 0;
	uint32_t rem;
	uint8_t octet;
	int i, j;
	const char *temp_buf, *q;


	if (is_table == 0) {
		/* create & calculate CRC_value 256 index table. */
		for (i = 0; i < 256; i++) {
			rem = i;  /* remainder from polynomial division */
			for (j = 0; j < 8; j++) {
				if (rem & 1) {
					rem >>= 1;
					rem ^= 0xedb88320; //
				} else
					rem >>= 1;
			}
			table[i] = rem;
		}
		is_table = 1;
	}

	CRC_value = ~CRC_value;
	q = buf + buf_length;
	for (temp_buf = buf; temp_buf < q; temp_buf++) {
		octet = *temp_buf;  /* Cast to unsigned octet. */
		CRC_value = (CRC_value >> 8) ^ table[(CRC_value & 0xff) ^ octet];
	}
	return ~CRC_value; // 0xFF FF FF FF , FF FF FF FF
}

static uint32_t crc32b(char *message)
{
   unsigned int i =0, j;
   uint32_t byte, mask;
   uint32_t  CRC_32 = 0xFFFFFFFF;

    while (message[i] != 0)
    {
       byte = message[i];            // Get next byte.
       CRC_32 ^=  byte; //xor

       for (j = 0; j < 8; j++) // eight times.
       {
         mask = -(CRC_32 & 1);
         CRC_32 = (CRC_32 >> 1) ^ (0xEDB88320 & mask);
       }

       i += 1;

    } // end of while

    return ~CRC_32;
}

static char *get_stm_id(void)
{
	char *stm32f7xx_id_buff;

	uint32_t stm32f7xx_id_byte_04 =  *(uint32_t *)0x1FF0F424; // lot_num & waf_num register 420 + 0x04
	uint32_t stm32f7xx_id_byte_08 =  *(uint32_t *)0x1FF0F428; // lot_num & waf_num register 420 + 0x08


	stm32f7xx_id_buff = (char *)malloc(sizeof(char)*16);

	// to simplify debug not used "for conditional  loop"
	stm32f7xx_id_buff[0] = (char)(((uint8_t*)&stm32f7xx_id_byte_04)[0]); //byte 0
	stm32f7xx_id_buff[1] = (char)(((uint8_t*)&stm32f7xx_id_byte_04)[1]); //byte 1
	stm32f7xx_id_buff[2] = (char)(((uint8_t*)&stm32f7xx_id_byte_04)[2]); //byte 2
	stm32f7xx_id_buff[3] = (char)(((uint8_t*)&stm32f7xx_id_byte_04)[3]); //byte 3

	stm32f7xx_id_buff[4] = (char)(((uint8_t*)&stm32f7xx_id_byte_04)[4]); //byte 4
	stm32f7xx_id_buff[5] = (char)(((uint8_t*)&stm32f7xx_id_byte_04)[5]); //byte 5
	stm32f7xx_id_buff[6] = (char)(((uint8_t*)&stm32f7xx_id_byte_04)[6]); //byte 6
	stm32f7xx_id_buff[7] = (char)(((uint8_t*)&stm32f7xx_id_byte_04)[7]); //byte 7

	/***********************************************************/
	stm32f7xx_id_buff[8] = (char)(((uint8_t*)&stm32f7xx_id_byte_08)[0]); //byte 0
	stm32f7xx_id_buff[9] = (char)(((uint8_t*)&stm32f7xx_id_byte_08)[1]); //byte 1
	stm32f7xx_id_buff[10] = (char)(((uint8_t*)&stm32f7xx_id_byte_08)[2]); //byte 2
	stm32f7xx_id_buff[11] = (char)(((uint8_t*)&stm32f7xx_id_byte_08)[3]); //byte 3

	stm32f7xx_id_buff[12] = (char)(((uint8_t*)&stm32f7xx_id_byte_08)[4]); //byte 4
	stm32f7xx_id_buff[13] = (char)(((uint8_t*)&stm32f7xx_id_byte_08)[5]); //byte 5
	stm32f7xx_id_buff[14] = (char)(((uint8_t*)&stm32f7xx_id_byte_08)[6]); //byte 6
	stm32f7xx_id_buff[15] = (char)(((uint8_t*)&stm32f7xx_id_byte_08)[7]); //byte 7

	return stm32f7xx_id_buff;
}

 static uint32_t calculate_crc_stm_id(unsigned char temp_choose)
{
	 char *pBuff_Store;
	 uint32_t temp_to_mac;

	 pBuff_Store = get_stm_id();
	 if(temp_choose == 0)
	   temp_to_mac = stm_id_CRC_value32(0xedb88320,pBuff_Store,sizeof(pBuff_Store));
	 else
		temp_to_mac = crc32b(pBuff_Store);
	 return temp_to_mac;
}

/*
 * app_CLI_MAC_ADR.c
 *
 *  Created on: 13 Dec 2021
 *      Author: SKH
 */


