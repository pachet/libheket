#include "heket.h"
#include "ruleset.h"
#include "parser.h"

const struct HeketAPI heket = {
	.ruleset_from_abnf = heket_ruleset_from_abnf,
	.ruleset_from_filepath = heket_ruleset_from_filepath,
	.parser_from_filepath = heket_parser_from_filepath,
	.parse = heket_parse
};
