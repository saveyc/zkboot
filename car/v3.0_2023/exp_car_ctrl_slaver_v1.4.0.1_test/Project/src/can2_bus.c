#include "stm32f10x.h"
#include "stm32f107.h"
#include "main.h"

// can2 和上游通讯
#define  canframebuffsize_two   60
sCanFrameExt    canframequeuebuff_two[canframebuffsize_two];
sCanFrameQueue   canframequeue_two;

// 接收下料数据
u8  can_recv_buff_two[CAN_RX_BUFF_SIZE_TWO];
u16 can_recv_len_two = 0;
sCanFrameCtrl   canctrl_two;

//
u8  can_send_buff_two[CAN_SEND_DATA_LEN];
u16 can_send_tot_two = 0;

// 接收其它信息
u8 can_recv_config_msg[CAN_RX_CONFIG_SIZE_TWO];
u16 can_recconf_len = 0;
sCanFrameCtrl   canconf_two;

u16 can2lastphoto = 0;
u32 can2lastunload = 0;


void vcan_sendmsg_two(u8* buff, u8 send_total_len, u8 type, u8 dst);




void vcanbus_init_cansendqueue_two(void)
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


u8 u8canbus_send_oneframe_two(sCanFrameExt sTxMsg)
{
    CanTxMsg TxMessage;

    TxMessage.ExtId = (sTxMsg.extId.mac_id & 0x7F) | ((sTxMsg.extId.func_id & 0x1F) << 7) | ((sTxMsg.extId.seg_num & 0xFF) << 12) | ((sTxMsg.extId.seg_polo & 0x3) << 20) | ((sTxMsg.extId.dst_id & 0x7F) << 22);
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = sTxMsg.data_len;
    memcpy(TxMessage.Data, sTxMsg.data, TxMessage.DLC);

    return CAN_Transmit(CAN2, &TxMessage);
}

void vcanbus_reply_ack_two(u16 fun_id)
{
    u8 buff[10] = { 0 };
    u8 len = 0;

    if (fun_id == CAN_FUNC_ID_UNLOAD_TYPE) {
        //        can_send_send_msg(buff, len, CAN_FUNC_ID_UNLOAD_TYPE, 1);
    }

}

