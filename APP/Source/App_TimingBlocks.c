/*
 * App_TimingBlocks.c
 *
 * Created: 24. 8. 2019 21:15:34
 *  Author: Miso
 */ 
#include "main.h"
#include "App_TimingBlocks.h"

bool CycleDelay(uint8_t *counter_value, uint8_t couter_max)
{
	if(*counter_value == couter_max)
	{
		*counter_value = 0;
		return TRUE;
	}
	else
	{
		*counter_value = *counter_value + 1;
		return FALSE;
	}
}