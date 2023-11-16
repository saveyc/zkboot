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
    u16 send_len = 0;
    char str[200];
    u8   temp[60];
    u32  valuetemp = 0;
    u8   dip = 0;
    u8   can = 0;
    
    frame.extId.dst_id = rxMsg.ExtId >> 22 & 0x7F;
    frame.extId.mac_id = rxMsg.ExtId & 0x7F;
    frame.extId.func_id = (rxMsg.ExtId >> 7) & 0x1F;
    frame.extId.seg_num = (rxMsg.ExtId >> 12) & 0xFF;
    frame.extId.seg_polo = (rxMsg.ExtId >> 20) & 0x3;

    frame.data_len = rxMsg.DLC;

    for(i=0;i<frame.data_len;i++){
        frame.data[i] = rxMsg.Data[i];
    }
    valuetemp = rxMsg.Data[3] | (rxMsg.Data[4] << 8) | (rxMsg.Data[5] << 16);
    for (i = 0; i < 24; i++) {
        temp[i] = (valuetemp >> i) & 0x01;
    }
    valuetemp = rxMsg.Data[0] | (rxMsg.Data[1] << 8) | (rxMsg.Data[2] << 16);
    for (i = 0; i < 24; i++) {
        temp[i + 24 ] = (valuetemp >> i) & 0x01;
    }
    dip = rxMsg.Data[6];
    for (i = 0; i < 8; i++) {
        temp[i + 48] = (dip >> i) & 0x01;
    }

    can = rxMsg.Data[7];

    if (can == 1) {
        send_len = sprintf(str, "CAN1:NORMAL: \r\n");
        DEBUG_process((u8*)str, send_len);
        return;
    }


    send_len = sprintf(str, "CAN2:NORMAL: \r\n");
    DEBUG_process((u8*)str, send_len);

    send_len = sprintf(str, "IN 1:%d IN 2:%d IN 3:%d IN 4:%d IN 5:%d IN 6:%d IN 7:%d IN 8:%d IN 9:%d IN10:%d IN11:%d IN12:%d   \
                             IN13:%d IN14:%d IN15:%d IN16:%d IN17:%d IN18:%d IN19:%d IN20:%d IN21:%d IN22:%d IN23:%d IN24:%d \n", temp[0],
                             temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7], temp[8], temp[9], temp[10], temp[11], temp[12], temp[13], temp[14], temp[15], 
                             temp[16], temp[17], temp[18], temp[19], temp[20], temp[21], temp[22], temp[23] );

    DEBUG_process((u8*)str, send_len);


    send_len = sprintf(str, "IN25:%d IN26:%d IN27:%d IN28:%d IN29:%d IN30:%d IN31:%d IN32:%d IN33:%d IN34:%d IN35:%d IN36:%d   \
                             IN37:%d IN38:%d IN39:%d IN40:%d IN41:%d IN42:%d IN43:%d IN44:%d IN45:%d IN46:%d IN47:%d IN48:%d \n", temp[24],
        temp[25], temp[26], temp[27], temp[28], temp[29], temp[30], temp[31], temp[32], temp[33], temp[34], temp[35], temp[36], temp[37], temp[38], temp[39],
        temp[40], temp[41], temp[42], temp[43], temp[44], temp[45], temp[46], temp[47] );

    DEBUG_process((u8*)str, send_len);

    send_len = sprintf(str, "DIP1:%d DIP2:%d DIP3:%d DIP4:%d DIP5:%d DIP6:%d DIP7:%d DIP8:%d \n", temp[48],
        temp[49], temp[50], temp[51], temp[52], temp[53], temp[54], temp[55]);
    DEBUG_process((u8*)str, send_len);

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
