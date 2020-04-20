/*
 * error.h
 *
 * Created: 3. 2. 2018 18:18:51
 *  Author: Miso
 */ 


#ifndef ERROR_H_
#define ERROR_H_


typedef enum _error_t {
	OK_NO_MESSAGE=0,
	OK_NO_ERROR=1,
	ERROR_BUFFER_FILLED_UP_BEFORE_SENTINEL_REACHED=2,
	ERROR_UNKNOWN_COMMAND=3,
	ERROR_MALFORMED_COMMAND=4,
	ERROR_UNKNOWN_BUS_LINE=5,
	ERROR_INVALID_HEX=6
} error_t;


#endif /* ERROR_H_ */