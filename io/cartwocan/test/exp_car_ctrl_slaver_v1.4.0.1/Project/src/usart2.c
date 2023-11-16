#include "stm32f10x.h"
#include "stm32f107.h"
#include "main.h"
#include "usart2.h"
#include "data.h"

u8 usart2_send_buff[USART2_BUFF_LEN];
u8 usart2_recv_buff[USART2_BUFF_LEN];
u16 usart2_send_len = 0;
u16 usart2_recv_len = 0;


moto_para_struct* usart2_cur_moto_para = NULL;

moto_para_struct  mototwopara[USART2_DRIVER_GRP_NUM];

// control reset car
moto_para_struct  mototwocopy[USART2_DRIVER_GRP_NUM];


u8	usart2_moto_commu_statue;
u8  usart2_moto_ctr_statue = WAIT_RECV_RUN;

/* poll para*/
u8 usart2_cur_moto_para_num = 0;
u8 usart2_pre_moto_para_num = 0;

u8 usart2_moto_reply_outtimer = 0;
u8 usart2_moto_waitrun_timer = 0;
u8 usart2_moto_resend_time = 0;

u8 usart2_moto_waitrunask_timer = 0;

u8 usart2_moto_send_num = 0;

u8  usart2_moto_recv_statue;
 
u8 moto_two_err[USART2_DRIVER_GRP_NUM];                                 //记录类型1驱动器的错误信息 ~-20220517 之前 
u8 mototwo_typetwo_err[USART2_DRIVER_GRP_NUM];                          //记录类型2、3驱动器的错误信息 20220517~- 之后

// 测试
u8 usart2_cur_test_cnt = 0;

void usart2_ready_recv_process(void);

void vusart2_moto_init(void)
{
    u16 i = 0;
    for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
        mototwopara[i].chutdown = 0;
        mototwopara[i].dely = 0;
        mototwopara[i].dir = 0;
        mototwopara[i].juli = 0;
        mototwopara[i].num = i + 1;
        mototwopara[i].runerr = NOERR_RUN;
        mototwopara[i].speed = 127;
        mototwopara[i].state = SNED_CMD_NONE;
        mototwopara[i].waitrun = 0;
        mototwopara[i].runcnt = 0;
        mototwopara[i].chuteindex = 0;
        mototwopara[i].zeroinit = INVALUE;
        mototwopara[i].juliextern = INVALUE;
        mototwopara[i].carresetcnt = 0;
        mototwopara[i].cartestcnt = 0;


        moto_two_err[i] = 0;
        mototwo_typetwo_err[i] = 0;
    }

    for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
        mototwocopy[i].chutdown = 0;
        mototwocopy[i].dely = 0;
        mototwocopy[i].dir = 0;
        mototwocopy[i].juli = 0;
        mototwocopy[i].num = i + 1;
        mototwocopy[i].runerr = NOERR_RUN;
        mototwocopy[i].speed = 127;
        mototwocopy[i].state = SNED_CMD_NONE;
        mototwocopy[i].waitrun = 0;
        mototwocopy[i].runcnt = 0;
        mototwocopy[i].chuteindex = 0;
        mototwocopy[i].zeroinit = INVALUE;
        mototwocopy[i].juliextern = INVALUE;
        mototwocopy[i].carresetcnt = 0;
        mototwocopy[i].cartestcnt = 0;


        moto_two_err[i] = 0;
        mototwo_typetwo_err[i] = 0;
    }
}

void vusart2_moto_defaut(void)
{
    u16 i =0;
    
    for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
        mototwocopy[i].zeroinit = INVALUE;
        mototwocopy[i].juliextern = INVALUE;
        mototwocopy[i].carresetcnt = 0;
        mototwocopy[i].cartestcnt = 0;
    }
}

void vusart2_moto_para_ready(u8  index)
{
    mototwopara[index].state = SNED_CMD_NONE;
    mototwopara[index].chutdown = 0;
    mototwopara[index].waitrun = 0;
    mototwopara[index].zeroinit = INVALUE;
    mototwopara[index].cartestcnt = 0;
}


