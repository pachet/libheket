#include <stdio.h>
#include "heket.h"

int main() {

	HeketRuleset ruleset = heket.ruleset_from_abnf("foo = \"bar\" | \"baz\"");

	printf("result was: %s\n", ruleset.abnf);

	return 0;
}

