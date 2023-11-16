#include "main.h"
#include "TCPclient.h"

/* pcfunmsg*/
#define pcfunmsgQueueSize  50
u16 pcfunmsgQueueBuff[pcfunmsgQueueSize];
MSG_SEND_QUEUE pcfunmsgQueue;

u8 heart_cnt = 5;




void vpcfun_init_net_Queue(void)
{
    MSG_SEND_QUEUE* q = NULL;
    q = &pcfunmsgQueue;
    q->queue = pcfunmsgQueueBuff;
    q->maxSize = pcfunmsgQueueSize;
    q->front = q->rear = 0;
}

void vpcfun_add_to_net_Queue(MSG_SEND_QUEUE* q, u16 x)
{
    //队列满
    if ((q->rear + 1) % q->maxSize == q->front)
    {
        return;
    }
    q->rear = (q->rear + 1) % q->maxSize;        // 求出队尾的下一个位置  
    q->queue[q->rear] = x;                       // 把x的值赋给新的队尾
}

u16  u16pcfun_get_msg_from_net_Queue(MSG_SEND_QUEUE* q)
{
    if (q->front == q->rear)
    {
        return 0;
    }
    q->front = (q->front + 1) % q->maxSize; // 使队首指针指向下一个位置
    return (u16)(q->queue[q->front]);       // 返回队首元素
}

void vpcfun_addto_sdsfunmsg(u16 value)
{
    vpcfun_add_to_net_Queue(&pcfunmsgQueue, value);
}

u16 u16pcfun_getmsg_from_sdsQueue(void)
{
    return u16pcfun_get_msg_from_net_Queue(&pcfunmsgQueue);
}


u8 u8pcfun_recv_msg_check(u8* point, u16 len)
{
    u16 i = 0;
    u8  sum = 0;

    if ((point[0] != 0xAA) || (point[1] != 0xAA))
        return 0;
    if ((point[6] | point[7] << 8) != len)
        return 0;
    sum = point[9];
//    for (i = 1; i < len - 9; i++)
//    {
//        sum ^= point[9 + i];
//    }
//    if (sum != point[8])
//        return 0;

    return 1;
}


void vpcfun_recv_msg_process(u8* point)
{
    MSG_HEAD_DATA* head = (MSG_HEAD_DATA*)point;



    if (head->MSG_TYPE == RECV_MSG_BOOT_CMD_TYPE)
    {
        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
        if (FLASH_ErasePage(UserAppEnFlagAddress) == FLASH_COMPLETE)
        {
            FLASH_ProgramHalfWord(UserAppEnFlagAddress, 0x0000);
        }

        FLASH_Lock();
        NVIC_SystemReset();
    }

}

void vpcfun_recv_message_from_sever(u8* point, u16* len)
{
    if (u8pcfun_recv_msg_check(point, *len) == 0)
    {
        *len = 0;
        return;
    }
    vpcfun_recv_msg_process(point);
    *len = 0;
}


void vpcfun_send_heart_msg(u8* buf, u16* len, u16 type)
{
    u8 sum = (type & 0xFF) ^ ((type >> 8) & 0xFF);

    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x01;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0x0B;
    buf[7] = 0x00;
    buf[8] = sum;
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;

    *len = 11;
}


void vpcfun_send_message_to_sever(void)
{
    u16 msg_type = 0;

    if (tcp_client_list[0].tcp_send_en == 0)      //tcp发送成功或没在发送
    {
        msg_type = u16pcfun_getmsg_from_sdsQueue();
    }
    else//tcp正在发送
    {
        return;
    }


    if (tcp_client_list[0].tcp_client_statue == CLIENT_CONNECT_OK)
    {
        if (heart_cnt == 0)
        {
            vpcfun_send_heart_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), PC_HEART_MSG_TYPE);
            tcp_client_list[0].tcp_send_en = 1;
            heart_cnt = 2;
        }
    }
}

