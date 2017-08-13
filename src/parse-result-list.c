#include "parse-result.h"
#include "parse-result-list.h"

bool append_parse_result_to_list(
	HeketParseResult parse_result,
	HeketParseResultList parse_result_list
)
{
}

bool append_empty_parse_result_to_list(HeketParseResultList result_list)
{
	HeketParseResult empty_result = create_empty_parse_result();

	return append_parse_result_to_list(empty_result, result_list);
}

bool truncate_parse_result_list(HeketParseResultList result_list, int len)
{
}

HeketParseResult combine_parse_result_list(HeketParseResultList result_list)
{
}
