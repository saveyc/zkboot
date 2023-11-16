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
#include "tftpserver.h"
#include "memp.h"
#include "udpclient.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 1ms */
uint32_t timingdelay;
u16 sec_reg = 250;
u16 sec_flag = 0;
u8  Local_Station;//±¾»úÕ¾ºÅ
__IO u16 app_flag = 0;

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;
extern uint8_t Programm_Completed_Success;
/* Private function prototypes -----------------------------------------------*/
void TFTP_IAP(void);

void scan_local_station(void)
{
    Local_Station = (~((DIP_6_STATE<<5)|(DIP_5_STATE<<4)|(DIP_4_STATE<<3)|(DIP_3_STATE<<2)|(DIP_2_STATE<<1)|DIP_1_STATE))&0x3F;
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
        }
        else
        {
            LED_OFF;
        }
    }
}

void Jump_To_User_App(void)
{
    /* Test if user code is programmed starting from address "ApplicationAddress" */
    if (((*(__IO uint32_t *)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
    {
        /* Jump to user application */
        JumpAddress = *(__IO uint32_t *) (ApplicationAddress + 4);
        Jump_To_Application = (pFunction) JumpAddress;
        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t *) ApplicationAddress);
        Jump_To_Application();
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
    scan_local_station();
    app_flag = *((u16*)UserAppEnFlagAddress);

    if(DIP_8_STATE==0 || RTC_ReadBackupRegister(RTC_BKP_DR0)==0x55 || app_flag!=0xAA)
    {
        if(Local_Station==1 || DIP_8_STATE==0)
            TFTP_IAP();
        else
            can_bus_reply_process();
    }
    else
    {
        Jump_To_User_App();
    }
}
void TFTP_IAP(void)
{
    while(EthInitStatus == 0 )
    {
        ETH_BSP_Config();
    }
    InitSendMsgQueue();
    /* Initialize the LwIP stack */
    LwIP_Init();
    /* Initialize the tftpd*/
    tftpd_init();
    /* Infinite loop */
    while (1)
    {
        /* Periodic tasks */
        if(ETH_CheckFrameReceived())
        {
	    LwIP_Pkt_Handle();
	}
        LwIP_Periodic_Handle(LocalTime);
        if(Programm_Completed_Success == 1)
        {
            Programm_Completed_Success = 0;
            
            if(sDLoad_Para_Data.mode == DLOAD_MODE_LOCAL)
            {
                g_download_status = DLOAD_INIT;
                FLASH_Unlock();
                FLASH_ProgramHalfWord(UserAppEnFlagAddress,0xAA);
                FLASH_Unlock();
            }
            else if(sDLoad_Para_Data.mode == DLOAD_MODE_TRANS)
            {
                g_download_status = DLOAD_TRAN_INIT;
                cur_tran_station = sDLoad_Para_Data.st_addr;
                if(file_tot_bytes % CAN_SEND_DATA_PK_SIZE)
                    pk_tot_num = (file_tot_bytes / CAN_SEND_DATA_PK_SIZE)+1;
                else
                    pk_tot_num = (file_tot_bytes / CAN_SEND_DATA_PK_SIZE);
                    pk_cur_num = 1;
                    trans_flag = 0;
            }
        }
        udp_client_process();
        send_message_to_sever();
        host_transmit_process();
        can_send_frame_process();
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
            sec_reg = 250;
            sec_flag = 1;
        }
    }
    sec_process();
    can_recv_timeout();
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
