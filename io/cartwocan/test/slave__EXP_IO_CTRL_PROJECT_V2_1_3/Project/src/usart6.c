#include "stm32f2XX_conf.h"
#include "usart6.h"
#include "main.h"

u16 usart6SendBuff[USART6_BUFF_LEN];
u16 usart6RecvBuff[USART6_BUFF_LEN];

u16 usart6RecvLen = 0;
u16 usart6SendLen = 0;






void usart6_send_buff_process(void)
{
	u16 i = 0;
	if (usart6SendLen > USART6_BUFF_LEN) {
		return;
	}

	UART6_TX_485;


	for (i = 0; i < usart6SendLen + 1; i++)
	{
		USART_SendData(USART6, ((usart6SendBuff[i]) & 0xFF));
		while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET) {};
	}

	UART6_RX_485;
        usart6RecvLen = 0;
}

void usart6_deal_with_recved_buff(void)
{
	u16 i = 0;
	u8 eor = 0;

	if (usart6RecvLen == 8) {
		for (i = 0; i < 8; i++) {
			usart6SendBuff[i] = usart6RecvBuff[i];
		}
                usart6SendBuff[0] = 6;
		usart6SendLen = 8;
		usart6_send_buff_process();
	}


}

void usart6_recv_buff_process(void)
{


		if (USART_GetFlagStatus(USART6, USART_FLAG_RXNE) != RESET) {
			if (usart6RecvLen >= (USART6_BUFF_LEN - 1)) {
				USART_ClearFlag(USART6, USART_FLAG_RXNE);

			}

			usart6RecvBuff[usart6RecvLen] = USART_ReceiveData(USART6) & 0xFF;
			usart6RecvLen++;
			usart6_deal_with_recved_buff();

		}

}


void usart6_send_test_msg(void)
{
	u16 i = 0;
	usart6SendLen = 8;
	for (i = 0; i < 8; i++) {
		usart6SendBuff[i] = i + 1;
	}

	usart6_send_buff_process();
}














