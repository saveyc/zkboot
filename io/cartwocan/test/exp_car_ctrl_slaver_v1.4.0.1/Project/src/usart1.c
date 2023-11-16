#include "stm32f10x.h"
#include "stm32f107.h"
#include "main.h"
#include "usart1.h"
#include "data.h"

u8 usart1_send_buff[USART1_BUFF_LEN];
u8 usart1_recv_buff[USART1_BUFF_LEN];
u16 usart1_send_len = 0;
u16 usart1_recv_len = 0;

u8 usart1_recv_testbuff[20];
u16 usart1_recv_testlen = 0;


moto_para_struct* usart1_cur_moto_para = NULL;

moto_para_struct  motoonepara[USART1_DRIVER_GRP_NUM];

u8  usart1_moto_commu_statue;
u8  usart1_moto_ctr_statue = WAIT_RECV_RUN;

/* poll para*/
u8 usart1_cur_moto_para_num = 0;
u8 usart1_pre_moto_para_num = 0;

u8 usart1_moto_reply_outtimer = 0;
u8 usart1_moto_waitrun_timer = 0;
u8 usart1_moto_resend_time = 0;

u8 usart1_moto_waitrunask_timer = 0;

u8 usart1_moto_send_num = 0;

u8  usart1_moto_recv_statue;

u8 moto_one_err[USART1_DRIVER_GRP_NUM];
u8 motoone_typetwo_err[USART1_DRIVER_GRP_NUM];

u8 usart1_cur_test_cnt = 0;

void usart1_ready_recv_process(void);

void vusart1_moto_init(void)
{
    u16 i = 0;
    for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {
        motoonepara[i].chutdown = 0;
        motoonepara[i].dely = 0;
        motoonepara[i].dir = 0;
        motoonepara[i].juli = 0;
        motoonepara[i].num = i + 1;
        motoonepara[i].runerr = NOERR_RUN;
        motoonepara[i].speed = 127;
        motoonepara[i].state = SNED_CMD_NONE;
        motoonepara[i].waitrun = 0;
        motoonepara[i].runcnt = 0;
        motoonepara[i].chuteindex = 0;

        moto_one_err[i] = 0;
        motoone_typetwo_err[i] = 0;
    }
}

void vusart1_moto_para_ready(u8  index)
{
    motoonepara[index].state = SNED_CMD_NONE;
    motoonepara[index].chutdown = 0;
    motoonepara[index].waitrun = 0;
}

void usart1_moto_reply_time_count(void)
{
    if (usart1_moto_reply_outtimer != 0) {
        usart1_moto_reply_outtimer--;
    }


    if (usart1_moto_waitrunask_timer != 0) {
        usart1_moto_waitrunask_timer--;
    }
}


void usart1_moto_state_init(void)
{
    u16 i = 0;
    for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
        motoonepara[i].num = i + 1;
        moto_one_err[i] = 0;
        motoone_typetwo_err[i] = 0;
    }
}
/**************************************************

**************************************************/
void  usart1_send_dma_process(void)
{
    if (usart1_moto_commu_statue == SENDING_DATA)
    {
        if (DMA_GetFlagStatus(DMA1_FLAG_TC7))
        {
            usart1_ready_recv_process();
        }
    }
}

/****************************************

*****************************************/
void usart1_ready_recv_process(void)
{
    usart1_moto_commu_statue = RECV_DATA;
    RX_MOTO_ONE_485;
    usart1_recv_len = 0;
    usart1_moto_recv_statue = 0;
}

/************************************

*************************************/
void usart1_moto_send_process(void)
{
    TX_MOTO_ONE_485;

    if (usart1_moto_commu_statue == SEND_DATA)
    {
        usart1_moto_commu_statue = SENDING_DATA;

        USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
        DMA_Cmd(DMA1_Channel4, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Channel4, usart1_send_len);  // 设置要发送的字节数目
        USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
        DMA_Cmd(DMA1_Channel4, ENABLE);             //开始DMA发送
    }
//    usart1_recv_len;
}

/************************************************

************************************************/
void usart1_moto_recv_now_process(void)
{
    u8 i = 1;
    u8 j = 1;


        if (usart1_recv_len >= 4)
        {
            for (i = 1; i < 8; i++) {
                if (usart1_recv_buff[i] != (i + 1)) {
                    usart1_recv_len = 0; 
                    return;
                }
            }

            if (i == 8) {



                for (j = 0; j < 8; j++)
                {
                    usart1_recv_testbuff[j] = usart1_recv_buff[j];
                }
                usart1_recv_testlen = 8;

            }

            usart1_recv_len = 0;
        }
}


