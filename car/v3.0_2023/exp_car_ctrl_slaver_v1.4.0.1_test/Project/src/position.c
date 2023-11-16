#include "main.h"

// cal car position
u16 u16position_cal_target_position(u16 cur, u16 goal)
{
    u16 index = 0;
    u16 goal_position = (cur - (goal - 1) + datapcconfig.car_num) % datapcconfig.car_num;

    if (goal_position == 0) {
        index = datapcconfig.car_num;
    }
    else {
        index = goal_position;
    }

    return index;
}

// cal car index
u16 u16position_cal_target_car(u16 cur, u16 position)
{
    u16 index = 0;
    u16 goal_position = ((cur - position) + datapcconfig.car_num) % datapcconfig.car_num + 2;
    goal_position = goal_position % datapcconfig.car_num;

    if (goal_position == 0) {
        index = datapcconfig.car_num;
    }
    else {
        index = goal_position;
    }

    return index;
}

void vposition_remove_repeat_carnum(u16 x)
{
    List_t* q = NULL;
    sData_pc2bd_unload_node* unloadnode = NULL;
    sData_pc2bd_load_node* loadnode = NULL;

    u16 enditem = 0;

    q = &list_unload;
    enditem = q->xListEnd.xItemValue;

    for (q->pxIndex = (ListItem_t*)q->xListEnd.pxNext; q->pxIndex->xItemValue != enditem; q->pxIndex = q->pxIndex->pxNext) {

        unloadnode = q->pxIndex->pvOwner;
        if (unloadnode->car_unload_index == x) {
            uxListRemove(unloadnode->index);
        }

    }

    q = &list_load;

    enditem = q->xListEnd.xItemValue;

    for (q->pxIndex = (ListItem_t*)q->xListEnd.pxNext; q->pxIndex->xItemValue != enditem; q->pxIndex = q->pxIndex->pxNext) {
         loadnode = q->pxIndex->pvOwner;
         if (loadnode->car_load_index == x) {
             uxListRemove(loadnode->index);
         }
    }
}


