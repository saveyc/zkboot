#ifndef _SDSFUN_H
#define _SDSFUN_H

#include "main.h"

/********�����**********/
#define		MSG_NULL_TYPE			                       0x0000
/**********��������***********/
#define     PC_HEART_MSG_TYPE                              0x9999

/**********���ؽ�������*********/
// ����boot
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
    u16* queue; /* ָ��洢���е�����ռ� */
    u16 front, rear, len; /* ����ָ�루�±꣩����βָ�루�±꣩�����г��ȱ��� */
    u16 maxSize; /* queue���鳤�� */
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