void usart1_recv_buff_process(void)
{
  while(1){
    
        if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET) {
                if (usart1_recv_len >= (USART1_BUFF_LEN - 1)) {
                        USART_ClearFlag(USART1, USART_FLAG_RXNE);
                        continue;
                }

                usart1_recv_buff[usart1_recv_len] = USART_ReceiveData(USART1) & 0xFF;
                usart1_recv_len++;
                usart1_moto_recv_now_process();
                continue;
        }
        break;
  }
}

/****************************************************

*****************************************************/
void usart1_moto_test_process(void)
{
    u8 i = 0;
    u8 j = 0;
    moto_para_struct  moto_para;
    /*
        for (i = usart1_cur_test_cnt; i < 20; i++) {
            moto_para.num = i + 1;
            moto_para.dir = 0;
            moto_para.dely = 0;
            moto_para.juli = 127;
            moto_para.speed = 127;
            Data_add_to_moto_para_queue(&motooneprarqueue[i], moto_para);
            num++;
            usart1_cur_test_cnt++;
            if (usart1_cur_test_cnt >= 20) {
                usart1_cur_test_cnt = 0;
            }
            if(num>3) return;

    */
    j = usart1_cur_test_cnt;
    for (i = 0; i < 1; i++) {

        moto_para.num = j + 1;

        moto_para.dir = 0;

        moto_para.dely = 0;

        moto_para.juli = 127;

        moto_para.speed = 127;

        Data_add_to_moto_para_queue(&motooneprarqueue[j], moto_para);

        j++;

        if (j >= USART1_DRIVER_GRP_NUM) {
            j = 0;
        }

    }

    usart1_cur_test_cnt++;

    if (usart1_cur_test_cnt >= USART1_DRIVER_GRP_NUM) {

        usart1_cur_test_cnt = 0;

    }
}


void usart1_send_moto_para_process(void)
{
    uint8_t buff_tmp[8];
    uint8_t i;


    for (i = 0; i < 8; i++)
    {
        buff_tmp[i] = 0;
    }
    buff_tmp[0] = 0x85;
    buff_tmp[1] = usart1_cur_moto_para->num;
    if (usart1_cur_moto_para->dir == 0)
    {
        buff_tmp[1] |= 0x40;
    }
    buff_tmp[2] = usart1_cur_moto_para->speed;
    if (usart1_cur_moto_para->dely > 127)
    {
        buff_tmp[3] = usart1_cur_moto_para->dely & 0x7f;
        buff_tmp[5] |= 0x01;
    }
    else
    {
        buff_tmp[3] = usart1_cur_moto_para->dely;
    }
    buff_tmp[4] = usart1_cur_moto_para->juli;

    if (datapcconfig.drive_type == DRIVER_TYPE_ONE) {
        buff_tmp[5] &= ~(1 << 1);
    }
    if (datapcconfig.drive_type == DRIVER_TYPE_TWO) {
        buff_tmp[5] |= 0x02;
    }

    buff_tmp[6] = usart1_moto_send_num++;
    if (buff_tmp[6] >= 0x80)
    {
        buff_tmp[6] = 0;
        usart1_moto_send_num = 0;
    }
    for (i = 1; i < 7; i++)
    {
        buff_tmp[7] ^= buff_tmp[i];
    }
    for (i = 0; i < 8; i++)
    {
        usart1_send_buff[i] = buff_tmp[i];
    }
    usart1_send_len = 8;
    usart1_moto_commu_statue = SEND_DATA;
    usart1_moto_send_process();
    usart1_moto_reply_outtimer = USART1_RECV_OUTTIMER;

}

void usart1_send_moto_askrun_process(void)
{
    uint8_t buff_tmp[8] = { 0 };
    uint8_t i = 0;


    for (i = 0; i < 8; i++)
    {
        buff_tmp[i] = 0;
    }
    buff_tmp[0] = 0x85;
    buff_tmp[1] = motoonepara[usart1_cur_moto_para_num].num;
    buff_tmp[2] = 100;
    buff_tmp[3] = 2;
    buff_tmp[4] = 2;
    buff_tmp[5] = 0x02;
    buff_tmp[6] = usart1_moto_send_num++;
    if (buff_tmp[6] >= 0x80)
    {
        buff_tmp[6] = 0;
        usart1_moto_send_num = 0;
    }
    for (i = 1; i < 7; i++)
    {
        buff_tmp[7] ^= buff_tmp[i];
    }
    for (i = 0; i < 8; i++)
    {
        usart1_send_buff[i] = buff_tmp[i];
    }
    usart1_send_len = 8;
    usart1_moto_commu_statue = SEND_DATA;
    usart1_moto_send_process();
    usart1_moto_waitrunask_timer = USART1_RECV_OUTTIMER;

}

