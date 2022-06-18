/*
 * app_CLI_MAC_ADR.h
 *
 *  Created on: 9 Dec 2021
 *      Author: SKH
 */

#ifndef APP_CLI_SRC_APP_CLI_MAC_ADR_H_
#define APP_CLI_SRC_APP_CLI_MAC_ADR_H_
#endif

/**#define MAC_address_size 6
typedef struct MAC_ADD_struct {
	uint8_t MAC_address[MAC_address_size];
	uint8_t eeprom_is_valid;

} MAC_ADD_STRUCT,*MAC_ADD_STRC_PTR;
**/
int8_t MAC_INIT(OTS_APPLICATION_BUNDLE_PTR OTS_APP );

static int8_t EEPROM_Read_MAC_ADD(RF_STRUCT_PTR  RF_STRCT_GLB);
static uint32_t    stm_id_CRC_value32(uint32_t CRC_value, const char *buf, size_t buf_length);
static uint32_t crc32b(char *message);

static char *get_stm_id(void);
static uint32_t calculate_crc_stm_id(unsigned char temp_choose);

static void Ethrn_Ip_reInit (void);
//void MAC_Modify_fnc(OTS_APPLICATION_BUNDLE_PTR OTS_APP );
extern void ethernetif_update_config(struct netif *netif);
extern void netif_set_link_callback(struct netif *netif, netif_status_callback_fn link_callback);

