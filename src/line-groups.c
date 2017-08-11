#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "string-array.h"
#include "util.h"
#include "line-groups.h"

int* heket_get_line_group_lengths(HeketStringArray lines)
{
	int line_count = lines.len;
	int* group_lengths = malloc(sizeof(int) * line_count);

	group_lengths[0] = 0;

	int group_idx = 0;

	int i = 0;
	int previous_indents = -1;

	while (i < lines.len) {
		char* line = lines.values[i];

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

		i++;
	}

	return group_lengths;
}

HeketLineGroups heket_group_lines(HeketStringArray lines)
{
	int* group_lengths = heket_get_line_group_lengths(lines);
	int i = 0;
	int group_count = 0;

	while (i < lines.len) {
		if (group_lengths[i] == 0) {
			break;
		}

		group_count++;
		i++;
	}

	int bytes = group_count * sizeof(HeketStringArray);
	HeketStringArray* groups = malloc(bytes);

	int line_idx = 0;
	int groups_idx = 0;

	while (line_idx < lines.len) {
		int group_len = group_lengths[groups_idx];

		assert(group_len != 0);

		char** group = malloc(sizeof(char*) * group_len);
		int group_idx = 0;

		while (group_idx < group_len) {
			group[group_idx] = lines.values[line_idx];
			line_idx++;
			group_idx++;
		}

		HeketStringArray wrapped_group = {
			values: group,
			len:    group_len
		};

		groups[groups_idx] = wrapped_group;
		groups_idx++;
	}

	HeketLineGroups result = {
		groups: groups,
		len:    group_count
	};

	return result;
}
