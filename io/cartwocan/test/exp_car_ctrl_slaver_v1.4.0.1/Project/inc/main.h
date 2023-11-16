/**
******************************************************************************
* @file    main.h
* @author  MCD Application Team
* @version V1.0.0
* @date    11/20/2009
* @brief   This file contains all the functions prototypes for the main.c
*          file.
******************************************************************************
* @copy
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

    /* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f107.h"
#include "fun.h"
#include <string.h>
#include "data.h"
#include "position.h"
#include "uart4.h"
#include "usart1.h"
#include "usart2.h"
#include "netconf.h"
#include "list.h"
#include "photo.h"
#include "can2_bus.h"

#define     USE_UDP

    /* Exported function prototypes ----------------------------------------------*/
void Time_Update(void);
void Delay(uint32_t nCount);

#define HEART_DELY       5

#define INVALUE          0
#define VALUE            1

#define HEART_DELAY    5

    /* ETHERNET errors */
#define  ETH_ERROR              ((uint32_t)0)
#define  ETH_SUCCESS            ((uint32_t)1)
#define  DP83848_PHY_ADDRESS       0x01

#define	TCP_RECEV_BUFF_SIZE		2048
#define	TCP_SEND_BUFF_SIZE		2048
#define	CLIENT_DIS_CONNECT		0
#define	CLIENT_CONNECT_OK		1
#define	CLIENT_CONNECT_RECV		2
#define	CLIENT_RE_CONNECT		3

#define DEST_IP_ADDR0   192
#define DEST_IP_ADDR1   168
#define DEST_IP_ADDR2   10
#define DEST_IP_ADDR3   12
#define DEST_PORT       9000

    /* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0   0
#define MAC_ADDR1   0
#define MAC_ADDR2   0
#define MAC_ADDR3   2
#define MAC_ADDR4   1
#define MAC_ADDR5   1
    /*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   10
#define IP_ADDR3   110
    /*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0
    /*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   10
#define GW_ADDR3   1

#define	 key1_STATUE	        GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)
#define	 key2_STATUE	        GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)
#define	 key3_STATUE	        GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)
#define	 key4_STATUE	        GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)

#define	 key5_STATUE	        GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)
#define	 key6_STATUE	        GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)
#define	 key7_STATUE	        GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)
#define	 key8_STATUE	        GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7)

#define	 INPUT4_STATUE	        GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)
#define	 INPUT3_STATUE	        GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)
#define	 INPUT2_STATUE	        GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)
#define	 INPUT1_STATUE	        GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9)

#define	 LED_STATUE		GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_2)
#define	 LED_ON			GPIO_ResetBits(GPIOE,GPIO_Pin_2)
#define	 LED_OFF		GPIO_SetBits(GPIOE,GPIO_Pin_2)

#define  UserAppEnFlagAddress      0x800F800
#define  CanCheckFlagAddr          0x803F800


#define  MOTO_THREE_UART        UART4       
#define  MOTO_TWO_USART		    USART2
#define  MOTO_ONE_USART	        USART1

#define  MOTO_THREE_IRQn        UART4_IRQn
#define  MOTO_TWO_IRQn	        USART2_IRQn
#define  MOTO_ONE_IRQn	        USART1_IRQn

#define	 MOTO_THREE_IRQHandler	UART4_IRQHandler
#define	 MOTO_TWO_IRQHandler	USART2_IRQHandler
#define	 MOTO_ONE_IRQHandler	USART1_IRQHandler

#define	 RX_MOTO_THREE_485		GPIO_ResetBits(GPIOD, GPIO_Pin_7);
#define	 TX_MOTO_THREE_485		GPIO_SetBits( GPIOD, GPIO_Pin_7);

#define	 RX_MOTO_TWO_485	    GPIO_ResetBits( GPIOD, GPIO_Pin_4);
#define	 TX_MOTO_TWO_485	    GPIO_SetBits( GPIOD, GPIO_Pin_4);

#define	 RX_MOTO_ONE_485		GPIO_ResetBits( GPIOA, GPIO_Pin_11);
#define	 TX_MOTO_ONE_485		GPIO_SetBits( GPIOA, GPIO_Pin_11);

#define	 RECV_DATA		1
#define	 SEND_DATA		2
#define  SENDING_DATA           3
#define	 RECV_DATA_END	        4

extern u8 key_value;    
extern u8 heart_dely;
extern u8 second_cnt;
extern u8 one_ms_flag;

#define     CAR_LENGTH          150  //mm


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

