#include "main.h"

/*****************************************************************************

******************************************************************************/
void RCC_Configuration(void)
{
    RCC_ClocksTypeDef RCC_Clocks;

    SystemCoreClockUpdate();

    /* Configure Systick clock source as HCLK */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

    /* SystTick configuration: an interrupt every 10ms */
    RCC_GetClocksFreq(&RCC_Clocks);

    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD |
                           RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG |
                           RCC_AHB1Periph_DMA1 | RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_BKPSRAM, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_USART6 | RCC_APB2Periph_SYSCFG, ENABLE);
    
    PWR_BackupAccessCmd(ENABLE);
}
/*************************************************************************

*************************************************************************/
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/****************************************************************

*****************************************************************/
void GPIO_Configuration(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    //led
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    
    //CAN 1
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    //CAN2

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_CAN2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_CAN2);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
}

void CAN_Configuration(void)
{
    CAN_InitTypeDef        CAN_InitStructure;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    /* CAN register init */
    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);

    /* CAN cell init */
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = ENABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = ENABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    //CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
    //CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;    //SamplePoint  (1+13) / (1+13+2) = 87.5%
    //CAN_InitStructure.CAN_Prescaler = 15;       //BaudRate   30M /15/(1+13+2) = 125kbps

    //    CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
    //    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;    //SamplePoint  (1+12) / (1+12+2) = 86.6%
    //    CAN_InitStructure.CAN_Prescaler = 8;       //BaudRate   30M /8/(1+12+2) = 250kbps

    CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;    //SamplePoint  (1+12) / (1+12+2) = 86.6%
    CAN_InitStructure.CAN_Prescaler = 4;       //BaudRate   30M /4/(1+12+2) = 500kbps

//CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
//CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;    //SamplePoint  (1+12) / (1+12+2) = 87.5%
//CAN_InitStructure.CAN_Prescaler = 2;       //BaudRate   30M /2/(1+12+2) = 1Mbps

    CAN_Init(CAN1, &CAN_InitStructure);

    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

    /* CAN register init */
    CAN_DeInit(CAN2);
    CAN_StructInit(&CAN_InitStructure);

    /* CAN cell init */
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = ENABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = ENABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    //    CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
    //    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;    //SamplePoint  (1+13) / (1+13+2) = 87.5%
    //    CAN_InitStructure.CAN_Prescaler = 15;       //BaudRate   30M /15/(1+13+2) = 125kbps

    //      CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
    //      CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;    //SamplePoint  (1+12) / (1+12+2) = 86.7%
    //      CAN_InitStructure.CAN_Prescaler = 8;       //BaudRate   30M /48(1+12+2) = 250kbps

    //    
    CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;    //SamplePoint  (1+12) / (1+12+2) = 86.7%
    CAN_InitStructure.CAN_Prescaler = 4;       //BaudRate   30M /4/(1+12+2) = 500kbps

//CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
//CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;    //SamplePoint  (1+12) / (1+12+2) = 86.7%
//CAN_InitStructure.CAN_Prescaler = 2;       //BaudRate   30M /2/(1+12+2) = 1Mbps

    CAN_Init(CAN2, &CAN_InitStructure);

    CAN_FilterInitStructure.CAN_FilterNumber = 14;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO1;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    CAN_ITConfig(CAN2, CAN_IT_FMP1, ENABLE);
}

/********************************************************************8

********************************************************************/
void BSP_Init(void)
{
    RCC_Configuration();
    NVIC_Configuration();
    GPIO_Configuration();
    CAN_Configuration();
}
