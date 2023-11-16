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
#include "fun.h"
#include "can_bus.h"
#include <string.h>

/* FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//扇区7起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//扇区11起始地址,128 Kbytes
*/

// boot区(SECTOR_0~2),程序有效性标记(SECTOR_3),用户程序区(SECTOR_4),临时存储区(SECTOR_5)
#define ApplicationAddress                0x8010000  //用户程序运行起始地址
#define AppTempAddress                    0x8020000  //转发从机程序的临时空间起始地址
#define UserAppEnFlagAddress              0x800C000  //程序有效性标记地址
//用户参数区(SECTOR_6)
#define UserParaStartAddress              0x8040000  //用户参数区起始地址

void Time_Update(void);
void Delay(uint32_t nCount);
void BSP_Init(void);

#define	UDP_RECV_BUFF_SIZE		200
#define UDP_SEND_BUFF_SIZE             200
#define	CLIENT_DIS_CONNECT		0
#define	CLIENT_CONNECT_OK		1
#define	CLIENT_CONNECT_RECV		2
#define	CLIENT_RE_CONNECT		3

#define DEST_IP_ADDR0   192
#define DEST_IP_ADDR1   168
#define DEST_IP_ADDR2   2
#define DEST_IP_ADDR3   100
#define DEST_PORT       9000

#define LOCAL_UDP_PORT  DEST_PORT
/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0   0
#define MAC_ADDR1   0
#define MAC_ADDR2   0
#define MAC_ADDR3   0
#define MAC_ADDR4   0
#define MAC_ADDR5   1
    /*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   2
#define IP_ADDR3   110
    /*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0
    /*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   2
#define GW_ADDR3   1

#define	 LED_STATE		GPIO_ReadOutputDataBit(GPIOG,GPIO_Pin_15)
#define	 LED_ON			GPIO_ResetBits(GPIOG,GPIO_Pin_15)
#define	 LED_OFF		GPIO_SetBits(GPIOG,GPIO_Pin_15)

/*拨码开关*/
#define  DIP_1_STATE            GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_15)
#define  DIP_2_STATE            GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_14)
#define  DIP_3_STATE            GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_13)
#define  DIP_4_STATE            GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_12)
#define  DIP_5_STATE            GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11)
#define  DIP_6_STATE            GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)
#define  DIP_7_STATE            GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)
#define  DIP_8_STATE            GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)

extern u8  Local_Station;
extern u8  cur_tran_station;
extern u16 pk_tot_num;
extern u16 pk_cur_num;
extern u8  trans_flag;

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

