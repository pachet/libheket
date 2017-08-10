#ifndef HEKET_H
#define HEKET_H

#include "ruleset.h"

HeketRuleset heket_ruleset_from_abnf(const char* abnf);

typedef struct HeketAPI {

	HeketRuleset (*ruleset_from_abnf)();

} HeketAPI;

extern const struct HeketAPI heket;

#endif /* HEKET_H */
