#ifndef __STM32F107_H
#define __STM32F107_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
//#include "stm32f2xx.h"
// boot��64K,�û�������96K,��ʱ�洢��96K
//#define ApplicationAddress                0x8010000  //�û�����������ʼ��ַ
//#define AppTempAddress                    0x8028000  //ת���ӻ��������ʱ�ռ���ʼ��ַ
//#define PAGE_SIZE                         (0x800)    /* 2 Kbyte */
//#define FLASH_SIZE                        (0x40000)  /* 256 KBytes */
//#define UserAppEnFlagAddress              0x800F800  //ApplicationAddress - PAGE_SIZE


#define     NET_PATA_FLASH_ADDR           0x080EA000
#define     SYS_LOCAL_IP_PARA_ADDR       (NET_PATA_FLASH_ADDR +8)    //4BYTE
#define     SYS_DEST_IP_PARA_ADDR        (NET_PATA_FLASH_ADDR +12)   //4
#define     SYS_MASK_PARA_ADDR           (NET_PATA_FLASH_ADDR +16)   //4
#define     SYS_NETWAY_PARA_ADDR         (NET_PATA_FLASH_ADDR +20)    //4
#define     SYS_MAC_PARA_ADDR            (NET_PATA_FLASH_ADDR +24)    //6
#define     SYS_LOCAL_PORT_PARA_ADDR     (NET_PATA_FLASH_ADDR +30)   //2
#define     SYS_DEST_PORT_PARA_ADDR      (NET_PATA_FLASH_ADDR +32)   //2
#define     SYS_ID_PARA_ADDR             (NET_PATA_FLASH_ADDR +34)   //1


/***********����������ã���ȡflash��******************************/

extern volatile u8  ParcelFeeder_ID_Para;  //20170621
extern uint32_t Mac_Code;



#pragma pack (1)

typedef struct
{  	
	u8 IP[8];  	// IP ���غ�Ŀ�ĵ�ַ
    u8 MASK[4];   //MASK ����
    u8 NETWAY[4];  //netway ����
    u8 MAC[6];    //mac��ַ
    u16 PORT[2];   //�˿� ���غ�Ŀ�Ķ˿�  
}NET_DATA_IN_FLASH;

extern NET_DATA_IN_FLASH	net_data_memory;


typedef struct
{  	
	u8 net_flag[7];  	// IP ���غ�Ŀ�ĵ�ַ    
}NET_FLAG_DATA_IN_FLASH;

#pragma pack ()

//extern NET_DATA_IN_FLASH	net_data_memory;
extern NET_FLAG_DATA_IN_FLASH  net_flag_data_memory;



void System_Init(void);
void RCC_Configuration(void);
void GPIO_Configuration(void);

void readFlash_contiue(uint32_t address,u8 *out_point, u32 len);
void read_sys_data_process(void);

//void System_Setup(void);
//void Ethernet_Configuration(void);
//void check_ETH_link(void);
//uint32_t FLASH_PagesMask(__IO uint32_t Size);

extern __IO uint32_t  EthInitStatus;
#ifdef __cplusplus
}
#endif

#endif /* __STM32F10F107_H */
