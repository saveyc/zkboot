#include "main.h"

u8  can_recv_buff[CAN_RX_BUFF_SIZE];
u16 can_recv_len = 0;
u8  g_SegPolo = CAN_SEG_POLO_NONE;
u8  g_SegNum = 0;
u16 g_SegBytes = 0;

u16 pk_tot_num;
u16 pk_cur_num;
u16 pk_len;
u16 slave_reply_cmd = DLOAD_CMD_NULL;
u16 slave_recv_pk_num = 0;

u16 record[100] = {0};



//CAN1 与上游通讯


#define  canframebuffsize_one   50
sCanFrameExt    canframequeuebuff_one[canframebuffsize_one];
sCanFrameQueue   canframequeue_one;

//
void vcan_sendmsg_one(u8* buff, u16 send_total_len, u8 type, u8 dst);



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
    u16 retry = 0;
    CanTxMsg TxMessage;
    uint8_t transmit_mailbox = 0;

    TxMessage.ExtId = (sTxMsg.extId.src_id & 0x7F) | ((sTxMsg.extId.func_id & 0x1F) << 7) | ((sTxMsg.extId.seg_num & 0xFF) << 12) | ((sTxMsg.extId.seg_polo & 0x3) << 20) | ((sTxMsg.extId.dst_id & 0x7F) << 22);
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = sTxMsg.data_len;
    memcpy(TxMessage.Data, sTxMsg.data, TxMessage.DLC);


    transmit_mailbox = CAN_Transmit(CAN2, &TxMessage);

    while (CAN_TransmitStatus(CAN2, transmit_mailbox) != CANTXOK)
    {
        retry++;
        if (retry == 0xFFFF)
        {
            retry++;
            break;
        }
    }

    return CAN_TransmitStatus(CAN1, transmit_mailbox);
}



