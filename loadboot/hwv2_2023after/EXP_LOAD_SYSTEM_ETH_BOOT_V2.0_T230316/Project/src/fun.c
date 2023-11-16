#include "main.h"
#include "udpclient.h"

#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/opt.h"


#define msgSendQueueSize  100
u16 msgQueueBuff[msgSendQueueSize];
MSG_SEND_QUEUE msgSendQueue;

DLOAD_STATUS g_download_status;

void initQueue(MSG_SEND_QUEUE *q, u16 ms)
{
    q->maxSize = ms;
    q->queue = msgQueueBuff;
    q->front = q->rear = 0;
}
void enQueue(MSG_SEND_QUEUE *q, u16 x)
{
    //队列满
    if ((q->rear + 1) % q->maxSize == q->front)
    {
        return;
    }
    q->rear = (q->rear + 1) % q->maxSize; // 求出队尾的下一个位置
    q->queue[q->rear] = x; // 把x的值赋给新的队尾
}
u16 outQueue(MSG_SEND_QUEUE *q)
{
    //队列空
    if (q->front == q->rear)
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
    u16 i = 0;
    u8  sum = 0;

    if ((point[0] != 0xAA) || (point[1] != 0xAA))
        return 0;
    if ((point[6] | point[7] << 8) != len)
        return 0;
    sum = point[9];
    for (i = 1; i < len - 9; i++)
    {
        sum ^= point[9 + i];
    }
    if (sum != point[8])
        return 0;

    return 1;
}
void recv_msg_process(u8 *point)
{
    MSG_HEAD_DATA *head = (MSG_HEAD_DATA *)point;

    switch (head->MSG_TYPE)
    {
    case RECV_MSG_BOOT_MODE_TYPE:
        g_download_status = DLOAD_INIT;
        AddSendMsgToQueue(REPLY_RECV_MSG_BOOT_MODE_TYPE);
        break;
    case RECV_MSG_DLOAD_PARA_TYPE:
        //sDLoad_Para_Data = *((DLOAD_PARA_DATA*)point);
        g_download_status = DLOAD_PARA;
        AddSendMsgToQueue(REPLY_RECV_MSG_DLOAD_PARA_TYPE);
        break;

    case RECV_VERSIONS_PARA_TYPE:           //版本号、ID查询
        AddSendMsgToQueue(REPLY_RECV_VERSIONS_PARA_TYPE);
        break;

    default:
        break;
    }
}
void recv_message_from_sever(u8 *point, u16 *len)
{
    if (recv_msg_check(point, *len) == 0)
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

void reply_recv_versions_msg(u8 *buf, u16 *len, u16 type) //发送版本号
{
    u8 i;
    u8 sum = 0;
    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x02;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0x12;
    buf[7] = 0x00;
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;
    buf[11] = 11;   //版本号为11223344，说明进入了boot模式
    buf[12] = 22;
    buf[13] = 33;
    buf[14] = 44;
    buf[15] = 0x00;
    buf[16] = 0x00;
    buf[17] = ParcelFeeder_ID_Para;
    *len = 18;
    sum = buf[9];
    for (i = 1; i < (*len) - 9; i++)
    {
        sum ^= buf[9 + i];
    }
    buf[8] = sum;
}
void send_message_to_sever(void)
{
    u16 msg_type;

    if (tcp_client_list[0].tcp_send_en == 0)//tcp发送成功或没在发送
    {
        msg_type = GetSendMsgFromQueue();
    }
    else//tcp正在发送
    {
        return;
    }
    switch (msg_type)
    {
    case REPLY_RECV_MSG_BOOT_MODE_TYPE:
    case REPLY_RECV_MSG_DLOAD_PARA_TYPE:
        reply_recv_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    case SEND_MSG_COMPLETE_UPGRADE_TYPE: //完成烧写后反馈
        reply_recv_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        Programm_Completed_Reply_flag = 1;//升级完成反馈应答标志
        break;
    case REPLY_RECV_VERSIONS_PARA_TYPE:  //版本号、ID查询---BOOT模式版本号默认为11223344，用以区分用户模式。ID为真实的。
        reply_recv_versions_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    default:
        break;
    }
}
//********接收广播消息*********//
#pragma pack (1)
typedef struct
{
    u8  recev_buf[9];
    u16 recev_type;
} udp_rev_table;
udp_rev_table  udp_rev;
#pragma pack ()
struct udp_pcb *g_pcb;
u8 udp_send_buf[30] = { 0 };
u16 udp_send_len = 0;
void reply_recv_broad_msg(u8 *buf, u16 *len, u16 type)//,u32 index)
{
    u8 i;
    u8 sum = 0;
    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x02;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0x16;
    buf[7] = 0x00;
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;
    buf[11] = static_ip[0];
    buf[12] = static_ip[1];
    buf[13] = static_ip[2];
    buf[14] = static_ip[3];
    buf[15] = local_port_data & 0xFF;
    buf[16] = (local_port_data >> 8) & 0xFF;
    buf[17] = 11;     //版本号为11223344，说明进入了boot模式
    buf[18] = 22;
    buf[19] = 33;
    buf[20] = 44;
    buf[21] = ParcelFeeder_ID_Para;
    *len = 22;
    sum = buf[9];
    for (i = 1; i < (*len) - 9; i++)
    {
        sum ^= buf[9 + i];
    }
    buf[8] = sum;
}
void udp_server_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                     struct ip_addr *addr, u16_t port)
{
    //u16_t nSendPort;
    struct ip_addr destAddr;// = *addr; /* 获取远程主机 IP地址*/
    udp_rev_table *udp_rev_head = (udp_rev_table *)p->payload;
    IP4_ADDR(&destAddr, 255, 255, 255, 255);
    if (recv_msg_check(p->payload, p->len) == 0)
    {
        p->len = 0;
        return;
    }

    if (udp_rev_head->recev_type == RECV_NET_PARA_TYPE)//0x2F01
    {
        struct pbuf *ps;
        reply_recv_broad_msg(&(udp_send_buf[0]), &(udp_send_len), REPLY_RECV_NET_PARA_TYPE);
        /* allocate pbuf from pool*/
        ps = pbuf_alloc(PBUF_TRANSPORT, udp_send_len, PBUF_POOL);
        if (ps != NULL)
        {
            /* copy data to pbuf */
            pbuf_take(ps, &(udp_send_buf[0]), udp_send_len);
            /* send udp data */
            udp_sendto(pcb, ps, &destAddr, DEST_PORT); /*发送数据出去 */
            pbuf_free(p);
            pbuf_free(ps);
            udp_send_len = 0;
        }
    }
}
void UDP_server_init(void)
{
    g_pcb = udp_new(); //申请udp控制块
    g_pcb->so_options = SOF_BROADCAST;
    udp_bind(g_pcb, IP_ADDR_ANY, LOCAL_UDP_PORT);
    udp_recv(g_pcb, udp_server_recv, NULL);  /* 设置UDP段到时的回调函数 */
}