#ifndef HEKET_PARSE_RESULT_LIST_H
#define HEKET_PARSE_RESULT_LIST_H

#include "rule.h"

typedef struct HeketParseResultList {

	HeketParseResult* parse_results;
	int result_count;

} HeketParseResultList;

bool append_empty_parse_result_to_list(HeketParseResultList result_list);
bool append_parse_result_to_list(
	HeketParseResult parse_result,
	HeketParseResultList parse_result_list
);
bool truncate_parse_result_list(HeketParseResultList result_list, int len);

HeketParseResult combine_parse_result_list(HeketParseResultList result_list);

#endif
