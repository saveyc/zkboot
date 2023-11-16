/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   Main program body
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
#include "netconf.h"
#include "main.h"
#include "TCPclient.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay;

u16 sec_reg = 1000;
u16 ms500_reg = 500;
u16 sec_flag = 0;

u8 key_value = 0;
u8 second_cnt = 0;
u8 one_ms_flag = 0;


/* Private function prototypes -----------------------------------------------*/
void System_Periodic_Handle(void);
void main_init_cmd(void);
void main_one_ms_sec_process(void);
void main_rolling_process(void);
void Pre_Read_UpdateFlag(void);
void scan_key_process(void);

u16 usart1_test_delay = 0;
u16 usart6_test_delay = 0;
u16 can1_test_delay = 0;
u16 can2_test_delay = 0;





//1s
void sec_process(void)
{
    u8 i;
    u8 buf[10] = { 0 };
    u8 send_len = 0;
    char str[25];
        if( sec_flag == 1 )
        {
            sec_flag = 0;
            second_cnt++;
            
            if(LED_STATUE)
            {
                LED_ON;
            }
            else
            {
                LED_OFF;
            }
            for(i = 0; i < TCP_CLIENT_MAX_NUM; i++)
            {
                if(tcp_client_list[i].connect_dely != 0 )
                {
                    tcp_client_list[i].connect_dely--;
                }
            }
    
//            udp_send_can_bus_test();
    
//            second_cnt++;
//            vcan_sendmsg_one(buf,8,CAN_FUNC_ID_HEART_TYPE,1);
//            vcan_sendmsg_two(buf,8,CAN_FUNC_ID_HEART_TYPE,1);
    
            if ((key_value != 1) && (EthInitStatus == 1)) {
                send_len = sprintf(str, "\n");
                usart1_test_delay = 10;
                usart6_test_delay = 80;
                can1_test_delay = 160;
                can2_test_delay = 240;
                DEBUG_process((u8*)str, send_len);
//                vcan1_send_check_slaver();
//                vcan2_send_check_slaver();
//                uart4_send_usart_send_recv_test();
//                usart1_send_usart_send_recv_test();
//                scan_key_process();
//                photo_test();
            }
    
    
    //                usart1_moto_test_process();
    //                usart2_moto_test_process();
    //                uart4_moto_test_process();
        }
        

 
}
/***********************************************************************

****************************************************************************/
void scan_key_process(void)
{
    u8 tmp, i;
    u8 j = 0;
    u8 ttmmp[10];
    tmp = 0;
    u8 send_len = 0;
    char str[80];

    for(i = 0; i < 20; i++)
    {
        for(j=0; j< 8; j++)
        {
           ttmmp[j] = 1;
        }
        if( key1_STATUE )
        {
            tmp |= 0x01;
            ttmmp[0] = 0;
            
        }
        if( key2_STATUE )
        {
            tmp |= 0x02;
            ttmmp[1] = 0;
        }
        if( key3_STATUE )
        {
            tmp |= 0x04;
            ttmmp[2] = 0;            
        }
        if( key4_STATUE )
        {
            tmp |= 0x08;
            ttmmp[3] = 0;            
        }
        if( key5_STATUE )
        {
            tmp |= 0x10;
            ttmmp[4] = 0;            
        }
        if( key6_STATUE )
        {
            tmp |= 0x20;              
            ttmmp[5] = 0;
        }
        if( key7_STATUE )
        {
            tmp |= 0x40;
            ttmmp[6] = 0;            
        }
        if( key8_STATUE )
        {
            tmp |= 0x80;
            ttmmp[7] = 0;            
        }
        if( key_value != tmp )
        {
            key_value = tmp;
            i = 0;
        }
    }
    key_value = ~key_value;

//    send_len = sprintf(str, "DIP1:%d DIP2:%d DIP3:%d DIP4:%d DIP5:%d DIP6:%d DIP7:%d DIP8:%d \n", ttmmp[0],
//                       ttmmp[1],ttmmp[2],ttmmp[3],ttmmp[4],ttmmp[5],ttmmp[6],ttmmp[7]);
//    DEBUG_process((u8*)str, send_len);
    

}
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    System_Setup();
    scan_key_process();
    Pre_Read_UpdateFlag();
    main_init_cmd();
    /* Infinite loop */
    while (1)
    {
        if (key_value != 1) {
            if (EthInitStatus == 0)
            {
                Ethernet_Configuration();
              if (EthInitStatus != 0)
              {
                    LwIP_Init();
              }
            }
            else
            {
                if (ETH_GetRxPktSize() != 0)
                {
                    LwIP_Pkt_Handle();
                }

                LwIP_Periodic_Handle(LocalTime);
                vpcfun_send_message_to_sever();
                udp_client_process();

            }


            main_one_ms_sec_process();
            sec_process();
            main_rolling_process();
            uart4_send_uart4_recv_msg();
            usart1_send_usart1_recv_msg();
//            usart4_recv_buff_process();
//            usart1_recv_buff_process();
        }
        else {
            sec_process();
            main_one_ms_sec_process();
            main_rolling_process();
            uart4_send_uart4_recv_msg();
            usart1_send_usart1_recv_msg();
//            usart4_recv_buff_process();
//            usart1_recv_buff_process();

        }

    }
}

