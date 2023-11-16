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
#include "io.h"
#include "can_bus.h"
#include "usart1.h"
#include "usart6.h"

void Time_Update(void);
void Delay(uint32_t nCount);
void BSP_Init(void);
#define  NULL    (void*)0

#define	 LED_STATE		GPIO_ReadOutputDataBit(GPIOG,GPIO_Pin_15)
#define	 LED_ON			GPIO_ResetBits(GPIOG,GPIO_Pin_15)
#define	 LED_OFF		GPIO_SetBits(GPIOG,GPIO_Pin_15)

#define  DIP_STATE              ((~GPIO_ReadInputData(GPIOA))&0xFF)

#define  IN_1_16_STATE          GPIO_ReadInputData(GPIOD)
#define  IN_17_32_STATE         GPIO_ReadInputData(GPIOE)
#define  IN_33_40_STATE         (GPIO_ReadInputData(GPIOF)&0xFF)
#define  IN_41_STATE            GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_11)
#define  IN_42_STATE            GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_12)
#define  IN_43_STATE            GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_13)
#define  IN_44_STATE            GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)
#define  IN_45_STATE            GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
#define  IN_46_STATE            GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_8)
#define  IN_47_STATE            GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_9)
#define  IN_48_STATE            GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10)

#define  OUT_1_(BitVal)         GPIO_WriteBit(GPIOA,GPIO_Pin_8,BitVal)
#define  OUT_2_(BitVal)         GPIO_WriteBit(GPIOA,GPIO_Pin_12,BitVal)
#define  OUT_3_(BitVal)         GPIO_WriteBit(GPIOB,GPIO_Pin_0,BitVal)
#define  OUT_4_(BitVal)         GPIO_WriteBit(GPIOB,GPIO_Pin_1,BitVal)
#define  OUT_5_(BitVal)         GPIO_WriteBit(GPIOB,GPIO_Pin_5,BitVal)
#define  OUT_6_(BitVal)         GPIO_WriteBit(GPIOB,GPIO_Pin_6,BitVal)
#define  OUT_7_(BitVal)         GPIO_WriteBit(GPIOB,GPIO_Pin_7,BitVal)
#define  OUT_8_(BitVal)         GPIO_WriteBit(GPIOG,GPIO_Pin_2,BitVal)
#define  OUT_9_(BitVal)         GPIO_WriteBit(GPIOG,GPIO_Pin_1,BitVal)
#define  OUT_10_(BitVal)        GPIO_WriteBit(GPIOB,GPIO_Pin_10,BitVal)
#define  OUT_11_(BitVal)        GPIO_WriteBit(GPIOB,GPIO_Pin_11,BitVal)
#define  OUT_12_(BitVal)        GPIO_WriteBit(GPIOC,GPIO_Pin_6,BitVal)
#define  OUT_13_(BitVal)        GPIO_WriteBit(GPIOC,GPIO_Pin_7,BitVal)
#define  OUT_14_(BitVal)        GPIO_WriteBit(GPIOB,GPIO_Pin_14,BitVal)
#define  OUT_15_(BitVal)        GPIO_WriteBit(GPIOB,GPIO_Pin_15,BitVal)
#define  OUT_16_(BitVal)        GPIO_WriteBit(GPIOC,GPIO_Pin_0,BitVal)
#define  OUT_17_(BitVal)        GPIO_WriteBit(GPIOC,GPIO_Pin_1,BitVal)
#define  OUT_18_(BitVal)        GPIO_WriteBit(GPIOC,GPIO_Pin_2,BitVal)
#define  OUT_19_(BitVal)        GPIO_WriteBit(GPIOC,GPIO_Pin_3,BitVal)
#define  OUT_20_(BitVal)        GPIO_WriteBit(GPIOC,GPIO_Pin_4,BitVal)
#define  OUT_21_(BitVal)        GPIO_WriteBit(GPIOC,GPIO_Pin_9,BitVal)
#define  OUT_22_(BitVal)        GPIO_WriteBit(GPIOC,GPIO_Pin_10,BitVal)
#define  OUT_23_(BitVal)        GPIO_WriteBit(GPIOC,GPIO_Pin_11,BitVal)
#define  OUT_24_(BitVal)        GPIO_WriteBit(GPIOC,GPIO_Pin_12,BitVal)

#define	 UART1_RX_485	        GPIO_ResetBits(GPIOA,GPIO_Pin_11)
#define	 UART1_TX_485	        GPIO_SetBits(GPIOA,GPIO_Pin_11)

#define	 UART6_RX_485	        GPIO_ResetBits(GPIOG,GPIO_Pin_10)
#define	 UART6_TX_485	        GPIO_SetBits(GPIOG,GPIO_Pin_10)


#define    USART1_BUFF_LEN      50
#define    USART1_MOTO_LEN      20

#define    USART1_RECV_TIMMER   10
#define    USART1_SEND_TIMMER   10

#define    USART6_BUFF_LEN      50
#define    USART6_MOTO_LEN      20

#define    USART6_RECV_TIMMER   10
#define    USART6_SEND_TIMMER   10

extern u8  Local_Station;

#endif /* __MAIN_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

