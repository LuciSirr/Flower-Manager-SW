/*
 * App_Timer.c
 *
 * Created: 21. 6. 2019 23:26:44
 * Author : Miso
 */ 

#include "main.h"
#include "App_Timer.h"

static T_TimedEvent paFcn10ms[TIMER_MAX_FCN];	//!< Array of pointers to timed function. Pointed function if called every 10ms
static T_TimedEvent paFcn100ms[TIMER_MAX_FCN];	//!< Array of pointers to timed function. Pointed function if called every 100ms
static T_TimedEvent paFcn1s[TIMER_MAX_FCN];		//!< Array of pointers to timed function. Pointed function if called every 1s

static uint16_t uiCnt10ms;		//!< Amount of registered functions in array; for 10ms array
static uint16_t uiCnt100ms;		//!< Amount of registered functions in array; for 100ms array
static uint16_t uiCnt1000ms;	//!< Amount of registered functions in array; for 1s array

uint16_t ulTmr10ms   = 0;
uint16_t ulTmr100ms	 = 0;
uint16_t ulTmr1000ms = 0;

uint16_t uiTmr10msTick   = 0;
uint16_t uiTmr100msTick  = 0;
uint16_t uiTmr1000msTick = 0;

void AppTimer_Init(void)
{
/**
 * \brief Timer initialization, clear all timers and arrays with timed functions
 */
	uint8_t ucTemp;
	
	// Initialize Timer 1 to generate time steps
	TCCR1A = 0x00;
	TCCR1B = (1<<CS12);		// Timer prescaler /256

	for (ucTemp = 0; ucTemp < TIMER_MAX_FCN; ucTemp++)
	{
		paFcn10ms[ucTemp] = 0;
		paFcn100ms[ucTemp] = 0;
		paFcn1s[ucTemp] = 0;	
	}

	uiCnt10ms = 0;
	uiCnt100ms = 0;
	uiCnt1000ms = 0;
}

void AppTimer_Update(void)
{
/**
 * \brief Timer update. Call timed function if is time to call it.
 */	
		uint8_t ucTemp;
 		volatile uint16_t uiTime;

		uiTime = TCNT1;

		if ((uiTime - ulTmr10ms) > TIME_10MS )
		{
			ulTmr10ms = uiTime;
			uiTmr10msTick = (uiTmr10msTick + 1) % 10000;
			for (ucTemp = 0; ucTemp < uiCnt10ms; ucTemp++)
			{
				paFcn10ms[ucTemp](uiTmr10msTick);
			}
		}
		
		if ((uiTime - ulTmr100ms) > TIME_100MS )
		{
			ulTmr100ms = uiTime;
			uiTmr100msTick = (uiTmr100msTick + 1) % 10000;
			for (ucTemp = 0; ucTemp < uiCnt100ms; ucTemp++)
			{
				paFcn100ms[ucTemp](uiTmr100msTick);
			}
		}
		
		if ((uiTime - ulTmr1000ms) > TIME_1000MS )
		{
			ulTmr1000ms = uiTime;
			uiTmr1000msTick = (uiTmr1000msTick + 1) % 10000;
			for (ucTemp = 0; ucTemp < uiCnt1000ms; ucTemp++)
			{
				paFcn1s[ucTemp](uiTmr1000msTick);				
			}
		}
}

uint16_t Timer_Register10msTimedEvent(T_TimedEvent pFunction)
{
/**
 * \brief Register function on 10ms timer. Registered function is called every 10ms
 * \param pFunction Pointer on timed function
 * \return 0 if function was not registered ,because there is registered too many functions
 * \return 1 if function was registered successfully 
 */
	if (uiCnt10ms < TIMER_MAX_FCN)
	{
		paFcn10ms[uiCnt10ms++] = pFunction;
	}
	else
	{
		return 0;
	}
	return 1;
}

uint16_t Timer_UnRegister10msTimedEvent(T_TimedEvent pFunction)
{
/**
 * \brief Unregister function from 10ms function queue
 * \param pFunction Pointer on timed function
 * \return 0 if function was not found in queue
 * \return count of unregistered functions 
 */
uint16_t uiTmp;
uint16_t uiFound = 0;
 
 	for (uiTmp = 0; uiTmp < uiCnt10ms; uiTmp++)
 	{
 		if (paFcn10ms[uiTmp] == pFunction)
 		{ 		
 			uiCnt10ms--;	
 			paFcn10ms[uiTmp] = paFcn10ms[uiCnt10ms];
 			uiFound++;
 		}
 	}
 		
	return uiFound;
}

uint16_t Timer_Register100msTimedEvent(T_TimedEvent pFunction)
{
/**
 * \brief Register function on 100ms timer. Registered function is called every 100ms
 * \param pFunction Pointer on timed function
 * \return 0 if function was not registered ,because there is registered too many functions
 * \return 1 if function was registered successfully 
 */
	if (uiCnt100ms < TIMER_MAX_FCN)
	{
		paFcn100ms[uiCnt100ms++] = pFunction;
	}
	else
	{
		return 0;
	}
	return 1;
}

uint16_t Timer_UnRegister100msTimedEvent(T_TimedEvent pFunction)
{
/**
 * \brief Unregister function from 10ms function queue
 * \param pFunction Pointer on timed function
 * \return 0 if function was not found in queue
 * \return count of unregistered functions
 */
uint16_t uiTmp;
uint16_t uiFound = 0;

 	for (uiTmp = 0; uiTmp < uiCnt100ms; uiTmp++)
 	{
 		if (paFcn100ms[uiTmp] == pFunction)
 		{
 			uiCnt100ms--;
 			paFcn100ms[uiTmp] = paFcn100ms[uiCnt10ms];
 			uiFound++;
 		}
 	}

	return uiFound;
}

uint16_t Timer_Register1sTimedEvent(T_TimedEvent pFunction)
{
/**
 * \brief Register function on 1s timer. Registered function is called every 1s
 * \param pFunction Pointer on timed function
 * \return 0 if function was not registered ,because there is registered too many functions
 * \return 1 if function was registered succesfuly 
 */

	if (uiCnt1000ms < TIMER_MAX_FCN)
	{
		paFcn1s[uiCnt1000ms++] = pFunction;		
	}
	else
	{
		return 0;
	}
	return 1;
}

uint16_t Timer_UnRegister1sTimedEvent(T_TimedEvent pFunction)
{
/**
 * \brief Unregister function from 1s function queue
 * \param pFunction Pointer on timed function
 * \return 0 if function was not found in queue
 * \return count of unregistered functions
 */
uint16_t uiTmp;
uint16_t uiFound = 0;

 	for (uiTmp = 0; uiTmp < uiCnt1000ms; uiTmp++)
 	{
 		if (paFcn1s[uiTmp] == pFunction)
 		{
 			uiCnt1000ms--;
 			paFcn1s[uiTmp] = paFcn1s[uiCnt1000ms];
 			uiFound++;
 		}
 	}

	return uiFound;
}
