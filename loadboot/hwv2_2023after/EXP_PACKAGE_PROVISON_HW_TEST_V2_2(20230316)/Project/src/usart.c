#include "main.h"
#include <string.h>

//RS232_1
u8  uart1_send_buff[UART1_BUFF_SIZE];
u8  uart1_recv_buff[UART1_BUFF_SIZE];
u16 uart1_send_count;
u16 uart1_recv_count;
u8  uart1_commu_state = RECV_DATA;
u16 uart1_tmr = 0;
//RS232_2
u8  uart2_send_buff[UART2_BUFF_SIZE];
u8  uart2_recv_buff[UART2_BUFF_SIZE];
u16 uart2_send_count;
u16 uart2_recv_count;
u8  uart2_commu_state = RECV_DATA;
u16 uart2_tmr = 0;
//RS232_5
u8  uart5_send_buff[UART5_BUFF_SIZE];
u8  uart5_recv_buff[UART5_BUFF_SIZE];
u16 uart5_send_count;
u16 uart5_recv_count;
u8  uart5_commu_state = RECV_DATA;
u16 uart5_tmr = 0;
//RS485_3
u8  uart3_send_buff[UART3_BUFF_SIZE];
u8  uart3_recv_buff[UART3_BUFF_SIZE];
u16 uart3_send_count;
u16 uart3_recv_count;
u8  uart3_commu_state = RECV_DATA;
u16 uart3_tmr = 0;
//RS485_4
u8  uart4_send_buff[UART4_BUFF_SIZE];
u8  uart4_recv_buff[UART4_BUFF_SIZE];
u16 uart4_send_count;
u16 uart4_recv_count;
u8  uart4_commu_state = RECV_DATA;
u16 uart4_tmr = 0;
//RS485_6
u8  uart6_send_buff[UART6_BUFF_SIZE];
u8  uart6_recv_buff[UART6_BUFF_SIZE];
u16 uart6_send_count;
u16 uart6_recv_count;
u8  uart6_commu_state = RECV_DATA;
u16 uart6_tmr = 0;

//1ms¶¨Ê±
void uart_recv_timeout(void)
{
    if( uart1_tmr != 0 )
    {
        uart1_tmr--;
        if( uart1_tmr == 0 )
        {
            uart1_commu_state = RECV_DATA_END;
        }
    }
    if( uart2_tmr != 0 )
    {
        uart2_tmr--;
        if( uart2_tmr == 0 )
        {
            uart2_commu_state = RECV_DATA_END;
        }
    }
    if( uart3_tmr != 0 )
    {
        uart3_tmr--;
        if( uart3_tmr == 0 )
        {
            uart3_commu_state = RECV_DATA_END;
        }
    }
    if( uart4_tmr != 0 )
    {
        uart4_tmr--;
        if( uart4_tmr == 0 )
        {
            uart4_commu_state = RECV_DATA_END;
        }
    }
    if( uart5_tmr != 0 )
    {
        uart5_tmr--;
        if( uart5_tmr == 0 )
        {
            uart5_commu_state = RECV_DATA_END;
        }
    }
    if( uart6_tmr != 0 )
    {
        uart6_tmr--;
        if( uart6_tmr == 0 )
        {
            uart6_commu_state = RECV_DATA_END;
        }
    }
}

