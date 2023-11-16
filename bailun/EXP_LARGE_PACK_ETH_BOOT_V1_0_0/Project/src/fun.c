#include "main.h"
#include "udpclient.h"

#define msgSendQueueSize  100
u16 msgQueueBuff[msgSendQueueSize];
MSG_SEND_QUEUE msgSendQueue;
DLOAD_PARA_DATA sDLoad_Para_Data;
DLOAD_STATUS g_download_status;
u32 file_tot_bytes;
u8  print_msg_buff[200];
u16 print_msg_len;
u8  user_para_send_buff[100];
u16 user_para_send_len;

void initQueue(MSG_SEND_QUEUE *q, u16 ms)
{
    q->maxSize = ms;
    q->queue = msgQueueBuff;
    q->front = q->rear = 0;
}
void enQueue(MSG_SEND_QUEUE *q, u16 x)
{
    //队列满
    if((q->rear + 1) % q->maxSize == q->front)
    {
        return;
    }
    q->rear = (q->rear + 1) % q->maxSize; // 求出队尾的下一个位置
    q->queue[q->rear] = x; // 把x的值赋给新的队尾
}
u16 outQueue(MSG_SEND_QUEUE *q)
{
    //队列空
    if(q->front == q->rear)
    {
        return 0;
    }
    q->front = (q->front + 1) % q->maxSize; // 使队首指针指向下一个位置
    return q->queue[q->front]; // 返回队首元素
}
void InitSendMsgQueue(void)
{
    initQueue(&msgSendQueue, msgSendQueueSize);
}
void AddSendMsgToQueue(u16 msg)
{
    enQueue(&msgSendQueue, msg);
}
u16 GetSendMsgFromQueue(void)
{
    return (outQueue(&msgSendQueue));
}

u8 recv_msg_check(u8 *point, u16 len)
{
    //u16 i = 0;
    //u8  sum = 0;

    if((point[0] != 0xAA) || (point[1] != 0xAA))
        return 0;
    if((point[6] | point[7] << 8) != len)
    {
        return 0;
    }
//    sum = point[9];
//    for(i = 1 ; i < len - 9 ; i++)
//    {
//        sum ^= point[9 + i];
//    }
//    if(sum != point[8])
//        return 0;

    return 1;
}
void read_user_para_handle(void)
{
    u16 i;
    u16 data;
    u16 flag;
    
    flag = *((u16*)UserParaStartAddress);
    
    if(flag == 0xAA)
    {
        for(i=0; i<20; i++)
        {
            data = *((u16*)(UserParaStartAddress+2+2*i));
            user_para_send_buff[2*i+0] = data&0xFF;
            user_para_send_buff[2*i+1] = (data>>8)&0xFF;
        }
    }
    else
    {
        for(i=0; i<20; i++)
        {
            user_para_send_buff[2*i+0] = 0;
            user_para_send_buff[2*i+1] = 0;
        }
    }
    user_para_send_len = 40;
}
void write_user_para_handle(USER_PARA_DATA* para)
{
    u8 i;
    
    FLASH_Unlock();
    FLASH_DataCacheCmd(DISABLE);
    FLASH_EraseSector(FLASH_Sector_6,VoltageRange_3);
    FLASH_DataCacheCmd(ENABLE);
    FLASH_Lock();
    FLASH_Unlock();
    FLASH_ProgramHalfWord(UserParaStartAddress,0xAA);//有效性标记
    for(i=0; i<20; i++)
    {
        FLASH_ProgramHalfWord(UserParaStartAddress+2+2*i,para->user_para[i]);
    }
    FLASH_Lock();
}
void recv_msg_process(u8 *point)
{
    MSG_HEAD_DATA *head = (MSG_HEAD_DATA *)point;

    switch(head->MSG_TYPE)
    {
    case RECV_MSG_BOOT_MODE_TYPE:
        g_download_status = DLOAD_INIT;
        AddSendMsgToQueue(REPLY_RECV_MSG_BOOT_MODE_TYPE);
        break;
    case RECV_MSG_DLOAD_PARA_TYPE:
        sDLoad_Para_Data = *((DLOAD_PARA_DATA*)point);
        g_download_status = DLOAD_PARA;
        file_tot_bytes = 0;
        AddSendMsgToQueue(REPLY_RECV_MSG_DLOAD_PARA_TYPE);
        break;
    case RECV_MSG_READ_PARA_TYPE:
        read_user_para_handle();
        AddSendMsgToQueue(REPLY_RECV_MSG_READ_PARA_TYPE);
        break;
    case RECV_MSG_WRITE_PARA_TYPE:
        write_user_para_handle((USER_PARA_DATA*)point);
        AddSendMsgToQueue(REPLY_RECV_MSG_WRITE_PARA_TYPE);
        break;
    default:
        break;
    }
}
void recv_message_from_sever(u8 *point, u16 *len)
{
    if(recv_msg_check(point, *len) == 0)
    {
        *len = 0;
        return;
    }
    recv_msg_process(point);
    *len = 0;
}
void reply_recv_msg(u8 *buf, u16 *len, u16 type)
{
    u8 sum = (type & 0xFF) ^ (type >> 8);

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
void send_user_para_msg(u8 *buf, u16 *len, u16 type)
{
    u8 sum;
    u16 sendlen;
    u16 i;
    
    sendlen = user_para_send_len + 11;
    
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;
    for(i = 0; i < user_para_send_len ; i++)
    {
        buf[11 + i] = user_para_send_buff[i];
    }
    sum = buf[9];
    for(i = 1 ; i < sendlen - 9 ; i++)
    {
        sum ^= buf[9 + i];
    }
    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x01;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = sendlen & 0xFF;
    buf[7] = (sendlen >> 8) & 0xFF;
    buf[8] = sum;

    *len = sendlen;
}
void send_message_to_sever(void)
{
    u16 msg_type;

    if(tcp_client_list[0].tcp_send_en == 0)//tcp发送成功或没在发送
    {
        msg_type = GetSendMsgFromQueue();
    }
    else//tcp正在发送
    {
        return;
    }

    switch(msg_type)
    {
    case REPLY_RECV_MSG_BOOT_MODE_TYPE:
    case REPLY_RECV_MSG_DLOAD_PARA_TYPE:
    case REPLY_RECV_MSG_WRITE_PARA_TYPE:
        reply_recv_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    case REPLY_RECV_MSG_READ_PARA_TYPE:
        send_user_para_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    default:
        break;
    }
}