void vcan_sendmsg_one(u8* buff, u16 send_total_len, u8 type, u8 dst)
{
    sCanFrameExt canTxMsg;
    u16 send_len = 0;

    canTxMsg.extId.func_id = type;
    canTxMsg.extId.src_id = Local_Station;
    canTxMsg.extId.dst_id = dst;

    if (send_total_len <= CAN_PACK_DATA_LEN)// 不分组
    {
        canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
        canTxMsg.extId.seg_num = 0;
        canTxMsg.data_len = send_total_len;
        memcpy(canTxMsg.data, buff, canTxMsg.data_len);
        vcanbus_addto_cansendqueue_one(canTxMsg);
    }
    else {
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


// 


u8 sum_check(u8* pdata,u16 len)
{
    u16 i;
    u8  sum = 0;
    
    for(i=0 ; i<len ; i++)
    {
        sum += pdata[i];
    }
    return sum;
}
u8 slave_recv_frame_check(void)
{
    if(can_recv_len < 5)
        return 0;
    if(can_recv_buff[0]!=0xAA || can_recv_buff[1]!=0xAA)
        return 0;
    if(can_recv_buff[2]!=Local_Station)
        return 0;
    if(can_recv_buff[can_recv_len-1]!=sum_check(can_recv_buff,can_recv_len-1))
        return 0;
    return 1;
}
void slave_recv_process(void)
{
    u8 recv_cmd;
    u16 i =0;
    
    if(slave_recv_frame_check() !=1)
    {
        return;
    }
    recv_cmd = can_recv_buff[3];
    
    if(recv_cmd == DLOAD_CMD_ENTER_BOOT)
    {
        //g_download_status = DLOAD_TRAN_INIT;
        slave_reply_cmd = DLOAD_CMD_ENTER_BOOT;
        slave_recv_pk_num = 0;
    }
    else if(recv_cmd == DLOAD_CMD_ERASE_FLASH)
    {
        //g_download_status = DLOAD_TRAN_ERASE;
        slave_reply_cmd = DLOAD_CMD_ERASE_FLASH;
        slave_recv_pk_num = 0;
    }
    else if(recv_cmd == DLOAD_CMD_TRAN_DATA)// && (g_download_status == DLOAD_TRAN_ERASE||g_download_status == DLOAD_TRAN_PROG))
    {
        pk_tot_num = can_recv_buff[4];
        pk_cur_num = can_recv_buff[5];
        pk_len     = can_recv_buff[6]|(can_recv_buff[7]<<8);
        record[pk_cur_num] = pk_cur_num;
        if(pk_len > CAN_RX_DATA_PK_SIZE)
            return;
        if(pk_cur_num > pk_tot_num)
            return;
        if((slave_recv_pk_num+1) != pk_cur_num)
        {
            if(slave_recv_pk_num == pk_cur_num)
            {
                //g_download_status = DLOAD_TRAN_PROG;
                slave_reply_cmd = DLOAD_CMD_TRAN_DATA;
            }
        }
        else
        {
            //g_download_status = DLOAD_TRAN_PROG;
            slave_reply_cmd = DLOAD_CMD_TRAN_DATA;
        }
        if(pk_cur_num >=2){
          i=0;
          return;
        }
    }
    else
    {
        //g_download_status = DLOAD_INIT;
        slave_reply_cmd = DLOAD_CMD_NULL;
    }
}
void can_bus_frame_receive(CanRxMsg rxMsg)
{
    sCanFrameExtID extID;
    sCanFrameExt   frame;
    u16 i =0;
    
    extID.func_id  = (rxMsg.ExtId>>7)&0x1F;
    extID.dst_id   = (rxMsg.ExtId>>22)&0x7F;
    extID.seg_polo = (rxMsg.ExtId>>20)&0x3;
    extID.seg_num  = (rxMsg.ExtId>>12)&0xFF;


    frame.extId.dst_id = rxMsg.ExtId >> 22 & 0x7F;
    frame.extId.src_id = rxMsg.ExtId & 0x7F;
    frame.extId.func_id = (rxMsg.ExtId >> 7) & 0x1F;
    frame.extId.seg_num = (rxMsg.ExtId >> 12) & 0xFF;
    frame.extId.seg_polo = (rxMsg.ExtId >> 20) & 0x3;

    frame.data_len = rxMsg.DLC;

    for (i = 0; i < frame.data_len; i++) {
        frame.data[i] = rxMsg.Data[i];
    }

    vcanbus_addto_cansendqueue_two(frame);

    
    
    if(extID.dst_id != Local_Station) return;
    if(extID.func_id != CAN_FUNC_ID_BOOT_MODE) return;
    
    if(extID.seg_polo == CAN_SEG_POLO_NONE)
    {
        memcpy(can_recv_buff,rxMsg.Data,rxMsg.DLC);
        can_recv_len = rxMsg.DLC;
        slave_recv_process();
    }
    else if(extID.seg_polo == CAN_SEG_POLO_FIRST)
    {
        memcpy(can_recv_buff,rxMsg.Data,rxMsg.DLC);
        
        g_SegPolo  = CAN_SEG_POLO_FIRST;
        g_SegNum   = extID.seg_num;
        g_SegBytes = rxMsg.DLC;
    }
    else if(extID.seg_polo == CAN_SEG_POLO_MIDDLE)
    {
        if(   (g_SegPolo == CAN_SEG_POLO_FIRST) 
           && (extID.seg_num == (g_SegNum+1)) 
           && ((g_SegBytes+rxMsg.DLC) <= CAN_RX_BUFF_SIZE) )
        {
            memcpy(can_recv_buff+g_SegBytes,rxMsg.Data,rxMsg.DLC);
            
            g_SegNum ++;
            g_SegBytes += rxMsg.DLC;
        }
    }
    else if(extID.seg_polo == CAN_SEG_POLO_FINAL)
    {
        if(   (g_SegPolo == CAN_SEG_POLO_FIRST) 
           && (extID.seg_num == (g_SegNum+1)) 
           && ((g_SegBytes+rxMsg.DLC) <= CAN_RX_BUFF_SIZE) )
        {
            memcpy(can_recv_buff+g_SegBytes,rxMsg.Data,rxMsg.DLC);
            can_recv_len = g_SegBytes + rxMsg.DLC;
            slave_recv_process();
            
            g_SegPolo = CAN_SEG_POLO_NONE;
            g_SegNum = 0;
            g_SegBytes = 0;
        }
    }
}
void can_bus_send_one_frame(sCanFrameExt sTxMsg)
{
    CanTxMsg TxMessage;
    
    TxMessage.ExtId = (sTxMsg.extId.src_id & 0x7F)|((sTxMsg.extId.func_id&0x1F)<<7)|((sTxMsg.extId.seg_num&0xFF)<<12)
                     |((sTxMsg.extId.seg_polo&0x3)<<20)|((sTxMsg.extId.dst_id&0x7F)<<22);
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = sTxMsg.data_len;
    memcpy(TxMessage.Data, sTxMsg.data, TxMessage.DLC);
    
    CAN_Transmit(CAN2,&TxMessage);
}
void slave_reply_boot_cmd()
{
    sCanFrameExt canTxMsg;
    
    canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
    canTxMsg.extId.seg_num  = 0;
    canTxMsg.extId.func_id  = CAN_FUNC_ID_BOOT_MODE;
    canTxMsg.extId.src_id   = Local_Station;
    canTxMsg.extId.dst_id   = 1;
    canTxMsg.data_len = 5;
    canTxMsg.data[0] = 0x55;
    canTxMsg.data[1] = 0x55;
    canTxMsg.data[2] = Local_Station;
    canTxMsg.data[3] = DLOAD_CMD_ENTER_BOOT;
    canTxMsg.data[4] = sum_check(canTxMsg.data,4);

    vcanbus_addto_cansendqueue_one(canTxMsg);
    
}
void slave_reply_erase_flash_cmd()
{
    sCanFrameExt canTxMsg;
    
    canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
    canTxMsg.extId.seg_num  = 0;
    canTxMsg.extId.func_id  = CAN_FUNC_ID_BOOT_MODE;
    canTxMsg.extId.src_id   = Local_Station;
    canTxMsg.extId.dst_id   = 1;
    canTxMsg.data_len = 5;
    canTxMsg.data[0] = 0x55;
    canTxMsg.data[1] = 0x55;
    canTxMsg.data[2] = Local_Station;
    canTxMsg.data[3] = DLOAD_CMD_ERASE_FLASH;
    canTxMsg.data[4] = sum_check(canTxMsg.data,4);
    
    vcanbus_addto_cansendqueue_one(canTxMsg);
}
void slave_reply_tran_data_cmd()
{
    sCanFrameExt canTxMsg;
    
    canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
    canTxMsg.extId.seg_num  = 0;
    canTxMsg.extId.func_id  = CAN_FUNC_ID_BOOT_MODE;
    canTxMsg.extId.src_id   = Local_Station;
    canTxMsg.extId.dst_id   = 1;
    canTxMsg.data_len = 6;
    canTxMsg.data[0] = 0x55;
    canTxMsg.data[1] = 0x55;
    canTxMsg.data[2] = Local_Station;
    canTxMsg.data[3] = DLOAD_CMD_TRAN_DATA;
    canTxMsg.data[4] = pk_cur_num;
    canTxMsg.data[5] = sum_check(canTxMsg.data,5);
    
    vcanbus_addto_cansendqueue_one(canTxMsg);
}
void erase_user_app_flash()
{
    FLASH_Status f_status = FLASH_COMPLETE;
    
    FLASH_Unlock();
    FLASH_DataCacheCmd(DISABLE);
    f_status = FLASH_EraseSector(FLASH_Sector_3,VoltageRange_3);
    f_status = FLASH_EraseSector(FLASH_Sector_4,VoltageRange_3);
    FLASH_DataCacheCmd(ENABLE);
    FLASH_Lock();
}
void program_user_app_flash()
{
    u32* source;
    u32  flashDst;
    u16  n;
    
    if(slave_recv_pk_num == pk_cur_num)
        return;
    else
        slave_recv_pk_num = pk_cur_num;
    
    source = (u32*)(can_recv_buff+8);
    flashDst = ApplicationAddress + (pk_cur_num-1)*CAN_RX_DATA_PK_SIZE;
    
//    FLASH_Unlock();
    for (n = 0; n < pk_len; n += 4)
    {
        FLASH_Unlock();
        FLASH_ProgramWord(flashDst, *source);
        FLASH_Lock();
        if(*(u32 *)flashDst != *source)
        {
            //g_download_status = DLOAD_INIT;
//            FLASH_Lock();
            return;
        }
        flashDst += 4;
        source += 1;
    }
//    FLASH_Lock();
    if(pk_cur_num == pk_tot_num)
    {
        FLASH_Unlock();
        FLASH_ProgramHalfWord(UserAppEnFlagAddress,0xAA);
        FLASH_Lock();
        //g_download_status = DLOAD_INIT;
    }
}
void can_bus_reply_process(void)
{
    u32 i = 0;
    while(1)
    {
        if(slave_reply_cmd == DLOAD_CMD_ENTER_BOOT)
        {
            slave_reply_boot_cmd();
            slave_reply_cmd = DLOAD_CMD_NULL;
        }
        else if(slave_reply_cmd == DLOAD_CMD_ERASE_FLASH)
        {
            erase_user_app_flash();
            slave_reply_erase_flash_cmd();
            slave_reply_cmd = DLOAD_CMD_NULL;
        }
        else if(slave_reply_cmd == DLOAD_CMD_TRAN_DATA)
        {
            program_user_app_flash();
//            while(1){
//                i++;
//                if(i > 0xFFFF){
//                  break;
//                }
//            }
            slave_reply_tran_data_cmd();
            slave_reply_cmd = DLOAD_CMD_NULL;
        }
        vcan_sendframe_process_one();
        vcan_sendframe_process_two();
    }
}