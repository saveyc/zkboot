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
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_CAN1, ENABLE);

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
    
    //CAN
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
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
    /* CAN cell init */
    CAN_InitStructure.CAN_TTCM = DISABLE;		//时间触发
    CAN_InitStructure.CAN_ABOM = ENABLE;		//自动离线管理
    CAN_InitStructure.CAN_AWUM = DISABLE;		//自动唤醒
    CAN_InitStructure.CAN_NART = DISABLE;		//自动重传
    CAN_InitStructure.CAN_RFLM = DISABLE;               //接收FIFO锁定
    CAN_InitStructure.CAN_TXFP = ENABLE;                //发送FIFO模式
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	//正常传输模式
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;    //SamplePoint  (1+13) / (1+13+2) = 87.5%
    CAN_InitStructure.CAN_Prescaler = 15;       //BaudRate   30M /15/(1+13+2) = 125kbps
    CAN_Init(CAN1, &CAN_InitStructure);

    /* CAN 过滤器设置 */
    //只接收主机发的CAN帧
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0008;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x07F8;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    /* 允许FMP0中断   FIFO消息挂号*/
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
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
