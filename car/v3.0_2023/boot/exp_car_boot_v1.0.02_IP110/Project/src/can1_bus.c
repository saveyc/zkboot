#include "stm32f10x.h"
#include "stm32f107.h"
#include "main.h"


//CAN1 与下游通讯


#define  canframebuffsize_one   50
sCanFrameExt    canframequeuebuff_one[canframebuffsize_one];
sCanFrameQueue   canframequeue_one;

//
void vcan_sendmsg_one(u8* buff, u16 send_total_len, u8 type, u8 dst);
u8 sum_check(u8* pdata, u16 len);



u8  cur_tran_station;
u16 pk_tot_num;
u16 pk_cur_num;
u16 pk_len;
u8  trans_flag = 0;
u16 trans_timeout = 0;

u16 can_resend_delay = 0;
u8  can_resend_cnt = 0;
u8  can_send_flag = 0;


u8  can_send_buff[300];
u16 can_send_tot = 0;
u16 can_send_len = 0;

u8  can_recv_buff[CAN_RX_BUFF_SIZE];
u16 can_recv_len = 0;
u8  g_SegPolo = CAN_SEG_POLO_NONE;
u8  g_SegNum = 0;
u16 g_SegBytes = 0;

u16 slave_reply_cmd = DLOAD_CMD_NULL;
u16 slave_recv_pk_num = 0;

u8 SlaveApp_UpdateFlag = 0;




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

    TxMessage.ExtId = (sTxMsg.extId.mac_id & 0x7F) | ((sTxMsg.extId.func_id & 0x1F) << 7) | ((sTxMsg.extId.seg_num & 0xFF) << 12) | ((sTxMsg.extId.seg_polo & 0x3) << 20) | ((sTxMsg.extId.dst_id & 0x7F) << 22);
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = sTxMsg.data_len;
    memcpy(TxMessage.Data, sTxMsg.data, TxMessage.DLC);

//    return CAN_Transmit(CAN1, &TxMessage);
    
    transmit_mailbox = CAN_Transmit(CAN1, &TxMessage);
    
    while(CAN_TransmitStatus(CAN1,transmit_mailbox)!= CANTXOK)
    {
        retry++;
        if(retry == 0xFFFF)
        {
            retry++;
            break;
        }
    }
    
     return CAN_TransmitStatus(CAN1,transmit_mailbox);
}


void vcanbus_framereceive_one(CanRxMsg rxMsg)
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


    if (Local_station == 1) {
        master_can_bus_frame_receive(rxMsg);
    }
    else {
        vcanbus_addto_cansendqueue_two(frame);
    }


}


void vcan_sendmsg_one(u8* buff, u16 send_total_len, u8 type, u8 dst)
{
    sCanFrameExt canTxMsg;
    u16 send_len = 0;

    canTxMsg.extId.func_id = type;
    canTxMsg.extId.mac_id = Local_station;
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

    if (Local_station == 1) {
        if ((canTxMsg->extId.seg_polo == CAN_SEG_POLO_NONE) || (canTxMsg->extId.seg_polo == CAN_SEG_POLO_FINAL)) {
                can_resend_delay = trans_timeout;
                can_send_flag = 0;
        }
    }
}

u8 sum_check(u8* pdata, u16 len)
{
    u16 i;
    u8  sum = 0;

    for (i = 0; i < len; i++)
    {
        sum += pdata[i];
    }
    return sum;
}


void can_recv_timeout()
{
    if ((can_resend_delay != 0) && (trans_flag == 1))
    {
        can_resend_delay--;
        if (can_resend_delay == 0)
        {
            if (can_resend_cnt >= 3)
            {
                g_download_status = DLOAD_INIT;
                can_resend_cnt = 0;
                FLASH_Unlock();
                FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
                if (FLASH_ErasePage(UserAppEnFlagAddress) == FLASH_COMPLETE)
                {
                    FLASH_ProgramHalfWord(UserAppEnFlagAddress, 0x00AA);
                }
                FLASH_Lock();
                loadctrl.trans = 5000;
            }
            else
            {
                can_resend_cnt++;
            }
            trans_flag = 0;
        }
    }
}


void master_can_bus_frame_receive(CanRxMsg rxMsg)
{
    u8 recv_cmd = rxMsg.Data[3];

    if (recv_cmd == DLOAD_CMD_ENTER_BOOT && g_download_status == DLOAD_TRAN_INIT)
    {
        print_msg_len = sprintf((char*)print_msg_buff, "Reply boot command from:%d\n", cur_tran_station);
        udp_send_message(print_msg_buff, print_msg_len);

        g_download_status = DLOAD_TRAN_ERASE;
    }
    else if (recv_cmd == DLOAD_CMD_ERASE_FLASH && g_download_status == DLOAD_TRAN_ERASE)
    {
        print_msg_len = sprintf((char*)print_msg_buff, "Reply erase flash command from:%d\n", cur_tran_station);
        udp_send_message(print_msg_buff, print_msg_len);

        g_download_status = DLOAD_TRAN_PROG;
    }
    else if (recv_cmd == DLOAD_CMD_TRAN_DATA && g_download_status == DLOAD_TRAN_PROG)
    {
        print_msg_len = sprintf((char*)print_msg_buff, "Reply pack %d of %d from:%d\n", pk_cur_num, pk_tot_num, cur_tran_station);
        udp_send_message(print_msg_buff, print_msg_len);

        pk_cur_num++;
    }

    trans_flag = 0;
    can_resend_delay = 0;
    can_resend_cnt = 0;
}


