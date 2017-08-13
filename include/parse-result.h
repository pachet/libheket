#ifndef HEKET_PARSE_RESULT_H
#define HEKET_PARSE_RESULT_H

typedef enum {

	ERROR_CODE_NONE,
	ERROR_CODE_INPUT_TOO_LONG

} HeketErrorCode;

typedef struct HeketParseResult {

	const char* text;
	HeketErrorCode error_code;

} HeketParseResult;

#endif
