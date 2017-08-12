#include <stdbool.h>
#include "parser.h"

bool add_ruleset_to_parser(HeketRuleset ruleset, HeketParser parser)
{
	return true;
}

HeketRuleset get_core_ruleset()
{
	char* core_rules_abnf = "\
ALPHA          =  %x41-5A / %x61-7A   ; A-Z / a-z\
\
BIT            =  \"0\" / \"1\"\
";

	HeketRuleset core_rules = heket_ruleset_from_abnf(core_rules_abnf);

	return core_rules;
}

HeketParser heket_parser_from_ruleset(HeketRuleset ruleset)
{
	HeketParser parser;

	add_ruleset_to_parser(get_core_ruleset(), parser);
	add_ruleset_to_parser(ruleset, parser);

	return parser;
}

HeketParser heket_parser_from_abnf(const char* abnf)
{
	HeketRuleset ruleset = heket_ruleset_from_abnf(abnf);

	return heket_parser_from_ruleset(ruleset);

}

HeketParser heket_parser_from_filepath(const char* filepath)
{
	HeketRuleset ruleset = heket_ruleset_from_filepath(filepath);

	return heket_parser_from_ruleset(ruleset);
}


