#ifndef HEKET_RULESET_H
#define HEKET_RULESET_H

#include "rule.h"

typedef struct HeketRuleset {

	HeketRule* rules;
	int rule_count;

} HeketRuleset;

HeketRuleset heket_ruleset_from_abnf(const char *abnf);
HeketRuleset heket_ruleset_from_filepath(const char *filepath);

HeketRule get_first_ruleset_rule(HeketRuleset ruleset);

#endif
