#include "stm32f10x.h"
#include "stm32f107.h"
#include "main.h"
#include "uart4.h"
#include "data.h"

u8 uart4_send_buff[UART4_BUFF_LEN];
u8 uart4_recv_buff[UART4_BUFF_LEN];
u16 uart4_send_len = 0;
u16 uart4_recv_len = 0;

u8 uart4_recv_testbuff[20];
u16 uart4_recv_testlen = 0;


moto_para_struct* uart4_cur_moto_para = NULL;

moto_para_struct  motothreepara[UART4_DRIVER_GRP_NUM];

u8  uart4_moto_commu_statue;
u8  uart4_moto_ctr_statue = WAIT_RECV_RUN;

/* poll para*/
u8 uart4_cur_moto_para_num = 0;
u8 uart4_pre_moto_para_num = 0;

u8 uart4_moto_reply_outtimer = 0;
u8 uart4_moto_waitrun_timer = 0;
u8 uart4_moto_resend_time = 0;

u8 uart4_moto_waitrunask_timer = 0;

u8 uart4_moto_send_num = 0;

u8  uart4_moto_recv_statue;

u8 moto_three_err[UART4_DRIVER_GRP_NUM];
u8 motothree_typetwo_err[UART4_DRIVER_GRP_NUM];

u8 uart4_cur_test_cnt = 0;

void uart4_ready_recv_process(void);

void vuart4_moto_init(void)
{
    u16 i = 0;
    for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {
        motothreepara[i].chutdown = 0;
        motothreepara[i].dely = 0;
        motothreepara[i].dir = 0;
        motothreepara[i].juli = 0;
        motothreepara[i].num = i + 1;
        motothreepara[i].runerr = NOERR_RUN;
        motothreepara[i].speed = 127;
        motothreepara[i].state = SNED_CMD_NONE;
        motothreepara[i].waitrun = 0;
        motothreepara[i].runcnt = 0;
        motothreepara[i].chuteindex = 0;

        moto_three_err[i] = 0;
        motothree_typetwo_err[i] = 0;
    }
}

void vuart4_moto_para_ready(u8  index)
{
    motothreepara[index].state = SNED_CMD_NONE;
    motothreepara[index].chutdown = 0;
    motothreepara[index].waitrun = 0;
}

void uart4_moto_reply_time_count(void)
{
    if (uart4_moto_reply_outtimer != 0) {
        uart4_moto_reply_outtimer--;
    }


    if (uart4_moto_waitrunask_timer != 0) {
        uart4_moto_waitrunask_timer--;
    }
}


void uart4_moto_state_init(void)
{
    u16 i = 0;
    for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {
        motothreepara[i].num = i + 1;
        moto_three_err[i] = 0;
        motothree_typetwo_err[i] = 0;
    }
}
/**************************************************

**************************************************/
void  uart4_send_dma_process(void)
{
    if (uart4_moto_commu_statue == SENDING_DATA)
    {
        if (DMA_GetFlagStatus(DMA2_FLAG_TC5))
        {
            uart4_ready_recv_process();
        }
    }
}

/****************************************

*****************************************/
void uart4_ready_recv_process(void)
{
    uart4_moto_commu_statue = RECV_DATA;
    RX_MOTO_THREE_485;
    uart4_recv_len = 0;
    uart4_moto_recv_statue = 0;
}

/************************************

*************************************/
void uart4_moto_send_process(void)
{
    TX_MOTO_THREE_485;

    if (uart4_moto_commu_statue == SEND_DATA)
    {
        uart4_moto_commu_statue = SENDING_DATA;

        USART_DMACmd(UART4, USART_DMAReq_Tx, DISABLE);
        DMA_Cmd(DMA2_Channel5, DISABLE);
        DMA_SetCurrDataCounter(DMA2_Channel5, uart4_send_len);  // 设置要发送的字节数目
        USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);
        DMA_Cmd(DMA2_Channel5, ENABLE);             //开始DMA发送 
    }
//    uart4_recv_len =0;
}

/************************************************

************************************************/
void uart4_moto_recv_now_process(void)
{
    u8 i = 1;
    u8 j = 1;
    u8 send_len = 0;
    char str[25];


        if (uart4_recv_len >= 4)
        {
            for (i = 1; i < 8; i++) {
                if (uart4_recv_buff[i] != (i + 1)) {
                    uart4_recv_len = 0;
                    return;
                }
            }
             
            if (i == 8) {



                for (j = 0; j < 8; j++)
                {
                    uart4_recv_testbuff[j] = uart4_recv_buff[j];
                }
                uart4_recv_testlen = 8;

            }

            uart4_recv_len = 0;
        }


}


