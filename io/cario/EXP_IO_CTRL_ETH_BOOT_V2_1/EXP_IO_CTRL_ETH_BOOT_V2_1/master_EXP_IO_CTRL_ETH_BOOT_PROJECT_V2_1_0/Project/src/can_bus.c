#include "stm32f10x.h"
#include "stm32f107.h"
#include <stdio.h>
#include "main.h"

sCanFrameExt canTxMsg;

u8  cur_tran_station;
u16 pk_tot_num;
u16 pk_cur_num;
u8  trans_flag = 0;
u16 trans_timeout = 0;

u16 can_resend_delay = 0;
u8  can_resend_cnt = 0;
u8  can_send_flag = 0;
u8  can_send_frame_start = 0;

u8  can_send_buff[300];
u16 can_send_tot = 0;
u16 can_send_len = 0;

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
void can_recv_timeout()
{
    if((can_resend_delay != 0) && (trans_flag == 1))
    {
        can_resend_delay--;
        if(can_resend_delay == 0)
        {
            if(can_resend_cnt >=3)
            {
                g_download_status = DLOAD_INIT;
                can_resend_cnt = 0;
            }
            else
            {
                can_resend_cnt++;
            }
            trans_flag = 0;
        }
    }
}
void can_bus_send_one_frame(sCanFrameExt sTxMsg)
{
    CanTxMsg TxMessage;
    u16 retry = 0;
    
    TxMessage.ExtId = (sTxMsg.extId.src_id)|((sTxMsg.extId.func_id&0xF)<<8)|((sTxMsg.extId.seg_num&0xFF)<<12)
                     |((sTxMsg.extId.seg_polo&0x3)<<20)|((sTxMsg.extId.dst_id&0x7F)<<22);
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = sTxMsg.data_len;
    memcpy(TxMessage.Data, sTxMsg.data, TxMessage.DLC);
    
    while(CAN_Transmit(CAN1,&TxMessage)==CAN_NO_MB)
    {
        retry++;
        if(retry == 10000)
        {
            break;
        }
    }
}
//CAN总线分包传输
void can_send_frame_process()
{
    if(can_send_flag)
    {
        canTxMsg.extId.func_id  = CAN_FUNC_ID_BOOT_MODE;
        canTxMsg.extId.src_id   = 1;
        canTxMsg.extId.dst_id   = cur_tran_station;
        
        if(can_send_tot <= CAN_PACK_DATA_LEN)//不需分段传输
        {
            canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
            canTxMsg.extId.seg_num  = 0;
            canTxMsg.data_len = can_send_tot;
            memcpy(canTxMsg.data, can_send_buff, canTxMsg.data_len);
            can_bus_send_one_frame(canTxMsg);
            
            can_resend_delay = trans_timeout;
            can_send_flag = 0;
        }
        else//需要分段传输
        {
            if(can_send_frame_start)
            {
                canTxMsg.extId.seg_polo = CAN_SEG_POLO_FIRST;
                canTxMsg.extId.seg_num  = 0;
                canTxMsg.data_len = CAN_PACK_DATA_LEN;
                memcpy(canTxMsg.data, can_send_buff, canTxMsg.data_len);
                can_bus_send_one_frame(canTxMsg);
                
                can_send_len = CAN_PACK_DATA_LEN;
                can_send_frame_start = 0;
            }
            else
            {
                if(can_send_len + CAN_PACK_DATA_LEN < can_send_tot)
                {
                    canTxMsg.extId.seg_polo = CAN_SEG_POLO_MIDDLE;
                    canTxMsg.extId.seg_num ++;
                    canTxMsg.data_len = CAN_PACK_DATA_LEN;
                    memcpy(canTxMsg.data, can_send_buff+can_send_len, canTxMsg.data_len);
                    can_bus_send_one_frame(canTxMsg);
                    
                    can_send_len += CAN_PACK_DATA_LEN;
                }
                else
                {
                    canTxMsg.extId.seg_polo = CAN_SEG_POLO_FINAL;
                    canTxMsg.extId.seg_num ++;
                    canTxMsg.data_len = can_send_tot-can_send_len;
                    memcpy(canTxMsg.data, can_send_buff+can_send_len, canTxMsg.data_len);
                    can_bus_send_one_frame(canTxMsg);
                    
                    can_resend_delay = trans_timeout;
                    can_send_flag = 0;
                }
            }
        }
    }
}
void can_bus_frame_receive(CanRxMsg rxMsg)
{
    u8 recv_cmd = rxMsg.Data[3];
    
    if(recv_cmd == DLOAD_CMD_ENTER_BOOT && g_download_status == DLOAD_TRAN_INIT)
    {
        print_msg_len = sprintf((char*)print_msg_buff,"Reply boot command from:%d\n",cur_tran_station);
        udp_send_message(print_msg_buff,print_msg_len);
        
        g_download_status = DLOAD_TRAN_ERASE;
    }
    else if(recv_cmd == DLOAD_CMD_ERASE_FLASH && g_download_status == DLOAD_TRAN_ERASE)
    {
        print_msg_len = sprintf((char*)print_msg_buff,"Reply erase flash command from:%d\n",cur_tran_station);
        udp_send_message(print_msg_buff,print_msg_len);
        
        g_download_status = DLOAD_TRAN_PROG;
    }
    else if(recv_cmd == DLOAD_CMD_TRAN_DATA && g_download_status == DLOAD_TRAN_PROG)
    {
        print_msg_len = sprintf((char*)print_msg_buff,"Reply pack %d of %d from:%d\n",pk_cur_num,pk_tot_num,cur_tran_station);
        udp_send_message(print_msg_buff,print_msg_len);
        
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
    can_send_buff[4] = sum_check(can_send_buff,4);
    can_send_tot = 5;
    
    can_send_flag = 1;
    trans_timeout = 1000;
    trans_flag = 1;
    
    print_msg_len = sprintf((char*)print_msg_buff,"Send boot command to:%d\n",cur_tran_station);
    udp_send_message(print_msg_buff,print_msg_len);
}
void host_send_erase_flash_cmd()
{
    can_send_buff[0] = 0xAA;
    can_send_buff[1] = 0xAA;
    can_send_buff[2] = cur_tran_station;
    can_send_buff[3] = DLOAD_CMD_ERASE_FLASH;
    can_send_buff[4] = sum_check(can_send_buff,4);
    can_send_tot = 5;
    
    can_send_flag = 1;
    trans_timeout = 5000;
    trans_flag = 1;
    
    print_msg_len = sprintf((char*)print_msg_buff,"Send erase flash command to:%d\n",cur_tran_station);
    udp_send_message(print_msg_buff,print_msg_len);
}
void host_send_tran_data_cmd()
{
    u8* pDataSrc = (u8*)AppTempAddress;
    u16 send_pack_len;
    u16 send_len;
    
    if(pk_cur_num > pk_tot_num)//完成烧写一个站
    {
        cur_tran_station++;
        pk_cur_num = 1;
        g_download_status = DLOAD_TRAN_INIT;
        return;
    }
    if(pk_cur_num < pk_tot_num)
    {
        send_pack_len = CAN_SEND_DATA_PK_SIZE;
    }
    else
    {
        send_pack_len = file_tot_bytes - (pk_tot_num-1)*CAN_SEND_DATA_PK_SIZE;
    }
    pDataSrc += (pk_cur_num-1)*CAN_SEND_DATA_PK_SIZE;
    send_len = send_pack_len + 9;
    
    can_send_buff[0] = 0xAA;
    can_send_buff[1] = 0xAA;
    can_send_buff[2] = cur_tran_station;
    can_send_buff[3] = DLOAD_CMD_TRAN_DATA;
    can_send_buff[4] = pk_tot_num;
    can_send_buff[5] = pk_cur_num;
    can_send_buff[6] = send_pack_len&0xFF;
    can_send_buff[7] = (send_pack_len>>8)&0xFF;
    memcpy(can_send_buff+8,pDataSrc,send_pack_len);
    can_send_buff[send_len-1] = sum_check(can_send_buff,send_len-1);
    can_send_tot = send_len;
    
    can_send_frame_start = 1;
    can_send_flag = 1;
    trans_timeout = 2000;
    trans_flag = 1;
    
    print_msg_len = sprintf((char*)print_msg_buff,"Send pack %d of %d to:%d\n",pk_cur_num,pk_tot_num,cur_tran_station);
    udp_send_message(print_msg_buff,print_msg_len);
}
//主机转发数据
void host_transmit_process(void)
{
    if(g_download_status == DLOAD_INIT || g_download_status == DLOAD_PARA)
    {
        return;
    }
    if(file_tot_bytes == 0)
    {
        g_download_status = DLOAD_INIT;
        return;
    }
    if(cur_tran_station >= (sDLoad_Para_Data.st_addr + sDLoad_Para_Data.addr_num))
    {
        g_download_status = DLOAD_INIT;
        return;
    }
    if(g_download_status == DLOAD_TRAN_INIT && trans_flag == 0)
    {
        host_send_boot_cmd();
    }
    if(g_download_status == DLOAD_TRAN_ERASE && trans_flag == 0)
    {
        host_send_erase_flash_cmd();
    }
    if(g_download_status == DLOAD_TRAN_PROG && trans_flag == 0)
    {
        host_send_tran_data_cmd();
    }
}