void uart1_send(void)
{
    u16 i;

    USART_GetFlagStatus(USART1, USART_FLAG_TC);
    for(i = 0 ; i < uart1_send_count ; i++)
    {
        USART_SendData(USART1, uart1_send_buff[i]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
    uart1_commu_state = RECV_DATA;
    uart1_recv_count = 0;
}
void uart2_send(void)
{
    u16 i;

    USART_GetFlagStatus(USART2, USART_FLAG_TC);
    for(i = 0 ; i < uart2_send_count ; i++)
    {
        USART_SendData(USART2, uart2_send_buff[i]);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    }
    uart2_commu_state = RECV_DATA;
    uart2_recv_count = 0;
}
void uart5_send(void)
{
    u16 i;

    USART_GetFlagStatus(UART5, USART_FLAG_TC);
    for(i = 0 ; i < uart5_send_count ; i++)
    {
        USART_SendData(UART5, uart5_send_buff[i]);
        while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
    }
    uart5_commu_state = RECV_DATA;
    uart5_recv_count = 0;
}
void uart3_send(void)
{
     u16 i;
    
     UART3_TX_485;
    
     for(i=0 ; i<uart3_send_count ; i++)
     {
         USART_SendData(USART3, uart3_send_buff[i]);
    	 while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
     }
     UART3_RX_485;
     uart3_commu_state = RECV_DATA;
     uart3_recv_count = 0;
}
void uart4_send(void)
{
     u16 i;
    
     UART4_TX_485;
    
     for(i=0 ; i<uart4_send_count ; i++)
     {
         USART_SendData(UART4, uart4_send_buff[i]);
    	 while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
     }
     UART4_RX_485;
     uart4_commu_state = RECV_DATA;
     uart4_recv_count = 0;
}
void uart6_send(void)
{
     u16 i;
    
     UART6_TX_485;
    
     for(i=0 ; i<uart6_send_count ; i++)
     {
         USART_SendData(USART6, uart6_send_buff[i]);
    	 while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
     }
     UART6_RX_485;
     uart6_commu_state = RECV_DATA;
     uart6_recv_count = 0;
}

void uart_test_send(void)
{
    u8 i;
    
    for(i = 0 ; i<10 ; i++)
    {
        uart1_send_buff[i] = i;
        uart2_send_buff[i] = i;
        uart3_send_buff[i] = i;
        uart4_send_buff[i] = i;
        uart5_send_buff[i] = i;
        uart6_send_buff[i] = i;
    }
    uart1_send_count = 10;
    uart2_send_count = 10;
    uart3_send_count = 10;
    uart4_send_count = 10;
    uart5_send_count = 10;
    uart6_send_count = 10;
    
    uart1_send();
    uart2_send();
    uart3_send();
    uart4_send();
    uart5_send();
    uart6_send();
}
u8 recvDataCheck(u8 *pbuf, u16 count)
{
    u8 i;

    if(count != 10)
        return 0;
    for(i=0 ; i<count ; i++)
    {
        if(pbuf[i] != i) return 0;
    }
    return 1;
}
void uart_test_recv(void)
{
    if(uart1_commu_state == RECV_DATA_END)
    {
        uart1_commu_state = SEND_DATA;

        if(recvDataCheck(uart1_recv_buff, uart1_recv_count))
        {
            rs232_1_debug_cnt++;
        }
    }
    if(uart2_commu_state == RECV_DATA_END)
    {
        uart2_commu_state = SEND_DATA;

        if(recvDataCheck(uart2_recv_buff, uart2_recv_count))
        {
            rs232_2_debug_cnt++;
        }
    }
    if(uart3_commu_state == RECV_DATA_END)
    {
        uart3_commu_state = SEND_DATA;

        if(recvDataCheck(uart3_recv_buff, uart3_recv_count))
        {
            rs485_3_debug_cnt++;
        }
    }
    if(uart4_commu_state == RECV_DATA_END)
    {
        uart4_commu_state = SEND_DATA;

        if(recvDataCheck(uart4_recv_buff, uart4_recv_count))
        {
            rs485_4_debug_cnt++;
        }
    }
    if(uart5_commu_state == RECV_DATA_END)
    {
        uart5_commu_state = SEND_DATA;

        if(recvDataCheck(uart5_recv_buff, uart5_recv_count))
        {
            rs232_5_debug_cnt++;
        }
    }
    if(uart6_commu_state == RECV_DATA_END)
    {
        uart6_commu_state = SEND_DATA;

        if(recvDataCheck(uart6_recv_buff, uart6_recv_count))
        {
            rs485_6_debug_cnt++;
        }
    }
}