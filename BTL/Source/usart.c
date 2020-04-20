/*
 * usart.c
 *
 * Created: 4. 2. 2018 21:16:58
 *  Author: Miso
 */ 
#include "main.h"
#include "usart.h"


 /* SETUP UART */
 void USART_Init( unsigned int ubrr)
 {
	 /*Set baud rate */
	 UBRR0H = (unsigned char)(ubrr>>8);
	 UBRR0L = (unsigned char)ubrr;
	 
	 /*Enable receiver and transmitter, RX interrupt */
	 UCSR0B = (1<<RXEN0)|(1<<TXEN0);	
	 
	 /* Set frame format: 8data, 1stop bit */
	 UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);	
 }

void USART_DeInit(void)
{
	/* Clear all USART registers to default value */
	UCSR0C = 0;
	UCSR0B = 0;
	UBRR0H = 0;
	UBRR0L = 0;
}

void USART_SendByte(uint8_t u8Data)
{
	// Wait until last byte has been transmitted
	while((UCSR0A &(1<<UDRE0)) == 0);

	// Transmit data
	UDR0 = u8Data;
}

void USART_SendCommand(uint8_t uiLenght, uint8_t uiCommand)
{
	// Wait until last byte has been transmitted
	while((UCSR0A &(1<<UDRE0)) == 0);

	// Transmit data
	UDR0 = uiLenght;
	while((UCSR0A &(1<<UDRE0)) == 0);

	UDR0 = uiCommand;
	while((UCSR0A &(1<<UDRE0)) == 0);
}

uint8_t USART_ReceiveMessage(sComData *ComData)
{	
	uint8_t uiMessageStatus=0;
	uint8_t uiReceivedChars=0;
	uint16_t uiTimeOut=0;

	if((UCSR0A &(1<<RXC0)))	//Some message is on the way
	{
		ComData->uiMsgLenght = UDR0;

		if(ComData->uiMsgLenght <= MAX_MESSAGE_LENGHT)
		{
			uiMessageStatus = 1;
			
			while (1)
			{
				while((UCSR0A &(1<<RXC0)) == 0)	//Busy wait for new byte
				{
					_delay_us(100);

					uiTimeOut++;

					if(uiTimeOut == 1000)
					{
						return FALSE;	// No new byte arrives in reasonable time 
					}
				}

				switch(uiMessageStatus)
				{
					case 1:
						ComData->uiCommand = UDR0;
						
						if(0 == ComData->uiMsgLenght)
						{
							return TRUE;	//packet received in full length
						}

						uiMessageStatus = 2;
						break;

					//-> Todo CRC
					
					case 2:
						ComData->uiData[uiReceivedChars]= UDR0;
						uiReceivedChars++;

						if(uiReceivedChars == ComData->uiMsgLenght)
						{
							return TRUE;	//packet received in full length
						}
						break;
				}
			}
		}
		else
		{
			//Flush Incoming message
			while (1)
			{
				while((UCSR0A &(1<<RXC0)) == 0)	//Busy wait for new byte
				{
					_delay_us(100);

					uiTimeOut++;

					if(uiTimeOut == 100)
					{
						return FALSE;	// No new byte arrives in reasonable time
					}
				}

				uiTimeOut=UDR0;
				uiTimeOut=0;
			}
		}
	}
	

	return FALSE;
}