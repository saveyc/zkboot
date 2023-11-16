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
    
    extID.func_id  = (rxMsg.ExtId>>8)&0xF;
    extID.dst_id   = (rxMsg.ExtId>>22)&0x7F;
    extID.seg_polo = (rxMsg.ExtId>>20)&0x3;
    extID.seg_num  = (rxMsg.ExtId>>12)&0xFF;
    
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
    
    TxMessage.ExtId = (sTxMsg.extId.src_id)|((sTxMsg.extId.func_id&0xF)<<8)|((sTxMsg.extId.seg_num&0xFF)<<12)
                     |((sTxMsg.extId.seg_polo&0x3)<<20)|((sTxMsg.extId.dst_id&0x7F)<<22);
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = sTxMsg.data_len;
    memcpy(TxMessage.Data, sTxMsg.data, TxMessage.DLC);
    
    CAN_Transmit(CAN1,&TxMessage);
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
    
    can_bus_send_one_frame(canTxMsg);
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
    
    can_bus_send_one_frame(canTxMsg);
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
    
    can_bus_send_one_frame(canTxMsg);
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
    
    for (n = 0; n < pk_len; n += 4)
    {
        FLASH_Unlock();
        FLASH_ProgramWord(flashDst, *source);
        FLASH_Lock();
        if(*(u32 *)flashDst != *source)
        {
            //g_download_status = DLOAD_INIT;
            return;
        }
        flashDst += 4;
        source += 1;
    }
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
            slave_reply_tran_data_cmd();
            slave_reply_cmd = DLOAD_CMD_NULL;
        }
    }
}