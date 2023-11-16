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
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 1ms */
uint32_t timingdelay;
u16 sec_reg = 100;
u16 sec_flag = 0;
u8  Local_Station = 0;



//1s
void sec_process(void)
{
    if( sec_flag == 1 )
    {
        sec_flag = 0;

        if( LED_STATE )
        {
            LED_ON;
        }
        else
        {
            LED_OFF;
        }
//        can_bus_send_port_enable_state();
//        usart6_send_test_msg();
//        usart1_send_test_msg();
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
    
    Local_Station = DIP_STATE;
    vcanbus_initcansendqueue_two();

    while (1)
    {
        sec_process();
        vcan_sendframe_process_two();
        usart6_recv_buff_process();
        usart1_recv_buff_process();
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

    if(manual_flag == MANUAL_VALID){
      handleInputScan();
    }
    else if(manual_flag == MANUAL_INVAILD){
      InputScanProc();
    }

    if(manual_operation == OPERATION_VALID){
      io_open_all();
      manual_operation = OPERATION_INVALID;
    }
//    OutPutCtrlProc();
    Local_Station = DIP_STATE;
    io_outtest_void();
    WritePortStateBit();
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
