/*
 * CharParser.h
 *
 * Created: 28. 2. 2018 21:02:49
 *  Author: Miso
 */ 


#ifndef CHARPARSER_H_
#define CHARPARSER_H_

//#include <stdint.h>
//
//#include <stdbool.h>
//
//bool is_digit(char ch);
//
//bool parse_digit(char ch, uint8_t *out);
//
//bool scan_for_uint(char *str, char **first, char **last);
//

// Warning: Does not detect overflow!  Don't parse anything over 255;
bool parse_uint8(char *str, uint8_t *out);
//
//// Warning: Does not detect overflow!  Don't parse anything over 65535;
//bool parse_uint16(char *str, uint16_t *out);
//
//// Warning: Does not detect overflow!  Don't parse anything over 4294967295;
//bool parse_uint32(char *str, uint32_t *out);


#endif /* CHARPARSER_H_ */