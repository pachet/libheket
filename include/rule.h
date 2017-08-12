#ifndef HEKET_RULE_H
#define HEKET_RULE_H

#include "node.h"
#include "string-array.h"

typedef struct HeketRule {

	char* name;
	HeketNode node;

} HeketRule;

HeketRule heket_rule_from_lines(HeketStringArray lines);
HeketParseResult parse_text_with_rule(const char* text, HeketRule rule);

#endif
