#ifndef HEKET_PARSER_H
#define HEKET_PARSER_H

#include <stdbool.h>
#include "ruleset.h"

typedef struct HeketParser {

	HeketRuleset* rulesets;

} HeketParser;

HeketParser heket_parser_from_abnf(const char* abnf);
HeketParser heket_parser_from_filepath(const char* filepath);
bool add_ruleset_to_parser(HeketRuleset ruleset, HeketParser parser);

#endif
