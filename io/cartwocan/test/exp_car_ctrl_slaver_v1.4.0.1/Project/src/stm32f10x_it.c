/**
  ******************************************************************************
  * @file    stm32f10x_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32_eth.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
extern void LwIP_Pkt_Handle(void);

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
{}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    /*while (1)
    {}*/
    NVIC_SystemReset();
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    /*while (1)
    {}*/
    NVIC_SystemReset();
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    /*while (1)
    {}*/
    NVIC_SystemReset();
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void  UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    /*while (1)
    {}*/
    NVIC_SystemReset();
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

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
/*************************************************
1MS
*************************************************/
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }

}

void DMA1_Channel4_IRQHandler(void)
{
    if (DMA_GetFlagStatus(DMA1_FLAG_TC4) == SET)
    {
        DMA_ClearFlag(DMA1_FLAG_TC4);
        USART_ClearITPendingBit(USART1, USART_IT_TC);
        USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    }

}

void DMA1_Channel7_IRQHandler(void)
{
    if (DMA_GetFlagStatus(DMA1_FLAG_TC7) == SET)
    {
        DMA_ClearFlag(DMA1_FLAG_TC7);
        USART_ClearITPendingBit(USART2, USART_IT_TC);
        USART_ITConfig(USART2, USART_IT_TC, ENABLE);
    }
}

void DMA2_Channel5_IRQHandler(void)
{
    if (DMA_GetFlagStatus(DMA2_FLAG_TC5) == SET)
    {
        DMA_ClearFlag(DMA2_FLAG_TC5);
        USART_ClearITPendingBit(UART4, USART_IT_TC);
        USART_ITConfig(UART4, USART_IT_TC, ENABLE);
    }

}

/**
  * @brief  This function handles USARTx global interrupt request.
  * @param  None
  * @retval None
  */

void USART2_IRQHandler(void)
{
    u16 curDataCnt;

    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        curDataCnt = USART2->SR;
        curDataCnt = USART2->DR;
        USART_ClearITPendingBit(USART2, USART_IT_IDLE);
        curDataCnt = DMA_GetCurrDataCounter(DMA1_Channel6);
        usart2_recv_len = USART2_BUFF_LEN - curDataCnt;
        usart2_moto_recv_now_process();
        DMA_Cmd(DMA1_Channel6, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Channel6, USART2_BUFF_LEN);
        DMA_Cmd(DMA1_Channel6, ENABLE);
    }
    if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
    {
        USART_ClearFlag(USART2, USART_IT_TC);
        NVIC_ClearPendingIRQ(USART2_IRQn);
        USART_ITConfig(USART2, USART_IT_TC, DISABLE);
        RX_MOTO_TWO_485;
        usart2_recv_len = 0;
    }
}

void USART1_IRQHandler(void)
{
    u16 curDataCnt;

    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        curDataCnt = USART1->SR;
        curDataCnt = USART1->DR;
        USART_ClearITPendingBit(USART1, USART_IT_IDLE);
        curDataCnt = DMA_GetCurrDataCounter(DMA1_Channel5);
        usart1_recv_len = USART1_BUFF_LEN - curDataCnt;
        usart1_moto_recv_now_process();
        DMA_Cmd(DMA1_Channel5, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Channel5, USART1_BUFF_LEN);
        DMA_Cmd(DMA1_Channel5, ENABLE);
    }
    if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
        USART_ClearFlag(USART1, USART_IT_TC);
        NVIC_ClearPendingIRQ(USART1_IRQn);
        USART_ITConfig(USART1, USART_IT_TC, DISABLE);
        RX_MOTO_ONE_485;
        usart1_recv_len = 0;
    }
}

void UART4_IRQHandler(void)
{
    u16 curDataCnt;

    if (USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)
    {
        curDataCnt = UART4->SR;
        curDataCnt = UART4->DR;
        USART_ClearITPendingBit(UART4, USART_IT_IDLE);
        curDataCnt = DMA_GetCurrDataCounter(DMA2_Channel3);
        uart4_recv_len = UART4_BUFF_LEN - curDataCnt;
        uart4_moto_recv_now_process();
        DMA_Cmd(DMA2_Channel3, DISABLE);
        DMA_SetCurrDataCounter(DMA2_Channel3, UART4_BUFF_LEN);
        DMA_Cmd(DMA2_Channel3, ENABLE);
    }
    if (USART_GetITStatus(UART4, USART_IT_TC) != RESET)
    {
        USART_ClearFlag(UART4, USART_IT_TC);
        NVIC_ClearPendingIRQ(UART4_IRQn);
        USART_ITConfig(UART4, USART_IT_TC, DISABLE);
        RX_MOTO_THREE_485;
        uart4_recv_len = 0;
    }
}

