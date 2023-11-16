#include "main.h"

u32 port_enable_state = 0;
u32 port_print_state = 0;
u32 port_print_state_bk = 0;
u32 port_full_state = 0;

u8  frame_count = 1;
u8  frame_count_bk = 0;

u16 manual_flag =0;
u16 manual_operation =0;

u16  result_CAN1 = 0;



//




void can_bus_send_one_frame(sCanFrameExt sTxMsg)
{
    CanTxMsg TxMessage;
    u8 i;
    
    TxMessage.ExtId = (sTxMsg.extId.src_id)|((sTxMsg.extId.func_id&0xF)<<8);
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = sTxMsg.data_len;
    
    for(i=0; i<TxMessage.DLC; i++)
    {
        TxMessage.Data[i] = sTxMsg.data[i];
    }
    
    result_CAN1 = CAN_Transmit(CAN1,&TxMessage);
    i = 0;
}

void can_bus_send_port_enable_state()
{
    sCanFrameExt canTxMsg;
    
    canTxMsg.extId.func_id  = CAN_FUNC_ID_PORT_STATE;
    canTxMsg.extId.src_id   = Local_Station;
    canTxMsg.data_len = 8;
    canTxMsg.data[0] = port_enable_state&0xFF;
    canTxMsg.data[1] = (port_enable_state>>8)&0xFF;
    canTxMsg.data[2] = (port_enable_state>>16)&0xFF;
    canTxMsg.data[3] = port_full_state&0xFF;
    canTxMsg.data[4] = (port_full_state>>8)&0xFF;
    canTxMsg.data[5] = (port_full_state>>16)&0xFF;
    canTxMsg.data[6] = 0;
    canTxMsg.data[7] = 0;
    
    can_bus_send_one_frame(canTxMsg);
    vcanbus_addto_cansendqueue_two(canTxMsg);
}


void can_bus_send_port_print_state()
{
    sCanFrameExt canTxMsg;
    
    if(frame_count_bk != frame_count)
    {
        frame_count_bk = frame_count;
        port_print_state_bk = port_print_state;
    }
    
    canTxMsg.extId.func_id  = CAN_FUNC_ID_PRINT_STATE;
    canTxMsg.extId.src_id   = Local_Station;
    canTxMsg.data_len = 4;
    canTxMsg.data[0] = frame_count;
    canTxMsg.data[1] = port_print_state_bk&0xFF;
    canTxMsg.data[2] = (port_print_state_bk>>8)&0xFF;
    canTxMsg.data[3] = (port_print_state_bk>>16)&0xFF;
    
    can_bus_send_one_frame(canTxMsg);
}


void can_bus_frame_receive(CanRxMsg rxMsg)
{
    sCanFrameExt canTxMsg;

    canTxMsg.extId.func_id = CAN_FUNC_ID_PORT_STATE;
    canTxMsg.extId.src_id = Local_Station;
    canTxMsg.data_len = 8;
    canTxMsg.data[0] = port_enable_state & 0xFF;
    canTxMsg.data[1] = (port_enable_state >> 8) & 0xFF;
    canTxMsg.data[2] = (port_enable_state >> 16) & 0xFF;
    canTxMsg.data[3] = port_full_state & 0xFF;
    canTxMsg.data[4] = (port_full_state >> 8) & 0xFF;
    canTxMsg.data[5] = (port_full_state >> 16) & 0xFF;
    canTxMsg.data[6] = Local_Station;
    canTxMsg.data[7] = 1;

    can_bus_send_one_frame(canTxMsg);
}