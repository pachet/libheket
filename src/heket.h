#ifndef HEKET_H
#define HEKET_H

#include "ruleset.h"

typedef struct HeketAPI {

	HeketRuleset (*ruleset_from_abnf)();
	HeketRuleset (*ruleset_from_filepath)();

} HeketAPI;

extern const struct HeketAPI heket;

#endif /* HEKET_H */
