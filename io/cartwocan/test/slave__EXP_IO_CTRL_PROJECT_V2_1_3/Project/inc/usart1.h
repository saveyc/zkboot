#ifndef _USART1_H
#define _USART1_H

#include "main.h"




extern u16 usart1SendBuff[];
extern u16 usart1RecvBuff[];

extern u16 usart1RecvLen;
extern u16 usart1SendLen;


extern u32                     usart1_swing_middle;
extern u32                     usart1_swing_left;
extern u32                     usart1_swing_right;




void usart1_recv_buff_process(void);
void usart1_send_buff_process(void);

void usart1_send_test_msg(void);

#endif

