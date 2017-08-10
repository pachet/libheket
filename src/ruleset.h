#ifndef HEKET_RULESET_H
#define HEKET_RULESET_H

typedef struct HeketRuleset {

	const char* abnf;

} HeketRuleset;

HeketRuleset heket_ruleset_from_abnf(const char *abnf);

#endif
