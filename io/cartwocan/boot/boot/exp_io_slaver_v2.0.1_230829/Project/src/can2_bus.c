#include "main.h"

//CAN1  与下游通讯

#define  canframebuffsize_two   70
sCanFrameExt    canframequeuebuff_two[canframebuffsize_two];
sCanFrameQueue   canframequeue_two;


void vcan_sendmsg_two(u8* buff, u16 send_total_len, u8 type, u8 dst);
u8 slave_recv_frame_check(void);
void slave_recv_process(void);




void vcanbus_initcansendqueue_two(void)
{
    sCanFrameQueue* q = NULL;
    q = &canframequeue_two;

    q->queue = canframequeuebuff_two;
    q->front = q->rear = 0;
    q->maxsize = canframebuffsize_two;
}

void vcanbus_addto_cansendqueue_two(sCanFrameExt x)
{
    sCanFrameQueue* q = NULL;
    q = &canframequeue_two;

    if ((q->rear + 1) % q->maxsize == q->front)
    {
        return;
    }

    q->rear = (q->rear + 1) % q->maxsize;

    q->queue[q->rear] = x;
}


u8 u8canbus_send_twoframe_two(sCanFrameExt sTxMsg)
{
    u16 retry = 0;
    CanTxMsg TxMessage;
    uint8_t transmit_mailbox = 0;

    TxMessage.ExtId = (sTxMsg.extId.src_id & 0x7F) | ((sTxMsg.extId.func_id & 0x1F) << 7) | ((sTxMsg.extId.seg_num & 0xFF) << 12) | ((sTxMsg.extId.seg_polo & 0x3) << 20) | ((sTxMsg.extId.dst_id & 0x7F) << 22);
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = sTxMsg.data_len;
    memcpy(TxMessage.Data, sTxMsg.data, TxMessage.DLC);

    return CAN_Transmit(CAN1, &TxMessage);
}


void vcanbus_framereceive_two(CanRxMsg rxMsg)
{
    u16 i = 0;
    sCanFrameExt   frame;
    frame.extId.dst_id = rxMsg.ExtId >> 22 & 0x7F;
    frame.extId.src_id = rxMsg.ExtId & 0x7F;
    frame.extId.func_id = (rxMsg.ExtId >> 7) & 0x1F;
    frame.extId.seg_num = (rxMsg.ExtId >> 12) & 0xFF;
    frame.extId.seg_polo = (rxMsg.ExtId >> 20) & 0x3;

    frame.data_len = rxMsg.DLC;

    for (i = 0; i < frame.data_len; i++) {
        frame.data[i] = rxMsg.Data[i];
    }

    vcanbus_addto_cansendqueue_one(frame);

}


void vcan_sendmsg_two(u8* buff, u16 send_total_len, u8 type, u8 dst)
{
    sCanFrameExt canTxMsg;
    u16 send_len = 0;

    canTxMsg.extId.func_id = type;
    canTxMsg.extId.src_id = Local_Station;
    canTxMsg.extId.dst_id = dst;

    if (send_total_len <= CAN_PACK_DATA_LEN)
    {
        canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
        canTxMsg.extId.seg_num = 0;
        canTxMsg.data_len = send_total_len;
        memcpy(canTxMsg.data, buff, canTxMsg.data_len);
        vcanbus_addto_cansendqueue_two(canTxMsg);
    }
    else {
        canTxMsg.extId.seg_polo = CAN_SEG_POLO_FIRST;
        canTxMsg.extId.seg_num = 0;
        canTxMsg.data_len = CAN_PACK_DATA_LEN;
        memcpy(canTxMsg.data, buff, canTxMsg.data_len);
        vcanbus_addto_cansendqueue_two(canTxMsg);
        send_len += CAN_PACK_DATA_LEN;
        while (1) {
            if (send_len + CAN_PACK_DATA_LEN < send_total_len)
            {
                canTxMsg.extId.seg_polo = CAN_SEG_POLO_MIDDLE;
                canTxMsg.extId.seg_num++;
                canTxMsg.data_len = CAN_PACK_DATA_LEN;
                memcpy(canTxMsg.data, buff + send_len, canTxMsg.data_len);
                vcanbus_addto_cansendqueue_two(canTxMsg);
                send_len += CAN_PACK_DATA_LEN;
            }
            else
            {
                canTxMsg.extId.seg_polo = CAN_SEG_POLO_FINAL;
                canTxMsg.extId.seg_num++;
                canTxMsg.data_len = send_total_len - send_len;
                memcpy(canTxMsg.data, buff + send_len, canTxMsg.data_len);
                vcanbus_addto_cansendqueue_two(canTxMsg);
                break;
            }
        }
    }
}

void vcan_sendframe_process_two(void)
{
    sCanFrameQueue* q = &canframequeue_two;
    sCanFrameExt* canTxMsg = NULL;
    u16 front = 0;
    u8  tx_mailbox = 0;

    if (q->front == q->rear)
    {
        return;
    }

    front = q->front;
    front = (front + 1) % q->maxsize;
    canTxMsg = (sCanFrameExt*)(&(q->queue[front]));
    tx_mailbox = u8canbus_send_twoframe_two(*canTxMsg);
    if (tx_mailbox != CAN_NO_MB)
    {
        q->front = front;
    }
}


