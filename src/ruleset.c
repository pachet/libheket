#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ruleset.h"
#include "util.h"
#include "string-array.h"
#include "line-groups.h"
#include "rule.h"

int heket_add_rule_to_ruleset(HeketRule rule, HeketRuleset ruleset)
{
	return 0;
}


HeketRuleset heket_ruleset_from_lines(HeketStringArray lines)
{
	HeketRuleset ruleset;

	if (!lines.len) {
		return ruleset;
	}

	HeketLineGroups line_groups = heket_group_lines(lines);

	int i = 0;

	while (i < line_groups.len) {
		HeketStringArray line_group = line_groups.groups[i];
		i++;

		HeketRule rule = heket_rule_from_lines(line_group);

		if (!heket_add_rule_to_ruleset(rule, ruleset)) {
			// TODO: cleanup
		}
	}

	return ruleset;
}



HeketRuleset heket_ruleset_from_abnf(const char* abnf)
{
	HeketStringArray lines = str_split(abnf, 0x0A);

	return heket_ruleset_from_lines(lines);
}

HeketRuleset heket_ruleset_from_filepath(const char* filepath)
{
	char* contents = NULL;

	FILE *fh = fopen(filepath, "r");

	assert(fh != NULL);

	size_t len;
	ssize_t bytes_read = getdelim(&contents, &len, 0x00, fh);

	assert(bytes_read != -1);

	return heket_ruleset_from_abnf(contents);
}




void heket_free_ruleset(HeketRuleset ruleset) {
}
