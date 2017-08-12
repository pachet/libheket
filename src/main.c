#include <stdio.h>
#include <assert.h>
#include "heket.h"
#include "parser.h"
#include "parse-result.h"

int main() {

	HeketParser parser = heket.parser_from_filepath("./test.abnf");
	assert(parser.ruleset_count == 2);
	HeketParseResult result = heket.parse("bar", parser);

	printf("result was: %s\n", "xxx");

	return 0;
}

