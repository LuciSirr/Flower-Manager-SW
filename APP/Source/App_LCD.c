/*
 * App_LCD.c
 *
 * Created: 23. 6. 2019 21:20:53
 *  Author: Miso
 */ 
 #include "main.h"
 #include "lcd_hd44780.h"  // do not forget define the ports in header file.
 #include "App_TimingBlocks.h"
 #include "App_LCD.h"

 bool LCD_BacklightStatus = FALSE;
 uint8_t LCD_Menu0_Index = LCD_INFO_Screen;
 uint8_t LCD_PreviousMenu0_Index = 0;
 uint8_t LCD_Menu1_Index = 1;
 uint8_t LCD_PreviousMenu1_Index = 0;
 uint8_t LCD_MainLevel_Index = 0;

 
 void AppLCD_Init()
 {
 	// Enable LCD Back light after startup
 	LCD_BacklightStatus = TRUE;
 	BACKLIGHT_ENABLE;

 }

 void AppLCD_MenuLevel0()
 {

 	if(LCD_Menu0_Index != LCD_PreviousMenu0_Index)
 	{
	 	lcd_clrscr();

	 	switch(LCD_Menu0_Index)
	 	{
		 	case LCD_INFO_Screen:
		 	//AppLCD_DisplayTime();
		 	//AppLCD_DisplayTemp();
		 	break;

		 	case LCD_OUTPUT_1:
		 	lcd_puts("OUTPUT 1");
		 	break;

		 	case LCD_OUTPUT_2:
		 	lcd_puts("OUTPUT 2");
		 	break;

		 	case LCD_OUTPUT_3:
		 	lcd_puts("OUTPUT 3");
		 	break;

		 	case LCD_OUTPUT_4:
		 	lcd_puts("OUTPUT 4");
		 	break;

		 	case LCD_OUTPUT_5:
		 	lcd_puts("OUTPUT 5");
		 	break;
	 	}
	 	LCD_PreviousMenu0_Index= LCD_Menu0_Index;
 	}
 }


 void AppLCD_MenuLevel1()
 {
 	if(LCD_Menu1_Index != LCD_PreviousMenu1_Index)
 	{
	 	lcd_clrscr();

	 	switch(LCD_Menu1_Index)
	 	{
		 	case 1:
		 	lcd_puts("0");
		 	break;

		 	case 2:
		 	lcd_puts("1");
		 	break;

			case 3:
			lcd_puts("2");
			break;
	 	}
	 	LCD_PreviousMenu1_Index = LCD_Menu1_Index;
 	}
 }


 void AppLCD_Menu(uint8_t *Ok,uint8_t *No,uint8_t *Up,uint8_t *Down)
 {
	static uint8_t LCD_BacklightTimeout = 0;

	if(LCD_BacklightStatus == TRUE)
	{
		if(CycleDelay(&LCD_BacklightTimeout, LCD_BACKLIGHT_TIMEOUT))
		{
			LCD_BacklightStatus = FALSE;
			BACKLIGHT_DISABLE;
			lcd_clrscr(); // Clean screen to avoid pixels burn out
			LCD_Menu0_Index = 1;
			LCD_Menu1_Index = 1;
			LCD_MainLevel_Index = 0;
		}
	}

	if(*Ok | *No | *Up | *Down)
	{
		if(LCD_BacklightStatus == TRUE)
		{
			switch(LCD_MainLevel_Index)
			{
				case 0:
					if((*Up   == TRUE) & (LCD_Menu0_Index < 5)) LCD_Menu0_Index++;
					if((*Down == TRUE) & (LCD_Menu0_Index > 1)) LCD_Menu0_Index--;
					if((*Ok   == TRUE) & (LCD_Menu0_Index > 1)) 
					{
						LCD_PreviousMenu1_Index = 0; // force lcd refresh
						LCD_MainLevel_Index = 1;	//Jump to sub level
						AppLCD_MenuLevel1();
						break;
					}
					if((*No   == TRUE)) LCD_Menu0_Index = 1;
				 
					AppLCD_MenuLevel0();
					break;

				case 1:
					if((*Up   == TRUE) & (LCD_Menu1_Index < 3)) LCD_Menu1_Index++;
					if((*Down == TRUE) & (LCD_Menu1_Index > 1)) LCD_Menu1_Index--;
					if((*No   == TRUE)) 
					{
						LCD_Menu1_Index = 1;
						LCD_PreviousMenu0_Index = 0; // force lcd refresh
						LCD_MainLevel_Index = 0;	 // jump to sub level
						AppLCD_MenuLevel0();
						break;
					} 

					AppLCD_MenuLevel1();
					break;
			}
		}

		// Handle LCD Backlight
		LCD_BacklightStatus = TRUE;
		BACKLIGHT_ENABLE;

		// Clear button events
		*Ok = FALSE;
		*No = FALSE;
		*Up = FALSE;
		*Down = FALSE;
	}

 }


 void AppLCD_DisplayTime(uint8_t startx, uint8_t starty, uint8_t rtc_hour, uint8_t rtc_min, uint8_t rtc_sec)
 {
	if(LCD_Menu0_Index != LCD_INFO_Screen) return; 
	if(LCD_BacklightStatus == FALSE) return;
		
	char buffer[7];
	lcd_gotoxy(startx,starty);

	itoa((rtc_hour & 0xF0)>>4, buffer, 10);
	lcd_puts(buffer);
	itoa(rtc_hour & 0x0F, buffer, 10);
	lcd_puts(buffer);
	lcd_putc(':');

	itoa((rtc_min & 0xF0)>>4, buffer, 10);
	lcd_puts(buffer);
	itoa(rtc_min & 0x0F, buffer, 10);
	lcd_puts(buffer);
	lcd_putc(':');

	itoa((rtc_sec & 0xF0)>>4, buffer, 10);
	lcd_puts(buffer);
	itoa(rtc_sec & 0x0F, buffer, 10);
	lcd_puts(buffer);
	
 }

 void AppLCD_DisplayTemp(uint8_t startx, uint8_t starty, uint8_t temp, uint8_t temp_decimals)
 {
	if(LCD_Menu0_Index != LCD_INFO_Screen) return; 
	if(LCD_BacklightStatus == FALSE) return;	

	char buffer[7];
	lcd_gotoxy(startx,starty);
	 
	itoa(temp, buffer, 10);
	lcd_puts(buffer);
	lcd_putc('.');
	itoa(temp_decimals, buffer, 10);
	lcd_puts(buffer);
	//lcd_putc('*'); //TODO: Special char
	lcd_puts("degC");
 }


  void AppLCD_BootloadScreen()
  {
	lcd_clrscr();
	lcd_puts(" BOOTLOADING...");
  }

