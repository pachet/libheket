#include <stdlib.h>
#include <string.h>
#include "string-array.h"
#include "util.h"
#include "line-groups.h"

int* heket_get_line_group_lengths(HeketStringArray lines)
{
	int line_count = lines.len;
	int* group_lengths = malloc(sizeof(int) * line_count);

	group_lengths[0] = 0;

	int group_idx = 0;

	int idx = 0;
	char* line = lines.values[idx];
	int previous_indents = -1;

	while (line) {
		if (strlen(line) == 0) {
			continue;
		}

		int indents = count_indents(line);

		if (indents > previous_indents) {
			group_lengths[group_idx]++;

			if (previous_indents == -1) {
				previous_indents = indents;
			}

		} else {
			group_idx++;
			group_lengths[group_idx] = 1;
			previous_indents = indents;
		}

		idx++;
		line = lines.values[idx];
	}

	return group_lengths;
}

HeketLineGroups heket_group_lines(HeketStringArray lines)
{
	HeketLineGroups result;

	int* group_lengths = heket_get_line_group_lengths(lines);

	return result;
}