void usart4_recv_buff_process(void)
{
	while (1) {
	
		if (USART_GetFlagStatus(UART4, USART_FLAG_RXNE) != RESET) {
			if (uart4_recv_len >= (UART4_BUFF_LEN - 1)) {
				USART_ClearFlag(UART4, USART_FLAG_RXNE);
                                				continue;

			}

			uart4_recv_buff[uart4_recv_len] = USART_ReceiveData(UART4) & 0xFF;
			uart4_recv_len++;
			uart4_moto_recv_now_process();
                        			continue;
			
		}
                	break;
        }
	
}

/****************************************************

*****************************************************/
void uart4_moto_test_process(void)
{
    u8 i = 0;
    u8 j = 0;
    moto_para_struct  moto_para;
    /*
        for (i = uart4_cur_test_cnt; i < 20; i++) {
            moto_para.num = i + 1;
            moto_para.dir = 0;
            moto_para.dely = 0;
            moto_para.juli = 127;
            moto_para.speed = 127;
            Data_add_to_moto_para_queue(&motothreeprarqueue[i], moto_para);
            num++;
            uart4_cur_test_cnt++;
            if (uart4_cur_test_cnt >= 20) {
                uart4_cur_test_cnt = 0;
            }
            if(num>3) return;

    */
    j = uart4_cur_test_cnt;
    for (i = 0; i < 1; i++) {

        moto_para.num = j + 1;

        moto_para.dir = 0;

        moto_para.dely = 0;

        moto_para.juli = 127;

        moto_para.speed = 127;

        Data_add_to_moto_para_queue(&motothreeprarqueue[j], moto_para);

        j++;

        if (j >= UART4_DRIVER_GRP_NUM) {
            j = 0;
        }

    }

    uart4_cur_test_cnt++;

    if (uart4_cur_test_cnt >= UART4_DRIVER_GRP_NUM) {

        uart4_cur_test_cnt = 0;

    }
}


void uart4_send_moto_para_process(void)
{
    uint8_t buff_tmp[8];
    uint8_t i;


    for (i = 0; i < 8; i++)
    {
        buff_tmp[i] = 0;
    }
    buff_tmp[0] = 0x85;
    buff_tmp[1] = uart4_cur_moto_para->num;
    if (uart4_cur_moto_para->dir == 0)
    {
        buff_tmp[1] |= 0x40;
    }
    buff_tmp[2] = uart4_cur_moto_para->speed;
    if (uart4_cur_moto_para->dely > 127)
    {
        buff_tmp[3] = uart4_cur_moto_para->dely & 0x7f;
        buff_tmp[5] |= 0x01;
    }
    else
    {
        buff_tmp[3] = uart4_cur_moto_para->dely;
    }
    buff_tmp[4] = uart4_cur_moto_para->juli;

    if (datapcconfig.drive_type == DRIVER_TYPE_ONE) {
        buff_tmp[5] &= ~(1 << 1);
    }
    if (datapcconfig.drive_type == DRIVER_TYPE_TWO) {
        buff_tmp[5] |= 0x02;
    }

    buff_tmp[6] = uart4_moto_send_num++;
    if (buff_tmp[6] >= 0x80)
    {
        buff_tmp[6] = 0;
        uart4_moto_send_num = 0;
    }
    for (i = 1; i < 7; i++)
    {
        buff_tmp[7] ^= buff_tmp[i];
    }
    for (i = 0; i < 8; i++)
    {
        uart4_send_buff[i] = buff_tmp[i];
    }
    uart4_send_len = 8;
    uart4_moto_commu_statue = SEND_DATA;
    uart4_moto_send_process();
    uart4_moto_reply_outtimer = UART4_RECV_OUTTIMER;

}

void uart4_send_moto_askrun_process(void)
{
    uint8_t buff_tmp[8] = { 0 };
    uint8_t i = 0;


    for (i = 0; i < 8; i++)
    {
        buff_tmp[i] = 0;
    }
    buff_tmp[0] = 0x85;
    buff_tmp[1] = motothreepara[uart4_cur_moto_para_num].num;
    buff_tmp[2] = 100;
    buff_tmp[3] = 2;
    buff_tmp[4] = 2;
    buff_tmp[5] = 0x02;
    buff_tmp[6] = uart4_moto_send_num++;
    if (buff_tmp[6] >= 0x80)
    {
        buff_tmp[6] = 0;
        uart4_moto_send_num = 0;
    }
    for (i = 1; i < 7; i++)
    {
        buff_tmp[7] ^= buff_tmp[i];
    }
    for (i = 0; i < 8; i++)
    {
        uart4_send_buff[i] = buff_tmp[i];
    }
    uart4_send_len = 8;
    uart4_moto_commu_statue = SEND_DATA;
    uart4_moto_send_process();
    uart4_moto_waitrunask_timer = UART4_RECV_OUTTIMER;

}