//  接收3包合1数据
void vcan_deal_with_recv_unload_data(void)
{
    u16  num = 0;
    u16  len = 0;
    u16  i = 0;
    u32  canrecvindex = 0;
    u32  unloadindex = 0;
    u32  localcarmin = 0;
    u32  localcarmax = 0;
    u8   loadnum, unloadnum;
    u16  ionum = 0;
    sData_pc2bd_unload_node unloadnode;
    sData_pc2bd_load_node loadnode;
    struct xLIST_ITEM  unloadlistitem;
    struct xLIST_ITEM  loadlistitem;
    unloadnode.index = &unloadlistitem;
    loadnode.index = &loadlistitem;
    u8 buff[10] = { 0 };


    canrecvindex = (can_recv_buff_two[0] | (can_recv_buff_two[1] << 8) | (can_recv_buff_two[2] << 16) | (can_recv_buff_two[3] << 24));
    unloadindex = (can_recv_buff_two[4] | (can_recv_buff_two[5] << 8) | (can_recv_buff_two[6] << 16) | (can_recv_buff_two[7] << 24));

    localcarmin = (key_value - 2) * datapcconfig.onegroupcarnum + 1;
    localcarmax = (key_value - 1) * datapcconfig.onegroupcarnum;

    for (i = 0; i < 4; i++) {
        buff[i] = can_recv_buff_two[4 + i];
    }
    buff[4] = datasysmsg.curposition & 0xFF;
    buff[5] = (datasysmsg.curposition >> 8) & 0xFF;
    
    if((datasysmsg.curposition != (can2lastphoto + 1)) && (datasysmsg.curposition != 1) && (datasysmsg.curposition != can2lastphoto)){
         buff[6] = 1;
    }
    else{
       buff[6] = 0;
    }

    can2lastphoto = datasysmsg.curposition;
//    if( lastslaver_station == 1){
    if(can2lastunload != unloadindex){
         vcan_sendmsg_two(&(buff[0]), 7, CAN_FUNC_ID_UNLOADMSG_REPLY, 1);
    }
//          vcan_sendmsg_two(&(buff[0]), 7, CAN_FUNC_ID_UNLOADMSG_REPLY, 1);
//    }
     can2lastunload = unloadindex;

    if (datasysmsg.can_prerecv_index == canrecvindex) return;

    datasysmsg.can_currecv_index = canrecvindex;



    if ((datasysmsg.can_currecv_index != datasysmsg.can_prerecv_index) || (datasysmsg.can_currecv_index == 0)) {
        if (datasysmsg.can_currecv_index == 0) {
            datasysmsg.can_prerecv_index = datasysmsg.can_currecv_index;
            datasysmsg.unload_pre_index = unloadindex;
        }
        else {
            num = datasysmsg.can_currecv_index - datasysmsg.can_prerecv_index;
            if (num > 1) {
                datasyserr.lostnum = num - 1;
                datasyserr.unload_last_index = datasysmsg.unload_pre_index;
            }
        }

        datasysmsg.unload_pre_index = unloadindex;
        datasysmsg.can_prerecv_index = datasysmsg.can_currecv_index;


        len = CAN_RECV_PREFRAME;

        //load msg
        loadnum = *(u8*)(can_recv_buff_two + len);
        len += 1;
        for (i = 0; i < loadnum; i++) {
            loadnode.car_load_index = *(u16*)(can_recv_buff_two + len);
            len += 2;
            loadnode.car_load_position = *(u16*)(can_recv_buff_two + len);
            len += 2;
            loadnode.car_load_length = *(u16*)(can_recv_buff_two + len);
            len += 2;
            loadnode.car_load_delay = *(u16*)(can_recv_buff_two + len);
            len += 2;
            loadnode.car_load_dir = *(u8*)(can_recv_buff_two + len);
            len += 1;
            loadnode.car_load_speed = *(u8*)(can_recv_buff_two + len);
            len += 1;

            if ((loadnode.car_load_index >= localcarmin) && (loadnode.car_load_index <= localcarmax)) {
                vposition_remove_repeat_carnum(loadnode.car_load_index);
                vdata_addto_list_load(loadnode);
            }
        }

        //unload msg
        unloadnum = *(u8*)(can_recv_buff_two + len);
        len += 1;

        for (i = 0; i < unloadnum; i++) {
            unloadnode.car_unload_index = *(u16*)(can_recv_buff_two + len);
            len += 2;
            unloadnode.car_unload_exitno = *(u16*)(can_recv_buff_two + len);
            len += 2;
            unloadnode.car_rotate_length = *(u16*)(can_recv_buff_two + len);
            len += 2;
            unloadnode.car_unload_delay = *(u16*)(can_recv_buff_two + len);
            len += 2;
            unloadnode.car_unload_dir = *(u8*)(can_recv_buff_two + len);
            len += 1;
            unloadnode.car_exit_speed = *(u8*)(can_recv_buff_two + len);
            len += 1;
            unloadnode.car_chute_index = *(u16*)(can_recv_buff_two + len);
            len += 2;

            if ((unloadnode.car_unload_index >= localcarmin) && (unloadnode.car_unload_index <= localcarmax)) {
                vposition_remove_repeat_carnum(unloadnode.car_unload_index);
                vdata_addto_list_unload(unloadnode);
            }
        }

        // io msg
        // init ?
        iostate.num = 0;
        ionum = 0;

        ionum = *(u16*)(can_recv_buff_two + len);
        len += 2;
        iostate.num = ionum;

        if (ionum < MAX_IO_NUM) {
            for (i = 0; i < ionum; i++) {
                iostate.io_info[i] = *(u16*)(can_recv_buff_two + len);
                len += 2;
            }
        }

        if (len != can_recv_len_two) {
            vdata_list_int();
            return;
        }

        datasysmsg.calculate++;
    }

}

