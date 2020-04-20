/*
 * main.h
 *
 * Created: 4. 2. 2018 21:20:08
 *  Author: Miso
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 8000000UL  // 8 MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

// Boolean extension
#define TRUE	1
#define FALSE	0

// Bootloader start address in bytes memory = 2 * 0x3E00 (words)
#define BOOT_SECTION_START	0x7C00	

// Application informations stored in flash 
#define APP_INFO_ADDRESS	0x7BF0
#define APP_INF_LENGHT		15
#define APP_CRC_OFFST		15

/* UART SERIAL DEFINES */
#define BAUDRATE 9600
#define UBRR_Value (F_CPU/16/BAUDRATE-1)
#define MAX_MESSAGE_LENGHT	18

// LEDs macros
//#define LED_INFO_INIT	DDRB|= (1<<PORTB5)
//#define LED_ON			PORTB|=  (1<<PORTB5)
//#define LED_OFF			PORTB&= ~(1<<PORTB5)
//#define LED_TOGGLE		PORTB^=  (1<<PORTB5)

// Global variables
typedef struct
{
	uint8_t uiCommand;
	uint8_t uiMsgLenght;
	uint8_t uiData[16];
}sComData;

#endif /* MAIN_H_ */