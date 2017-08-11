#ifndef HEKET_RULE_H
#define HEKET_RULE_H

#include "node.h"

typedef struct HeketRule {

	char* name;
	HeketNode node;

} HeketRule;

HeketRule heket_rule_from_lines(HeketStringArray lines);

#endif
