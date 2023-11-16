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

void Time_Update(void);
void Delay(uint32_t nCount);
void BSP_Init(void);

#define	UDP_RECV_BUFF_SIZE		100
#define UDP_SEND_BUFF_SIZE              100
#define	CLIENT_DIS_CONNECT		0
#define	CLIENT_CONNECT_OK		1
#define	CLIENT_CONNECT_RECV		2
#define	CLIENT_RE_CONNECT		3

#define DEST_IP_ADDR0   192
#define DEST_IP_ADDR1   168
#define DEST_IP_ADDR2   10
#define DEST_IP_ADDR3   13
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
#define IP_ADDR2   10
#define IP_ADDR3   100
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

#define	 LED_STATE		GPIO_ReadOutputDataBit(GPIOG,GPIO_Pin_15)
#define	 LED_ON			GPIO_ResetBits(GPIOG,GPIO_Pin_15)
#define	 LED_OFF		GPIO_SetBits(GPIOG,GPIO_Pin_15)

#define  IN_1_STATE             ((~GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4))&0x1)
#define  IN_2_STATE             ((~GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9))&0x1)
#define  IN_3_STATE             ((~GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6))&0x1)
#define  IN_4_STATE             ((~GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_9))&0x1)
#define  IN_5_STATE             ((~GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7))&0x1)
#define  IN_6_STATE             ((~GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_13))&0x1)
#define  IN_7_STATE             ((~GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_14))&0x1)
#define  IN_8_STATE             ((~GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_15))&0x1)
#define  IN_9_STATE             ((~GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11))&0x1)
#define  IN_10_STATE            ((~GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_0))&0x1)
#define  IN_11_STATE            ((~GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_1))&0x1)
#define  IN_12_STATE            ((~GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2))&0x1)
#define  IN_13_STATE            ((~GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8))&0x1)
#define  IN_14_STATE            ((~GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7))&0x1)
#define  IN_15_STATE            ((~GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6))&0x1)
#define  IN_16_STATE            ((~GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6))&0x1)
#define  IN_17_STATE            ((~GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_10))&0x1)
#define  IN_18_STATE            ((~GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14))&0x1)
#define  IN_19_STATE            ((~GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15))&0x1)
#define  IN_20_STATE            ((~GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6))&0x1)
#define  IN_21_STATE            ((~GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8))&0x1)
#define  IN_22_STATE            ((~GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10))&0x1)
#define  IN_23_STATE            ((~GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_6))&0x1)
#define  IN_24_STATE            ((~GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_12))&0x1)
#define  IN_25_STATE            ((~GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4))&0x1)
#define  IN_26_STATE            ((~GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14))&0x1)
#define  IN_27_STATE            ((~GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7))&0x1)
#define  IN_28_STATE            ((~GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_11))&0x1)
#define  IN_29_STATE            ((~GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_13))&0x1)
#define  IN_30_STATE            ((~GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_10))&0x1)
#define  IN_31_STATE            ((~GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5))&0x1)
#define  IN_32_STATE            ((~GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7))&0x1)
#define  IN_33_STATE            ((~GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3))&0x1)
#define  IN_34_STATE            ((~GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_8))&0x1)

#define  OUT_1_(BitVal)         GPIO_WriteBit(GPIOG,GPIO_Pin_3,BitVal)
#define  OUT_2_(BitVal)         GPIO_WriteBit(GPIOG,GPIO_Pin_1,BitVal)
#define  OUT_3_(BitVal)         GPIO_WriteBit(GPIOG,GPIO_Pin_0,BitVal)
#define  OUT_4_(BitVal)         GPIO_WriteBit(GPIOF,GPIO_Pin_12,BitVal)
#define  OUT_5_(BitVal)         GPIO_WriteBit(GPIOC,GPIO_Pin_0,BitVal)
#define  OUT_6_(BitVal)         GPIO_WriteBit(GPIOF,GPIO_Pin_7,BitVal)
#define  OUT_7_(BitVal)         GPIO_WriteBit(GPIOG,GPIO_Pin_11,BitVal)
#define  OUT_8_(BitVal)         GPIO_WriteBit(GPIOE,GPIO_Pin_1,BitVal)
#define  OUT_9_(BitVal)         GPIO_WriteBit(GPIOF,GPIO_Pin_3,BitVal)
#define  OUT_10_(BitVal)        GPIO_WriteBit(GPIOF,GPIO_Pin_4,BitVal)
#define  OUT_11_(BitVal)        GPIO_WriteBit(GPIOF,GPIO_Pin_5,BitVal)
#define  OUT_12_(BitVal)        GPIO_WriteBit(GPIOG,GPIO_Pin_4,BitVal)
#define  OUT_13_(BitVal)        GPIO_WriteBit(GPIOG,GPIO_Pin_5,BitVal)
#define  OUT_14_(BitVal)        GPIO_WriteBit(GPIOD,GPIO_Pin_11,BitVal)
#define  OUT_15_(BitVal)        GPIO_WriteBit(GPIOE,GPIO_Pin_4,BitVal)
#define  OUT_16_(BitVal)        GPIO_WriteBit(GPIOE,GPIO_Pin_12,BitVal)

