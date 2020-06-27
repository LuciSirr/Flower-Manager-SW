/*
 * FlowerManager.c
 *
 * Created: 21. 6. 2019 23:26:44
 * Author : Miso
 */ 
#include "main.h"

#include "usart.h"
#include "error.h"
#include "lcd_hd44780.h"  // do not forget define the ports in header file.
#include "rtc_ds1307.h"
#include "tmp_ds18b20.h"
#include "App_Timer.h"
#include "App_SerialCom.h"
#include "App_LCD.h"
#include "App_DataLog.h"
#include "App_WateringControl.h"
#include "button.h"

// --- Application info stored in Flash (last line before bootloader, here from 0x3DF8) ---
typedef struct
{
	uint8_t uAppNameLenght;
	uint8_t uAppNameString[14];
	uint8_t uAppCRC;
}sAppInfo ;
static volatile sAppInfo AppInformation __attribute__ ((section (".AppInfo"))) = {APP_NAME_LENGHT, APP_NAME, 0xCC}; // real CRC will be added by boot load application //0x48 is a real CRC, in future put to 0xFF

button_t	SW1_Ok;
button_t	SW2_No;
button_t	SW3_Up;
button_t	SW4_Down;
rtc_t		rtc_time;
ds18b20_t	temp_sensor;
char	    Serial_Buffer[SERIAL_BUFFER_LENGHT];




void RefreshApp(void)
{
	RTC_GetDateTime(&rtc_time);
	
	AppLCD_DisplayTime(4,0,rtc_time.hour,rtc_time.min,rtc_time.sec);

	DS18b20_GetTemp(&temp_sensor);
	
	AppLCD_DisplayTemp(4,1,temp_sensor.Temperature, temp_sensor.Temperature_Dec);

	AppData_Record(rtc_time.month,rtc_time.date,rtc_time.hour,rtc_time.min,temp_sensor.Temperature,temp_sensor.Temperature_Dec);

	App_WaterThePlants(rtc_time.hour,rtc_time.min);
}


void WateringControl(void)
{
	App_WateringControl();		
}

void RefreshScreen(void)
{
	AppLCD_Menu(&SW1_Ok.single_evnt,&SW2_No.single_evnt,&SW3_Up.single_evnt,&SW4_Down.single_evnt);		
}

void CheckButtons(void)
{
	check_button(&SW1_Ok);
	check_button(&SW2_No);
	check_button(&SW3_Up);
	check_button(&SW4_Down);
}

void Check_SerialMailbox()
{
	error_t err = read_serial_line(&Serial_Buffer[0]);

	if (err == OK_NO_MESSAGE)
	{
		return;
	}
	else if (err != OK_NO_ERROR)
	{
		AppSerial_PrintError(err);
		return;
	}
			
	if (!AppSerial_RunCMD(&Serial_Buffer[0]))
	{
		serial_write_str("Error/Timeout\n");
	}

		
	CleanBuffer(&Serial_Buffer[0]);
}

void RTC_SetTime(void)
{
	//Set time
	rtc_t rtc;
    rtc.hour = 0x13; //  20:03:15 
    rtc.min =  0x32;
    rtc.sec =  0x00;
    rtc.date = 0x27; //1st Jan 2016
    rtc.month = 0x06;
    rtc.year = 0x20;
    rtc.weekDay = 2; // Friday: 5th day of week considering monday as first day.
	///*##### Set the time and Date only once. Once the Time and Date is set, comment these lines
        //and re-flash the code. Else the time will be set every time the controller is reset*/
    RTC_SetDateTime(&rtc);  //  10:40:20 am, 1st Jan 2016

}

