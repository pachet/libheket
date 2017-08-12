#include <stdbool.h>
#include "parser.h"

bool add_ruleset_to_parser(HeketRuleset ruleset, HeketParser parser)
{
	return true;
}

HeketParser heket_parser_from_abnf(const char* abnf)
{
	HeketParser parser;

	add_ruleset_to_parser(CORE_RULESET, parser);

	HeketRuleset ruleset = heket_ruleset_from_abnf(abnf);
	add_ruleset_to_parser(ruleset, parser);

	return parser;
}