/****************************************************

*****************************************************/
void uart4_send_moto_run_process(void)
{
    uint8_t buff_tmp[8];
    uint8_t i;
    uint8_t j;
    u8 num = 0;
    u8  uart4_moto_run_flag[5] = { 0 };


    for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {

        if (motothreepara[i].state == SEND_RUN_CMD) {
            j = motothreepara[i].num % 8;
            if (j != 0) {
                uart4_moto_run_flag[motothreepara[i].num / 8] = (uart4_moto_run_flag[motothreepara[i].num / 8]) | (1 << (j - 1));
            }
            else {
                uart4_moto_run_flag[4] = (uart4_moto_run_flag[4]) | (1 << ((motothreepara[i].num / 8) - 1));
            }
            num++;

            motothreepara[i].state = SNED_CMD_NONE;
            motothreepara[i].waitrun = MOTO_RUNASK_SHUT_TIMER;
        }
    }

    if (num == 0) {
        uart4_pre_moto_para_num = 0;
        uart4_moto_ctr_statue = SEND_PARA_STATUE;
        return;
    }

    for (i = 0; i < 8; i++)
    {
        buff_tmp[i] = 0;
    }
    buff_tmp[0] = 0x8A;
    for (i = 0; i < 5; i++) {
        buff_tmp[i + 1] = uart4_moto_run_flag[i];
    }

    buff_tmp[6] = uart4_moto_send_num++;
    if (buff_tmp[6] >= 0x80)
    {
        buff_tmp[6] = 0;
        uart4_moto_send_num = 0;
    }
    for (i = 1; i < 7; i++)
    {
        buff_tmp[7] ^= buff_tmp[i];
    }
    for (i = 0; i < 8; i++)
    {
        uart4_send_buff[i] = buff_tmp[i];
    }
    uart4_send_len = 8;
    uart4_moto_commu_statue = SEND_DATA;
    uart4_moto_send_process();

}

