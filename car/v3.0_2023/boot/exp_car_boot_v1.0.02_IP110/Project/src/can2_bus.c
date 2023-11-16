#include "main.h"

//CAN2  与上游通讯

#define  canframebuffsize_two   70
sCanFrameExt    canframequeuebuff_two[canframebuffsize_two];
sCanFrameQueue   canframequeue_two;


void vcan_sendmsg_two(u8* buff, u8 send_total_len, u8 type, u8 dst);
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

    TxMessage.ExtId = (sTxMsg.extId.mac_id & 0x7F) | ((sTxMsg.extId.func_id & 0x1F) << 7) | ((sTxMsg.extId.seg_num & 0xFF) << 12) | ((sTxMsg.extId.seg_polo & 0x3) << 20) | ((sTxMsg.extId.dst_id & 0x7F) << 22);
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = sTxMsg.data_len;
    memcpy(TxMessage.Data, sTxMsg.data, TxMessage.DLC);
    
    return CAN_Transmit(CAN2, &TxMessage);

//    transmit_mailbox = CAN_Transmit(CAN2, &TxMessage);
//    
//    while(CAN_TransmitStatus(CAN2,transmit_mailbox)!= CANTXOK)
//    {
//        retry++;
//        if(retry == 0xFFFF)
//        {
//            retry++;
//            break;
//        }
//    }
//    
//     return CANTXOK;
}


void vcanbus_framereceive_two(CanRxMsg rxMsg)
{
    u16 i = 0;
    sCanFrameExt   frame;
    frame.extId.dst_id = rxMsg.ExtId >> 22 & 0x7F;
    frame.extId.mac_id = rxMsg.ExtId & 0x7F;
    frame.extId.func_id = (rxMsg.ExtId >> 7) & 0x1F;
    frame.extId.seg_num = (rxMsg.ExtId >> 12) & 0xFF;
    frame.extId.seg_polo = (rxMsg.ExtId >> 20) & 0x3;

    frame.data_len = rxMsg.DLC;

    for (i = 0; i < frame.data_len; i++) {
        frame.data[i] = rxMsg.Data[i];
    }

    vcanbus_addto_cansendqueue_one(frame);

    if (frame.extId.dst_id != Local_station) return;
    if (frame.extId.func_id != CAN_FUNC_ID_BOOT_MODE) return;

    if (frame.extId.seg_polo == CAN_SEG_POLO_NONE)
    {
        memcpy(can_recv_buff, rxMsg.Data, rxMsg.DLC);
        can_recv_len = rxMsg.DLC;
        slave_recv_process();
    }
    else if (frame.extId.seg_polo == CAN_SEG_POLO_FIRST)
    {
        memcpy(can_recv_buff, rxMsg.Data, rxMsg.DLC);

        g_SegPolo = CAN_SEG_POLO_FIRST;
        g_SegNum = frame.extId.seg_num;
        g_SegBytes = rxMsg.DLC;
    }
    else if (frame.extId.seg_polo == CAN_SEG_POLO_MIDDLE)
    {
        if ((g_SegPolo == CAN_SEG_POLO_FIRST)
            && (frame.extId.seg_num == (g_SegNum + 1))
            && ((g_SegBytes + rxMsg.DLC) <= CAN_RX_BUFF_SIZE))
        {
            memcpy(can_recv_buff + g_SegBytes, rxMsg.Data, rxMsg.DLC);

            g_SegNum++;
            g_SegBytes += rxMsg.DLC;
        }
    }
    else if (frame.extId.seg_polo == CAN_SEG_POLO_FINAL)
    {
        if ((g_SegPolo == CAN_SEG_POLO_FIRST)
            && (frame.extId.seg_num == (g_SegNum + 1))
            && ((g_SegBytes + rxMsg.DLC) <= CAN_RX_BUFF_SIZE))
        {
            memcpy(can_recv_buff + g_SegBytes, rxMsg.Data, rxMsg.DLC);
            can_recv_len = g_SegBytes + rxMsg.DLC;
            slave_recv_process();

            g_SegPolo = CAN_SEG_POLO_NONE;
            g_SegNum = 0;
            g_SegBytes = 0;
        }
    }

}