//处理光电信息
void vcan_bus_deal_with_photo_cmd(u8* buff)
{
    u16 i = 0;
    u16 cnt = 0;
    u8  rstflag = 0;
    u8  plcflag = 0;

    cnt = (buff[0] | (buff[1] << 8));
    rstflag = buff[2];
    plcflag = buff[3];
    
    plcflag = 1;

    if ((datasysmsg.curposition == cnt) && (cnt != 0)) return;      
    if (rstflag == 0) {
        if ((((cnt == datasysmsg.curposition + 1) && (cnt <= datapcconfig.car_num)) ||
            ((cnt == 1) && (datasysmsg.curposition == datapcconfig.car_num))) &&
            (cnt != 0)) {
            datasysmsg.runflag++;
            datasysmsg.curposition = cnt;
            datasysmsg.calculate++;
        }
        else {
               //每次重新开线，第一次复位时， 清空小车报警状态 
                if (cnt == 0) {
                datasysmsg.curposition = datapcconfig.car_num;
                datasyserr.photoerr = 0;
                Data_Init_motor_para_queue();
                for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {
                    motoonepara[i].state = SNED_CMD_NONE;
                    moto_one_err[i] = 0;
                }
                for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {
                    motothreepara[i].state = SNED_CMD_NONE;
                    moto_three_err[i] = 0;
                }
                for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
                    mototwopara[i].state = SNED_CMD_NONE;
                    moto_two_err[i] = 0;
                }
                return;
            }

            if (plcflag == 0) {
                if (cnt > datasysmsg.curposition) {
                    datasyserr.photoerr += (cnt - datasysmsg.curposition - 1);
                }
                else if ((cnt + datapcconfig.car_num) > datasysmsg.curposition) {
                    datasyserr.photoerr += (cnt + datapcconfig.car_num - datasysmsg.curposition - 1);
                }
            }

            datasysmsg.curposition = cnt;

            // 发生光电计数错误  该次不分拣    
            for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {
                motothreepara[i].state = SNED_CMD_NONE;
            }
            for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {
                motoonepara[i].state = SNED_CMD_NONE;
            }
            for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
                mototwopara[i].state = SNED_CMD_NONE;
            }

        }
    }
    else {
        datasysmsg.runflag++;
        datasysmsg.curposition = cnt;
        datasysmsg.calculate++;

        for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {
            motoonepara[i].state = SNED_CMD_NONE;
            moto_one_err[i] = 0;
        }
        for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {
            motothreepara[i].state = SNED_CMD_NONE;
            moto_three_err[i] = 0;
        }
        for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
            mototwopara[i].state = SNED_CMD_NONE;
            moto_two_err[i] = 0;
        }
    }

}

//处理允许分拣命令
void vcan_bus_recv_license_cmd(u8* buff)
{
    u8 flag = 0;

    flag = buff[0];

    if (flag == 0) {
        datasysmsg.load_forbid = LICENSE_NO;
    }
    else if (flag == 1) {
        datasysmsg.load_forbid = LICENSE_OK;
    }
    datasyserr.photoerr = 0;
    datasyserr.lostnum = 0;
}

//处理配置信息
void vcan_bus_recv_sysconfig_cmd(u8* buff)
{
    u16 tmp = 0;
    datapcconfig.car_num = buff[0] | buff[1] << 8;
    datapcconfig.mainlinespd = buff[2] | buff[3] << 8;
    datapcconfig.car_mode = buff[4];
    if (buff[5] == 1) {
        datapcconfig.drive_type = DRIVER_TYPE_ONE;
    }
    else if (buff[5] == 2) {
        datapcconfig.drive_type = DRIVER_TYPE_TWO;
    }
    else if (buff[5] == 3) {
        datapcconfig.drive_type = DRIVER_TYPE_THREE;
    }
    datapcconfig.onegroupcarnum = buff[6];


    tmp = datapcconfig.car_num / datapcconfig.onegroupcarnum + 2;
    if (tmp == key_value) {
        lastslaver_station = 1;
    }
    else {
        lastslaver_station = 0;
    }
}

//接收强排口信息
void vcan_bus_recv_fixed_msg(void)
{
    u16 i = 0;

    datafixedqueue.num = can_recv_config_msg[0];
    if (can_recv_config_msg[0] >= 4) return;
    for (i = 0; i < can_recv_config_msg[0]; i++) {
        datafixedqueue.fixedmsg[i].fixed_position = (can_recv_config_msg[8 * i + 1]) | (can_recv_config_msg[8 * i + 2] << 8);
        datafixedqueue.fixedmsg[i].fixed_dir = can_recv_config_msg[8 * i + 3];
        datafixedqueue.fixedmsg[i].fixed_spd = can_recv_config_msg[8 * i + 4];
        datafixedqueue.fixedmsg[i].fixed_len = (can_recv_config_msg[8 * i + 5]) | (can_recv_config_msg[8 * i + 6] << 8);
        datafixedqueue.fixedmsg[i].fixed_delay = (can_recv_config_msg[8 * i + 7]) | (can_recv_config_msg[8 * i + 8] << 8);
    }
}




