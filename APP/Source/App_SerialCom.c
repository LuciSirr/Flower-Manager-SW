/*
 * App_SerialCom.c
 *
 * Created: 23. 6. 2019 21:14:28
 *  Author: Miso
 */ 
 #include "main.h"
 #include "error.h"
 #include "usart.h"
 #include "App_DataLog.h"
 #include "App_LCD.h"
 #include "CharParser.h"

void AppSerial_PrintError(error_t err)	// TODO add ring buffer overflow error
{
	switch (err)
	{
		case OK_NO_MESSAGE:
		case OK_NO_ERROR:
		return;
		case ERROR_BUFFER_FILLED_UP_BEFORE_SENTINEL_REACHED:
		serial_write_str("Error: buffer filled up before sentinel reached.\n");
		break;
		case ERROR_UNKNOWN_COMMAND:
		serial_write_str("Error: unknown command.\n");
		break;
		case ERROR_MALFORMED_COMMAND:
		serial_write_str("Error: malformed command.\n");
		break;
		case ERROR_UNKNOWN_BUS_LINE:
		serial_write_str("Error: unknown bus line.\n");
		break;
		case ERROR_INVALID_HEX:
		serial_write_str("Error: invalid hex value.\n");
		break;
	}
}



// Returns FW version
bool try_ver_cmd(char *buff)
{
	if (strncmp(buff, "++ver", strlen("++ver")) != 0)
	{
		return FALSE;
	}

	serial_write_str(FIRMWARE_VERSION);
	return TRUE;
}

// Jumps to bootloader
bool try_jump_to_boot(char *buff)
{
	if (strncmp(buff, "++boot", strlen("++boot")) != 0)
	{
		return FALSE;
	}

	AppLCD_BootloadScreen();	// Time to say goodbye

	cli();

	wdt_enable(WDTO_15MS);

	while(TRUE);				// Goodbye Application, watchdog creates SW reset.

	return TRUE;
}

// Returns Current index
bool try_read_idx(char *buff)
{
 	if (strncmp(buff, "++idx", strlen("++idx")) != 0)
	{
		return FALSE;
	}

	char number[10];
	serial_write_str("Day Index: ");
	sprintf(number, "%d", AppData_GetIndex());
	serial_write_str(number);
	serial_write_str("\n");

	return TRUE;
}

// Returns all records
bool try_read_rcrd(char *buff)
{
	if (strncmp(buff, "++rcd", strlen("++rcd")) != 0)
	{
		return FALSE;
	}

	char number[10];
	serial_write_str("Total Records: ");
	sprintf(number, "%d", AppData_GetTotalRecords());
	serial_write_str(number);
	serial_write_str("\n");

	return TRUE;
}


// Returns Day report
bool try_read_day_rec(char *buff)
{
	if (strncmp(buff, "++day", strlen("++day")) != 0)
	{
	  return FALSE;
	}

	uDayRecord DayRecord;
	
	buff += strlen("++day ");  
	uint8_t address = 0;	//Multimeter Address
	parse_uint8(buff, &address);

	AppData_GetDayRecord(address, &DayRecord);

	char number[10];
	serial_write_str("DayIndex: ");
	sprintf(number, "%d", DayRecord.Record.DayIndex);
	serial_write_str(number);
	serial_write_str("\n");

	serial_write_str("Date: ");
	sprintf(number, "%d", DayRecord.Record.Day);
	serial_write_str(number);
	serial_write_str(".");
	sprintf(number, "%d", DayRecord.Record.Month);
	serial_write_str(number);
	serial_write_str(".");
	serial_write_str("\n");

	for(uint8_t hour=0; hour<24; hour++)
	{
		sprintf(number, "%d", hour);
		serial_write_str(number);
		serial_write_str("h ");
		sprintf(number, "%d", DayRecord.Record.Temp[hour*2]);
		serial_write_str(number);
		serial_write_str(".");
		sprintf(number, "%d", DayRecord.Record.Temp[hour*2+1]);
		serial_write_str(number);
		serial_write_str("*C");
		serial_write_str("\n");
	}
	 return TRUE;
}






 // Parse and Run command
 bool AppSerial_RunCMD(char *pbuff)
 {
	 if (strncmp(pbuff, "++", 2) == 0)
	 {
		 if (try_ver_cmd(pbuff))		{ return TRUE; }
		 if (try_jump_to_boot(pbuff))	{ return TRUE; }
		 if (try_read_idx(pbuff))		{ return TRUE; }
		 if (try_read_rcrd(pbuff))		{ return TRUE; }
		 if (try_read_day_rec(pbuff))	{ return TRUE; }
	 }

	 return FALSE;
 }