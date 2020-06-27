/*
 * App_WateringControl.h
 *
 * Created: 30. 6. 2019 20:49:52
 *  Author: Miso
 */ 


#ifndef APP_WATERINGCONTROL_H_
#define APP_WATERINGCONTROL_H_

#define MORNING_WATERING_HOUR	8	// hour of the day
#define EVENING_WATERING_HOUR	20	// hour of the day

#define TEMP_AVERAGE_PERIOD		8	// hours, should be less than MORNING_WATERING_HOUR!


#define BOOST_ENABLE	((PORTC) |=  (1<<PORTC3))
#define BOOST_DISABLE   ((PORTC) &= ~(1<<PORTC3))
#define PUMP_ENABLE		((PORTB) |=  (1<<PORTB4))
#define PUMP_DISABLE	((PORTB) &= ~(1<<PORTB4))

#define PUMP_PWM_REG		OCR0B
#define PUMP_PWM_DDR		DDRB
#define PUMP_PWM_GATE		PORTB4

#define VALVE_PWM_REG		OCR0A
#define VALVE_PWM_DDR		DDRB
#define VALVE_PWM_GATE		PORTB3

#define VALVE1_PORT			PORTB
#define VALVE1_GATE			PORTB2

#define VALVE2_PORT			PORTA
#define VALVE2_GATE			PORTA0

#define VALVE3_PORT			PORTA
#define VALVE3_GATE			PORTA2

#define VALVE4_PORT			PORTA
#define VALVE4_GATE			PORTA4

#define VALVE5_PORT			PORTA
#define VALVE5_GATE			PORTA6


typedef enum _WaterStates_t {
	NONE		= 0,
	START_BOOST	= 1,
	OUTPUT_1	= 2,
	OUTPUT_2	= 3,
	OUTPUT_3	= 4,
	OUTPUT_4	= 5,
	OUTPUT_5	= 6,	
	STOP_BOOST	= 7,
} WaterStates_t;

typedef enum _OutputStates_t {
	ENABLE_CHECK	= 0,
	OPEN_VALVE		= 1,
	VALVE_REDUCE_P	= 2,
	WAIT1			= 3,
	START_PUMP		= 4,
	PUMP_REDUCE_P	= 5,
	WATERING_TIME	= 6,
	STOP_PUMP		= 7,
	CLOSE_VALVE		= 8,
	FINAL			= 9
} OutputStates_t;


typedef struct
{
	uint8_t	 Enabled;
	uint8_t	 *ValvePORTx;
	uint8_t	 ValveGate;
	uint8_t	 ValveOpen;
	uint16_t Const_K;	
	uint16_t Const_Q;	
	uint16_t WateringTime;		// Time x*100ms 
}WaterOutput_t;

extern bool WateringTrigger;

 void App_WaterThePlants(uint8_t day_hour, uint8_t minute);
 void App_WateringControl(void);
 uint16_t App_CalculateDayAverage(uint8_t day_hour);
 uint8_t App_ControlOutputs(WaterOutput_t *output_x);

#endif /* APP_WATERINGCONTROL_H_ */