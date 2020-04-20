/*
 * button.c
 *
 * Created: 26. 5. 2019 19:03:59
 *  Author: Miso
 */ 

 #include "button.h"
  #include "main.h"

 void check_button(button_t *button)
 {
	if((*button->pin_reg &= (1<<button->pin_number)) == button->polarity)	
	{
		if(button->on_state_delay_cnt > button->on_state_delay)
		{
			if(!button->pressed)
			{
				button->pressed = TRUE;
				button->released = FALSE;
				button->off_state_delay_cnt = 0;
				
				button->single_evnt|=1;
				button->latch_evnt^= 1;
			}
		}
		else
		{
			button->on_state_delay_cnt++;
		}
	}
	else
	{
		if(button->off_state_delay_cnt > button->off_state_delay)
		{
			if(!button->released)
			{
				button->pressed = FALSE;
				button->released = TRUE;
				button->on_state_delay_cnt = 0;
			}
		}
		else
		{
			button->off_state_delay_cnt++;
		}
	}
 }