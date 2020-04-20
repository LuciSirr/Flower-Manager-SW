/*
 * App_DataLog.c
 *
 * Created: 23. 6. 2019 21:51:46
 *  Author: Miso
 */ 

 #include "main.h"
 #include "App_DataLog.h"

 uint8_t  ucRecordIndex = 0;
 uint16_t uiTotalRecords = 0;

void AppData_Init(void)
{
	ucRecordIndex  = eeprom_read_byte((uint8_t*)0x00);
	uiTotalRecords = eeprom_read_word((uint16_t*)0x01);

	if(ucRecordIndex == 0xFF)
	{
		ucRecordIndex = 0;
		eeprom_write_byte((uint8_t*)0x00, 0x00);
	}

	if(uiTotalRecords == 0xFFFF)
	{
		uiTotalRecords = 0;
		eeprom_write_word((uint16_t*)0x01, 0x0000);
	}
}

uint8_t AppData_GetIndex(void)
{
	return ucRecordIndex;	//return day index
}

uint16_t AppData_GetTotalRecords(void)
{
	return uiTotalRecords;	//return day index
}

uint8_t AppData_GetDayRecord(uint8_t DayIdx, uDayRecord* data)
{
	if((DayIdx*DAY_RECORD_LENGHT+7+48) > EEPROM_SIZE)
	{
		return FALSE;
	}
	
	for(uint16_t lenght=0; lenght < DAY_RECORD_LENGHT; lenght++)
	{
		data->AllData[lenght]=eeprom_read_byte((uint8_t*)(3+(DAY_RECORD_LENGHT*DayIdx)+lenght));
	}

	return TRUE;	//return day index
}

 void AppData_Record(uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t temp, uint8_t temp_dec)
 {
 	 static uint8_t DayRecord_Stored=FALSE;
	 static uint8_t HourRecord_Stored=FALSE;

	 if(min == 0x05) //at 5min
	 {
		if(HourRecord_Stored == FALSE)
		{
			HourRecord_Stored = TRUE;

			if(hour == 0)	// Flip Index - new day
			{	
				if(DayRecord_Stored == FALSE)
				{
					DayRecord_Stored = TRUE;
					ucRecordIndex++;
					uiTotalRecords++;

					if((ucRecordIndex*DAY_RECORD_LENGHT+7+48) > EEPROM_SIZE)
					{
						ucRecordIndex=0;
					}
			
					eeprom_write_byte((uint8_t*)0x00, ucRecordIndex);		// Store new index for next start
					eeprom_write_word((uint16_t*)0x01, uiTotalRecords);
				}
			}
			else
			{
				DayRecord_Stored = FALSE;
			}

			uint8_t DayInDecimal = (((day & 0xF0) >> 4) * 10) + (day & 0x0F);		// Convert from BCD to decimal
			uint8_t MonthInDecimal = (((month & 0xF0) >> 4) * 10) + (month & 0x0F);	// Convert from BCD to decimal	

			// Store Day index and data
			if((eeprom_read_byte((uint8_t*)(3+(DAY_RECORD_LENGHT*ucRecordIndex)+1)) != DayInDecimal) | (eeprom_read_byte((uint8_t*)(3+(DAY_RECORD_LENGHT*ucRecordIndex)+2)) != MonthInDecimal))
			{
				eeprom_write_byte((uint8_t*)(3+(DAY_RECORD_LENGHT*ucRecordIndex)+0), ucRecordIndex);
				eeprom_write_byte((uint8_t*)(3+(DAY_RECORD_LENGHT*ucRecordIndex)+1), DayInDecimal);		
				eeprom_write_byte((uint8_t*)(3+(DAY_RECORD_LENGHT*ucRecordIndex)+2), MonthInDecimal);
			}

			// Record temperature for corresponding hour

			uint8_t HourInDecimal = (((hour & 0xF0) >> 4) * 10) + (hour & 0x0F);	// Convert from BCD to decimal

			eeprom_write_byte((uint8_t*)(3+(DAY_RECORD_LENGHT*ucRecordIndex)+3+(2*HourInDecimal)), temp);
			eeprom_write_byte((uint8_t*)(3+(DAY_RECORD_LENGHT*ucRecordIndex)+4+(2*HourInDecimal)), temp_dec);
		}

	 }
	 else
	 {
		HourRecord_Stored = FALSE;
	 }
 }