void usart2_moto_state_init(void)
{
    u16 i = 0;
    for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
        mototwopara[i].num = i + 1;
        moto_two_err[i] = 0;
        mototwo_typetwo_err[i] = 0;
    }
}

void usart2_moto_reply_time_count(void)
{
    if (usart2_moto_reply_outtimer != 0) {
        usart2_moto_reply_outtimer--;
    }

    if (usart2_moto_waitrunask_timer != 0) {
        usart2_moto_waitrunask_timer--;
    }
}
/**************************************************

**************************************************/
void  usart2_send_dma_process(void)
{
    if(usart2_moto_commu_statue == SENDING_DATA )
    {
        if( DMA_GetFlagStatus(DMA1_FLAG_TC7))
        {
            usart2_ready_recv_process();
        }
    }
}

/****************************************

*****************************************/
void usart2_ready_recv_process(void)
{
    usart2_moto_commu_statue = RECV_DATA;
    RX_MOTO_TWO_485;
    usart2_recv_len = 0;
    usart2_moto_recv_statue = 0;
}

/************************************

*************************************/
void usart2_moto_send_process(void)
{
    TX_MOTO_TWO_485;

    if(usart2_moto_commu_statue == SEND_DATA)
    {
        usart2_moto_commu_statue = SENDING_DATA;

        USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);
        DMA_Cmd(DMA1_Channel7, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Channel7, usart2_send_len);  // 设置要发送的字节数目
        USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
        DMA_Cmd(DMA1_Channel7, ENABLE);             //开始DMA发送
    }
}

/************************************************

************************************************/
void usart2_moto_recv_now_process(void)
{
    if(usart2_recv_buff[0] == 0x99)
    {
        if(usart2_recv_len >= 4 )
        {
            if(usart2_recv_buff[1] == usart2_cur_moto_para->num )
            {
                if( usart2_recv_buff[3] == ( usart2_recv_buff[1] ^ usart2_recv_buff[2] ) )
                {
                    if (usart2_moto_ctr_statue == WAIT_RECV_PARA) {

                        //type driver one
                        if (datapcconfig.drive_type == DRIVER_TYPE_ONE) {
                            moto_two_err[usart2_cur_moto_para_num] = 0x00;
                            if (usart2_recv_buff[2] != 0) {
                                if ((usart2_recv_buff[2] & 0x01) == 1) {
                                    moto_two_err[usart2_cur_moto_para_num] = COM_ERR;
                                }
                                else if (((usart2_recv_buff[2] >> 1) & 0x01) == 1) {
                                    moto_two_err[usart2_cur_moto_para_num] = CURRENT_PROTECTION;
                                }
                                else if (((usart2_recv_buff[2] >> 2) & 0x01) == 1) {
                                    moto_two_err[usart2_cur_moto_para_num] = MOTOR_BELT;
                                }
                                else if (((usart2_recv_buff[2] >> 3) & 0x01) == 1) {
                                    moto_two_err[usart2_cur_moto_para_num] = OVER_VOLTAGE;
                                }
                            }
                        }
                        //type driver two
                        if ((datapcconfig.drive_type == DRIVER_TYPE_TWO) || (datapcconfig.drive_type == DRIVER_TYPE_THREE)) {
                            mototwo_typetwo_err[usart2_cur_moto_para_num] = 0x00;

                            if (((usart2_recv_buff[2] >> 6) & 0x01) == 1) {

                                if (((usart2_recv_buff[2]) & 0x1F) != 0) {
                                    mototwo_typetwo_err[usart2_cur_moto_para_num] = (usart2_recv_buff[2] ) & 0x1F;
                                }

                            }
                            else {
                                mototwo_typetwo_err[usart2_cur_moto_para_num] = COM_TWO_ERR;
                            }
                        }

                        if ((usart2_cur_moto_para->carresetcnt != 0) && (mototwo_typetwo_err[usart2_cur_moto_para_num] == 0) && (moto_two_err[usart2_cur_moto_para_num] == 0)) {
                            vusart2_copy_moto_two(usart2_cur_moto_para_num, usart2_cur_moto_para);
                        }
                    

                        usart2_cur_moto_para_num++;
                        usart2_pre_moto_para_num = usart2_cur_moto_para_num;
                        usart2_moto_ctr_statue = SEND_PARA_STATUE;
                        usart2_moto_reply_outtimer = 0;

                    }
                    else if (usart2_moto_ctr_statue == RECV_RUN_ACK_STATE) {
                        mototwo_typetwo_err[usart2_cur_moto_para_num] = 0x00;

                        if (((usart2_recv_buff[2]>>6) & 0x01) == 1) {

                            if (((usart2_recv_buff[2]>>5) & 0x01) == 1) {
                                mototwopara[usart2_cur_moto_para_num].runerr = ERR_RUN;
                            }
                            else if (((usart2_recv_buff[2] >> 5) & 0x01) == 0) {
//                                mototwopara[usart2_cur_moto_para_num].runerr = NOERR_RUN;
                            }

                            if (((usart2_recv_buff[2] ) & 0x1F) != 0) {
                                mototwo_typetwo_err[usart2_cur_moto_para_num] = (usart2_recv_buff[2]) & 0x1F;
                            }

                        }
                        else {
                            mototwo_typetwo_err[usart2_cur_moto_para_num] = COM_TWO_ERR;
                        }

                        usart2_cur_moto_para_num++;
                        usart2_pre_moto_para_num = usart2_cur_moto_para_num;
                        usart2_moto_ctr_statue = SEND_ASK_RUN_STATE;
                        usart2_moto_waitrunask_timer = 0;
                    }                   
                }
            }
        }
    }
}


