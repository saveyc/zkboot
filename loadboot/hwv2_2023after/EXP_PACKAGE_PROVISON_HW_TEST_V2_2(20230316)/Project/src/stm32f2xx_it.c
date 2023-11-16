/**
  ******************************************************************************
  * @file    stm32f2xx_it.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    07-October-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_it.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    /* Update the LocalTime by adding SYSTEMTICK_PERIOD_MS each SysTick interrupt */
    Time_Update();
}

void EXTI15_10_IRQHandler(void)
{
  if(EXTI_GetITStatus(ETH_LINK_EXTI_LINE) != RESET)
  {
    Eth_Link_ITHandler(DP83848_PHY_ADDRESS);
    /* Clear interrupt pending bit */
    EXTI_ClearITPendingBit(ETH_LINK_EXTI_LINE);
  }
}

void USART1_IRQHandler(void)
{
    u8 tmp;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        if( uart1_commu_state == RECV_DATA)
        {
            uart1_tmr = 2;
            tmp = USART_ReceiveData(USART1);
            uart1_recv_buff[uart1_recv_count] = tmp;
            if( uart1_recv_count < UART1_BUFF_SIZE )
            {
                uart1_recv_count++;
            }
        }
        USART_ClearFlag(USART1, USART_FLAG_RXNE);
    }
}
void USART2_IRQHandler(void)
{
    u8 tmp;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        if( uart2_commu_state == RECV_DATA)
        {
            uart2_tmr = 2;
            tmp = USART_ReceiveData(USART2);
            uart2_recv_buff[uart2_recv_count] = tmp;
            if( uart2_recv_count < UART2_BUFF_SIZE )
            {
                uart2_recv_count++;
            }
        }
        USART_ClearFlag(USART2, USART_FLAG_RXNE);
    }
}
void USART3_IRQHandler(void)
{
    u8 tmp;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        if( uart3_commu_state == RECV_DATA)
        {
            uart3_tmr = 2;
            tmp = USART_ReceiveData(USART3);
            uart3_recv_buff[uart3_recv_count] = tmp;
            if( uart3_recv_count < UART3_BUFF_SIZE )
            {
                uart3_recv_count++;
            }
        }
        USART_ClearFlag(USART3, USART_FLAG_RXNE);
    }
}
void UART4_IRQHandler(void)
{
    u8 tmp;
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        if( uart4_commu_state == RECV_DATA)
        {
            uart4_tmr = 2;
            tmp = USART_ReceiveData(UART4);
            uart4_recv_buff[uart4_recv_count] = tmp;
            if( uart4_recv_count < UART4_BUFF_SIZE )
            {
                uart4_recv_count++;
            }
        }
        USART_ClearFlag(UART4, USART_FLAG_RXNE);
    }
}
void UART5_IRQHandler(void)
{
    u8 tmp;
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
    {
        if( uart5_commu_state == RECV_DATA)
        {
            uart5_tmr = 2;
            tmp = USART_ReceiveData(UART5);
            uart5_recv_buff[uart5_recv_count] = tmp;
            if( uart5_recv_count < UART5_BUFF_SIZE )
            {
                uart5_recv_count++;
            }
        }
        USART_ClearFlag(UART5, USART_FLAG_RXNE);
    }
}
void USART6_IRQHandler(void)
{
    u8 tmp;
    if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
    {
        if( uart6_commu_state == RECV_DATA)
        {
            uart6_tmr = 2;
            tmp = USART_ReceiveData(USART6);
            uart6_recv_buff[uart6_recv_count] = tmp;
            if( uart6_recv_count < UART6_BUFF_SIZE )
            {
                uart6_recv_count++;
            }
        }
        USART_ClearFlag(USART6, USART_FLAG_RXNE);
    }
}
void CAN1_RX0_IRQHandler(void)
{
    CanRxMsg RxMessage;
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
    CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
