
#include "main.h"
#include "usart.h"


//************************************************************************************************************
// Erase all pages
//************************************************************************************************************
uint8_t boot_erase_flash()
{
	uint8_t sreg;
	// Disable interrupts.
	sreg = SREG;
	cli();
	eeprom_busy_wait ();

	for(uint32_t page=0; page<BOOT_SECTION_START; page+=0x80)
	{
		boot_page_erase (page);
		boot_spm_busy_wait ();      // Wait until the memory is erased.
	}

	// Re-enable RWW-section again. We need this if we want to jump back
	// to the application after boot loading.
	boot_rww_enable ();
	// Re-enable interrupts (if they were ever enabled).
	SREG = sreg;

	return TRUE;
}

//************************************************************************************************************
// Program Page
//************************************************************************************************************
void boot_program_page (uint32_t page, uint8_t *buf)
{
	uint16_t i;
	uint8_t sreg;

	sreg = SREG;				// Disable interrupts.
	cli();
	eeprom_busy_wait ();
	boot_spm_busy_wait ();      // Wait until the memory is erased.
	
	for (i=0; i<SPM_PAGESIZE; i+=2)
	{
		// Set up little-endian word.
		uint16_t w = *buf++;
		w += (*buf++) << 8;
		
		boot_page_fill (page + i, w);
	}
	boot_page_write (page);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.

	// Re-enable RWW-section again. We need this if we want to jump back
	// to the application after boot loading.
	boot_rww_enable ();
	// Re-enable interrupts (if they were ever enabled).
	SREG = sreg;
}

//************************************************************************************************************
// Calculate flash CRC till BOOT_SECTION_START - 0x10
//************************************************************************************************************
uint8_t Calculate_Flash_CRC8(void)	// Valid only up to 16kByte of memory!
{
	uint8_t ucCalculatedCRC=0;
	
	for(uint16_t address=0; address<(BOOT_SECTION_START-0x10); address++)
	{
		ucCalculatedCRC ^=  pgm_read_byte((void *)address);
	}

	return ucCalculatedCRC;
}

//************************************************************************************************************
// Check CRC, if matching with CRC on specific application address -> jump to application
//************************************************************************************************************
uint8_t JumpToApplication()
{
	if(Calculate_Flash_CRC8() == pgm_read_byte((void *)APP_INFO_ADDRESS+APP_CRC_OFFST))
	{
		cli();

		USART_DeInit();

		asm ( "jmp 0x0000" );

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//************************************************************************************************************
// main.c
//************************************************************************************************************
int main ( void )
{
	// Disable Watchdog, important for Watchdog reset
	MCUSR = 0;
	wdt_disable();

	// Communication protocol
	sComData sSerialData;

	// Timer variable to start application
	uint8_t  uAppStartupTimer = 0;
	
	// Bootloader related variables
	uint8_t  PageBytes[ 0x80 ];
	uint8_t  uiPageByteIndex = 0;
	uint32_t PageStartAddress = 0;

	// Bootload Info LED
	//LED_INFO_INIT;

	// Initialize application specific GPIO
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

	// Turn On Info LED
	//LED_ON;

	USART_Init(UBRR_Value);		// USART initialization 

	while(1)
	{
		if(USART_ReceiveMessage(&sSerialData))
		{
			switch (sSerialData.uiCommand)
			{
				// Read application information from specific memory sector
				case 'I':

					// Master is waiting for 'I' response
					USART_SendCommand( APP_INF_LENGHT, 'I');

					for (uint16_t i = APP_INFO_ADDRESS; i < (APP_INFO_ADDRESS + APP_INF_LENGHT); i ++ )
					{
						USART_SendByte(pgm_read_byte((void *)i));
					}
					break;

				// Erase whole MCU memory 
				case 'E':

					//LED_ON;
					
					if(boot_erase_flash())
					{
						USART_SendCommand(0,'S');
					}
					
					//LED_OFF;
					break;

				// Page line received
				case 'P':

					for (uint16_t i = 0; i < sSerialData.uiMsgLenght; i ++ )
					{
						PageBytes[uiPageByteIndex+i]=sSerialData.uiData[i];
					}
					uiPageByteIndex+=sSerialData.uiMsgLenght;
					break;

				// Write received page to specific address
				case 'W':

					if(uiPageByteIndex == 0x80)
					{
						//LED_ON;

						// Decode page address
						PageStartAddress  = 0;
						PageStartAddress |= (uint32_t)sSerialData.uiData[0] << 24;
						PageStartAddress |= (uint32_t)sSerialData.uiData[1] << 16;
						PageStartAddress |= (uint32_t)sSerialData.uiData[2] <<  8;
						PageStartAddress |= (uint32_t)sSerialData.uiData[3] <<  0;

						// Check if the address is not out of available memory range 
						if(PageStartAddress >= BOOT_SECTION_START) break;

						// Write page
						boot_program_page(PageStartAddress, &PageBytes[0]);

						// Master is waiting for 'S' response 
						USART_SendCommand(0,'S');

						//LED_OFF;
					}
					
					uiPageByteIndex = 0;
					break;

				// Jump to application, if everything successful you should not see blinking LED
				case 'X':

					JumpToApplication();
					break;
			}
			
			uAppStartupTimer=0;
		}
		else
		{
			uAppStartupTimer++;
		}

		_delay_ms(1);
		
		if(uAppStartupTimer == 0xFF)
		{
			JumpToApplication();

			//LED_TOGGLE;
		}
	}

}

