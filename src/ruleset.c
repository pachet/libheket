#include "ruleset.h"

HeketRuleset heket_ruleset_from_abnf(const char *abnf) {
	HeketRuleset ruleset;

	ruleset.abnf = abnf;

	return ruleset;
}
