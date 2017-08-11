#include <stdio.h>
#include "heket.h"

int main() {

	// HeketRuleset ruleset = heket.ruleset_from_abnf("    foo = \"bar\" | \"baz\"\n    bam\n        baw");
	HeketRuleset ruleset = heket.ruleset_from_filepath("./test.abnf");

	printf("result was: %s\n", "xxx");

	return 0;
}

