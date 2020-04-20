/*
 * button.h
 *
 * Created: 26. 5. 2019 19:04:10
 *  Author: Miso
 */ 


#ifndef BUTTON_H_
#define BUTTON_H_

#include "stdutils.h"

/***************************************************************************************************
                                 Struct/Enums used
***************************************************************************************************/
typedef struct
{
  // Settings
  uint8_t polarity;	// 1 - active high, 0 - active low
  uint8_t on_state_delay;
  uint8_t off_state_delay;

  // Active variables
  volatile uint8_t *pin_reg;
  uint8_t pin_number;
  uint8_t pressed;
  uint8_t released;

  uint8_t on_state_delay_cnt;
  uint8_t off_state_delay_cnt;

  // Result
  uint8_t single_evnt;
  uint8_t latch_evnt;

}button_t;

 void check_button(button_t *button);


#endif /* BUTTON_H_ */