//测试小车
void vcan_bus_recv_car_test_msg(u8* buff)
{
    moto_para_struct x;
    u16  carnum;
    u16  localcarmin;
    u16  localcarmax;
    u8   carindex;
    u8   groupindex;
    u16  i = 0;

    carnum = buff[0] | buff[1] << 8;
    localcarmin = (key_value - 2) * datapcconfig.onegroupcarnum + 1;
    localcarmax = (key_value - 1) * datapcconfig.onegroupcarnum;

    if ((carnum < localcarmin) || (carnum > localcarmax)) return;

    if ((carnum % datapcconfig.onegroupcarnum) == 0) {
        carindex = datapcconfig.onegroupcarnum;
    }
    else {
        carindex = carnum % datapcconfig.onegroupcarnum;
    }

    if (carindex > (datapcconfig.onegroupcarnum / 2)) {
        if ((carindex % (datapcconfig.onegroupcarnum / 2)) == 0) {
            carindex = datapcconfig.onegroupcarnum / 2;
        }
        else {
            carindex = (carindex % (datapcconfig.onegroupcarnum / 2));
        }
        groupindex = 2;
    }
    else {
        if ((carindex % (datapcconfig.onegroupcarnum / 2)) == 0) {
            carindex = datapcconfig.onegroupcarnum / 2;
        }
        else {
            carindex = (carindex % (datapcconfig.onegroupcarnum / 2));
        }

        groupindex = 1;
    }

    x.num = carindex;
    x.dir = buff[2];
    if ((buff[3]) > 127) {
        x.speed = 127;
    }
    else {
        x.speed = buff[3];
    }
    if (((buff[4] | (buff[5] << 8))) > 127) {
        x.juli = 127;
    }
    else {
        x.juli = ((buff[4] | (buff[5] << 8)));
    }
    x.dely = 0;

    if (groupindex == 1) {
        Data_add_to_moto_para_queue(&motooneprarqueue[carindex - 1], x);
        for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {
            motoonepara[i].state = SNED_CMD_NONE;
            motoonepara[i].chutdown = 0;
            motoonepara[i].waitrun = 0;
        }
        usart1_moto_ctr_statue = WAIT_RECV_RUN;
        motoonepara[carindex - 1].chutdown = 100;
    }

    if (groupindex == 2) {
        Data_add_to_moto_para_queue(&motothreeprarqueue[carindex - 1], x);
        for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {
            motothreepara[i].state = SNED_CMD_NONE;
            motothreepara[i].chutdown = 0;
            motothreepara[i].waitrun = 0;
        }
        uart4_moto_ctr_statue = WAIT_RECV_RUN;
        motothreepara[carindex - 1].chutdown = 100;
    }

}

// NC交叉带复位信息
void vcan_bus_recv_carreset_config_msg(u8* buff)
{
    
    car_resetcal = buff[0] | (buff[1] << 8);
}