/****************************************************

*****************************************************/
void usart2_moto_test_process(void)
{
    u8 i = 0;
    u8 j = 0;
    moto_para_struct  moto_para;


    j = usart2_cur_test_cnt;
    for (i = 0; i < 1; i++) {

        moto_para.num = j + 1;

        moto_para.dir = 0;

        moto_para.dely = 0;

        moto_para.juli = 127;

        moto_para.speed = 127;

        Data_add_to_moto_para_queue(&mototwoprarqueue[j], moto_para);

        j++;

        if (j >= USART2_DRIVER_GRP_NUM) {
            j = 0;
        }

    }

    usart2_cur_test_cnt++;

    if (usart2_cur_test_cnt >= USART2_DRIVER_GRP_NUM) {

        usart2_cur_test_cnt = 0;

    }
}


void usart2_send_moto_para_process(void)
{
    uint8_t buff_tmp[8] = { 0 };
    uint8_t i = 0;


    for(i = 0; i < 8; i++)
    {
        buff_tmp[i] = 0;
    }

    if (datapcconfig.drive_type == DRIVER_TYPE_THREE) {
        buff_tmp[0] = 0x87;
    }
    else {
        buff_tmp[0] = 0x85;
    }
    

    buff_tmp[1] = usart2_cur_moto_para->num;
    if(usart2_cur_moto_para->dir == 0 )
    {
        buff_tmp[1] |= 0x40;
    }
    buff_tmp[2] = usart2_cur_moto_para->speed;

    if(usart2_cur_moto_para->dely > 127)
    {
        buff_tmp[3] = usart2_cur_moto_para->dely & 0x7f;
        buff_tmp[5] |= 0x01;
    }
    else
    {
        buff_tmp[3] = usart2_cur_moto_para->dely;
    }
    buff_tmp[4] = usart2_cur_moto_para->juli & 0x7F;


    buff_tmp[5] |= ((usart2_cur_moto_para->juliextern & 0x0007) << 3);
    if (datapcconfig.drive_type == DRIVER_TYPE_ONE) {
        buff_tmp[5] &= ~(1 << 1);
    }
    if ((datapcconfig.drive_type == DRIVER_TYPE_TWO) || (datapcconfig.drive_type == DRIVER_TYPE_THREE)) {
        buff_tmp[5] |= 0x02;
    }
    if ((usart2_cur_moto_para->zeroinit == VALUE)  &&(datapcconfig.drive_type == DRIVER_TYPE_THREE)) {
        buff_tmp[5] |= 0x40;
    }

    buff_tmp[6] = usart2_moto_send_num++;
    if( buff_tmp[6] >= 0x80 )
    {
        buff_tmp[6] = 0;
        usart2_moto_send_num = 0;
    }
    for(i = 1; i < 7; i++)
    {
        buff_tmp[7] ^= buff_tmp[i];
    }
    for(i = 0; i < 8; i++)
    {
        usart2_send_buff[i] = buff_tmp[i];
    }
    usart2_send_len = 8;
    usart2_moto_commu_statue = SEND_DATA;
    usart2_moto_send_process();
    usart2_moto_reply_outtimer = USART2_RECV_OUTTIMER;
  
}

