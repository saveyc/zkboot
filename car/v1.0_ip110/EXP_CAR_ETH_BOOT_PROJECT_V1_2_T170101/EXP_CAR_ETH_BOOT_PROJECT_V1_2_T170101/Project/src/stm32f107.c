/**
  ******************************************************************************
  * @file    stm32f107.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   STM32F107 hardware configuration
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
#include "stm32_eth.h"
#include "stm32f107.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DP83848_PHY        /* Ethernet pins mapped on STM3210C-EVAL Board */
#define PHY_ADDRESS       0x01 /* Relative to STM3210C-EVAL Board */

#define MII_MODE          /* MII mode for STM3210C-EVAL Board (MB784) (check jumpers setting) */
//#define RMII_MODE       /* RMII mode for STM3210C-EVAL Board (MB784) (check jumpers setting) */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void Ethernet_Configuration(void);

__IO uint32_t  EthInitStatus = 0;

/*****************************************************************************8

*****************************************************************************/
void USART_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = UART_BANDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(UART4, &USART_InitStructure);
    USART_DMACmd(UART4, USART_DMAReq_Tx, DISABLE);
    USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
    USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
    USART_Cmd(UART4, ENABLE);
}

/****************************************************************************

******************************************************************************/
void DMA_Configuration(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    
    /* DMA2 Channel5 */
    DMA_DeInit(DMA2_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&UART4->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uart4_send_buff;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 32;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA2_Channel5, &DMA_InitStructure);
    DMA_ITConfig(DMA2_Channel5, DMA_IT_TC, ENABLE);
    DMA_ClearFlag(DMA2_FLAG_TC5);
    DMA_Cmd(DMA2_Channel5, DISABLE);
    /* DMA2 Channel3 */
    DMA_DeInit(DMA2_Channel3);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&UART4->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uart4_recv_buff;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = UART4_BUFF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA2_Channel3, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel3, ENABLE);
}

/**
  * @brief  Setup STM32 system (clocks, Ethernet, GPIO, NVIC) and STM3210C-EVAL resources.
  * @param  None
  * @retval None
  */
void System_Setup(void)
{
    RCC_ClocksTypeDef RCC_Clocks;

    /* Setup STM32 clock, PLL and Flash configuration) */
    SystemInit();

    /* Enable USART2 clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_TIM2 | RCC_APB1Periph_USART2 | RCC_APB1Periph_UART4
                           | RCC_APB1Periph_PWR | RCC_APB1Periph_CAN1, ENABLE);


    /* Enable ETHERNET clock  */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2 | RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx |
                          RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);

    /* Enable GPIOs and ADC1 clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                           RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);
    
    PWR_BackupAccessCmd(ENABLE);
    /* NVIC configuration */
    NVIC_Configuration();

    /* Configure the GPIO ports */
    GPIO_Configuration();
    USART_Configuration();
    DMA_Configuration();
    /* Configure the Ethernet peripheral */
    //Ethernet_Configuration();

    /* SystTick configuration: an interrupt every 10ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.SYSCLK_Frequency / 1000);

    /* Update the SysTick IRQ priority should be higher than the Ethernet IRQ */
    /* The Localtime should be updated during the Ethernet packets processing */
    NVIC_SetPriority (SysTick_IRQn, 2);
}

/**
  * @brief  Configures the Ethernet Interface
  * @param  None
  * @retval None
  */
void Ethernet_Configuration(void)
{
    ETH_InitTypeDef ETH_InitStructure;

    /* MII/RMII Media interface selection ------------------------------------------*/
#ifdef MII_MODE /* Mode MII with STM3210C-EVAL  */
    GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_MII);

    /* Get HSE clock = 25MHz on PA8 pin (MCO) */
    RCC_MCOConfig(RCC_MCO_HSE);

