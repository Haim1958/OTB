/*
 * MAC_Modify.c
 *
 *  Created on: 16 Dec 2021
 *      Author: SKH
 */


#include <string.h>
#include "cmsis_os.h"
#include "UserLib.h"
#include "app.h"
#include "app_CLI_MAC_ADR.h"

#include "stm32f7xx_hal_cortex.h"
#include "netif/ethernet.h"
#include "etharp.h"
#include "lwip.h"
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
//#include "lwip/netif.h"
#include "ethernetif.h"


//#include "app_CLI_MAC_ADR.h"

HAL_StatusTypeDef HAL_ETH_Init(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETH_DeInit(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETH_Start(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETH_Stop(ETH_HandleTypeDef *heth);


struct link_str link_arg;

/* Variables Initialization */
struct netif  gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;
uint8_t IP_ADDRESS[4];
uint8_t NETMASK_ADDRESS[4];
uint8_t GATEWAY_ADDRESS[4];

ETH_HandleTypeDef gheth;

struct netconn * conn;


char buf[100];

TaskHandle_t  xB_MAC_ModifyTaskHandler;
osThreadId ethernetif_set_link_hndl;

extern char chrEMAC_ADDRESS[];

extern osSemaphoreId Netif_LinkSemaphore ;

//void Ethrn_Ip_reInit (void);

RF_STRUCT_PTR     RF_STRCT_GLB;
static void Ethrn_Ip_reInit (void);
//static int8_t EEPROM_Read_MAC_ADD(RF_STRUCT_PTR  RF_STRCT_GLB);
extern void ethernetif_update_config(struct netif *netif);
extern void netif_set_link_callback(struct netif *netif, netif_status_callback_fn link_callback);

OTS_APPLICATION_BUNDLE_PTR OTS_APP;
MAC_ADD_STRC_PTR MAC_ADD_PTR_GLB;
static void MAC_Modify_fnc(MAC_ADD_STRC_PTR MAC_ADD_PTR);

static void MAC_Modify_fnc(MAC_ADD_STRC_PTR MAC_ADD_PTR)
{

       //ip4_addr_t newIp, newMask, newGW;

    	osDelay(5);

    	taskENTER_CRITICAL();

    	LOCK_TCPIP_CORE();///......
    	HAL_ETH_Stop(&gheth);
    	HAL_NVIC_DisableIRQ(ETH_IRQn);


    	//memcpy((int8_t *)&gnetif.hwaddr, (int8_t *)&chrEMAC_ADDRESS[0], 6);
    	memcpy((int8_t *)&gnetif.hwaddr, (int8_t *)&MAC_ADD_PTR->MAC_address, 6);


    	HAL_ETH_ConfigMAC(&gheth, (ETH_MACInitTypeDef *) NULL);


    	ethernetif_update_config(&gnetif);

    	Ethrn_Ip_reInit();

    	netif_set_link_callback(&gnetif, ethernetif_update_config);

    	HAL_ETH_Start(&gheth);

    	HAL_NVIC_EnableIRQ(ETH_IRQn);

    	UNLOCK_TCPIP_CORE();//.....

    	taskEXIT_CRITICAL();


    	/**
    	    	newMask.addr = inet_addr(IP_MASK);
    	    	newGW.addr = inet_addr(IP_DEFAULT_GATEWAY);
    	    	newIp.addr = inet_addr(IP_ADDRESS_OTBA);
    	    	netif_set_addr(&gnetif, &newIp, &newMask, &newGW);
    	        **/



}

static void Ethrn_Ip_reInit (void)
{


	  /* Registers the default network interface */
	   netif_set_default(&gnetif);

	   if (netif_is_link_up(&gnetif))
	   {
	     /* When the netif is fully configured this function must be called */
	     netif_set_up(&gnetif);
	   }
	   else
	   {
	     /* When the netif link is down this function must be called */
	     netif_set_down(&gnetif);
	   }

	 	 /* Set the link callback function, this function is called on change of link status*/
	   //...netif_set_link_callback(&gnetif, ethernetif_update_config);


	   ///link_arg.netif = &gnetif;

}


//********************************************************************************************
//save file to eeprom

int32_t EEPROM_Write_MAC_ADD(RF_STRUCT_PTR RF, char  *pemac_char)
{
	uint32_t err=ERROR_OK;

	MAC_ADD_STRC_PTR pMAC_ADD = MAC_ADD_PTR_GLB;

	RF->TABLE_FILE[1] = TABLE_FILE_GLB[1];
	memcpy(&pMAC_ADD->MAC_address,(uint8_t *)pemac_char,6);

	   err = io_write(RF->TABLE_FILE[1/*MAC_file*/], (uint8_t*)&pMAC_ADD->MAC_address, 6);
			if (err != IO_ERROR_SETTINGS__OK)
		{
			return err;
		}

		err = io_ioctl(RF->TABLE_FILE[1/*MAC_file*/], IO_IOCTL_SETTINGS__PULL, NULL);
		if (err != IO_ERROR_SETTINGS__OK)
		{
			return err;
		}
		//osDelay(5);
		MAC_Modify_fnc(pMAC_ADD);
	 return err;

}

//********************************************************************************************
//read  file from eeprom
static int8_t EEPROM_Read_MAC_ADD(RF_STRUCT_PTR  RF)
{
	uint32_t err=ERROR_OK;
	uint8_t is_valid = 1, temp;
	MAC_ADD_STRC_PTR ptable_struct = NULL;
	MAC_ADD_STRC_PTR pMAC_ADD = MAC_ADD_PTR_GLB;
	temp = 0;
    //char accept_data[6];
	RF->TABLE_FILE[1] = TABLE_FILE_GLB[1];
   	err = io_ioctl(RF->TABLE_FILE[1/*MAC_file*/], IO_IOCTL_SETTINGS__IS_FILE_VALID, &temp);
			if (err != IO_ERROR_SETTINGS__OK)
			{
				return err;
			}

			is_valid = is_valid & temp;
			pMAC_ADD->eeprom_is_valid=is_valid;

			if (is_valid)
			{

				err = io_read(RF->TABLE_FILE[1/*MAC_file*/], (uint8_t*)&ptable_struct, 1);


		      	  if (err != IO_ERROR_SETTINGS__OK)
		      	  {
		      		  printf("RF_ERROR__EEPROM_INVALID_VALUE \r\n ");
		      		  return RF_ERROR__INVALID_VALUE;
		      	  }
		      	  memcpy(&pMAC_ADD->MAC_address,(uint8_t*)ptable_struct,6);
		      	  memcpy(&chrEMAC_ADDRESS,(uint8_t *)ptable_struct,6);


			}


	 return err;
}

int8_t MAC_INIT(OTS_APPLICATION_BUNDLE_PTR OTS_APP )

{
	int32_t err=0;

	  //char emac_char[6];
	  uint32_t ser_id_crc;
		   //int8_t MACAddr[6];
	    MAC_ADD_STRC_PTR MAC_ADD_PTR;
	    MAC_ADD_PTR_GLB = MAC_ADD_PTR = (MAC_ADD_STRC_PTR) pvPortMalloc(sizeof(MAC_ADD_STRUCT));
	    MAC_ADD_PTR->eeprom_is_valid=0;

		 err= EEPROM_Read_MAC_ADD(RF_STRCT_GLB);

		 if (!MAC_ADD_PTR->eeprom_is_valid)
		  {

			 ser_id_crc = calculate_crc_stm_id(0); //0xXX.XX.XX.XX|XX.XX.XX.XX

			 chrEMAC_ADDRESS[5] = MAC_ADD_PTR->MAC_address[5] = (char)ser_id_crc ;
			 chrEMAC_ADDRESS[4] = MAC_ADD_PTR->MAC_address[4] = (char)(ser_id_crc = ser_id_crc >> 8);
			 chrEMAC_ADDRESS[3] = MAC_ADD_PTR->MAC_address[3] = (char)(ser_id_crc = ser_id_crc >> 8);
			  (ser_id_crc = ser_id_crc >> 8); //skip 3 char
			 chrEMAC_ADDRESS[2] = MAC_ADD_PTR->MAC_address[2] = (char)(ser_id_crc = ser_id_crc >> 8);
			 chrEMAC_ADDRESS[1] = MAC_ADD_PTR->MAC_address[1] = 0x80;
			 chrEMAC_ADDRESS[0] = MAC_ADD_PTR->MAC_address[0] = 0x00;
		  }
             /********************/

			return err;
}



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

/********* TEST ************************/
void test_config_mac (void)
{

	char emac_char[7];
	int8_t MACAddr[6];
	uint32_t ser_id_crc;
    ser_id_crc = calculate_crc_stm_id(0);


	/****************************************************************/
	emac_char[5] = (char)ser_id_crc ;
	emac_char[4] = (char)(ser_id_crc = ser_id_crc >> 8);
	emac_char[3] = (char)(ser_id_crc = ser_id_crc >> 8);
	(ser_id_crc = ser_id_crc >> 8); //skip 3 char
	emac_char[2] = (char)(ser_id_crc = ser_id_crc >> 8);
	emac_char[1] = 0x80;
	emac_char[0] = 0x00;


	chrEMAC_ADDRESS[0] = MACAddr[0] =  (int8_t)emac_char[0];
	chrEMAC_ADDRESS[1] = MACAddr[1] =  (int8_t)emac_char[1];
	chrEMAC_ADDRESS[2] = MACAddr[2] =  (int8_t)emac_char[2];
	chrEMAC_ADDRESS[3] = MACAddr[3] =  (int8_t)emac_char[3];
	chrEMAC_ADDRESS[4] = MACAddr[4] =  (int8_t)emac_char[4];
	chrEMAC_ADDRESS[5] = MACAddr[5] =  (int8_t)emac_char[5];



	/****************************************************************/


	memcpy((int8_t *)&gnetif.hwaddr, (int8_t *)&chrEMAC_ADDRESS[0], 6);
	//HAL_ETH_ConfigMAC(&gheth, (ETH_MACInitTypeDef *) NULL);
	//
	low_level_init(&gnetif);
	ethernetif_update_config(&gnetif);

}



