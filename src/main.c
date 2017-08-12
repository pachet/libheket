#include <stdio.h>
#include "heket.h"
#include "parser.h"
#include "parse-result.h"

int main() {

	HeketParser parser = heket.parser_from_filepath("./test.abnf");

	HeketParseResult result = heket.parse("bar", parser);

	printf("result was: %s\n", "xxx");

	return 0;
}