#define  MOTOR_OUT_1_(BitVal)   GPIO_WriteBit(GPIOG,GPIO_Pin_13,BitVal)
#define  MOTOR_OUT_2_(BitVal)   GPIO_WriteBit(GPIOE,GPIO_Pin_2,BitVal)
#define  MOTOR_OUT_3_(BitVal)   GPIO_WriteBit(GPIOF,GPIO_Pin_2,BitVal)
#define  MOTOR_OUT_4_(BitVal)   GPIO_WriteBit(GPIOE,GPIO_Pin_3,BitVal)

#define  MOTOR_DIR_1_(BitVal)   GPIO_WriteBit(GPIOD,GPIO_Pin_0,BitVal)
#define  MOTOR_DIR_2_(BitVal)   GPIO_WriteBit(GPIOD,GPIO_Pin_1,BitVal)
#define  MOTOR_DIR_3_(BitVal)   GPIO_WriteBit(GPIOC,GPIO_Pin_7,BitVal)
#define  MOTOR_DIR_4_(BitVal)   GPIO_WriteBit(GPIOE,GPIO_Pin_8,BitVal)

#define	 UART3_RX_485	        GPIO_ResetBits(GPIOE,GPIO_Pin_15)
#define	 UART3_TX_485	        GPIO_SetBits(GPIOE,GPIO_Pin_15)

#define	 UART4_RX_485	        GPIO_ResetBits(GPIOC,GPIO_Pin_13)
#define	 UART4_TX_485	        GPIO_SetBits(GPIOC,GPIO_Pin_13)

#define	 UART6_RX_485	        GPIO_ResetBits(GPIOG,GPIO_Pin_10)
#define	 UART6_TX_485	        GPIO_SetBits(GPIOG,GPIO_Pin_10)

#define	 RECV_DATA		1
#define	 SEND_DATA		2
#define  SENDING_DATA           3
#define	 RECV_DATA_END	        4

#define  UART1_BUFF_SIZE        50
extern u8  uart1_send_buff[UART1_BUFF_SIZE];
extern u8  uart1_recv_buff[UART1_BUFF_SIZE];
extern u16 uart1_send_count;
extern u16 uart1_recv_count;
extern u8  uart1_commu_state;
extern u16 uart1_tmr;
#define  UART2_BUFF_SIZE        50
extern u8  uart2_send_buff[UART2_BUFF_SIZE];
extern u8  uart2_recv_buff[UART2_BUFF_SIZE];
extern u16 uart2_send_count;
extern u16 uart2_recv_count;
extern u8  uart2_commu_state;
extern u16 uart2_tmr;
#define  UART3_BUFF_SIZE        50
extern u8  uart3_send_buff[UART3_BUFF_SIZE];
extern u8  uart3_recv_buff[UART3_BUFF_SIZE];
extern u16 uart3_send_count;
extern u16 uart3_recv_count;
extern u8  uart3_commu_state;
extern u16 uart3_tmr;
#define  UART4_BUFF_SIZE        50
extern u8  uart4_send_buff[UART4_BUFF_SIZE];
extern u8  uart4_recv_buff[UART4_BUFF_SIZE];
extern u16 uart4_send_count;
extern u16 uart4_recv_count;
extern u8  uart4_commu_state;
extern u16 uart4_tmr;
#define  UART5_BUFF_SIZE        50
extern u8  uart5_send_buff[UART5_BUFF_SIZE];
extern u8  uart5_recv_buff[UART5_BUFF_SIZE];
extern u16 uart5_send_count;
extern u16 uart5_recv_count;
extern u8  uart5_commu_state;
extern u16 uart5_tmr;
#define  UART6_BUFF_SIZE        50
extern u8  uart6_send_buff[UART6_BUFF_SIZE];
extern u8  uart6_recv_buff[UART6_BUFF_SIZE];
extern u16 uart6_send_count;
extern u16 uart6_recv_count;
extern u8  uart6_commu_state;
extern u16 uart6_tmr;

extern u8  rs232_1_debug_cnt;
extern u8  rs232_2_debug_cnt;
extern u8  rs232_5_debug_cnt;
extern u8  rs485_3_debug_cnt;
extern u8  rs485_4_debug_cnt;
extern u8  rs485_6_debug_cnt;

void uart_recv_timeout(void);
void uart_test_send(void);
void uart_test_recv(void);
extern void ETH_GPIO_Config(void);

#endif /* __MAIN_H */



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