//NC交叉带寻零信息
void vcan_bus_recv_car_findzero_msg(u8* buff, u16 len)
{
    moto_para_struct x;
    u8   carnum = 0;
    u16  localcarmin = 0;
    u16  localcarmax;
    u8   carindex =0;
    u8   groupindex= 0;
    u16  i = 0;
    u16  car = 0;

    carnum = buff[0];
    localcarmin = (key_value - 2) * datapcconfig.onegroupcarnum + 1;
    localcarmax = (key_value - 1) * datapcconfig.onegroupcarnum;

    for (i = 0; i < carnum; i++) {
        car = (buff[2 * i + 1]) | (buff[2 * i + 1 + 1] << 8);
        if ((car < localcarmin) || (car > localcarmax)) continue;

        if ((car % datapcconfig.onegroupcarnum) == 0) {
            carindex = datapcconfig.onegroupcarnum;
        }
        else {
            carindex = car % datapcconfig.onegroupcarnum;
        }

        if (carindex > (datapcconfig.onegroupcarnum / 2)) {
            if ((carindex % (datapcconfig.onegroupcarnum / 2)) == 0) {
                carindex = datapcconfig.onegroupcarnum / 2;
            }
            else {
                carindex = (carindex % (datapcconfig.onegroupcarnum / 2));
            }
            groupindex = 2;
        }
        else {
            if ((carindex % (datapcconfig.onegroupcarnum / 2)) == 0) {
                carindex = datapcconfig.onegroupcarnum / 2;
            }
            else {
                carindex = (carindex % (datapcconfig.onegroupcarnum / 2));
            }

            groupindex = 1;
        }


        x.num = carindex;
        x.dir = 1;
        x.speed = 100;
        x.juli = 100;
        x.dely = 0;
        x.zeroinit = VALUE;

        if (groupindex == 1) {
            Data_add_to_moto_para_queue(&motooneprarqueue[carindex - 1], x);
            motoonepara[carindex - 1].state = SNED_CMD_NONE;
            motoonepara[carindex - 1].chutdown = 0;
            motoonepara[carindex - 1].waitrun = 0;
            motoonepara[carindex - 1].cartestcnt = 0;
            motoonepara[carindex - 1].carresetcnt = 0;
        }

        if (groupindex == 2) {
            Data_add_to_moto_para_queue(&motothreeprarqueue[carindex - 1], x);
            motothreepara[carindex - 1].state = SNED_CMD_NONE;
            motothreepara[carindex - 1].chutdown = 0;
            motothreepara[carindex - 1].waitrun = 0;
            motothreepara[carindex - 1].cartestcnt = 0;
            motothreepara[carindex - 1].carresetcnt = 0;
        }

    }
  
}

// 接收从板卡光电配置信息
void canbus_recv_slaver_photo_conf(u8* buff)
{
    u16 num = 0;
    u16 i =0;

    photovalue = INVALUE;
    num = buff[1];
    if (num > 5) {
        return;
    }
    for (i = 0; i < num; i++) {
        if (buff[i + 2] == key_value) {
            photovalue = VALUE;
        }
    }
}

//接收一帧之内的数据
void canbus_recv_all_short_msg(u8* buff, u16 length)
{
    u8 type = buff[0];

    switch(type){
        case CAN_FUNC_ID_FORBID_TYPE:
            vcan_bus_recv_license_cmd(&(buff[1]));
        break;
        case CAN_FUNC_ID_CONFIG_TYPE:
            vcan_bus_recv_sysconfig_cmd(&(buff[1]));
            photovalue = INVALUE;
        break;
        case CAN_FUNC_ID_REPAIRE_TYPE:
            vcan_bus_recv_car_test_msg(&(buff[1]));
        break;
        case CAN_FUNC_ID_SLAVER_CAR_RESET_CONFIG:
            vcan_bus_recv_carreset_config_msg(&(buff[1]));
        break;
        case CAN_FUNC_ID_SLAVER_PHOTO_CONF:
             canbus_recv_slaver_photo_conf(&(buff[1]));
        break;
        default:
        break;
    }
}


//can2接收数据
void vcanbus_frame_receive_two(CanRxMsg rxMsg)
{
    sCanFrameExtID extID;
    sCanFrameExt   frame;
    u8 send_len = 0;
    char str[25];

    extID.seg_polo = (rxMsg.ExtId >> 20) & 0x3;
    extID.seg_num = (rxMsg.ExtId >> 12) & 0xFF;
    extID.func_id = (rxMsg.ExtId >> 7) & 0x1F;
    extID.mac_id = rxMsg.ExtId & 0x7F;
    extID.dst_id = rxMsg.ExtId >> 22 & 0x7F;

    frame.extId = extID;
    frame.data_len = rxMsg.DLC;

    memcpy(frame.data, rxMsg.Data, rxMsg.DLC);

    
   if (frame.extId.func_id == 2) {

        if (key_value == 1) {
            vcan2_reply_check_slaver();
        }
        else {
            send_len = sprintf(str, "CAN2:NORMAL:%d\n", rxMsg.Data[0]);
            DEBUG_process((u8*)str, send_len);
        }
   } 




}



void vcan_send_finish(u8 func_id)
{


    if (func_id == CAN_FUNC_ID_UNLOAD_TYPE)
    {
    }
}