void vcan_sendmsg_two(u8* buff, u8 send_total_len, u8 type, u8 dst)
{
    sCanFrameExt canTxMsg;
    u8 send_len = 0;

    canTxMsg.extId.func_id = type;
    canTxMsg.extId.mac_id = Local_station;
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

///////   以下处理从机接收   ///////
u8 slave_recv_frame_check(void)
{
    if (can_recv_len < 5)
        return 0;
    if (can_recv_buff[0] != 0xAA || can_recv_buff[1] != 0xAA)
        return 0;
    if (can_recv_buff[2] != Local_station)
        return 0;
    if (can_recv_buff[can_recv_len - 1] != sum_check(can_recv_buff, can_recv_len - 1))
        return 0;
    return 1;
}

void slave_recv_process(void)
{
    u8 recv_cmd;

    if (slave_recv_frame_check() != 1)
    {
        return;
    }
    recv_cmd = can_recv_buff[3];

    if (recv_cmd == DLOAD_CMD_ENTER_BOOT)
    {
        slave_reply_cmd = DLOAD_CMD_ENTER_BOOT;
        slave_recv_pk_num = 0;
    }
    else if (recv_cmd == DLOAD_CMD_ERASE_FLASH)
    {
        slave_reply_cmd = DLOAD_CMD_ERASE_FLASH;
        slave_recv_pk_num = 0;
    }
    else if (recv_cmd == DLOAD_CMD_TRAN_DATA)
    {
        pk_tot_num = can_recv_buff[4];
        pk_cur_num = can_recv_buff[5];
        pk_len = can_recv_buff[6] | (can_recv_buff[7] << 8);
        if (pk_len > CAN_RX_DATA_PK_SIZE)
            return;
        if (pk_cur_num > pk_tot_num)
            return;
        if ((slave_recv_pk_num + 1) != pk_cur_num)
        {
            if (slave_recv_pk_num == pk_cur_num)
            {
                slave_reply_cmd = DLOAD_CMD_TRAN_DATA;
            }
        }
        else
        {
            slave_reply_cmd = DLOAD_CMD_TRAN_DATA;
        }
    }
    else
    {
        slave_reply_cmd = DLOAD_CMD_NULL;
    }
}



void slave_reply_boot_cmd()
{
    sCanFrameExt canTxMsg;

    canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
    canTxMsg.extId.seg_num = 0;
    canTxMsg.extId.func_id = CAN_FUNC_ID_BOOT_MODE;
    canTxMsg.extId.mac_id = Local_station;
    canTxMsg.extId.dst_id = 1;
    canTxMsg.data_len = 5;
    canTxMsg.data[0] = 0x55;
    canTxMsg.data[1] = 0x55;
    canTxMsg.data[2] = Local_station;
    canTxMsg.data[3] = DLOAD_CMD_ENTER_BOOT;
    canTxMsg.data[4] = sum_check(canTxMsg.data, 4);

    vcanbus_addto_cansendqueue_two(canTxMsg);

}
void slave_reply_erase_flash_cmd()
{
    sCanFrameExt canTxMsg;

    canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
    canTxMsg.extId.seg_num = 0;
    canTxMsg.extId.func_id = CAN_FUNC_ID_BOOT_MODE;
    canTxMsg.extId.mac_id = Local_station;
    canTxMsg.extId.dst_id = 1;
    canTxMsg.data_len = 5;
    canTxMsg.data[0] = 0x55;
    canTxMsg.data[1] = 0x55;
    canTxMsg.data[2] = Local_station;
    canTxMsg.data[3] = DLOAD_CMD_ERASE_FLASH;
    canTxMsg.data[4] = sum_check(canTxMsg.data, 4);

    vcanbus_addto_cansendqueue_two(canTxMsg);

}
void slave_reply_tran_data_cmd()
{
    sCanFrameExt canTxMsg;

    canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
    canTxMsg.extId.seg_num = 0;
    canTxMsg.extId.func_id = CAN_FUNC_ID_BOOT_MODE;
    canTxMsg.extId.mac_id = Local_station;
    canTxMsg.extId.dst_id = 1;
    canTxMsg.data_len = 6;
    canTxMsg.data[0] = 0x55;
    canTxMsg.data[1] = 0x55;
    canTxMsg.data[2] = Local_station;
    canTxMsg.data[3] = DLOAD_CMD_TRAN_DATA;
    canTxMsg.data[4] = pk_cur_num;
    canTxMsg.data[5] = sum_check(canTxMsg.data, 5);

    vcanbus_addto_cansendqueue_two(canTxMsg);
}
void erase_user_app_flash()
{
    FLASH_Status f_status = FLASH_COMPLETE;
    u32 pageNum;
    u32 eraseCnt;

    pageNum = FLASH_PagesMask(0x18000);

    FLASH_Unlock();
    f_status = FLASH_ErasePage(UserAppEnFlagAddress);
    for (eraseCnt = 0; (eraseCnt < pageNum) && (f_status == FLASH_COMPLETE); eraseCnt++)
    {
        f_status = FLASH_ErasePage(ApplicationAddress + (PAGE_SIZE * eraseCnt));
    }
    FLASH_Lock();
}
void program_user_app_flash()
{
    u32* source;
    u32  flashDst;
    u16  n;

    if (slave_recv_pk_num == pk_cur_num)
        return;
    else
        slave_recv_pk_num = pk_cur_num;

    source = (u32*)(can_recv_buff + 8);
    flashDst = ApplicationAddress + (pk_cur_num - 1) * CAN_RX_DATA_PK_SIZE;

    for (n = 0; n < pk_len; n += 4)
    {
        FLASH_Unlock();
        FLASH_ProgramWord(flashDst, *source);
        FLASH_Lock();
        if (*(u32*)flashDst != *source)
        {
            return;
        }
        flashDst += 4;
        source += 1;
    }
    if (pk_cur_num == pk_tot_num)
    {
        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
        if (FLASH_ErasePage(UserAppEnFlagAddress) == FLASH_COMPLETE)
        {
            FLASH_ProgramHalfWord(UserAppEnFlagAddress, 0x00AA);
        }
        FLASH_Lock();
        SlaveApp_UpdateFlag = 1;
    }
}

void can_bus_reply_process(void)
{
  u8 buff[10] = {0};
    while (1)
    {
        if (slave_reply_cmd == DLOAD_CMD_ENTER_BOOT)
        {
            slave_reply_boot_cmd();
            slave_reply_cmd = DLOAD_CMD_NULL;
        }
        else if (slave_reply_cmd == DLOAD_CMD_ERASE_FLASH)
        {
            erase_user_app_flash();
            slave_reply_erase_flash_cmd();
            slave_reply_cmd = DLOAD_CMD_NULL;
        }
        else if (slave_reply_cmd == DLOAD_CMD_TRAN_DATA)
        {
            program_user_app_flash();
            slave_reply_tran_data_cmd();
            slave_reply_cmd = DLOAD_CMD_NULL;

            if (SlaveApp_UpdateFlag == 1)
            {
                SlaveApp_UpdateFlag = 0;
//                Delay(2000);  //2s
//                NVIC_SystemReset();
            }
        }

        vcan_sendframe_process_two();
        vcan_sendframe_process_one();

    }
}
