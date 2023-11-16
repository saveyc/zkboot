#ifndef _UART4_H
#define _UART4_H

#define UART4_BUFF_LEN   30


#define UART4_RECV_OUTTIMER     10


extern u8  uart4_send_buff[];
extern u8  uart4_recv_buff[];
extern u16 uart4_send_len;
extern u16 uart4_recv_len;



extern u8  uart4_moto_commu_statue;
extern u8  uart4_moto_ctr_statue;

/* poll para*/
extern u8 uart4_cur_moto_para_num;
extern u8 uart4_pre_moto_para_num;

extern u8 uart4_moto_reply_outtimer;
extern u8 uart4_moto_waitrun_timer;
extern u8 uart4_moto_resend_time;
extern u8 uart4_moto_waitrunask_timer;

extern u8 uart4_moto_send_num;

extern u8  uart4_moto_recv_statue;

extern u8 moto_three_err[];
extern u8 motothree_typetwo_err[];
extern moto_para_struct  motothreepara[];

extern u8 uart4_cur_test_cnt;


void vuart4_moto_init(void);
void uart4_moto_recv_now_process(void);
void uart4_send_dma_process(void);
void uart4_ready_recv_process(void);
void uart4_ctr_process(void);
void uart4_moto_reply_time_count(void);
void uart4_moto_test_process(void);
void vuart4_upload_cartest_peocess(void);

void vuart4_moto_para_ready(u8 index);
void uart4_send_usart_send_recv_test(void);

void uart4_send_uart4_recv_msg(void);


#endif