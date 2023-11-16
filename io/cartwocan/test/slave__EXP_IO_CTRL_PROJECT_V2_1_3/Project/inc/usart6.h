#ifndef _USART6_H
#define _USART6_H

#include "main.h"




extern u16 usart6SendBuff[];
extern u16 usart6RecvBuff[];

extern u16 usart6RecvLen;
extern u16 usart6SendLen;


extern u32                     usart6_swing_middle;
extern u32                     usart6_swing_left;
extern u32                     usart6_swing_right;




void usart6_recv_buff_process(void);
void usart6_send_buff_process(void);

void usart6_send_test_msg(void);


#endif