void host_send_boot_cmd()
{
    can_send_buff[0] = 0xAA;
    can_send_buff[1] = 0xAA;
    can_send_buff[2] = cur_tran_station;
    can_send_buff[3] = DLOAD_CMD_ENTER_BOOT;
    can_send_buff[4] = sum_check(can_send_buff, 4);
    can_send_tot = 5;

    can_send_flag = 1;
    trans_timeout = 1000;
    trans_flag = 1;

    print_msg_len = sprintf((char*)print_msg_buff, "Send boot command to:%d\n", cur_tran_station);
    udp_send_message(print_msg_buff, print_msg_len);

    vcan_sendmsg_one(can_send_buff, can_send_tot, CAN_FUNC_ID_BOOT_MODE, cur_tran_station);


}

void host_send_erase_flash_cmd()
{
    can_send_buff[0] = 0xAA;
    can_send_buff[1] = 0xAA;
    can_send_buff[2] = cur_tran_station;
    can_send_buff[3] = DLOAD_CMD_ERASE_FLASH;
    can_send_buff[4] = sum_check(can_send_buff, 4);
    can_send_tot = 5;

    can_send_flag = 1;
    trans_timeout = 5000;
    trans_flag = 1;

    print_msg_len = sprintf((char*)print_msg_buff, "Send erase flash command to:%d\n", cur_tran_station);
    udp_send_message(print_msg_buff, print_msg_len);
    vcan_sendmsg_one(can_send_buff, can_send_tot, CAN_FUNC_ID_BOOT_MODE, cur_tran_station);
}


void host_send_tran_data_cmd()
{
    u8* pDataSrc = (u8*)AppTempAddress;
    u16 send_pack_len;
    u16 send_len;

    if (pk_cur_num > pk_tot_num)//完成烧写一个站
    {
        cur_tran_station++;
        pk_cur_num = 1;
        g_download_status = DLOAD_TRAN_INIT;
        return;
    }
    if (pk_cur_num < pk_tot_num)
    {
        send_pack_len = CAN_SEND_DATA_PK_SIZE;
    }
    else
    {
        send_pack_len = file_tot_bytes - (pk_tot_num - 1) * CAN_SEND_DATA_PK_SIZE;
    }
    pDataSrc += (pk_cur_num - 1) * CAN_SEND_DATA_PK_SIZE;
    send_len = send_pack_len + 9;

    can_send_buff[0] = 0xAA;
    can_send_buff[1] = 0xAA;
    can_send_buff[2] = cur_tran_station;
    can_send_buff[3] = DLOAD_CMD_TRAN_DATA;
    can_send_buff[4] = pk_tot_num;
    can_send_buff[5] = pk_cur_num;
    can_send_buff[6] = send_pack_len & 0xFF;
    can_send_buff[7] = (send_pack_len >> 8) & 0xFF;
    memcpy(can_send_buff + 8, pDataSrc, send_pack_len);
    can_send_buff[send_len - 1] = sum_check(can_send_buff, send_len - 1);
    can_send_tot = send_len;

    can_send_flag = 1;
    trans_timeout = 2000;
    trans_flag = 1;

    print_msg_len = sprintf((char*)print_msg_buff, "Send pack %d of %d to:%d\n", pk_cur_num, pk_tot_num, cur_tran_station);
    udp_send_message(print_msg_buff, print_msg_len);
    vcan_sendmsg_one(can_send_buff, can_send_tot, CAN_FUNC_ID_BOOT_MODE, cur_tran_station);
}


//主机转发数据
void host_transmit_process(void)
{
    if (g_download_status == DLOAD_INIT || g_download_status == DLOAD_PARA)
    {
        return;
    }
    if (file_tot_bytes == 0)
    {
        g_download_status = DLOAD_INIT;
        return;
    }
    if (cur_tran_station >= (sDLoad_Para_Data.st_addr + sDLoad_Para_Data.addr_num))
    {
        g_download_status = DLOAD_INIT;
        if (readFlash_uint(ApplicationAddress) != 0xFFFF)
        {
            FLASH_Unlock();
            FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
            if (FLASH_ErasePage(UserAppEnFlagAddress) == FLASH_COMPLETE)
            {
                FLASH_ProgramHalfWord(UserAppEnFlagAddress, 0x00AA);
            }
            FLASH_Lock();
            loadctrl.trans = 5000;
        }
        return;
    }
    if (g_download_status == DLOAD_TRAN_INIT && trans_flag == 0)
    {
        host_send_boot_cmd();
    }
    if (g_download_status == DLOAD_TRAN_ERASE && trans_flag == 0)
    {
        host_send_erase_flash_cmd();
    }
    if (g_download_status == DLOAD_TRAN_PROG && trans_flag == 0)
    {
        host_send_tran_data_cmd();
    }
}