/*********************************************************8

**********************************************************/
void uart4_ctr_process(void)
{
    switch (uart4_moto_ctr_statue)
    {
    case SEND_PARA_STATUE:
        if (uart4_moto_waitrun_timer != 0) {
            uart4_moto_waitrun_timer--;
            return;
        }
        for (uart4_cur_moto_para_num = uart4_pre_moto_para_num; uart4_cur_moto_para_num < UART4_DRIVER_GRP_NUM; uart4_cur_moto_para_num++)
        {
            uart4_cur_moto_para = getmsgfrommotoparaqueue(&(motothreeprarqueue[uart4_cur_moto_para_num]));
            if (uart4_cur_moto_para != NULL && (motothreepara[uart4_cur_moto_para_num].state == SNED_CMD_NONE)) {
                uart4_send_moto_para_process();
                uart4_moto_ctr_statue = WAIT_RECV_PARA;
                motothreepara[uart4_cur_moto_para_num].state = SNED_PARA_CMD;
                return;
            }
        }
        uart4_send_moto_run_process();

        if (datapcconfig.drive_type == DRIVER_TYPE_ONE) {
            uart4_moto_ctr_statue = WAIT_RECV_RUN;
            uart4_pre_moto_para_num = 0;
            uart4_moto_waitrun_timer = UART4_RECV_OUTTIMER;
        }
        else if (datapcconfig.drive_type == DRIVER_TYPE_TWO) {
            uart4_moto_ctr_statue = SEND_ASK_RUN_STATE;
            uart4_pre_moto_para_num = 0;
            uart4_moto_waitrun_timer = UART4_RECV_OUTTIMER;
        }

        break;
    case WAIT_RECV_PARA:
        if (uart4_moto_reply_outtimer == 0)
        {
            if (uart4_moto_resend_time < 1) {
                Data_add_to_moto_para_queue(&motothreeprarqueue[uart4_cur_moto_para_num], *uart4_cur_moto_para);
                uart4_moto_resend_time++;
                uart4_moto_ctr_statue = SEND_PARA_STATUE;
                motothreepara[uart4_cur_moto_para_num].state = SNED_CMD_NONE;

            }
            else {
                uart4_moto_resend_time = 0;
                if (datapcconfig.drive_type == DRIVER_TYPE_ONE) {
                    moto_three_err[uart4_cur_moto_para_num] = 0x00;
                    moto_three_err[uart4_cur_moto_para_num] = COM_LOST;
                }
                if (datapcconfig.drive_type == DRIVER_TYPE_TWO) {
                    motothree_typetwo_err[uart4_cur_moto_para_num] = 0x00;
                    motothree_typetwo_err[uart4_cur_moto_para_num] = COM_TWO_LOST;
                }
                uart4_cur_moto_para_num++;
                uart4_pre_moto_para_num = uart4_cur_moto_para_num;
                uart4_moto_ctr_statue = SEND_PARA_STATUE;
            }
        }
        break;
    case WAIT_RECV_RUN:
        uart4_pre_moto_para_num = 0;
        uart4_moto_ctr_statue = SEND_PARA_STATUE;
        uart4_moto_waitrun_timer = UART4_RECV_OUTTIMER;
        break;
    case SEND_ASK_RUN_STATE:

        if (uart4_moto_waitrun_timer != 0) {
            uart4_moto_waitrun_timer--;
            return;
        }

        for (uart4_cur_moto_para_num = uart4_pre_moto_para_num; uart4_cur_moto_para_num < UART4_DRIVER_GRP_NUM; uart4_cur_moto_para_num++)
        {
            if (motothreepara[uart4_cur_moto_para_num].state == SEND_ASKRUN_CMD) {
                uart4_cur_moto_para = &(motothreepara[uart4_cur_moto_para_num]);;
                uart4_send_moto_askrun_process();
                uart4_moto_ctr_statue = RECV_RUN_ACK_STATE;
                motothreepara[uart4_cur_moto_para_num].state = SNED_CMD_NONE;
                return;
            }
        }
        uart4_pre_moto_para_num = 0;
        uart4_moto_ctr_statue = SEND_PARA_STATUE;
        uart4_moto_waitrun_timer = UART4_RECV_OUTTIMER;
        break;
    case RECV_RUN_ACK_STATE:
        if (uart4_moto_waitrunask_timer == 0)
        {
            motothree_typetwo_err[uart4_cur_moto_para_num] = 0x00;
            motothree_typetwo_err[uart4_cur_moto_para_num] = COM_TWO_LOST;
            motothreepara[uart4_cur_moto_para_num].state = SNED_CMD_NONE;
            uart4_cur_moto_para_num++;
            uart4_pre_moto_para_num = uart4_cur_moto_para_num;
            uart4_moto_ctr_statue = SEND_ASK_RUN_STATE;
        }
        break;
    default:
        break;
    }
}


void vuart4_upload_cartest_peocess(void)
{
    u16 i = 0;
    for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {
        if (motothreepara[i].chutdown != 0) {
            motothreepara[i].chutdown--;
            if (motothreepara[i].chutdown == 0) {
                motothreepara[i].state = SEND_RUN_CMD;
                uart4_moto_ctr_statue = WAIT_RECV_RUN;
            }
        }
    }

    for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {
        if (motothreepara[i].waitrun != 0) {
            motothreepara[i].waitrun--;
            if (motothreepara[i].waitrun == 0) {
                motothreepara[i].state = SEND_ASKRUN_CMD;
            }
        }
    }
}

void uart4_send_usart_send_recv_test(void)
{
    u8 i =0;
    for (i = 0; i < 8; i++)
    {
        uart4_send_buff[i] = i + 1;
    }
    uart4_send_len = 8;
    uart4_moto_commu_statue = SEND_DATA;
    uart4_moto_send_process();
}


void uart4_send_uart4_recv_msg(void)
{
    u8 send_len = 0;
    char str[50];
    u16 j = 0;

    if (uart4_recv_testlen != 0) {
//        send_len = sprintf(str, "UART6:NORMAL:%d \n", 6);
//        DEBUG_process((u8*)str, send_len);
        if (key_value != 1) {
            if (uart4_recv_testbuff[0] == 1) {
                send_len = sprintf(str, "UART1:NORMAL:%d \n", 4);
                DEBUG_process((u8*)str, send_len);
            }
            if (uart4_recv_testbuff[0] == 6) {
                send_len = sprintf(str, "UART6:NORMAL:%d \n", 4);
                DEBUG_process((u8*)str, send_len);
            }
        }
        else {
            for (j = 0; j < 8; j++)
            {
                uart4_send_buff[j] = uart4_recv_testbuff[j];
            }
            uart4_send_len = 8;
            uart4_moto_commu_statue = SEND_DATA;
            uart4_moto_send_process();
        }

        uart4_recv_testlen = 0;
    }


}