u16 readFlash_uint(uint32_t address)
{
    u8* source;
    u32 i;
    u16 out_point;

    source = (u8*)(address);
    out_point = 0;
    for (i = 0; i < 2; i++)
    {
        out_point = out_point << 8;
        out_point |= *(source + (1 - i));
    }
    return(out_point);
}


void Pre_Read_UpdateFlag(void)
{
    if (readFlash_uint(UserAppEnFlagAddress) != 0x00AA)
    {
        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
        if (FLASH_ErasePage(UserAppEnFlagAddress) == FLASH_COMPLETE)
        {
            FLASH_ProgramHalfWord(UserAppEnFlagAddress, 0x00AA);
        }
        FLASH_Lock();
    }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 10ms periods to wait for.
  * @retval None
  */
void Delay(uint32_t nCount)
{
    // Capture the current local time
    timingdelay = LocalTime + nCount;

    // wait until the desired delay finish
    while(timingdelay > LocalTime)
    {
    }
}

/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
u16 s_LED_Interval = 0;
u16 s_LED_Local_cnt = 0;

void Time_Update(void)
{

        /*-------------------------------------------*/
        LocalTime += SYSTEMTICK_PERIOD_MS;
        /*-------------------------------------------*/
         
        one_ms_flag = VALUE;

        if( sec_reg != 0 )
        {
            sec_reg--;
            if( sec_reg == 0 )
            {
                sec_reg = 1000;
                sec_flag = 1;
            }
        }
}

void main_one_ms_sec_process(void)
{
    if (one_ms_flag == VALUE) {
        one_ms_flag = INVALUE;


        //uart
        usart1_moto_reply_time_count();
        vusart1_upload_cartest_peocess();
        usart1_ctr_process();

        uart4_moto_reply_time_count();
        vuart4_upload_cartest_peocess();
        uart4_ctr_process();


        vusart2_upload_cartest_peocess();
        usart2_moto_reply_time_count();
        usart2_ctr_process();
        
        if(usart1_test_delay !=0){
          usart1_test_delay--;
          if(usart1_test_delay ==0){
             usart1_send_usart_send_recv_test();
          }
        }
        
                if(usart6_test_delay !=0){
          usart6_test_delay--;
          if(usart6_test_delay ==0){
             uart4_send_usart_send_recv_test();
          }
        }
        
                if(can1_test_delay !=0){
          can1_test_delay--;
          if(can1_test_delay ==0){
             vcan1_send_check_slaver();
          }
        }
        
                if(can2_test_delay !=0){
          can2_test_delay--;
          if(can2_test_delay ==0){
             vcan2_send_check_slaver();
          }
        }

        //photo

      
    }
}

void main_init_cmd(void)
{
    //fun init
    vpcfun_init_net_Queue();

    //can init
    vcanbus_initcansendqueue_one();
    vcanbus_init_cansendqueue_two();

    //data init
    Data_Init_struct();
    Data_Init_motor_para_queue();

    //uart init
    vuart4_moto_init();
    vusart1_moto_init();
    vusart2_moto_init();

    // list init
    vdata_list_item_init();
    vdata_list_int();

    //photo init
    vphoto_init_msg();

    
}

void main_rolling_process(void)
{
    vcan_sendframe_process_one();
    vcan_send_frame_process();
}


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


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
