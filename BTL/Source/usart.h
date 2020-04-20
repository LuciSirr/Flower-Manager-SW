/*
 * usart.h
 *
 * Created: 4. 2. 2018 21:17:14
 *  Author: Miso
 */ 


#ifndef USART_H_
#define USART_H_

extern void		USART_Init( unsigned int ubrr);
extern void		USART_DeInit(void);
extern void		USART_SendByte(uint8_t u8Data);
extern void		USART_SendCommand(uint8_t uiLenght, uint8_t uiCommand);
extern uint8_t	USART_ReceiveMessage(sComData *ComData);


#endif /* USART_H_ */