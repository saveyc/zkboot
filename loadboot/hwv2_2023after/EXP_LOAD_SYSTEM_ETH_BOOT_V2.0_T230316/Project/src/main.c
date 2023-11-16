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
//#include "httpserver.h"
#include "udpclient.h"
/* Private typedef -----------------------------------------------------------*/
typedef  void(*pFunction)(void);
/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  1
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay;
pFunction Jump_To_Application;
uint32_t JumpAddress;
void Jump_To_User_App(void); //20170605 用户程序
void TFTP_IAP(void); //网络升级
void sec_process(void);
__IO u8 App_flag = 0;
u16 sec_reg = 200;//1000;
u16 sec_flag = 0;
u16 reset_cnt;
/****local-net-data*******/
u8 static_ip[4] = { 0 }; //110
u8 static_mask[4] = { 0 };
u8 static_netway[4] = { 0 };
u8 mac_addr[6] = { 0 };
u16 local_port_data = 0;
/****dest-net-data*******/
u8 Dest_ip_Addr[4] = { 0 };
u16 dest_port_data = 0;
u8  net_sys_para_temp[7] = { 0 };
//********end**********//
u8 Programm_Completed_Reply_flag = 0;//烧写完成标志

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
//******烧写成功***********//
int main(void)
{
    System_Init();//系统初始化
    
    FLASH_Unlock();
    FLASH_EraseSector(FLASH_Sector_9, VoltageRange_3);
    FLASH_ProgramHalfWord(UserMACFlagAddress ,1);
    FLASH_ProgramHalfWord(UserMACFlagAddress + 2 ,2);
    FLASH_ProgramHalfWord(UserMACFlagAddress + 4 ,3);
    FLASH_ProgramHalfWord(UserMACFlagAddress + 6 ,4);
    FLASH_ProgramHalfWord(UserMACFlagAddress + 8 ,5);
    FLASH_ProgramHalfWord(UserMACFlagAddress + 10 ,6);
    FLASH_Lock();
    
    while(1)//检测是否需要清空用户程序
    {
        if(IN_3_STATE)
        {
            if(reset_cnt > 25)//5秒
            {
                FLASH_Unlock();
                FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
                FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3); /* 64 Kbyte */
                FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3); /* 128 Kbyte */
                FLASH_EraseSector(FLASH_Sector_10, VoltageRange_3);
                FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3);
                FLASH_Lock();
                NVIC_SystemReset();
            }
        }
        else
        {
            break;
        }
        sec_process();
    }

    read_sys_data_process(); //初始化网络参数
    App_flag = *((u8 *)UserAppEnFlagAddress);
    if (App_flag == 0xFF)
    {
        App_flag = 0;
    }
    if (App_flag != 0xAA)
    {
        TFTP_IAP();
    }
    else
    {
        Jump_To_User_App();
    }
}
void Jump_To_User_App(void)
{
    if (((*(__IO uint32_t *)USER_FLASH_FIRST_PAGE_ADDRESS) & 0x2FFE0000) == 0x20000000)
    {
        u8 i;
        for (i = 0; i < 8; i++)
        {
            NVIC->ICER[i] = 0xFFFFFFFF;
            NVIC->ICPR[i] = 0xFFFFFFFF;
        }
        JumpAddress = *(__IO uint32_t *) (USER_FLASH_FIRST_PAGE_ADDRESS + 4);
        Jump_To_Application = (pFunction)JumpAddress;
        __set_MSP(*(__IO uint32_t *) USER_FLASH_FIRST_PAGE_ADDRESS);
        Jump_To_Application();
    }
}
/***********网络升级******************/
void TFTP_IAP(void)
{
    InitSendMsgQueue();
    ETH_BSP_Config();
    LwIP_Init();
    UDP_server_init();
    IAP_tftpd_init();
    while (1)
    {
        if (ETH_CheckFrameReceived())
        {
            LwIP_Pkt_Handle();
        }
        LwIP_Periodic_Handle(LocalTime);
        if (Programm_Completed_Success == 1)
        {
            Programm_Completed_Success = 0;

            if (g_download_status == DLOAD_PARA)
            {
                g_download_status = DLOAD_INIT;
                FLASH_Unlock();
                FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

                if (FLASH_EraseSector(FLASH_Sector_10, VoltageRange_3) == FLASH_COMPLETE)
                {
                    FLASH_ProgramByte(UserAppEnFlagAddress, 0xAA);  //表已经升级完成
                }
                FLASH_Lock();
                AddSendMsgToQueue(SEND_MSG_COMPLETE_UPGRADE_TYPE);  //升级完成反馈
            }
        }
        udp_client_process();
        send_message_to_sever();
        sec_process();
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
    while (timingdelay > LocalTime)
    {
    }
}
void sec_process(void)
{
    if (sec_flag == 1)
    {
        sec_flag = 0;
        if (LED_STATE)
        {
            LED_ON;
        }
        else
        {
            LED_OFF;
        }
        reset_cnt++;
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

    if (sec_reg != 0)
    {
        sec_reg--;
        if (sec_reg == 0)
        {
            sec_reg = 200;//1000;
            sec_flag = 1;
        }
    }
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
