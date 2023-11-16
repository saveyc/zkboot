#include "stm32f10x.h"
#include "stm32f107.h"
#include "main.h"


//CAN1  和下游通讯


#define  canframebuffsize_one   100
sCanFrameExt    canframequeuebuff_one[canframebuffsize_one];
sCanFrameQueue   canframequeue_one;

void vcan_sendmsg_one(u8* buff, u8 send_total_len, u8 type, u8 dst);




void vcanbus_initcansendqueue_one(void)
{
    sCanFrameQueue* q = NULL;
    q = &canframequeue_one;

    q->queue = canframequeuebuff_one;
    q->front = q->rear = 0;
    q->maxsize = canframebuffsize_one;
}

void vcanbus_addto_cansendqueue_one(sCanFrameExt x)
{
    sCanFrameQueue* q = NULL;
    q = &canframequeue_one;

    if ((q->rear + 1) % q->maxsize == q->front)
    {
        return;
    }

    q->rear = (q->rear + 1) % q->maxsize;

    q->queue[q->rear] = x;
}


u8 u8canbus_send_oneframe_one(sCanFrameExt sTxMsg)
{
    CanTxMsg TxMessage;

    TxMessage.ExtId = (sTxMsg.extId.mac_id & 0x7F) | ((sTxMsg.extId.func_id & 0x1F) << 7) | ((sTxMsg.extId.seg_num & 0xFF) << 12) | ((sTxMsg.extId.seg_polo & 0x3) << 20) | ((sTxMsg.extId.dst_id & 0x7F) << 22);
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = sTxMsg.data_len;
    memcpy(TxMessage.Data, sTxMsg.data, TxMessage.DLC);

    return CAN_Transmit(CAN1, &TxMessage);
}


void vcanbus_framereceive_one(CanRxMsg rxMsg)
{
    u16 i =0;
    sCanFrameExt   frame;
    u8 send_len = 0;
    char str[25];
    
    frame.extId.dst_id = rxMsg.ExtId >> 22 & 0x7F;
    frame.extId.mac_id = rxMsg.ExtId & 0x7F;
    frame.extId.func_id = (rxMsg.ExtId >> 7) & 0x1F;
    frame.extId.seg_num = (rxMsg.ExtId >> 12) & 0xFF;
    frame.extId.seg_polo = (rxMsg.ExtId >> 20) & 0x3;

    frame.data_len = rxMsg.DLC;

    for(i=0;i<frame.data_len;i++){
        frame.data[i] = rxMsg.Data[i];
    }

    if (frame.extId.func_id == 2) {

        if (key_value == 1) {
            vcan1_reply_check_slaver();
        }
        else {
            send_len = sprintf(str, "CAN1:NORMAL:%d\n", rxMsg.Data[0]);
            DEBUG_process((u8*)str, send_len);
        }
    }
}


void vcan_sendmsg_one(u8* buff, u8 send_total_len, u8 type, u8 dst)
{
    sCanFrameExt canTxMsg;
    u8 send_len = 0;

    canTxMsg.extId.func_id = type;
    canTxMsg.extId.mac_id = key_value;
    canTxMsg.extId.dst_id = dst;

    if (send_total_len <= CAN_PACK_DATA_LEN)   //不需分段传输
    {
        canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
        canTxMsg.extId.seg_num = 0;
        canTxMsg.data_len = send_total_len;
        memcpy(canTxMsg.data, buff, canTxMsg.data_len);
        vcanbus_addto_cansendqueue_one(canTxMsg);
    }  
    else    //需要分段传输
    {                        
        canTxMsg.extId.seg_polo = CAN_SEG_POLO_FIRST;
        canTxMsg.extId.seg_num = 0;
        canTxMsg.data_len = CAN_PACK_DATA_LEN;
        memcpy(canTxMsg.data, buff, canTxMsg.data_len);
        vcanbus_addto_cansendqueue_one(canTxMsg);
        send_len += CAN_PACK_DATA_LEN;
        while (1) {
            if (send_len + CAN_PACK_DATA_LEN < send_total_len)
            {
                canTxMsg.extId.seg_polo = CAN_SEG_POLO_MIDDLE;
                canTxMsg.extId.seg_num++;
                canTxMsg.data_len = CAN_PACK_DATA_LEN;
                memcpy(canTxMsg.data, buff + send_len, canTxMsg.data_len);
                vcanbus_addto_cansendqueue_one(canTxMsg);
                send_len += CAN_PACK_DATA_LEN;
            }
            else
            {
                canTxMsg.extId.seg_polo = CAN_SEG_POLO_FINAL;
                canTxMsg.extId.seg_num++;
                canTxMsg.data_len = send_total_len - send_len;
                memcpy(canTxMsg.data, buff + send_len, canTxMsg.data_len);
                vcanbus_addto_cansendqueue_one(canTxMsg);
                break;
            }
        }
    }
}

void vcan_sendframe_process_one(void)
{
    sCanFrameQueue* q = &canframequeue_one;
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
    tx_mailbox = u8canbus_send_oneframe_one(*canTxMsg);
    if (tx_mailbox != CAN_NO_MB)
    {
        q->front = front;
    }
}

void vcan1_send_check_slaver(void)
{
    u8  buff[10] = { 0 };
    u8 sendlen = 0;

    buff[0] = 1;
    buff[1] = 1;
    buff[2] = 1;
    buff[3] = 1;
    buff[4] = 1;
    buff[5] = 1;
    buff[6] = 1;
    buff[7] = 1;

    sendlen = 8;

    vcan_sendmsg_one(buff, sendlen, 2, 1);
}


void vcan1_reply_check_slaver(void)
{
    u8  buff[10] = { 0 };
    u8 sendlen = 0;

    buff[0] = 1;
    buff[1] = 1;
    buff[2] = 1;
    buff[3] = 1;
    buff[4] = 1;
    buff[5] = 1;
    buff[6] = 1;
    buff[7] = 1;

    sendlen = 8;

    vcan_sendmsg_one(buff, sendlen, 2, 1);
}

void udp_send_can_bus_test(void)
{
    u8 send_len = 0;
    char str[25];
    
    send_len = sprintf(str, "CAN1:NORMAL:%d",123
       );
    DEBUG_process((u8*)str, send_len);
}
