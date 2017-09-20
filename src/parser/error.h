#ifndef _PARSER_ERROR_H_ 
#define _PARSER_ERROR_H_

#include <stdio.h>

typedef unsigned char parser_error_t; 

#define PARSER_SUCESS								0
#define PARSER_ERROR_PARSE_FILE			1
#define PARSER_ERROR_XPATH_CONTEXT 	2
#define PARSER_ERROR_EVALUATE_XPATH	3
#define PARSER_ERROR_BOUNDS_NUMBER	4
#define PARSER_ERROR_BOUNDS_TYPE	5

extern void fprint_parser_error(FILE* stream, const parser_error_t error);
extern void print_parser_error(const parser_error_t error);

#endif /* _PARSER_ERROR_H_ */
