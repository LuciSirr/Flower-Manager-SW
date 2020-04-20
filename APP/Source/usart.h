/*
 * usart.h
 *
 * Created: 4. 2. 2018 21:17:14
 *  Author: Miso
 */ 


#ifndef USART_H_
#define USART_H_

extern char chSerial_Buffer[SERIAL_BUFFER_LENGHT];

extern void USART_Init( unsigned int ubrr);
extern bool SerialData_available(void);
extern char Serial_Read(void);
extern bool is_sentinel(char ch);
extern void serial_write_str(char *buff);
extern void CleanBuffer(char *pbuff);
extern uint8_t read_serial_line(char *pbuffer);

#endif /* USART_H_ */