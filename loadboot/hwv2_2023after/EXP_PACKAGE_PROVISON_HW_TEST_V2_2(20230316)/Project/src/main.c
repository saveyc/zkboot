/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    07-October-2011
  * @brief   Main program body
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
#include "stm32f2x7_eth.h"
#include "netconf.h"
#include "main.h"
#include "udpclient.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 1ms */
uint32_t timingdelay;
u16 sec_reg = 1000;
u16 sec_flag = 0;
u16 udp_send_reg = 100;
u8  print_msg_buff[200];
u8  rs232_1_debug_cnt = 0;
u8  rs232_2_debug_cnt = 0;
u8  rs232_5_debug_cnt = 0;
u8  rs485_3_debug_cnt = 0;
u8  rs485_4_debug_cnt = 0;
u8  rs485_6_debug_cnt = 0;

void udp_send_encoder_cnt_test(void)
{
    u16 print_msg_len;
    
    if( udp_send_reg != 0 )
    {
        udp_send_reg--;
        if( udp_send_reg == 0 )
        {
            udp_send_reg = 100;
            print_msg_len = sprintf((char*)print_msg_buff,"Encoder:%d,%d,%d  RS1:%d,RS2:%d,RS3:%d,RS4:%d,RS5:%d,RS6:%d\n",
                                    TIM_GetCounter(TIM3),TIM_GetCounter(TIM4),TIM_GetCounter(TIM2),
                                    rs232_1_debug_cnt,rs232_2_debug_cnt,rs485_3_debug_cnt,rs485_4_debug_cnt,rs232_5_debug_cnt,rs485_6_debug_cnt);
            udp_send_message(print_msg_buff,print_msg_len);
        }
    }
}
void can_send_test(void)
{
    CanTxMsg TxMessage;
    
    TxMessage.ExtId = 0;
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = 5;
    TxMessage.Data[0] = IN_1_STATE|(IN_2_STATE<<1)|(IN_3_STATE<<2)|(IN_4_STATE<<3)|(IN_5_STATE<<4)|(IN_6_STATE<<5)|(IN_7_STATE<<6)|(IN_8_STATE<<7);
    TxMessage.Data[1] = IN_9_STATE|(IN_10_STATE<<1)|(IN_11_STATE<<2)|(IN_12_STATE<<3)|(IN_13_STATE<<4)|(IN_14_STATE<<5)|(IN_15_STATE<<6)|(IN_16_STATE<<7);
    TxMessage.Data[2] = IN_17_STATE|(IN_18_STATE<<1)|(IN_19_STATE<<2)|(IN_20_STATE<<3)|(IN_21_STATE<<4)|(IN_22_STATE<<5)|(IN_23_STATE<<6)|(IN_24_STATE<<7);
    TxMessage.Data[3] = IN_25_STATE|(IN_26_STATE<<1)|(IN_27_STATE<<2)|(IN_28_STATE<<3)|(IN_29_STATE<<4)|(IN_30_STATE<<5)|(IN_31_STATE<<6)|(IN_32_STATE<<7);
    TxMessage.Data[4] = IN_33_STATE|(IN_34_STATE<<1);
    
    CAN_Transmit(CAN1,&TxMessage);
}
void out_put_test(BitAction BitVal)
{
    OUT_1_(BitVal);
    OUT_2_(BitVal);
    OUT_3_(BitVal);
    OUT_4_(BitVal);
    OUT_5_(BitVal);
    OUT_6_(BitVal);
    OUT_7_(BitVal);
    OUT_8_(BitVal);
    OUT_9_(BitVal);
    OUT_10_(BitVal);
    OUT_11_(BitVal);
    OUT_12_(BitVal);
    OUT_13_(BitVal);
    OUT_14_(BitVal);
    OUT_15_(BitVal);
    OUT_16_(BitVal);
    
    MOTOR_OUT_1_(BitVal);
    MOTOR_OUT_2_(BitVal);
    MOTOR_OUT_3_(BitVal);
    MOTOR_OUT_4_(BitVal);
    
    MOTOR_DIR_1_(BitVal);
    MOTOR_DIR_2_(BitVal);
    MOTOR_DIR_3_(BitVal);
    MOTOR_DIR_4_(BitVal);
}
void pwm_test(void)
{
    TIM_SetCompare1(TIM10,sec_reg);
    TIM_SetCompare1(TIM11,sec_reg);
    TIM_SetCompare1(TIM9,sec_reg);
    TIM_SetCompare4(TIM1,sec_reg);
}
//1s
void sec_process(void)
{
    if( sec_flag == 1 )
    {
        sec_flag = 0;

        if( LED_STATE )
        {
            LED_ON;
            
            out_put_test(Bit_SET);
        }
        else
        {
            LED_OFF;
            
            out_put_test(Bit_RESET);
        }
        uart_test_send();
        can_send_test();
    }
}
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
 int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured to
         120 MHz, this is done through SystemInit() function which is called from
         startup file (startup_stm32f2xx.s) before to branch to application main.
         To reconfigure the default setting of SystemInit() function, refer to
         system_stm32f2xx.c file
       */
    BSP_Init();

    while (1)
    {
        if(EthInitStatus == 0 )
        {
            ETH_BSP_Config();
            if( EthInitStatus != 0 )
            {
                LwIP_Init();
            }
        }
        else
        {
            if(ETH_CheckFrameReceived())
            {
	        LwIP_Pkt_Handle();
	    }
            LwIP_Periodic_Handle(LocalTime);
            udp_client_process();
        }
        sec_process();
        uart_test_recv();
    }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 1ms periods to wait for.
  * @retval None
  */
void Delay(uint32_t nCount)
{
    /* Capture the current local time */
    timingdelay = LocalTime + nCount;

    /* wait until the desired delay finish */
    while(timingdelay > LocalTime)
    {
    }
}

/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
void Time_Update(void)
{
    LocalTime += SYSTEMTICK_PERIOD_MS;

    if( sec_reg != 0 )
    {
        sec_reg--;
        if( sec_reg == 0 )
        {
            sec_reg = 1000;
            sec_flag = 1;
        }
    }
    uart_recv_timeout();
    pwm_test();
    udp_send_encoder_cnt_test();
}

/**
  * @brief  Initializes the STM322xG-EVAL's LCD and LEDs resources.
  * @param  None
  * @retval None
  */
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {}
}
#endif


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
