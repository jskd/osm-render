#include "error.h"

void fprint_parser_error(FILE* stream, const parser_error_t error){
	switch(error) {
		case PARSER_SUCESS:
			fprintf(stream, "Parse sucess\n");	
    break;
		case PARSER_ERROR_PARSE_FILE:
			fprintf(stream, "Error: unable to parse file\n");	
    break;
		case PARSER_ERROR_XPATH_CONTEXT :
			fprintf(stream, "Error: unable to create new XPath context\n");	
		break; 
		case PARSER_ERROR_EVALUATE_XPATH :
			fprintf(stream, "Error: unable to evaluate xpath expression\n");	
		break; 
		default: 
			fprintf(stream, "Error: unknown\n");	
		break; 
	}
}

void print_parser_error(const parser_error_t error){
	fprint_parser_error(stderr, error);
}

