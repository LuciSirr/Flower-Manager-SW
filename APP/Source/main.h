/*
 * main.h
 *
 * Created: 4. 2. 2018 21:20:08
 *  Author: Miso
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#define  F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stdutils.h"

/* UART SERIAL DEFINES */
#define BAUDRATE 9600
#define UBRR_Value F_CPU/16/BAUDRATE-1
#define SERIAL_BUFFER_LENGHT 128

// App informations stored from 0x3DF8*2 in flash
#define APP_NAME		"FlowerMng."
#define APP_NAME_LENGHT 10

// Defines
#define FIRMWARE_VERSION "FW Version: X.Y.Z\n"

#define SET_BIT(x,y)    ((x) |=  (1<<y))
#define CLEAR_BIT(x,y)  ((x) &= ~(1<<y))
#define TOGGLE_BIT(x,y) ((x) ^=  (1<<y))
/***** Test Inputs *****/
#define GET_BIT(x,y) ((x) &= (1<<y))
//#define is_low(bit)                         (! (bit ## _PIN & (1 << bit)))

// GPIO Control
#define BACKLIGHT_ENABLE	((PORTC) |=  (1<<PORTC2))
#define BACKLIGHT_DISABLE	((PORTC) &= ~(1<<PORTC2))

 // boolean extension
typedef uint8_t bool;

#endif /* MAIN_H_ */