int main(void)
{
	uint8_t CompilerInfo = AppInformation.uAppCRC;	// Tell the linker that the AppInformation is not for fun

	// Disable Watchdog, important for Watchdog reset
	MCUSR = 0;
	wdt_disable();

	/*GPIO initialization */
	PORTA = 0x00;			// All Outputs Low
	DDRA |= (1 << PORTA0); 	// PA0 is now an output - VALVE2_EN
	DDRA |= (1 << PORTA2); 	// PA2 is now an output - VALVE3_EN
	DDRA |= (1 << PORTA4); 	// PA4 is now an output - VALVE4_EN
	DDRA |= (1 << PORTA6); 	// PA6 is now an output - VALVE5_EN

	PORTB = 0x00;			// All Outputs Low
	DDRB |= (1 << PORTB2); 	// PB0 is now an output - EN_VALVE1
	DDRB |= (1 << PORTB3); 	// PB0 is now an output - COMMON_PWM
	DDRB |= (1 << PORTB4); 	// PB1 is now an output - PUMP_ENABLE

	PORTC = 0x00;			// All Outputs Low
	DDRC |= (1 << PORTC2); 	// PC2 is now an output - EN_BACKLIGHT
	DDRC |= (1 << PORTC3); 	// PC3 is now an output - BOOST_ENABLE
	DDRC |= (1 << PORTC4); 	// PC4 is now an output - INRSH_ENABLE (INRUSH DONE)

	/*Disable Watchdog, important for Watchdog reset */
	MCUSR = 0;
	wdt_disable();

	/*USART bus initialization */
	USART_Init(UBRR_Value);

	/*Connect SCL->PC0, SDA->PC1*/
	RTC_Init();

	/*initialize display, cursor off*/
    lcd_init(LCD_DISP_ON);

	/*clear display and home cursor */
    lcd_clrscr();

	/* Initialize buttons */
	SW1_Ok.polarity = 0;			// Active low
	SW1_Ok.pin_reg = &PINB;
	SW1_Ok.pin_number = PINB0;
	SW1_Ok.on_state_delay = 5;		// x cycles of loop
	SW1_Ok.off_state_delay = 5;	// x cycles of loop

	SW2_No.polarity = 0;			// Active low
	SW2_No.pin_reg = &PINA;
	SW2_No.pin_number = PINA1;
	SW2_No.on_state_delay = 5;		// x cycles of loop
	SW2_No.off_state_delay = 5;	// x cycles of loop

	SW3_Up.polarity = 0;			// Active low
	SW3_Up.pin_reg = &PINA;
	SW3_Up.pin_number = PINA3;
	SW3_Up.on_state_delay = 5;		// x cycles of loop
	SW3_Up.off_state_delay = 5;	// x cycles of loop

	SW4_Down.polarity = 0;			// Active low
	SW4_Down.pin_reg = &PINA;
	SW4_Down.pin_number = PINA5;
	SW4_Down.on_state_delay = 5;		// x cycles of loop
	SW4_Down.off_state_delay = 5;	// x cycles of loop

	/*Watch dog enable*/
	wdt_enable(WDTO_2S);	
	
	/* Initialize TCCR0 as PWM generator */
	TCCR0A = (1 << COM0A1)|(1 << COM0B1)|(1 << WGM01)|(1 << WGM00);	// Fast PWM 8 Bit, Clear OCA0/OCB0 on Compare Match, Set on TOP	
	TCCR0B = (1 << CS00);											// No Prescaler
	TCNT0 = 0;														// Reset TCNT0
	OCR0A = 0;														// Initial the Output Compare register A & B
	OCR0B = 0;

	/*Init eeprom recording*/
	AppData_Init();

	/*initialize Application timer and register functions*/
	AppTimer_Init();

	Timer_Register1sTimedEvent((T_TimedEvent)&RefreshApp);
	Timer_Register100msTimedEvent((T_TimedEvent)&WateringControl);
	Timer_Register100msTimedEvent((T_TimedEvent)&RefreshScreen);
	Timer_Register10msTimedEvent((T_TimedEvent)&CheckButtons);
	Timer_Register10msTimedEvent((T_TimedEvent)&Check_SerialMailbox);

	/*initialize LCD screen*/
	AppLCD_Init();

	//for(uint16_t i=0; i<EEPROM_SIZE; i++)
	//{
		//eeprom_write_byte((uint8_t*)i, 0xFF);
	//}

	//RTC_SetTime();

	while(1)
	{
		AppTimer_Update();
		
		wdt_reset();	//Feed WatchDog
	};	
}