#elif defined RMII_MODE  /* Mode RMII with STM3210C-EVAL */
    GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);

    /* Set PLL3 clock output to 50MHz (25MHz /5 *10 =50MHz) */
    RCC_PLL3Config(RCC_PLL3Mul_10);
    /* Enable PLL3 */
    RCC_PLL3Cmd(ENABLE);
    /* Wait till PLL3 is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET)
    {}

    /* Get PLL3 clock on PA8 pin (MCO) */
    RCC_MCOConfig(RCC_MCO_PLL3CLK);
#endif

    /* Reset ETHERNET on AHB Bus */
    ETH_DeInit();

    /* Software reset */
    ETH_SoftwareReset();

    /* Wait for software reset */
    while (ETH_GetSoftwareResetStatus() == SET);

    /* ETHERNET Configuration ------------------------------------------------------*/
    /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
    ETH_StructInit(&ETH_InitStructure);

    /* Fill ETH_InitStructure parametrs */
    /*------------------------   MAC   -----------------------------------*/
    ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable  ;
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
    ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

    /*------------------------   DMA   -----------------------------------*/

    /* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
    the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum,
    if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
    ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
    ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
    ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
    ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
    ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

    /* Configure Ethernet */
    EthInitStatus = ETH_Init(&ETH_InitStructure, PHY_ADDRESS);

    /* Enable the Ethernet Rx Interrupt */
    ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);

}
/**************************************************************8

****************************************************************/
uint32_t check_link(uint16_t PHYAddress)
{
    if( !(ETH_ReadPHYRegister(PHYAddress, PHY_BSR) & PHY_Linked_Status) )
    {
        return ETH_ERROR;
    }
    else
    {
        return ETH_SUCCESS;
    }
}
void  check_ETH_link(void)
{
    EthInitStatus = check_link(DP83848_PHY_ADDRESS);
}
/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ETHERNET pins configuration */
    /* AF Output Push Pull:
    - ETH_MII_MDIO / ETH_RMII_MDIO: PA2
    - ETH_MII_MDC / ETH_RMII_MDC: PC1
    - ETH_MII_TXD2: PC2
    - ETH_MII_TX_EN / ETH_RMII_TX_EN: PB11
    - ETH_MII_TXD0 / ETH_RMII_TXD0: PB12
    - ETH_MII_TXD1 / ETH_RMII_TXD1: PB13
    - ETH_MII_PPS_OUT / ETH_RMII_PPS_OUT: PB5
    - ETH_MII_TXD3: PB8 */

    /* Configure PA2 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure PC1, PC2 and PC3 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Configure PB5, PB8, PB11, PB12 and PB13 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_11 |
                                  GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /**************************************************************/
    /*               For Remapped Ethernet pins                   */
    /*************************************************************/
    /* Input (Reset Value):
    - ETH_MII_CRS CRS: PA0
    - ETH_MII_RX_CLK / ETH_RMII_REF_CLK: PA1
    - ETH_MII_COL: PA3
    - ETH_MII_RX_DV / ETH_RMII_CRS_DV: PD8
    - ETH_MII_TX_CLK: PC3
    - ETH_MII_RXD0 / ETH_RMII_RXD0: PD9
    - ETH_MII_RXD1 / ETH_RMII_RXD1: PD10
    - ETH_MII_RXD2: PD11
    - ETH_MII_RXD3: PD12
    - ETH_MII_RX_ER: PB10 */

    /* ETHERNET pins remapp in STM3210C-EVAL board: RX_DV and RxD[3:0] */
    GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);

    /* Configure PA0, PA1 and PA3 as input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure PB10 as input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure PC3 as input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Configure PD8, PD9, PD10, PD11 and PD12 as input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* MCO pin configuration------------------------------------------------- */
    /* Configure MCO (PA8) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //LED
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    //��ʼ������4����--RS485:3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //485
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    UART4_RX_485;

    //DIP
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef   NVIC_InitStructure;

    /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* Enable the Ethernet global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

/**
  * @brief  Calculate the number of pages
  * @param  Size: The image size
  * @retval The number of pages
  */
uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
    uint32_t pagenumber = 0x0;
    uint32_t size = Size;

    if ((size % PAGE_SIZE) != 0)
    {
        pagenumber = (size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;

}
/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