void usart2_send_moto_askrun_process(void)
{
    uint8_t buff_tmp[8] = { 0 };
    uint8_t i = 0;


    for (i = 0; i < 8; i++)
    {
        buff_tmp[i] = 0;
    }
    buff_tmp[0] = 0x85;
    buff_tmp[1] = mototwopara[usart2_cur_moto_para_num].num;
    buff_tmp[2] = 100;
    buff_tmp[3] = 2;
    buff_tmp[4] = 2;
    buff_tmp[5] = 0x02;
    buff_tmp[6] = usart2_moto_send_num++;
    if (buff_tmp[6] >= 0x80)
    {
        buff_tmp[6] = 0;
        usart2_moto_send_num = 0;
    }
    for (i = 1; i < 7; i++)
    {
        buff_tmp[7] ^= buff_tmp[i];
    }
    for (i = 0; i < 8; i++)
    {
        usart2_send_buff[i] = buff_tmp[i];
    }
    usart2_send_len = 8;
    usart2_moto_commu_statue = SEND_DATA;
    usart2_moto_send_process();
    usart2_moto_waitrunask_timer = USART2_RECV_OUTTIMER;

}


/****************************************************

*****************************************************/
void usart2_send_moto_run_process(void)
{
    uint8_t buff_tmp[8];
    uint8_t i;
    uint8_t j;
    u8 num = 0;
    u8  usart2_moto_run_flag[5] = { 0 };

    for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {

        if (mototwopara[i].state == SEND_RUN_CMD) {
            j = mototwopara[i].num % 8;
            if (j != 0) {
                usart2_moto_run_flag[mototwopara[i].num / 8] = (usart2_moto_run_flag[mototwopara[i].num / 8]) | (1 << (j - 1));
            }
            else {
                usart2_moto_run_flag[4] = (usart2_moto_run_flag[4]) | (1 << ((mototwopara[i].num / 8) - 1));
            }
            num++;
            mototwopara[i].state = SNED_CMD_NONE;
            mototwopara[i].waitrun = MOTO_RUNASK_SHUT_TIMER;
        }
    }

    if (num == 0) {
        usart2_pre_moto_para_num = 0;
        usart2_moto_ctr_statue = SEND_PARA_STATUE;
        return;
    }

    for(i = 0; i < 8; i++)
    {
        buff_tmp[i] = 0;
    }
    buff_tmp[0] = 0x8A;
    for (i = 0; i < 5; i++) {
        buff_tmp[i + 1] = usart2_moto_run_flag[i];
    }

    buff_tmp[6] = usart2_moto_send_num++;
    if( buff_tmp[6] >= 0x80 )
    {
        buff_tmp[6] = 0;
        usart2_moto_send_num = 0;
    }
    for(i = 1; i < 7; i++)
    {
        buff_tmp[7] ^= buff_tmp[i];
    }
    for(i = 0; i < 8; i++)
    {
        usart2_send_buff[i] = buff_tmp[i];
    }
    usart2_send_len = 8;
    usart2_moto_commu_statue = SEND_DATA;
    usart2_moto_send_process();

}

