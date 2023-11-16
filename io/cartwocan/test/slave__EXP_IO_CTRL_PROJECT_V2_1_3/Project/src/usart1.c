#include "stm32f2XX_conf.h"
#include "usart1.h"
#include "main.h"

u16 usart1SendBuff[USART1_BUFF_LEN];
u16 usart1RecvBuff[USART1_BUFF_LEN];

u16 usart1RecvLen = 0;
u16 usart1SendLen = 0;

u32 usart1_swing_middle;
u32 usart1_swing_left;
u32 usart1_swing_right;


void usart1_send_buff_process(void)
{
	u16 i = 0;
	if (usart1SendLen > USART1_BUFF_LEN) {
		return;
	}

	UART1_TX_485;


	for (i = 0; i < usart1SendLen + 1; i++)
	{
		USART_SendData(USART1, ((usart1SendBuff[i]) & 0xFF));
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {};
	}

	UART1_RX_485;
	usart1RecvLen = 0;
}


void usart1_deal_with_recved_buff(void)
{
	u16 i = 0;
	u8 eor = 0;
	if (usart1RecvLen == 8) {
		for (i = 0; i < 8; i++) {
			usart1SendBuff[i] = usart1RecvBuff[i];
		}
                usart1SendBuff[0] = 1;
		usart1SendLen = 8;
		usart1_send_buff_process();
	}

}

void usart1_recv_buff_process(void)
{

		if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET) {
			if (usart1RecvLen >= (USART1_BUFF_LEN - 1)) {
				USART_ClearFlag(USART1, USART_FLAG_RXNE);
			}

			usart1RecvBuff[usart1RecvLen] = USART_ReceiveData(USART1) & 0xFF;
			usart1RecvLen++;
			usart1_deal_with_recved_buff();
		}
}


void usart1_send_test_msg(void)
{
	u16 i = 0;
	usart1SendLen = 8;
	for (i = 0; i < 8; i++) {
		usart1SendBuff[i] = i + 1;
	}

	usart1_send_buff_process();
}










