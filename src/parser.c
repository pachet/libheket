#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "parser.h"
#include "rule.h"
#include "parse-result.h"
#include "parse-state.h"
#include "util.h"
#include "ruleset.h"

bool add_ruleset_to_parser(HeketRuleset ruleset, HeketParser* parser_ptr)
{
	int bytes = (parser_ptr->ruleset_count + 1) * sizeof(HeketRuleset);
	parser_ptr->rulesets = realloc(parser_ptr->rulesets, bytes);
	parser_ptr->rulesets[parser_ptr->ruleset_count] = ruleset;
	parser_ptr->ruleset_count++;

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
	HeketParser parser = {
		ruleset_count: 0
	};

	HeketParser* parser_ptr = &parser;

	assert(add_ruleset_to_parser(get_core_ruleset(), parser_ptr));
	assert(add_ruleset_to_parser(ruleset, parser_ptr));
	assert(parser.ruleset_count == 2);

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

HeketRuleset get_first_parser_ruleset(HeketParser parser)
{
	assert(parser.ruleset_count > 0);
	return parser.rulesets[0];
}

HeketRule get_first_parser_rule(HeketParser parser)
{
	HeketRuleset ruleset = get_first_parser_ruleset(parser);

	return get_first_ruleset_rule(ruleset);
}

HeketParseResult heket_parse(const char* text, HeketParser parser)
{
	HeketRule rule = get_first_parser_rule(parser);
	HeketParseResult parse_result = parse_text_with_rule(text, rule, false);

	return parse_result;
}


HeketParseResult parse_text_with_sequential_node(
	const char* text,
	HeketNode node,
	HeketParseState state
)
{
	char* original_string = str_copy(text);
	HeketParseResult result;

	return result;
}

HeketParseResult parse_text_with_alternative_node(
	const char* text,
	HeketNode node,
	HeketParseState state
)
{
	HeketParseResult result;

	return result;
}

HeketParseResult parse_text_with_optional_node(
	const char* text,
	HeketNode node,
	HeketParseState state
)
{
	HeketParseResult result;

	return result;
}

HeketParseResult parse_text_with_string_node(
	const char* text,
	HeketNode node,
	HeketParseState state
)
{
	HeketParseResult result;

	return result;
}

HeketParseResult parse_text_with_numeric_range_node(
	const char* text,
	HeketNode node,
	HeketParseState state
)
{
	HeketParseResult result;

	return result;
}

HeketParseResult parse_text_with_numeric_set_node(
	const char* text,
	HeketNode node,
	HeketParseState state
)
{
	HeketParseResult result;

	return result;
}

HeketParseResult parse_text_with_numeric_value_node(
	const char* text,
	HeketNode node,
	HeketParseState state
)
{
	HeketParseResult result;

	return result;
}

HeketParseResult parse_text_with_repeating_node(
	const char* text,
	HeketNode node,
	HeketParseState state
)
{
	HeketParseResult result;

	return result;
}

HeketParseResult parse_text_with_rule_node(
	const char* text,
	HeketNode node,
	HeketParseState state
)
{
	HeketParseResult result;

	return result;
}

HeketParseResult parse_text_with_node(
	const char* text,
	HeketNode node,
	HeketParseState state
)
{
	bool allow_partial_match = state.allow_partial_match;
	state.allow_partial_match = true;

	HeketParseResult result;

	switch (node.type) {
	case NODE_TYPE_SEQUENTIAL:
		result = parse_text_with_sequential_node(text, node, state);
		break;

	case NODE_TYPE_ALTERNATIVE:
		result = parse_text_with_alternative_node(text, node, state);
		break;

	case NODE_TYPE_OPTIONAL:
		result = parse_text_with_optional_node(text, node, state);
		break;

	case NODE_TYPE_STRING:
		result = parse_text_with_string_node(text, node, state);
		break;

	case NODE_TYPE_NUMERIC_RANGE:
		result = parse_text_with_numeric_range_node(text, node, state);
		break;


	case NODE_TYPE_NUMERIC_SET:
		result = parse_text_with_numeric_set_node(text, node, state);
		break;

	case NODE_TYPE_NUMERIC_VALUE:
		result = parse_text_with_numeric_value_node(text, node, state);
		break;

	case NODE_TYPE_REPEATING:
		result = parse_text_with_repeating_node(text, node, state);
		break;

	case NODE_TYPE_RULE:
		result = parse_text_with_rule_node(text, node, state);
		break;

	case NODE_TYPE_UNSPECIFIED:
	default:
		assert(false);
	}

	if (allow_partial_match == false && result.error_code == ERROR_CODE_NONE) {
		int actual_len = strlen(result.text);
		int expected_len = strlen(text);

		if (actual_len < expected_len) {
			HeketParseResult result = {
				text: text,
				error_code: ERROR_CODE_INPUT_TOO_LONG
			};
		}
	}

	return result;
}