//
//void DMA1_Channel4_IRQHandler(void)
//{
//    if (DMA_GetFlagStatus(DMA1_FLAG_TC4) == SET)
//    {
//        DMA_ClearFlag(DMA1_FLAG_TC4);
//        USART_ClearITPendingBit(USART1, USART_IT_TC);
//        USART_ITConfig(USART1, USART_IT_TC, ENABLE);
///*
//        if (usart1_moto_commu_statue == SENDING_DATA)
//        {
//            usart1_ready_recv_process();
//        }
//*/        
//    }
//      
//}
//
//void DMA1_Channel7_IRQHandler(void)
//{
//    if (DMA_GetFlagStatus(DMA1_FLAG_TC7) == SET)
//    {
//        DMA_ClearFlag(DMA1_FLAG_TC7);
//        USART_ClearITPendingBit(USART2, USART_IT_TC);
//        USART_ITConfig(USART2, USART_IT_TC, ENABLE);
//
///*        if (usart2_moto_commu_statue == SENDING_DATA)
//        {
//            usart2_ready_recv_process();
//        }
//*/        
//    }
//}
//
//void DMA2_Channel5_IRQHandler(void)
//{
//    if (DMA_GetFlagStatus(DMA2_FLAG_TC5) == SET)
//    {
//        DMA_ClearFlag(DMA2_FLAG_TC5);
//        USART_ClearITPendingBit(UART4, USART_IT_TC);
//        USART_ITConfig(UART4, USART_IT_TC, ENABLE);
///*
//        if (uart4_moto_commu_statue == SENDING_DATA)
//        {
//            uart4_ready_recv_process();
//        }
//*/        
//    }
//        
//}

/**
  * @brief  This function handles USARTx global interrupt request.
  * @param  None
  * @retval None
  */
//
//void USART2_IRQHandler(void) 
//{
//    u16 curDataCnt;
//
//    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
//    {
//        curDataCnt = USART2->SR;
//        curDataCnt = USART2->DR;
//        USART_ClearITPendingBit(USART2, USART_IT_IDLE);
//        curDataCnt = DMA_GetCurrDataCounter(DMA1_Channel6);
//        usart2_recv_len = USART2_BUFF_LEN - curDataCnt;
//        usart2_moto_recv_now_process();
//        DMA_Cmd(DMA1_Channel6, DISABLE);
//        DMA_SetCurrDataCounter(DMA1_Channel6, USART2_BUFF_LEN);
//        DMA_Cmd(DMA1_Channel6, ENABLE);
//    }
//    if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
//    {
//        USART_ClearFlag(USART2, USART_IT_TC);
//        NVIC_ClearPendingIRQ(USART2_IRQn);
//        USART_ITConfig(USART2, USART_IT_TC, DISABLE);
//        RX_MOTO_TWO_485;
//    }
//}
//
//void USART1_IRQHandler(void)
//{
//    u16 curDataCnt;
//
//    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
//    {
//        curDataCnt = USART1->SR;
//        curDataCnt = USART1->DR;
//        USART_ClearITPendingBit(USART1, USART_IT_IDLE);
//        curDataCnt = DMA_GetCurrDataCounter(DMA1_Channel5);
//        usart1_recv_len = USART1_BUFF_LEN - curDataCnt;
//        usart1_moto_recv_now_process();
//        DMA_Cmd(DMA1_Channel5, DISABLE);
//        DMA_SetCurrDataCounter(DMA1_Channel5, USART1_BUFF_LEN);
//        DMA_Cmd(DMA1_Channel5, ENABLE);
//    }
//    if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
//    {
//        USART_ClearFlag(USART1, USART_IT_TC);
//        NVIC_ClearPendingIRQ(USART1_IRQn);
//        USART_ITConfig(USART1, USART_IT_TC, DISABLE);
//        RX_MOTO_ONE_485;
//        usart1_recv_len = 0;
//    }
//}
//
//void UART4_IRQHandler(void)
//{
//    u16 curDataCnt;
//
//    if (USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)
//    {
//        curDataCnt = UART4->SR;
//        curDataCnt = UART4->DR;
//        USART_ClearITPendingBit(UART4, USART_IT_IDLE);
//        curDataCnt = DMA_GetCurrDataCounter(DMA2_Channel3);
//        uart4_recv_len = UART4_BUFF_LEN - curDataCnt;
//        uart4_moto_recv_now_process();
//        DMA_Cmd(DMA2_Channel3, DISABLE);
//        DMA_SetCurrDataCounter(DMA2_Channel3, UART4_BUFF_LEN);
//        DMA_Cmd(DMA2_Channel3, ENABLE);
//    }
//    if (USART_GetITStatus(UART4, USART_IT_TC) != RESET)
//    {
//        USART_ClearFlag(UART4, USART_IT_TC);
//        NVIC_ClearPendingIRQ(UART4_IRQn);
//        USART_ITConfig(UART4, USART_IT_TC, DISABLE);
//        RX_MOTO_THREE_485;
//        usart1_recv_len = 0;
//    }
//}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles ETH interrupt request.
  * @param  None
  * @retval None
  */
//void ETH_IRQHandler(void)
//{
//    /* Handles all the received frames */
//    while(ETH_GetRxPktSize() != 0)
//    {
//        LwIP_Pkt_Handle();
//    }
//
//    /* Clear the Eth DMA Rx IT pending bits */
//    ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
//    ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
//}

/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */


void CAN1_RX0_IRQHandler(void)
{
    CanRxMsg RxMessage;
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
    CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
    vcanbus_framereceive_one(RxMessage);
}

void CAN2_RX1_IRQHandler(void)
{
    CanRxMsg RxMessage;
    CAN_Receive(CAN2, CAN_FIFO1, &RxMessage);
    CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);
    vcanbus_frame_receive_two(RxMessage);
}



/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
