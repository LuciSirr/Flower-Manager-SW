/*
 * App_LCD.h
 *
 * Created: 23. 6. 2019 21:20:44
 *  Author: Miso
 */ 


#ifndef APP_LCD_H_
#define APP_LCD_H_

#define LCD_BACKLIGHT_TIMEOUT 80


typedef enum _MainMenuItems_t {
	LCD_INFO_Screen	= 1,
	LCD_OUTPUT_1	= 2,
	LCD_OUTPUT_2	= 3,
	LCD_OUTPUT_3	= 4,
	LCD_OUTPUT_4	= 5,
	LCD_OUTPUT_5	= 6,
} MainMenuItems_t;


 void AppLCD_Init(void);
 void AppLCD_DisplayTime(uint8_t startx, uint8_t starty, uint8_t rtc_hour, uint8_t rtc_min, uint8_t rtc_sec);
 void AppLCD_DisplayTemp(uint8_t startx, uint8_t starty, uint8_t temp, uint8_t temp_decimals);
 void AppLCD_Menu(uint8_t *Ok,uint8_t *No,uint8_t *Up,uint8_t *Down);
 void AppLCD_BootloadScreen(void);

#endif /* APP_LCD_H_ */