void vposition_dealwith_unload_process(void)
{

    List_t* q = NULL;
    sData_pc2bd_unload_node* node = NULL;
    u16 enditem = 0;
    u16 curposition = 0;
    u16 temp = 0;
    u8  closestate = 0;
    u16 leaderposition = 0;
    u8  carindex = 0;
    u8  groupindex = 1;
    u8  doubleflag = 0;
    
    moto_para_struct x;

    q = &list_unload;
    enditem = q->xListEnd.xItemValue;
    
    leaderposition = datasysmsg.curposition;
    if (datasysmsg.curposition == 0) {
        leaderposition = datapcconfig.car_num;
    }

    for (q->pxIndex = (ListItem_t*)q->xListEnd.pxNext; q->pxIndex->xItemValue != enditem; q->pxIndex = q->pxIndex->pxNext) {

        node = q->pxIndex->pvOwner;
        doubleflag = 0;

        doubleflag = (node->car_unload_index >> 15) & 0x1;

        curposition = u16position_cal_target_position(leaderposition, node->car_unload_index);
        
        
        closestate = u8data_check_io_close_state(node->car_chute_index);
        if (closestate == IO_CLOSE) {

            uxListRemove(node->index);
            continue;
        }
        if (node->car_rotate_length == 0) {
            uxListRemove(node->index);
            continue;
        }

        if ((node->car_unload_index % datapcconfig.onegroupcarnum) == 0) {
            carindex = datapcconfig.onegroupcarnum;
        }
        else {
            carindex = node->car_unload_index % datapcconfig.onegroupcarnum;
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

        //IOB
        if (node->car_unload_exitno == 0) {

            x.num = carindex;
            x.chutdown = 0;
            x.dely = 0;

            if (datapcconfig.drive_type == DRIVER_TYPE_THREE) {
                x.juli = (node->car_rotate_length) & 0x7F;
                x.juliextern = (node->car_rotate_length >> 7) & 0x0007;
            }
            else {
                if ((node->car_rotate_length) > 127) {
                    x.juli = 127;
                }
                else {
                    x.juli = node->car_rotate_length;
                }

                x.juliextern = 0;
            }

            x.dir = node->car_unload_dir;
            if (node->car_exit_speed > 127) {
                x.speed = 127;
            }
            else {
                x.speed = node->car_exit_speed;
            }

//            x.carresetcnt = car_resetcal;

            if (datapcconfig.car_mode == ONE_CAR_ONE_TIME) {
                if (groupindex == 1) {
                    Data_add_to_moto_para_queue(&motooneprarqueue[carindex - 1], x);
                    vusart1_moto_para_ready(carindex - 1);
                    if (doubleflag == 1) {
                        x.num = carindex + 1;
                        Data_add_to_moto_para_queue(&motooneprarqueue[carindex], x);
                        vusart1_moto_para_ready(carindex);
                    }
                }
                if (groupindex == 2) {
                    Data_add_to_moto_para_queue(&motothreeprarqueue[carindex - 1], x);
                    vuart4_moto_para_ready(carindex - 1);
                    if (doubleflag == 1) {
                        x.num = carindex + 1;
                        Data_add_to_moto_para_queue(&motothreeprarqueue[carindex], x);
                        vuart4_moto_para_ready(carindex);
                    }
                }
            }

            if (datapcconfig.car_mode == TWO_CAR_ONE_TIME) {

                if (groupindex == 1) {
                    Data_add_to_moto_para_queue(&motooneprarqueue[carindex - 1], x);
                    vusart1_moto_para_ready(carindex - 1);
                    //if (doubleflag == 1) {
                    //    Data_add_to_moto_para_queue(&mototwoprarqueue[carindex], x);
                    //    vusart2_moto_para_ready(carindex);
                    //}
                }
                if (groupindex == 2) {
                    Data_add_to_moto_para_queue(&motothreeprarqueue[carindex - 1], x);
                    vuart4_moto_para_ready(carindex - 1);
                    //if (doubleflag == 1) {
                    //    Data_add_to_moto_para_queue(&motooneprarqueue[carindex], x);
                    //    vusart1_moto_para_ready(carindex);
                    //}

                }

                x.num = carindex + 1;

                if (groupindex == 1) {
                    Data_add_to_moto_para_queue(&motooneprarqueue[carindex], x);
                    vusart1_moto_para_ready(carindex);
                }
                if (groupindex == 2) {
                    Data_add_to_moto_para_queue(&motothreeprarqueue[carindex], x);
                    vuart4_moto_para_ready(carindex);
                }
            }

            uxListRemove(node->index);

            continue;
        }

        if (datapcconfig.mainlinespd >= 3500) {
            temp = curposition + 4;
        }
        else if ((datapcconfig.mainlinespd >= 2500)  && (datapcconfig.mainlinespd < 3500)){
            temp = curposition + 3;
        }
        else {
            temp = curposition + 2;
        }


        if (temp > datapcconfig.car_num) {
            temp = temp - datapcconfig.car_num;
        }

//        if (temp > node->car_unload_exitno) {
//
//            uxListRemove(node->index);
//            continue;
//        }

        if (temp == (node->car_unload_exitno)) {

            closestate = u8data_check_io_close_state(node->car_chute_index);
            if (closestate == IO_CLOSE) {

                uxListRemove(node->index);
                continue;
            }

            x.num = carindex;
            x.chutdown = 0;
            if ((node->car_unload_delay / 10) > 255) {
                x.dely = 255;
            }
            else {
                x.dely = (node->car_unload_delay / 10);
            }

            if (datapcconfig.drive_type == DRIVER_TYPE_THREE) {
                x.juli = (node->car_rotate_length) & 0x7F;
                x.juliextern = (node->car_rotate_length >> 7) & 0x0007;
            }
            else {
                if ((node->car_rotate_length) > 127) {
                    x.juli = 127;
                }
                else {
                    x.juli = node->car_rotate_length;
                }

                x.juliextern = 0;
            }

            x.dir = node->car_unload_dir;
            if ((node->car_exit_speed) > 127) {
                x.speed = 127;
            }
            else {
                x.speed = node->car_exit_speed;
            }
            x.carresetcnt = car_resetcal;


            if (datapcconfig.car_mode == ONE_CAR_ONE_TIME) {
                if (groupindex == 1) {
                    Data_add_to_moto_para_queue(&motooneprarqueue[carindex - 1], x);
                    vusart1_moto_para_ready(carindex - 1);
                    motoonepara[carindex - 1].chuteindex = node->car_chute_index;
                    if (datapcconfig.mainlinespd >= 3500) {
                        motoonepara[carindex - 1].runcnt = 4;
                    }
                    else if ((datapcconfig.mainlinespd >= 2500) && (datapcconfig.mainlinespd < 3500)) {
                        motoonepara[carindex - 1].runcnt = 3;
                    }
                    else {
                        motoonepara[carindex - 1].runcnt = 2;
                    }
                    if (doubleflag == 1) {
                        x.num = carindex + 1;
                        Data_add_to_moto_para_queue(&motooneprarqueue[carindex], x);
                        vusart1_moto_para_ready(carindex);
                        motoonepara[carindex].chuteindex = node->car_chute_index;
                        if (datapcconfig.mainlinespd >= 3500) {
                            motoonepara[carindex].runcnt = 4;
                        }
                        else if ((datapcconfig.mainlinespd >= 2500) && (datapcconfig.mainlinespd < 3500)) {
                            motoonepara[carindex].runcnt = 3;
                        }
                        else {
                            motoonepara[carindex].runcnt = 2;
                        }
                    }
                }
                if (groupindex == 2) {
                    Data_add_to_moto_para_queue(&motothreeprarqueue[carindex - 1], x);
                    vuart4_moto_para_ready(carindex - 1);
                    motothreepara[carindex - 1].chuteindex = node->car_chute_index;

                    if (datapcconfig.mainlinespd >= 3500) {
                        motothreepara[carindex - 1].runcnt = 4;
                    }
                    else if ((datapcconfig.mainlinespd >= 2500) && (datapcconfig.mainlinespd < 3500)) {
                        motothreepara[carindex - 1].runcnt = 3;
                    }
                    else {
                        motothreepara[carindex - 1].runcnt = 2;
                    }

                    if (doubleflag == 1) {
                        x.num = carindex + 1;
                        Data_add_to_moto_para_queue(&motothreeprarqueue[carindex], x);
                        vuart4_moto_para_ready(carindex);
                        motothreepara[carindex].chuteindex = node->car_chute_index;

                        if (datapcconfig.mainlinespd >= 3500) {
                            motothreepara[carindex].runcnt = 4;
                        }
                        else if ((datapcconfig.mainlinespd >= 2500) && (datapcconfig.mainlinespd < 3500)) {
                            motothreepara[carindex].runcnt = 3;
                        }
                        else {
                            motothreepara[carindex].runcnt = 2;
                        }
                    }
                }
            }

            if (datapcconfig.car_mode == TWO_CAR_ONE_TIME) {

                if (groupindex == 1) {
                    Data_add_to_moto_para_queue(&motooneprarqueue[carindex - 1], x);
                    vusart1_moto_para_ready(carindex - 1);
                    motoonepara[carindex - 1].chuteindex = node->car_chute_index;
                    if (datapcconfig.mainlinespd >= 3500) {
                        motoonepara[carindex - 1].runcnt = 4;
                    }
                    else if ((datapcconfig.mainlinespd >= 2500) && (datapcconfig.mainlinespd < 3500)) {
                        motoonepara[carindex - 1].runcnt = 3;
                    }
                    else {
                        motoonepara[carindex - 1].runcnt = 2;
                    }
                }
                if (groupindex == 2) {
                    Data_add_to_moto_para_queue(&motothreeprarqueue[carindex - 1], x);
                    vuart4_moto_para_ready(carindex - 1);
                    motothreepara[carindex - 1].chuteindex = node->car_chute_index;
                    if (datapcconfig.mainlinespd >= 3500) {
                        motothreepara[carindex - 1].runcnt = 4;
                    }
                    else if ((datapcconfig.mainlinespd >= 2500) && (datapcconfig.mainlinespd < 3500)) {
                        motothreepara[carindex - 1].runcnt = 3;
                    }
                    else {
                        motothreepara[carindex - 1].runcnt = 2;
                    }
                }

                x.num = carindex + 1;

                if (groupindex == 1) {
                    Data_add_to_moto_para_queue(&motooneprarqueue[carindex], x);
                    vusart1_moto_para_ready(carindex );
                    motoonepara[carindex].chuteindex = node->car_chute_index;
                    if (datapcconfig.mainlinespd >= 3500) {
                        motoonepara[carindex].runcnt = 4;
                    }
                    else if ((datapcconfig.mainlinespd >= 2500) && (datapcconfig.mainlinespd < 3500)) {
                        motoonepara[carindex].runcnt = 3;
                    }
                    else {
                        motoonepara[carindex].runcnt = 2;
                    }
                }
                if (groupindex == 2) {
                    Data_add_to_moto_para_queue(&motothreeprarqueue[carindex], x);
                    vuart4_moto_para_ready(carindex);
                    motothreepara[carindex].chuteindex = node->car_chute_index;
                    if (datapcconfig.mainlinespd >= 3500) {
                        motothreepara[carindex].runcnt = 4;
                    }
                    else if ((datapcconfig.mainlinespd >= 2500) && (datapcconfig.mainlinespd < 3500)) {
                        motothreepara[carindex].runcnt = 3;
                    }
                    else {
                        motothreepara[carindex].runcnt = 2;
                    }
                }
            }

            uxListRemove(node->index);
            continue;
        }

    }
}


void vphoto_deal_with_load_data_process(void)
{
    List_t* q = NULL;
    sData_pc2bd_load_node* node = NULL;
    u16 enditem = 0;
    u16 curposition = 0;
    u16 temp = 0;
    u16 leaderposition = 0;
    u8  carindex = 0;
    u8  groupindex = 1;
    u8  doubleflag = 0;

    moto_para_struct x;

    q = &list_load;

    leaderposition = datasysmsg.curposition;
    if (datasysmsg.curposition == 0)  leaderposition = datapcconfig.car_num;

    enditem = q->xListEnd.xItemValue;


    for (q->pxIndex = (ListItem_t*)q->xListEnd.pxNext; q->pxIndex->xItemValue != enditem; q->pxIndex = q->pxIndex->pxNext) {
        node = q->pxIndex->pvOwner;
        doubleflag = 0;
        doubleflag = (node->car_load_index >> 15) & 0x1;

        curposition = u16position_cal_target_position(leaderposition, node->car_load_index);

        if ((node->car_load_index % datapcconfig.onegroupcarnum) == 0) {
            carindex = datapcconfig.onegroupcarnum;
        }
        else {
            carindex = node->car_load_index % datapcconfig.onegroupcarnum;
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

        temp = curposition + 1;

        if (temp > datapcconfig.car_num) {
            temp = temp - datapcconfig.car_num;
        }
//
//        if (temp > node->car_load_position) {
//
//            uxListRemove(node->index);
//            continue;
//        }

        if (temp == (node->car_load_position)) {

            x.num = carindex;
            x.chutdown = 0;
            x.dely = node->car_load_delay;
            if ((node->car_load_delay / 10) > 255) {
                x.dely = 255;
            }
            else{
                x.dely = node->car_load_delay / 10;
            }


            if (datapcconfig.drive_type == DRIVER_TYPE_THREE) {
                x.juli = (node->car_load_length) & 0x7F;
                x.juliextern = (node->car_load_length >> 7) & 0x0007;
            }
            else {
                if ((node->car_load_length) > 127) {
                    x.juli = 127;
                }
                else {
                    x.juli = node->car_load_length;
                }
                x.juliextern = 0;
            }


            x.dir = node->car_load_dir;
            if (node->car_load_speed > 127) {
                x.speed = 127;
            }
            else {
                x.speed = node->car_load_speed;
            }

            x.carresetcnt = 0;

            if (datapcconfig.car_mode == ONE_CAR_ONE_TIME) {
                if (groupindex == 1) {
                    Data_add_to_moto_para_queue(&motooneprarqueue[carindex - 1], x);
                    vusart1_moto_para_ready(carindex - 1);
                    if (doubleflag == 1) {
                        x.num = carindex + 1;
                        Data_add_to_moto_para_queue(&motooneprarqueue[carindex], x);
                        vusart1_moto_para_ready(carindex);
                    }
                }
                if (groupindex == 2) {
                    Data_add_to_moto_para_queue(&motothreeprarqueue[carindex - 1], x);
                    vuart4_moto_para_ready(carindex - 1);
                    if (doubleflag == 1) {
                        x.num = carindex + 1;
                        Data_add_to_moto_para_queue(&motothreeprarqueue[carindex], x);
                        vuart4_moto_para_ready(carindex);
                    }
                }
            }

            if (datapcconfig.car_mode == TWO_CAR_ONE_TIME) {

                if (groupindex == 1) {
                    Data_add_to_moto_para_queue(&motooneprarqueue[carindex - 1], x);
                    vusart1_moto_para_ready(carindex - 1);
                    motoonepara[carindex -1].state = SNED_CMD_NONE;
                }
                if (groupindex == 2) {
                    Data_add_to_moto_para_queue(&motothreeprarqueue[carindex - 1], x);
                    vuart4_moto_para_ready(carindex - 1);
                }

                x.num = carindex + 1;

                if (groupindex == 1) {
                    Data_add_to_moto_para_queue(&motooneprarqueue[carindex], x);
                    vusart1_moto_para_ready(carindex);;
                }
                if (groupindex == 2) {
                    Data_add_to_moto_para_queue(&motothreeprarqueue[carindex], x);
                    vuart4_moto_para_ready(carindex);
                }
            }

            uxListRemove(node->index);
            continue;
        }
    }
}

void vposition_dealwith_fixed_data_process(void)
{
    u16 i = 0;
    u16  carnum = 0;
    u16  localcarmin = 0;
    u16  localcarmax = 0;
    u8   carindex = 0;
    u8   groupindex = 0;
    u16  leaderposition = 0;
    moto_para_struct x;

    if (datafixedqueue.num == 0)  return;
    
    leaderposition = datasysmsg.curposition;
    if (datasysmsg.curposition == 0)  leaderposition = datapcconfig.car_num;

    for (i = 0; i < datafixedqueue.num; i++) {
        carnum = u16position_cal_target_car(leaderposition, datafixedqueue.fixedmsg[i].fixed_position);

        localcarmin = (key_value - 2) * datapcconfig.onegroupcarnum + 1;
        localcarmax = (key_value - 1) * datapcconfig.onegroupcarnum;

        if ((carnum < localcarmin) || (carnum > localcarmax)) continue;

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
        x.dir = datafixedqueue.fixedmsg[i].fixed_dir;
        if ((datafixedqueue.fixedmsg[i].fixed_spd) > 127) {
            x.speed = 127;
        }
        else {
            x.speed = datafixedqueue.fixedmsg[i].fixed_spd;
        }
        if ((datafixedqueue.fixedmsg[i].fixed_len / 15) > 127) {
            x.juli = 127;
        }
        else {
            x.juli = datafixedqueue.fixedmsg[i].fixed_len / 15;
        }

        if ((datafixedqueue.fixedmsg[i].fixed_delay / 10) > 255) {
            x.dely = 255;
        }
        else {
            x.dely = datafixedqueue.fixedmsg[i].fixed_delay / 10;
        }

        if (groupindex == 1) {
            Data_add_to_moto_para_queue(&motooneprarqueue[carindex - 1],x);
            vusart1_moto_para_ready(carindex - 1);
        }
        if (groupindex == 2) {
            Data_add_to_moto_para_queue(&motothreeprarqueue[carindex - 1],x);
            vuart4_moto_para_ready(carindex - 1);
        }

    }
}


// 追踪下料期间，是否发送关闭格口信息
void vposition_moto_run_state_process(void)
{
    u16 i = 0;
    u8 closestate = IO_OPEN;

    for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {
        if (motoonepara[i].runcnt != 0) {
            closestate = u8data_check_io_close_state(motoonepara[i].chuteindex);
            if (closestate == IO_CLOSE) {
                motoonepara[i].runerr = RUN_CLOSE;
                motoonepara[i].runcnt = 0;
            }
            motoonepara[i].runcnt--;
        }
    }

    for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
        if (mototwopara[i].runcnt != 0) {
            closestate = u8data_check_io_close_state(mototwopara[i].chuteindex);
            if (closestate == IO_CLOSE) {
                mototwopara[i].runerr = RUN_CLOSE;
                mototwopara[i].runcnt = 0;
            }
            mototwopara[i].runcnt--;
        }
    }

    for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {
        if (motothreepara[i].runcnt != 0) {
            closestate = u8data_check_io_close_state(motothreepara[i].chuteindex);
            if (closestate == IO_CLOSE) {
                motothreepara[i].runerr = RUN_CLOSE;
                motothreepara[i].runcnt = 0;
            }
            motothreepara[i].runcnt--;
        }
    }
}

void vposition_calculate_unload_load_process(void)
{
    u8 i = 0;

    if (datasysmsg.calculate != 0) {
        datasysmsg.calculate--;
        vposition_dealwith_fixed_data_process();
        if (datasysmsg.load_forbid == LICENSE_OK) {
            vposition_moto_run_state_process();
            vposition_dealwith_unload_process();
            vphoto_deal_with_load_data_process();
        }
    }

    if (datasysmsg.runflag != 0) {
        datasysmsg.runflag--;
        for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
            if (mototwopara[i].state == SNED_PARA_CMD) {
                mototwopara[i].state = SEND_RUN_CMD;
            }
        }

        for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {
            if (motoonepara[i].state == SNED_PARA_CMD) {
                motoonepara[i].state = SEND_RUN_CMD;
            }
        }

        for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {
            if (motothreepara[i].state == SNED_PARA_CMD) {
                motothreepara[i].state = SEND_RUN_CMD;
            }
        }
    }

}

void position_test(void)
{
    moto_para_struct x;

    static u8 index = 0;

    index++;

    if (index > 15) {
        index = 0;
    }

    x.num = index + 1;
    x.dir = 1;
    x.speed = 127;

    x.juli = 127;

    x.dely = 255;

    x.juliextern = 0;
    x.carresetcnt = car_resetcal;

    Data_add_to_moto_para_queue(&mototwoprarqueue[index], x);
    vusart2_moto_para_ready(index);

    Data_add_to_moto_para_queue(&motooneprarqueue[index], x);
    vusart1_moto_para_ready(index);

    Data_add_to_moto_para_queue(&motothreeprarqueue[index], x);
    vuart4_moto_para_ready(index);

    mototwopara[index].chutdown = 100;

    motoonepara[index].chutdown = 100;

    motothreepara[index].chutdown = 100;
}