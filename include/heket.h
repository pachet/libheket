#ifndef HEKET_H
#define HEKET_H

#include "ruleset.h"
#include "parser.h"

typedef struct HeketAPI {

	HeketRuleset (*ruleset_from_abnf)();
	HeketRuleset (*ruleset_from_filepath)();
	HeketParser (*parser_from_filepath)();
	HeketParseResult (*parse)();

} HeketAPI;

extern const struct HeketAPI heket;

#endif
