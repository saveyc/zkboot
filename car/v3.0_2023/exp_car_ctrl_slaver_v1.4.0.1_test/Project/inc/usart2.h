#ifndef _USART2_H
#define _USART2_H

#define USART2_BUFF_LEN   30

#define USART2_RECV_OUTTIMER  10


extern u8  usart2_send_buff[];
extern u8  usart2_recv_buff[];
extern u16 usart2_send_len ;
extern u16 usart2_recv_len ;



extern u8  usart2_moto_commu_statue;
extern u8  usart2_moto_ctr_statue ;


/* poll para*/
extern u8 usart2_cur_moto_para_num;
extern u8 usart2_pre_moto_para_num;

extern u8 usart2_moto_reply_outtimer;
extern u8 usart2_moto_waitrun_timer;
extern u8 usart2_moto_waitrunask_timer;

extern u8 usart2_moto_send_num;

extern u8  usart2_moto_recv_statue;
extern moto_para_struct  mototwopara[];

extern u8 moto_two_err[];
extern u8 mototwo_typetwo_err[];

extern u8 usart2_cur_test_cnt;

void vusart2_moto_init(void);
void usart2_moto_recv_now_process(void);
void usart2_send_dma_process(void);
void usart2_ready_recv_process(void);
void usart2_ctr_process(void);
void usart2_moto_reply_time_count(void);
void usart2_moto_test_process(void);
void vusart2_upload_cartest_peocess(void);

void vusart2_moto_para_ready(u8 index);
void vusart2_copy_moto_two(u8 index, moto_para_struct* x);
void vusart2_unload_car_reset(void);

void vusart2_moto_defaut(void);

#endif