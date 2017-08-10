#include "heket.h"
#include "ruleset.h"

const struct HeketAPI heket = {
	.ruleset_from_abnf = heket_ruleset_from_abnf
};
