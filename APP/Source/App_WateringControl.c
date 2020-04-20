/*
 * WateringControl.c
 *
 * Created: 30. 6. 2019 20:49:24
 *  Author: Miso
 */ 
 #include "main.h"
 #include "App_WateringControl.h"
 #include "App_TimingBlocks.h"
 #include "rtc_ds1307.h"
 #include "App_DataLog.h"

uint8_t WateringStateMachineState = NONE;
uDayRecord DayRecordData;
uint16_t day_average1;
uint16_t day_average2;

bool WateringTrigger = FALSE;

// Tomato 1 - 15*C - 40s, 35*C - 320s
WaterOutput_t Output_1 = {
	.Enabled      = TRUE,
 	.ValvePORTx   = (uint8_t*)&VALVE1_PORT,
 	.ValveGate    = VALVE1_GATE,
 	.ValveOpen    = FALSE,
	.Const_K      = 14,
	.Const_Q      = 170,
 	.WateringTime = 200
};

// Tomato 2 - 15*C - 20s, 35*C - 200s		
WaterOutput_t Output_2 = {
	.Enabled      = TRUE,
	.ValvePORTx   = (uint8_t*)&VALVE2_PORT,
	.ValveGate    = VALVE2_GATE,
	.ValveOpen	  = FALSE,
	.Const_K      = 9,
	.Const_Q      = 115,
	.WateringTime = 200
};

// Tomato 2 - 15*C - 20s, 35*C - 200s
WaterOutput_t Output_3 = {
	.Enabled      = TRUE,
	.ValvePORTx  = (uint8_t*)&VALVE3_PORT,
	.ValveGate   = VALVE3_GATE,
	.ValveOpen  = FALSE,
	.Const_K = 9,
	.Const_Q = 115,
	.WateringTime = 200
};

// Tomato 2 - 15*C - 40s, 35*C - 200s
WaterOutput_t Output_4 = {
	.Enabled      = TRUE,
	.ValvePORTx  = (uint8_t*)&VALVE4_PORT,
	.ValveGate   = VALVE4_GATE,
	.ValveOpen	 = FALSE,
	.Const_K = 9,
	.Const_Q = 115,
	.WateringTime = 200
};