/*********************************************************8

**********************************************************/
void usart2_ctr_process(void)
{

    switch (usart2_moto_ctr_statue)
    {
    case SEND_PARA_STATUE:
        if (usart2_moto_waitrun_timer != 0) {
            usart2_moto_waitrun_timer--;
            return;
        }
        for (usart2_cur_moto_para_num = usart2_pre_moto_para_num; usart2_cur_moto_para_num < USART2_DRIVER_GRP_NUM; usart2_cur_moto_para_num++)
        {
            usart2_cur_moto_para = getmsgfrommotoparaqueue(&(mototwoprarqueue[usart2_cur_moto_para_num]));
            if ((usart2_cur_moto_para != NULL) && (mototwopara[usart2_cur_moto_para_num].state == SNED_CMD_NONE)) {
                usart2_send_moto_para_process();
                usart2_moto_ctr_statue = WAIT_RECV_PARA;
                mototwopara[usart2_cur_moto_para_num].state = SNED_PARA_CMD;
                if (usart2_cur_moto_para->zeroinit == VALUE) {
                    usart2_cur_moto_para->zeroinit = INVALUE;
                    mototwopara[usart2_cur_moto_para_num].state = SNED_CMD_NONE;
                }
                return;
            }
        }
        usart2_send_moto_run_process();
        if ((datapcconfig.drive_type == DRIVER_TYPE_ONE) || (datapcconfig.drive_type == DRIVER_TYPE_THREE)) {
            usart2_moto_ctr_statue = WAIT_RECV_RUN;
            usart2_pre_moto_para_num = 0;
            usart2_moto_waitrun_timer = USART2_RECV_OUTTIMER;
        }
        else if (datapcconfig.drive_type == DRIVER_TYPE_TWO) {
            usart2_moto_ctr_statue = SEND_ASK_RUN_STATE;
            usart2_pre_moto_para_num = 0;
            usart2_moto_waitrun_timer = USART2_RECV_OUTTIMER;
        }
        break;
    case WAIT_RECV_PARA:
        if (usart2_moto_reply_outtimer == 0)
        {
            if ((usart2_moto_resend_time < 1) && (mototwopara[usart2_cur_moto_para_num].state == SNED_PARA_CMD) ) {
                usart2_moto_resend_time++;
                usart2_moto_ctr_statue = SEND_PARA_STATUE;
                Data_add_to_moto_para_queue(&mototwoprarqueue[usart2_cur_moto_para_num], *usart2_cur_moto_para);
                mototwopara[usart2_cur_moto_para_num].state = SNED_CMD_NONE;
            }
            else {
                usart2_moto_resend_time = 0;
                if (datapcconfig.drive_type == DRIVER_TYPE_ONE) {
                    moto_two_err[usart2_cur_moto_para_num] = 0x00;
                    moto_two_err[usart2_cur_moto_para_num] = COM_LOST;
                }
                if ((datapcconfig.drive_type == DRIVER_TYPE_TWO) || (datapcconfig.drive_type == DRIVER_TYPE_THREE)) {
                    mototwo_typetwo_err[usart2_cur_moto_para_num] = 0x00;
                    mototwo_typetwo_err[usart2_cur_moto_para_num] = COM_TWO_LOST;
                }

                usart2_cur_moto_para_num++;
                usart2_pre_moto_para_num = usart2_cur_moto_para_num;
                usart2_moto_ctr_statue = SEND_PARA_STATUE;
            }
        }
        break;
    case WAIT_RECV_RUN:
        usart2_pre_moto_para_num = 0;
        usart2_moto_ctr_statue = SEND_PARA_STATUE;
        usart2_moto_waitrun_timer = USART2_RECV_OUTTIMER;
        usart2_moto_waitrunask_timer = 0;
        break;
    case SEND_ASK_RUN_STATE:

        if (usart2_moto_waitrun_timer != 0) {
            usart2_moto_waitrun_timer--;
            return;
        }

        for (usart2_cur_moto_para_num = usart2_pre_moto_para_num; usart2_cur_moto_para_num < USART2_DRIVER_GRP_NUM; usart2_cur_moto_para_num++)
        {
            if (mototwopara[usart2_cur_moto_para_num].state == SEND_ASKRUN_CMD) {
                usart2_cur_moto_para = &(mototwopara[usart2_cur_moto_para_num]);
                usart2_send_moto_askrun_process();
                usart2_moto_ctr_statue = RECV_RUN_ACK_STATE;
                mototwopara[usart2_cur_moto_para_num].state = SNED_CMD_NONE;
                return;
            }
        }

        usart2_pre_moto_para_num = 0;
        usart2_moto_ctr_statue = SEND_PARA_STATUE;
        usart2_moto_waitrun_timer = USART2_RECV_OUTTIMER;
        break;
    case RECV_RUN_ACK_STATE:
        if (usart2_moto_waitrunask_timer == 0)
        {
            mototwo_typetwo_err[usart2_cur_moto_para_num] = 0x00;
            mototwo_typetwo_err[usart2_cur_moto_para_num] = COM_TWO_LOST;
            mototwopara[usart2_cur_moto_para_num].state = SNED_CMD_NONE;
            usart2_cur_moto_para_num++;
            usart2_pre_moto_para_num = usart2_cur_moto_para_num;
            usart2_moto_ctr_statue = SEND_ASK_RUN_STATE;
        }
        break;

    default:
        break;
    }
}

