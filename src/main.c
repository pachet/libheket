#include <stdio.h>
#include "heket.h"
#include "parser.h"

int main() {

	HeketParser parser = heket.parser_from_filepath("./test.abnf");

	printf("result was: %s\n", "xxx");

	return 0;
}

