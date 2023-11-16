/* Includes ------------------------------------------------------------------*/
//#include "stm32_eth.h"
//#include "stm32f207.h"
#include "main.h"
//#include "flash_if.h"

volatile u8  ParcelFeeder_ID_Para=0;  //20170621

static uint32_t CpuID[3];
uint32_t Mac_Code;
static uint16_t flash_memory;


NET_DATA_IN_FLASH	net_data_memory;		//主服务器地址需要存储
NET_FLAG_DATA_IN_FLASH  net_flag_data_memory; //20170615


/************初始化函数****************/
void System_Init(void)
{
    RCC_Configuration();
    GPIO_Configuration();
//    ETH_GPIO_Config();

}
/*************服务子函数****************/


void RCC_Configuration(void)
{
	 RCC_ClocksTypeDef RCC_Clocks;
	 
	 SystemCoreClockUpdate();
	/* printf("\n\r 系统内核时钟频率(SystemCoreClock)为：%dHz.\n\r",
			SystemCoreClock);*/

	 /* Configure Systick clock source as HCLK */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	
    /* SystTick configuration: an interrupt every 10ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.SYSCLK_Frequency / 1000);
  
    /* Update the SysTick IRQ priority should be higher than the Ethernet IRQ */
    /* The Localtime should be updated during the Ethernet packets processing */
    NVIC_SetPriority (SysTick_IRQn, 0);
	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | 
						   RCC_AHB1Periph_GPIOE |RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG // RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_BKPSRAM 
						    , ENABLE);

//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | 
//						   RCC_AHB1Periph_GPIOE |RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG |// RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_BKPSRAM 
//						   RCC_AHB1Periph_DMA1 |RCC_AHB1Periph_DMA2 , ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_USART3| RCC_APB1Periph_UART4|RCC_APB1Periph_PWR |
						   RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM4//RCC_APB2Periph_ADC1|RCC_APB2Periph_ADC3|RCC_APB1Periph_CAN1
                                                   , ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_USART3| RCC_APB1Periph_UART4| RCC_APB1Periph_UART5|RCC_APB1Periph_PWR |
//						   RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM12|//RCC_APB2Periph_ADC1|RCC_APB2Periph_ADC3|RCC_APB1Periph_CAN1
//                                                   RCC_APB2Periph_ADC3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_TIM8|RCC_APB2Periph_TIM11|
                                                   RCC_APB2Periph_SYSCFG,ENABLE);							   	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_USART6|RCC_APB2Periph_TIM1|RCC_APB2Periph_TIM8|RCC_APB2Periph_TIM9|RCC_APB2Periph_TIM11|
//                                                   RCC_APB2Periph_SYSCFG,ENABLE);
}
void GPIO_Configuration(void)
{
  
    u32 i =0;
    GPIO_InitTypeDef  GPIO_InitStructure; 
    
    //PHY RESET
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_RESET);
//    for(i= 0x00124F80; i>0; i--){
//    }
//    for(i= 0x00249F00; i>0; i--){
//    }
    for(i= 0x00F5FFFF; i>0; i--){
    }
//    GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_SET);
    
    /*********LED configuration*******/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    /* key button configuration*/     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_6;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//**********READ_STM32ID*************// 96bit
