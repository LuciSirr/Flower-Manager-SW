/*
 * App_Timer.h
 *
 * Created: 21. 6. 2019 23:26:44
 * Author : Miso
 */ 

#ifndef TIMER_H_
#define TIMER_H_

#define TIMER_MAX_FCN		3

typedef void (*T_TimedEvent)(uint16_t uiTick);

void AppTimer_Init(void);
void AppTimer_Update(void);

uint16_t Timer_Register10msTimedEvent(T_TimedEvent pFunction);
uint16_t Timer_UnRegister10msTimedEvent(T_TimedEvent pFunction);
uint16_t Timer_Register100msTimedEvent(T_TimedEvent pFunction);
uint16_t Timer_UnRegister100msTimedEvent(T_TimedEvent pFunction);
uint16_t Timer_Register1sTimedEvent(T_TimedEvent pFunction);
uint16_t Timer_UnRegister1sTimedEvent(T_TimedEvent pFunction);

extern uint16_t ulTmr10ms;
extern uint16_t ulTmr100ms;
extern uint16_t ulTmr1000ms;

extern uint16_t uiTmr10msTick;
extern uint16_t uiTmr100msTick;
extern uint16_t uiTmr1000msTick;

#define TMR_PRESCALED	(F_CPU/256)

#define TIME_10MS		(TMR_PRESCALED/100)
#define TIME_100MS		(TMR_PRESCALED/10)
#define TIME_1000MS		(TMR_PRESCALED/1)

#endif /*TIMER_H_*/
