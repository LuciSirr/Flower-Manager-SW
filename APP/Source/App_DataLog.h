/*
 * App_DataLog.h
 *
 * Created: 23. 6. 2019 21:51:58
 *  Author: Miso
 */ 


#ifndef APP_DATALOG_H_
#define APP_DATALOG_H_

#define  START_OFFSET		3	
#define  HEADER_INFO		3		// Index, Day, Month		
#define  TIME_RECORDS		24*2	// 24hour * 2bytes
#define  DAY_RECORD_LENGHT	(HEADER_INFO+TIME_RECORDS)

#define  EEPROM_SIZE	1024

typedef union
{
	uint8_t AllData[DAY_RECORD_LENGHT];
	struct
	{
		uint8_t DayIndex;
		uint8_t Day;
		uint8_t Month;
		uint8_t Temp[TIME_RECORDS];
	}Record;

}uDayRecord;

void AppData_Init(void);
void AppData_Record(uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t temp, uint8_t temp_dec);
uint8_t AppData_GetIndex(void);
uint16_t AppData_GetTotalRecords(void);
uint8_t AppData_GetDayRecord(uint8_t DayIdx, uDayRecord* data);

#endif /* APP_DATALOG_H_ */