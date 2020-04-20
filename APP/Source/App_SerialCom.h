/*
 * App_SerialCom.h
 *
 * Created: 23. 6. 2019 21:14:53
 *  Author: Miso
 */ 


#ifndef APP_SERIALCOM_H_
#define APP_SERIALCOM_H_

void AppSerial_PrintError(error_t err);
bool AppSerial_RunCMD(char *pbuff);

#endif /* APP_SERIALCOM_H_ */