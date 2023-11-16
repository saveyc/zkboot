#ifndef _USART1_H
#define _USART1_H

#define USART1_BUFF_LEN   30


#define USART1_RECV_OUTTIMER  10


extern u8  usart1_send_buff[];
extern u8  usart1_recv_buff[];
extern u16 usart1_send_len;
extern u16 usart1_recv_len;


extern u8  usart1_moto_commu_statue;
extern u8  usart1_moto_ctr_statue;

/* poll para*/
extern u8 usart1_cur_moto_para_num;
extern u8 usart1_pre_moto_para_num;

extern u8 usart1_moto_reply_outtimer;
extern u8 usart1_moto_waitrun_timer;
extern u8 usart1_moto_resend_time;

extern u8 usart2_moto_waitrunask_timer;

extern u8 usart1_moto_send_num;

extern u8  usart1_moto_recv_statue;

extern u8 moto_one_err[];
extern u8 motoone_typetwo_err[];

extern moto_para_struct  motoonepara[];

extern u8 usart1_cur_test_cnt;

void vusart1_moto_init(void);
void usart1_moto_recv_now_process(void);
void usart1_send_dma_process(void);
void usart1_ready_recv_process(void);
void usart1_ctr_process(void);
void usart1_moto_reply_time_count(void);
void usart1_moto_test_process(void);
void vusart1_upload_cartest_peocess(void);

void vusart1_moto_para_ready(u8  index);
void usart1_send_usart_send_recv_test(void);
void usart1_send_usart1_recv_msg(void);
void usart1_recv_buff_process(void);

extern u8 usart1_recv_testbuff[];

#endif