/****************************************************

*****************************************************/
void usart1_send_moto_run_process(void)
{
    uint8_t buff_tmp[8];
    uint8_t i;
    uint8_t j;
    u8 num = 0;
    u8  usart1_moto_run_flag[5] = { 0 };


    for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {

        if (motoonepara[i].state == SEND_RUN_CMD) {
            j = motoonepara[i].num % 8;
            if (j != 0) {
                usart1_moto_run_flag[motoonepara[i].num / 8] = (usart1_moto_run_flag[motoonepara[i].num / 8]) | (1 << (j - 1));
            }
            else {
                usart1_moto_run_flag[4] = (usart1_moto_run_flag[4]) | (1 << ((motoonepara[i].num / 8) - 1));
            }
            num++;

            motoonepara[i].state = SNED_CMD_NONE;
            motoonepara[i].waitrun = MOTO_RUNASK_SHUT_TIMER;
        }
    }

    if (num == 0) {
        usart1_pre_moto_para_num = 0;
        usart1_moto_ctr_statue = SEND_PARA_STATUE;
        return;
    }

    for (i = 0; i < 8; i++)
    {
        buff_tmp[i] = 0;
    }
    buff_tmp[0] = 0x8A;
    for (i = 0; i < 5; i++) {
        buff_tmp[i + 1] = usart1_moto_run_flag[i];
    }

    buff_tmp[6] = usart1_moto_send_num++;
    if (buff_tmp[6] >= 0x80)
    {
        buff_tmp[6] = 0;
        usart1_moto_send_num = 0;
    }
    for (i = 1; i < 7; i++)
    {
        buff_tmp[7] ^= buff_tmp[i];
    }
    for (i = 0; i < 8; i++)
    {
        usart1_send_buff[i] = buff_tmp[i];
    }
    usart1_send_len = 8;
    usart1_moto_commu_statue = SEND_DATA;
    usart1_moto_send_process();

}

