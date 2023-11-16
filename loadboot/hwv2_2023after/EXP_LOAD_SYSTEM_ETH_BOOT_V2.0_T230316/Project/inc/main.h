/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    07-October-2011 
  * @brief   This file contains all the functions prototypes for the main.c 
  *          file.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif
	/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "stm32f2x7_eth_bsp.h"
#include "stm32f207.h" //20170605
#include "fun.h"//20170608
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* IAP options selection ******************************************************/
#define USE_IAP_TFTP   /* enable IAP using TFTP */
   //*********boot-64k,app-704k,backup-256k *************//
#define USER_FLASH_FIRST_PAGE_ADDRESS           0x08010000
#define USER_FLASH_LAST_PAGE_ADDRESS            0x08020000
#define USER_FLASH_END_ADDRESS                  0x080BFFFF
#define UserAppEnFlagAddress                    0x080C0000
#define UserMACFlagAddress                      0x080A0000
//NEW
#define	UDP_RECV_BUFF_SIZE		100  //new
#define UDP_SEND_BUFF_SIZE      100  //new
#define	CLIENT_DIS_CONNECT		0
#define	CLIENT_CONNECT_OK		1
#define	CLIENT_CONNECT_RECV		2
#define	CLIENT_RE_CONNECT		3
#define	 LED_STATE		GPIO_ReadOutputDataBit(GPIOG, GPIO_Pin_15)
#define	 LED_ON			GPIO_ResetBits(GPIOG,GPIO_Pin_15)
#define	 LED_OFF		GPIO_SetBits(GPIOG,GPIO_Pin_15)
#define  IN_3_STATE             ((~GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6))&0x1)
/**********DEST_DATA**************/
#define DEST_IP_ADDR0   Dest_ip_Addr[0]//192
#define DEST_IP_ADDR1   Dest_ip_Addr[1]//168
#define DEST_IP_ADDR2   Dest_ip_Addr[2]//10
#define DEST_IP_ADDR3   Dest_ip_Addr[3]//12
#define DEST_PORT       dest_port_data     //new
/* UserID and Password definition *********************************************/
#define USERID       "piotec"
#define PASSWORD     "1234"
#define LOGIN_SIZE   (15+ sizeof(USERID) + sizeof(PASSWORD))
/* Static IP Address definition ***********************************************/
#define IP_ADDR0   static_ip[0]	//192
#define IP_ADDR1   static_ip[1]	//168
#define IP_ADDR2   static_ip[2]	//10
#define IP_ADDR3   static_ip[3]	//110
/* NETMASK definition *********************************************************/
#define NETMASK_ADDR0   static_mask[0]//255
#define NETMASK_ADDR1   static_mask[1]//255
#define NETMASK_ADDR2   static_mask[2]//255
#define NETMASK_ADDR3   static_mask[3]//0
/* Gateway Address definition *************************************************/
#define GW_ADDR0   static_netway[0]//192
#define GW_ADDR1   static_netway[1]//168
#define GW_ADDR2   static_netway[2]//10
#define GW_ADDR3   static_netway[3]//1  
/* MAC Address definition *****************************************************/
#define MAC_ADDR0   0//mac_addr[0]//2 //0
#define MAC_ADDR1   mac_addr[1]//0
#define MAC_ADDR2   mac_addr[2]//0
#define MAC_ADDR3   mac_addr[3]//0 
#define MAC_ADDR4   mac_addr[4]//0
#define MAC_ADDR5   mac_addr[5]//0 //2
#define LOCAL_UDP_PORT  local_port_data  //new
/********local-net-data************/
extern u8 static_ip[];
extern u8 static_mask[];
extern u8 static_netway[];
extern u8 mac_addr[6];
extern u16 local_port_data;
/********dest-net-data************/
extern u8 Dest_ip_Addr[4];
extern u16 dest_port_data;
extern u8 net_sys_para_temp[7];
extern u8 Programm_Completed_Reply_flag;//烧写完成标志
//#define MII_MODE
//#ifdef 	MII_MODE
//#define PHY_CLOCK_MCO
//#endif
	void Time_Update(void);
	void Delay(uint32_t nCount);
        void ETH_GPIO_Config(void);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/


