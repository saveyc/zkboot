#include "main.h"
#include "udpclient.h"

#define msgSendQueueSize  100
u16 msgQueueBuff[msgSendQueueSize];
MSG_SEND_QUEUE msgSendQueue;

void initQueue(MSG_SEND_QUEUE *q, u16 ms)
{
    q->maxSize = ms;
    q->queue = msgQueueBuff;
    q->front = q->rear = 0;
}
void enQueue(MSG_SEND_QUEUE *q, u16 x)
{
    //������
    if((q->rear + 1) % q->maxSize == q->front)
    {
        return;
    }
    q->rear = (q->rear + 1) % q->maxSize; // �����β����һ��λ��
    q->queue[q->rear] = x; // ��x��ֵ�����µĶ�β
}
u16 outQueue(MSG_SEND_QUEUE *q)
{
    //���п�
    if(q->front == q->rear)
    {
        return 0;
    }
    q->front = (q->front + 1) % q->maxSize; // ʹ����ָ��ָ����һ��λ��
    return q->queue[q->front]; // ���ض���Ԫ��
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

void recv_message_from_sever(u8 *point, u16 *len)
{
    *len = 0;
}

void send_message_to_sever(void)
{
    
}
