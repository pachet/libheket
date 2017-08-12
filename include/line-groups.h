#ifndef HEKET_LINE_GROUPS_H
#define HEKET_LINE_GROUPS_H

typedef struct HeketLineGroups {

	HeketStringArray* groups;
	int len;

} HeketLineGroups;

int* heket_get_line_group_lengths(HeketStringArray lines);

HeketLineGroups heket_group_lines(HeketStringArray lines);

#endif