// Kohlrabi - 15*C - 30s, 35*C - 190s
WaterOutput_t Output_5 = {
	.Enabled      = TRUE,
	.ValvePORTx  = (uint8_t*)&VALVE5_PORT,
	.ValveGate   = VALVE5_GATE,
	.ValveOpen	 = FALSE,
	.Const_K = 8,
	.Const_Q = 90,
	.WateringTime = 200
};

 void App_UpdateWateringTime(uint16_t DayAvgTemp, WaterOutput_t *Output)
 {
	Output->WateringTime = ((DayAvgTemp*Output->Const_K)/10 - Output->Const_Q)*10;	// *10 - 0.1s every countdown cycle
 }



 void App_WaterThePlants(uint8_t day_hour, uint8_t minute)
 {	
	static bool MorningWateringDone = FALSE;
	static bool EveningWateringDone = FALSE;

 	// Morning Plants Watering
	if((day_hour == 0x08) & (minute == 0x10))
	{
		if(MorningWateringDone == FALSE)
		{
			day_average1 = App_CalculateDayAverage(MORNING_WATERING_HOUR);

			App_UpdateWateringTime(day_average1, &Output_1);
			App_UpdateWateringTime(day_average1, &Output_2);
			App_UpdateWateringTime(day_average1, &Output_3);
			App_UpdateWateringTime(day_average1, &Output_4);
			App_UpdateWateringTime(day_average1, &Output_5);

			WateringTrigger = TRUE;		// Time to Water the flowers
			MorningWateringDone = TRUE;
		}
	}
	else
	{
		MorningWateringDone = FALSE;
	}

	// Evening Plants Watering
	if((day_hour == 0x20) & (minute == 0x10))
	{
		if(EveningWateringDone == FALSE)
		{
			day_average2 = App_CalculateDayAverage(EVENING_WATERING_HOUR);

			App_UpdateWateringTime(day_average2, &Output_1);
			App_UpdateWateringTime(day_average2, &Output_2);
			App_UpdateWateringTime(day_average2, &Output_3);
			App_UpdateWateringTime(day_average2, &Output_4);
			App_UpdateWateringTime(day_average2, &Output_5);

			WateringTrigger = TRUE;		// Time to Water the flowers
			EveningWateringDone = TRUE;
		}
	}
	else
	{
		EveningWateringDone = FALSE;
	}
 }


  uint16_t App_CalculateDayAverage(uint8_t day_hour)
  {
	  uint8_t  Temp = 0;
	  uint8_t  TempDecimals = 0;
	  uint16_t TempAverage = 0;
	  uint8_t  DayIndex = AppData_GetIndex();

	  AppData_GetDayRecord(DayIndex, &DayRecordData);

	  for (uint8_t time= day_hour - TEMP_AVERAGE_PERIOD; time < day_hour; time++)
	  {
		  Temp		 = DayRecordData.Record.Temp[2*time + 0];
		  TempDecimals = DayRecordData.Record.Temp[2*time + 1];

		  if((Temp<100) & (TempDecimals<10))	// Skip sample if corrupted
		  {
			  TempAverage += Temp*10;
			  TempAverage += TempDecimals;
		  }
	  }
	  
	  TempAverage = TempAverage/TEMP_AVERAGE_PERIOD;

	  if(TempAverage>400)
	  {
		  TempAverage = 400;
	  }
	  else if(TempAverage<150)
	  {
		  TempAverage = 150;
	  }

	  return TempAverage;
  }


 void App_WateringControl(void)
 {	
	static uint8_t timer_boost = 0;

	switch (WateringStateMachineState)
	{
		case NONE:
			if(WateringTrigger == TRUE)
			{
			   WateringStateMachineState = START_BOOST;
		  	}
			break;

		case START_BOOST:
			BOOST_ENABLE;
			if(CycleDelay(&timer_boost, 2))					// 200ms cycle time, wait for situation settle down
			{
				WateringStateMachineState = OUTPUT_1;
			}
			break;

		case OUTPUT_1:
			if(App_ControlOutputs(&Output_1))
			{
				WateringStateMachineState = OUTPUT_2;
			}
			break;

		case OUTPUT_2:
			if(App_ControlOutputs(&Output_2))
			{
				WateringStateMachineState = OUTPUT_3;
			}
			break;

		case OUTPUT_3:
			if(App_ControlOutputs(&Output_3))
			{
				WateringStateMachineState = OUTPUT_4;
			}
			break;

		case OUTPUT_4:
			if(App_ControlOutputs(&Output_4))
			{
				WateringStateMachineState = OUTPUT_5;
			}
			break;

		case OUTPUT_5:
			if(App_ControlOutputs(&Output_5))
			{
				WateringStateMachineState = STOP_BOOST;
			}
			break;

		case STOP_BOOST:		// Watering sequence done
			BOOST_DISABLE;
			WateringTrigger = FALSE;			
			WateringStateMachineState = NONE;
			break;
	}
}


 uint8_t App_ControlOutputs(WaterOutput_t *output_x)
 {
 	static uint16_t ValveStateMachine = 0;
	static uint16_t WateringTimer = 0;
	static uint8_t  delay_timer = 0;

	switch (ValveStateMachine)
	{
		case ENABLE_CHECK:
			if(output_x->Enabled != TRUE)
			{
				return TRUE;
			}
			ValveStateMachine = OPEN_VALVE;
			break;

		case OPEN_VALVE:					// Initialize counter and open the Valve
			WateringTimer = output_x->WateringTime;					
			SET_BIT(*output_x->ValvePORTx, output_x->ValveGate);
			output_x->ValveOpen = TRUE;

			VALVE_PWM_REG = 255;						
			ValveStateMachine = VALVE_REDUCE_P;
			break;

		case VALVE_REDUCE_P:				// Reduce duty cycle to 30% save power 
			VALVE_PWM_REG = 80;							
			ValveStateMachine = WAIT1;
			break;

		case WAIT1:							// 200ms cycle time, wait for situation settle down
			if(CycleDelay(&delay_timer, 2))				
			{
				ValveStateMachine = START_PUMP;
			}
			break;

		case START_PUMP:					// Start Pump full power									
			PUMP_PWM_REG = 180;	
			ValveStateMachine = WATERING_TIME;
			break;

		case WATERING_TIME:					// Flower watered, proceed with turn offs										
			WateringTimer--;

			if(WateringTimer == 0)					
			{
				ValveStateMachine = STOP_PUMP;
			}
			break;

		case STOP_PUMP:						// Stop Pump									
			PUMP_PWM_REG = 0;
			ValveStateMachine = CLOSE_VALVE;
			break;

		case CLOSE_VALVE:					// Close valve									
			CLEAR_BIT(*output_x->ValvePORTx, output_x->ValveGate);	
			output_x->ValveOpen = FALSE;
			VALVE_PWM_REG = 0; 
			ValveStateMachine = FINAL;
			break;

		case FINAL:							// 500ms cycle time, wait for situation settle down	
			if(CycleDelay(&delay_timer, 5))				
			{
				ValveStateMachine = ENABLE_CHECK;
				return TRUE;
			}
			break;
	}

	return FALSE;
 }