void vusart2_upload_cartest_peocess(void)
{
    u16 i = 0;


    for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
        if (mototwopara[i].chutdown != 0) {
            mototwopara[i].chutdown--;
            if (mototwopara[i].chutdown == 0) {
                mototwopara[i].state = SEND_RUN_CMD;
                usart2_moto_ctr_statue = WAIT_RECV_RUN;
            }
        }
    }

    for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
        if (mototwopara[i].waitrun != 0) {
            mototwopara[i].waitrun--;
            if (mototwopara[i].waitrun == 0) {
                mototwopara[i].state = SEND_ASKRUN_CMD;
            }
        }
    }

    //反转计时
    for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
        if (mototwocopy[i].cartestcnt != 0) {
            mototwocopy[i].cartestcnt--;
            if (mototwocopy[i].cartestcnt == 0) {
                mototwocopy[i].state = SNED_CMD_NONE;
                if ((mototwocopy[i].num > 0) && (mototwocopy[i].num <= USART2_DRIVER_GRP_NUM)) {
                    Data_add_to_moto_para_queue(&mototwoprarqueue[mototwocopy[i].num - 1], mototwocopy[i]);
                    mototwopara[i].state = SNED_CMD_NONE;
                    mototwopara[i].waitrun = 0;
                    usart2_moto_ctr_statue = WAIT_RECV_RUN;
                    mototwopara[mototwocopy[i].num - 1].chutdown = 100;
                }
            }
        }
    }
}

void vusart2_unload_car_reset(void)
{
    u16 i = 0;
    for (i = 0; i < USART2_DRIVER_GRP_NUM; i++)
    {
        if (mototwocopy[i].carresetcnt != 0) {
            mototwocopy[i].carresetcnt--;
            if (mototwocopy[i].carresetcnt == 0) {
                mototwocopy[i].state = SNED_CMD_NONE;
                if ((mototwocopy[i].num > 0) && (mototwocopy[i].num <= USART2_DRIVER_GRP_NUM)) {
                    Data_add_to_moto_para_queue(&mototwoprarqueue[mototwocopy[i].num - 1], mototwocopy[i]);
                    vusart2_moto_para_ready(mototwocopy[i].num - 1);
                }
            }
        }
    }
}

void vusart2_copy_moto_two(u8 index, moto_para_struct* x)
{
    if (index >= USART2_DRIVER_GRP_NUM) {
        return;
    }

    if (x->carresetcnt != 0) {
        mototwocopy[index].carresetcnt = x->carresetcnt;
        mototwocopy[index].cartestcnt = 0;
    }
    else {
        mototwocopy[index].carresetcnt = 0;
        if (x->chutdown != 0) {
            mototwocopy[index].cartestcnt = 2000;
        }
        else {
            mototwocopy[index].cartestcnt = 0;
        }
    }


    mototwocopy[index].chutdown = 0;
    mototwocopy[index].chuteindex = 0xFFFF;
    mototwocopy[index].dely = 0;

    if (x->dir == 0) {
        mototwocopy[index].dir = 1;
    }
    else {
        mototwocopy[index].dir = 0;
    }

    mototwocopy[index].juli = x->juli;
    mototwocopy[index].juliextern = x->juliextern;
    mototwocopy[index].num = x->num;
    mototwocopy[index].runcnt = 0;
    mototwocopy[index].runerr = 0;
    mototwocopy[index].speed = x->speed;
    mototwocopy[index].state = SNED_CMD_NONE;
    mototwocopy[index].waitrun = 0;
    mototwocopy[index].zeroinit = INVALUE;

}


