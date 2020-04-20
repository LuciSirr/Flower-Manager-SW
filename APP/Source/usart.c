/*
 * usart.c
 *
 * Created: 4. 2. 2018 21:16:58
 *  Author: Miso
 */ 
 #include "main.h"
 #include <avr/io.h>
 #include <util/delay.h>
 #include <avr/interrupt.h>
 #include <stdint.h>
 #include <string.h>
 #include "usart.h"
 #include "error.h"
  

 char chSerial_Buffer[SERIAL_BUFFER_LENGHT];
 uint8_t uWrIndx=0;
 uint8_t uRdIndx=0;
 uint8_t uLenght=0;

 uint8_t uReceiverOverflow=0;
 

  ISR(USART0_RX_vect)
  {
	  while ((UCSR0A & (1<<RXC0)))
	  {
		  if(uLenght<(SERIAL_BUFFER_LENGHT-1))
		  {
			  chSerial_Buffer[uWrIndx]=UDR0;
			  uWrIndx++;
			  uLenght++;
		  }
		  else
		  {
			  //flush buffer - receiver overflow - set fault and read buffer
			  volatile uint8_t uScrappedSample;
			  uReceiverOverflow=1;
			  serial_write_str(" too long\n");
			  while ( UCSR0A & (1<<RXC0) ) uScrappedSample = UDR0;
 			  
			  uWrIndx=0;	//Reset FIFO buffer 
			  uRdIndx=0;
			  uLenght=0;

			  return;
		  }
	  }
  }

 /* SETUP UART */
 void USART_Init( unsigned int ubrr)
 {
	 /*Set baud rate */
	 UBRR0H = (unsigned char)(ubrr>>8);
	 UBRR0L = (unsigned char)ubrr;
	 
	 /*Enable receiver and transmitter, RX interrupt */
	 UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);	
	 
	 /* Set frame format: 8data, 1stop bit */
	 UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);	
 }
 

 const char *sentinels = "\r\n|";

 bool is_sentinel(char ch)
 {
	 for (size_t i = 0; i < strlen(sentinels); i++)
	 {
		 if (ch == sentinels[i])
		 {
			 return TRUE;
		 }
	 }

	 return FALSE;
 }

 void serial_write_str(char *buff)
 {
	 uint8_t len = strlen(buff);
	 
	 for (size_t i = 0; i < len; i++)
	 {
		 if(buff[i]==0) return;
		 UDR0 = buff[i];
		 while ( !( UCSR0A & (1<<UDRE0)));
	 }
 }

 void CleanBuffer(char *pbuff)
 {
	 uint8_t len = strlen(pbuff);
	 
	 for (size_t i = 0; i < len; i++)
	 {
		 pbuff[i]=0;
	 }
 }


  bool SerialData_available()
  {
	  //if (! (UCSR0A & (1 << RXC0)))
	  //{
		  //return FALSE;
	  //}
	  //else
	  //{
		  //return TRUE;
	  //}

	  if(uLenght) return TRUE;
	  else return FALSE;
  }
  

  char Serial_Read(void)
  {
	//return (char)UDR0;
	
	  char chReturnChar=0;

  	  if(uLenght)
  	  {
	  	  chReturnChar = chSerial_Buffer[uRdIndx];
//
		  //UDR0 = chReturnChar;
		  //while ( !( UCSR0A & (1<<UDRE0)));

	  	  chSerial_Buffer[uRdIndx]=0;

	  	  uRdIndx++;
	  	  uLenght--;
  	  }
	  return chReturnChar;
  }

  //bool SerialData_available_CircleBuff()
  //{
	  //while(uLenght)
	  //{
		  //UDR0 = chSerial_Buffer[uRdIndx];
		  //while ( !( UCSR0A & (1<<UDRE0)));
//
		  //chSerial_Buffer[uRdIndx]=0;
//
		  //uRdIndx++;
		  //uLenght--;
	  //}
//
	  //serial_write_str("\n");
//
	  //return 0;
  //}


// Read from serial until '\n', writing to a buffer.
uint8_t read_serial_line(char *pbuffer)
{
	uint32_t TestBlink=0;
	uint8_t num_chars_consumed = 0;
	uint8_t has_read_first_char = FALSE;

	if(SerialData_available() == 0)
	{
		return OK_NO_MESSAGE;
	}

	while (TRUE)
	{
		// busy-wait for serial data to become available
		while (SerialData_available() == 0)
		{
			TestBlink++;

			if(TestBlink>50000)
			{
				TestBlink=0;
				
				//LED_LSTN_TOGGLE;
				//LED_TALK_TOGGLE;
			}

			continue;
		}

		*pbuffer = Serial_Read();

		// throw away any leading \n \r garbage leftover from the previous line
		if ((is_sentinel(*pbuffer)) & (has_read_first_char == FALSE)) //has_read_first_char == 0
		{
			continue;
		}
		
		has_read_first_char = TRUE;
		
		num_chars_consumed++;

		if (is_sentinel(*pbuffer))
		{
			*pbuffer = '\0';
			return OK_NO_ERROR;
		}
		else if (num_chars_consumed == SERIAL_BUFFER_LENGHT-1)
		{

			*pbuffer = '\0';
			return ERROR_BUFFER_FILLED_UP_BEFORE_SENTINEL_REACHED;
		}
		else
		{
			pbuffer++;
			continue;
		}
	}
}