/*********************************************************8

**********************************************************/
void usart1_ctr_process(void)
{
    switch (usart1_moto_ctr_statue)
    {
    case SEND_PARA_STATUE:
        if (usart1_moto_waitrun_timer != 0) {
            usart1_moto_waitrun_timer--;
            return;
        }
        for (usart1_cur_moto_para_num = usart1_pre_moto_para_num; usart1_cur_moto_para_num < USART1_DRIVER_GRP_NUM; usart1_cur_moto_para_num++)
        {
            usart1_cur_moto_para = getmsgfrommotoparaqueue(&(motooneprarqueue[usart1_cur_moto_para_num]));
            if (usart1_cur_moto_para != NULL && (motoonepara[usart1_cur_moto_para_num].state == SNED_CMD_NONE)) {
                usart1_send_moto_para_process();
                usart1_moto_ctr_statue = WAIT_RECV_PARA;
                motoonepara[usart1_cur_moto_para_num].state = SNED_PARA_CMD;
                return;
            }
        }
        usart1_send_moto_run_process();

        if (datapcconfig.drive_type == DRIVER_TYPE_ONE) {
            usart1_moto_ctr_statue = WAIT_RECV_RUN;
            usart1_pre_moto_para_num = 0;
            usart1_moto_waitrun_timer = USART1_RECV_OUTTIMER;
        }
        else if (datapcconfig.drive_type == DRIVER_TYPE_TWO) {
            usart1_moto_ctr_statue = SEND_ASK_RUN_STATE;
            usart1_pre_moto_para_num = 0;
            usart1_moto_waitrun_timer = USART1_RECV_OUTTIMER;
        }

        break;
    case WAIT_RECV_PARA:
        if (usart1_moto_reply_outtimer == 0)
        {
            if (usart1_moto_resend_time < 1) {
                Data_add_to_moto_para_queue(&motooneprarqueue[usart1_cur_moto_para_num], *usart1_cur_moto_para);
                usart1_moto_resend_time++;
                usart1_moto_ctr_statue = SEND_PARA_STATUE;
                motoonepara[usart1_cur_moto_para_num].state = SNED_CMD_NONE;

            }
            else {
                usart1_moto_resend_time = 0;
                if (datapcconfig.drive_type == DRIVER_TYPE_ONE) {
                    moto_one_err[usart1_cur_moto_para_num] = 0x00;
                    moto_one_err[usart1_cur_moto_para_num] = COM_LOST;
                }
                if (datapcconfig.drive_type == DRIVER_TYPE_TWO) {
                    motoone_typetwo_err[usart1_cur_moto_para_num] = 0x00;
                    motoone_typetwo_err[usart1_cur_moto_para_num] = COM_TWO_LOST;
                }
                usart1_cur_moto_para_num++;
                usart1_pre_moto_para_num = usart1_cur_moto_para_num;
                usart1_moto_ctr_statue = SEND_PARA_STATUE;
            }
        }
        break;
    case WAIT_RECV_RUN:
        usart1_pre_moto_para_num = 0;
        usart1_moto_ctr_statue = SEND_PARA_STATUE;
        usart1_moto_waitrun_timer = USART1_RECV_OUTTIMER;
        break;
    case SEND_ASK_RUN_STATE:

        if (usart1_moto_waitrun_timer != 0) {
            usart1_moto_waitrun_timer--;
            return;
        }

        for (usart1_cur_moto_para_num = usart1_pre_moto_para_num; usart1_cur_moto_para_num < USART1_DRIVER_GRP_NUM; usart1_cur_moto_para_num++)
        {
            if (motoonepara[usart1_cur_moto_para_num].state == SEND_ASKRUN_CMD) {
                usart1_cur_moto_para = &(motoonepara[usart1_cur_moto_para_num]);;
                usart1_send_moto_askrun_process();
                usart1_moto_ctr_statue = RECV_RUN_ACK_STATE;
                motoonepara[usart1_cur_moto_para_num].state = SNED_CMD_NONE;
                return;
            }
        }
        usart1_pre_moto_para_num = 0;
        usart1_moto_ctr_statue = SEND_PARA_STATUE;
        usart1_moto_waitrun_timer = USART1_RECV_OUTTIMER;
        break;
    case RECV_RUN_ACK_STATE:
        if (usart1_moto_waitrunask_timer == 0)
        {
            motoone_typetwo_err[usart1_cur_moto_para_num] = 0x00;
            motoone_typetwo_err[usart1_cur_moto_para_num] = COM_TWO_LOST;
            motoonepara[usart1_cur_moto_para_num].state = SNED_CMD_NONE;
            usart1_cur_moto_para_num++;
            usart1_pre_moto_para_num = usart1_cur_moto_para_num;
            usart1_moto_ctr_statue = SEND_ASK_RUN_STATE;
        }
        break;
    default:
        break;
    }
}


void vusart1_upload_cartest_peocess(void)
{
    u16 i = 0;
    for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {
        if (motoonepara[i].chutdown != 0) {
            motoonepara[i].chutdown--;
            if (motoonepara[i].chutdown == 0) {
                motoonepara[i].state = SEND_RUN_CMD;
                usart1_moto_ctr_statue = WAIT_RECV_RUN;
            }
        }
    }

    for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {
        if (motoonepara[i].waitrun != 0) {
            motoonepara[i].waitrun--;
            if (motoonepara[i].waitrun == 0) {
                motoonepara[i].state = SEND_ASKRUN_CMD;
            }
        }
    }
}


void usart1_send_usart_send_recv_test(void)
{
    u8 i =0;
    for (i = 0; i < 8; i++)
    {
        usart1_send_buff[i] = i+1;
    }
    usart1_send_len = 8;
    usart1_moto_commu_statue = SEND_DATA;
    usart1_moto_send_process();
    
    
}

void usart1_send_usart1_recv_msg(void)
{
    u8 send_len = 0;
    char str[50];
    u16 j = 0;

    if (usart1_recv_testlen != 0) {
//        send_len = sprintf(str, "USART1:NORMAL:%d\n", 1);
//        DEBUG_process((u8*)str, send_len);
        if (key_value != 1) {
            if (usart1_recv_testbuff[0] == 1)
            {
                send_len = sprintf(str, "USART1:NORMAL:%d\n", 1);
                DEBUG_process((u8*)str, send_len);

            }
            if (usart1_recv_testbuff[0] == 6)
            {
                send_len = sprintf(str, "USART6:NORMAL:%d\n", 1);
                DEBUG_process((u8*)str, send_len);

            }
        }
        else {
            for (j = 0; j < 8; j++)
            {
                usart1_send_buff[j] = usart1_recv_testbuff[j];
            }
            usart1_send_len = 8;
            usart1_moto_commu_statue = SEND_DATA;
            usart1_moto_send_process();
        }

        usart1_recv_testlen = 0;
    }


}