//can2 将将要发送的数据加入队列
void vcan_sendmsg_two(u8* buff, u8 send_total_len, u8 type, u8 dst)
{
    sCanFrameExt canTxMsg;
    u16 send_len = 0;

    canTxMsg.extId.func_id = type;
    canTxMsg.extId.mac_id = key_value;
    canTxMsg.extId.dst_id = dst;

    if (send_total_len <= CAN_PACK_DATA_LEN)//不分段发送
    {
        canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
        canTxMsg.extId.seg_num = 0;
        canTxMsg.data_len = send_total_len;
        memcpy(canTxMsg.data, buff, canTxMsg.data_len);
        vcanbus_addto_cansendqueue_two(canTxMsg);
    }
    else//分段发送    
    {
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

// 发送队列数据
void vcan_send_frame_process(void)
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
    tx_mailbox = u8canbus_send_oneframe_two(*canTxMsg);
    if (tx_mailbox != CAN_NO_MB)
    {
        q->front = front;
    }
}

//更新心跳信息
void vcanbus_upload_heart_msg(void)
{
    u8 len = 0;
    u8 buff[10] = { 0 };
    
    datasyserr.lostnum = 0;
    datasyserr.photoerr = 0;

    buff[0] = datasyserr.photoerr & 0xFF;
    buff[1] = (datasyserr.photoerr >> 8) & 0xFF;
    buff[2] = datasyserr.lostnum & 0xFF;
    buff[3] = (datasyserr.lostnum >> 8) & 0xFF;
    buff[4] = datasyserr.unload_last_index & 0xFF;
    buff[5] = (datasyserr.unload_last_index >> 8) & 0xFF;
    buff[6] = (datasyserr.unload_last_index >> 16) & 0xFF;
    buff[7] = (datasyserr.unload_last_index >> 24) & 0xFF;

    len = 8;

    vcan_sendmsg_two(buff, len, CAN_FUNC_ID_HEART_TYPE, 1);

    datasyserr.photoerr = 0;
    datasyserr.lostnum = 0;
}

//更新电滚筒错误信息
void vcanbus_upload_motor_err_msg(void)
{
    u8 len = 0;
    u8 buff[200] = { 0 };
    u16 i = 0;
    u16 j = 0;
    u16 offset = 0;

    if(key_value < 2){
        return;
    }

    offset = (key_value -2)*datapcconfig.onegroupcarnum;

    if (datapcconfig.drive_type == DRIVER_TYPE_ONE) {

        for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {
            if (moto_one_err[i] != 0) {
                buff[1 + j * 3] = (motoonepara[i].num + offset) & 0xFF;
                buff[2 + j * 3] = ((motoonepara[i].num + offset) >> 8) & 0xFF;
                buff[3 + j * 3] = moto_one_err[i];
                j++;
                if(j>=2){               
                    buff[0] = j;
                    len = 1 + 3 * j;
                    vcan_sendmsg_two(buff, len, CAN_FUNC_ID_MOTO_ERR_TYPE, 1);
                    j = 0;
                }
            }
        }

        for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {
            if (moto_three_err[i] != 0) {
                buff[1 + j * 3] = (motothreepara[i].num + offset + datapcconfig.onegroupcarnum / 2) & 0xFF;
                buff[2 + j * 3] = ((motothreepara[i].num + offset + datapcconfig.onegroupcarnum / 2) >> 8) & 0xFF;
                buff[3 + j * 3] = moto_three_err[i];
                j++;

                if(j>=2){               
                    buff[0] = j;
                    len = 1 + 3 * j;
                    vcan_sendmsg_two(buff, len, CAN_FUNC_ID_MOTO_ERR_TYPE, 1);
                    j = 0;
                }

            }
        }
    }

    if (datapcconfig.drive_type == DRIVER_TYPE_TWO) {


        for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {
            if (motoone_typetwo_err[i] != 0) {
                buff[1 + j * 3] = (motoonepara[i].num + offset) & 0xFF;
                buff[2 + j * 3] = ((motoonepara[i].num + offset) >> 8) & 0xFF;
                buff[3 + j * 3] = motoone_typetwo_err[i];
                j++;
                if(j>=2){               
                    buff[0] = j;
                    len = 1 + 3 * j;
                    vcan_sendmsg_two(buff, len, CAN_FUNC_ID_MOTO_ERR_TYPE, 1);
                    j = 0;
                }
            }
        }

        for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {
            if (motothree_typetwo_err[i] != 0) {
                buff[1 + j * 3] = (motothreepara[i].num + offset + datapcconfig.onegroupcarnum / 2) & 0xFF;
                buff[2 + j * 3] = ((motothreepara[i].num + offset + datapcconfig.onegroupcarnum / 2) >> 8) & 0xFF;
                buff[3 + j * 3] = motothree_typetwo_err[i];
                j++;

                if(j>=2){               
                    buff[0] = j;
                    len = 1 + 3 * j;
                    vcan_sendmsg_two(buff, len, CAN_FUNC_ID_MOTO_ERR_TYPE, 1);
                    j = 0;
                }

            }
        }
    }

    buff[0] = j;
    len = 1 + 3 * j;

    if ((buff[0] !=0) && (buff[0] <= 2)) {
        vcan_sendmsg_two(buff, len, CAN_FUNC_ID_MOTO_ERR_TYPE, 1);
    }
}

//更新未动作小车的信息
void vcanbus_upload_motor_run_abnorable_msg(void)
{
    u8 len = 0;
    u8 buff[200] = { 0 };
    u16 i = 0;
    u16 j = 0;
    u16 offset = 0;

    if(key_value < 2){
        return;
    }

    offset = (key_value -2)*datapcconfig.onegroupcarnum;


    for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {
        if (motoonepara[i].runerr != NOERR_RUN) {
            buff[1 + j * 3] = (motoonepara[i].num + offset) & 0xFF;
            buff[2 + j * 3] = ((motoonepara[i].num + offset) >> 8) & 0xFF;
            buff[3 + j * 3] = motoonepara[i].runerr;
            motoonepara[i].runerr = NOERR_RUN;
            j++;

            if(j>=2){               
                buff[0] = j;
                len = 1 + 3 * j;
                vcan_sendmsg_two(buff, len, CAN_FUNC_ID_MOTO_RUN_STATE_TYPE, 1);
                j = 0;
            }
        }
    }

    for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {

        if (motothreepara[i].runerr != NOERR_RUN) {
            buff[1 + j * 3] = (motothreepara[i].num + offset+ datapcconfig.onegroupcarnum / 2) & 0xFF;
            buff[2 + j * 3] = ((motothreepara[i].num + offset+ datapcconfig.onegroupcarnum / 2) >> 8) & 0xFF;
            buff[3 + j * 3] = motothreepara[i].runerr;
            motothreepara[i].runerr = NOERR_RUN;
            j++;

            if(j>=2){               
                buff[0] = j;
                len = 1 + 3 * j;
                vcan_sendmsg_two(buff, len, CAN_FUNC_ID_MOTO_RUN_STATE_TYPE, 1);
                j = 0;
            }
        }
    }

    buff[0] = j;
    len = 1 + 2 * j;

    if ((buff[0] != 0) && (buff[2] <=2)) {
        vcan_sendmsg_two(buff, len, CAN_FUNC_ID_MOTO_RUN_STATE_TYPE, 1);
    }
}


//从板卡丢失配置信息，异常心跳
void vcanbus_upload_err_heart_process(void)
{
    u8 len = 0;
    u8 buff[10] = { 0 };

    if (datapcconfig.car_num == 0) {
        len = 0;
        vcan_sendmsg_two(buff, len, CAN_FUNC_ID_ERR_HEART_TYPE, 1);
    }

}

void vcan2_send_check_slaver(void)
{
    u8  buff[10] = { 0 };
    u16 sendlen = 0;

    buff[0] = 2;
    buff[1] = 2;
    buff[2] = 2;
    buff[3] = 2;
    buff[4] = 2;
    buff[5] = 2;
    buff[6] = 2;
    buff[7] = 2;

    sendlen = 8;

    vcan_sendmsg_two(buff, sendlen, 2, 1);
}

void vcan2_reply_check_slaver(void)
{
    u8  buff[10] = { 0 };
    u8 sendlen = 0;

    buff[0] = 2;
    buff[1] = 2;
    buff[2] = 2;
    buff[3] = 2;
    buff[4] = 2;
    buff[5] = 2;
    buff[6] = 2;
    buff[7] = 2;

    sendlen = 8;

    vcan_sendmsg_two(buff, sendlen, 2, 1);
}
 
 