void GetCPU_ID(void)
{
	//获取CPU唯一ID
	CpuID[0]=*(uint32_t*)(0x1fff7a10); //高地址
	CpuID[1]=*(uint32_t*)(0x1fff7a14);
	CpuID[2]=*(uint32_t*)(0x1fff7a18);  //低地址

	flash_memory = *(uint16_t *)(0X1FFF7a22);

	Mac_Code = (CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3);
}
/********************************************************************
读取系统参数
*********************************************************************/
void read_sys_data_process(void)
{
    GetCPU_ID();//read_stm32ID
    
    readFlash_contiue(NET_PATA_FLASH_ADDR,(u8*)(&net_flag_data_memory), sizeof(NET_FLAG_DATA_IN_FLASH)); // 参数读取标志位
    readFlash_contiue(SYS_LOCAL_IP_PARA_ADDR,(u8*)(&net_data_memory), sizeof(NET_DATA_IN_FLASH)); //读取flash网络参数
 
    ParcelFeeder_ID_Para = *((u8*)SYS_ID_PARA_ADDR); //20170621,ID

   //参数读取flash标志
    net_sys_para_temp[0] = net_flag_data_memory. net_flag[0];
    net_sys_para_temp[1] = net_flag_data_memory. net_flag[1];
    net_sys_para_temp[2] = net_flag_data_memory. net_flag[2];
    net_sys_para_temp[3] = net_flag_data_memory. net_flag[3];
    net_sys_para_temp[4] = net_flag_data_memory. net_flag[4];
    net_sys_para_temp[5] = net_flag_data_memory. net_flag[5];
    net_sys_para_temp[6] = net_flag_data_memory. net_flag[6];

   if(net_sys_para_temp[0] == 0x0060)               
   {
    
      static_ip[0] = net_data_memory.IP[0];   //local-ip
    	static_ip[1] = net_data_memory.IP[1];
    	static_ip[2] = net_data_memory.IP[2];
    	static_ip[3] = net_data_memory.IP[3];
   }
   else
   {
      static_ip[0] = 10;     //local ip
    	static_ip[1] = 48;
    	static_ip[2] = 10;
    	static_ip[3] = 110;   
   } 

   //if((net_sys_para_temp == 0xFFFF) || (net_sys_para_temp != 0x0061))
   if(net_sys_para_temp[1] == 0x0061)      
   {
      
        Dest_ip_Addr[0] = net_data_memory.IP[4];  //dest-ip
        Dest_ip_Addr[1] = net_data_memory.IP[5];
        Dest_ip_Addr[2] = net_data_memory.IP[6];
        Dest_ip_Addr[3] = net_data_memory.IP[7];
    }
    else
    {
        Dest_ip_Addr[0]=10;   //dest ip
        Dest_ip_Addr[1]=48;
        Dest_ip_Addr[2]=10;
        Dest_ip_Addr[3]=210;
    
    }
   //if((net_sys_para_temp == 0xFFFF) || (net_sys_para_temp != 0x0062))
   if(net_sys_para_temp[2] == 0x0062)
   {   
        static_mask[0] = net_data_memory.MASK[0];  //mask
    	static_mask[1] = net_data_memory.MASK[1];
    	static_mask[2] = net_data_memory.MASK[2];
    	static_mask[3] = net_data_memory.MASK[3];

    }
    else
    {
       	static_mask[0] = 255;    //mask
    	static_mask[1] = 255;
    	static_mask[2] = 255;
    	static_mask[3] = 0;
    
    }
    
    //if((net_sys_para_temp == 0xFFFF) || (net_sys_para_temp != 0x0063))
    if(net_sys_para_temp[3] == 0x0063)	
    {
    	
        static_netway[0] = net_data_memory.NETWAY[0];  //netway
    	static_netway[1] = net_data_memory.NETWAY[1];
    	static_netway[2] = net_data_memory.NETWAY[2];
    	static_netway[3] = net_data_memory.NETWAY[3];
    }
    else
    {
        static_netway[0] = 10;  //net way
    	static_netway[1] = 48;
    	static_netway[2] = 10;
    	static_netway[3] = 1;   
    }
    

     if(net_sys_para_temp[4] == 0x0064)	
     {   
        mac_addr[0] = net_data_memory.MAC[0];    //mac
        mac_addr[1] = net_data_memory.MAC[1];
        mac_addr[2]  = (u8) ((Mac_Code >>24)& 0x000000FF);
        mac_addr[3]  = (u8) ((Mac_Code >>16)& 0x000000FF);
        mac_addr[4]  = (u8) ((Mac_Code >>8)& 0x000000FF);
        mac_addr[5]  = (u8) ( Mac_Code & 0x000000FF);
//        mac_addr[2] = net_data_memory.MAC[2];
//        mac_addr[3] = net_data_memory.MAC[3];
//        mac_addr[4] = net_data_memory.MAC[4];
//        mac_addr[5] = net_data_memory.MAC[5];
     }
     else
     {
        mac_addr[0]  = 1;       //mac
        mac_addr[1]  = 0;
        mac_addr[2]  = (u8) ((Mac_Code >>24)& 0x000000FF);
        mac_addr[3]  = (u8) ((Mac_Code >>16)& 0x000000FF);
        mac_addr[4]  = (u8) ((Mac_Code >>8)& 0x000000FF);
        mac_addr[5]  = (u8) ( Mac_Code & 0x000000FF);
     }
    
     
     if(net_sys_para_temp[5] == 0x0065)
     {
        local_port_data = net_data_memory.PORT[0];//local   
     }
     else
     {
        local_port_data=9000;   //local
     }
     
     if(net_sys_para_temp[6] == 0x0066)        
     {	
        dest_port_data = net_data_memory.PORT[1]; //dest
     }
     else
     {        
        dest_port_data=8000;    //dest
     }

}


/******************************************************************
读连续地址的数据，按字节为单位
address：读取的首地址
out_point：存储数据首地址
len：读取长度
******************************************************************/
void readFlash_contiue(uint32_t address,u8 *out_point, u32 len) 
{
	u8 *source;
	u32 i;

	source=(u8*)(address);
	for(i = 0;i < len;i++)
	{
		*( out_point + i ) = *(source+i);	
	}
}





