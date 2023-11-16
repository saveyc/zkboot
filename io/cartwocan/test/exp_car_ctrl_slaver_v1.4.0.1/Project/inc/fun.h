#ifndef _SDSFUN_H
#define _SDSFUN_H

#include "main.h"

/********锟斤拷锟筋定锟斤拷**********/
#define		MSG_NULL_TYPE			                       0x0000
/**********锟斤拷锟斤拷锟斤拷锟斤拷***********/
#define     PC_HEART_MSG_TYPE                              0x9999

/**********锟斤拷锟截斤拷锟斤拷锟斤拷锟斤拷*********/
// 锟斤拷锟斤拷boot
#define     RECV_MSG_BOOT_CMD_TYPE                         0x1F01


#pragma pack (1) 

typedef struct {
    u8  MSG_TAG[2];
    u32 MSG_ID;
    u16 MSG_LENGTH;
    u8  MSG_CRC;
    u16 MSG_TYPE;
}MSG_HEAD_DATA;

typedef struct {
    u16* queue; /* 指向存储队列的数组空间 */
    u16 front, rear, len; /* 队首指针（下标），队尾指针（下标），队列长度变量 */
    u16 maxSize; /* queue数组长度 */
}MSG_SEND_QUEUE;

#pragma pack () 

extern u8 heart_cnt;


void vpcfun_init_net_Queue(void);
void vpcfun_addto_sdsfunmsg(u16 value);
void vpcfun_recv_message_from_sever(u8* point, u16* len);
void vpcfun_send_message_to_sever(void);
void vpcfun_upload_car_online_process(void);
void vpcfun_send_heart_cmd_process(void);

#endif