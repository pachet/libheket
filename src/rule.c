#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "string-array.h"
#include "rule.h"

static char* remove_line_comment(char* line) {
	int within_quoted_str = 0;
	int i = 0;
	int len = strlen(line);

	while (i < len) {
		char character = line[i];

		if (character == 0x22) {
			within_quoted_str = !within_quoted_str;
		}

		if (character == 0x3B && !within_quoted_str) {
			char* result = malloc(sizeof(char) * i);
			strncpy(result, line, i);
			return result;
		}

		i++;
	}

	return line;
}

static HeketStringArray remove_line_comments(HeketStringArray lines)
{
	int i = 0;
	char** cleaned_strings = malloc(sizeof(char*) * lines.len);

	while (i < lines.len) {
		cleaned_strings[i] = remove_line_comment(lines.values[i]);
		i++;
	}

	HeketStringArray result = {
		len:    lines.len,
		values: cleaned_strings
	};

	return result;
}

HeketRule heket_rule_from_lines(HeketStringArray lines) {
	HeketRule result;

	HeketStringArray cleaned_lines = remove_line_comments(lines);

	char* definition = heket_string_array_to_string(cleaned_lines);

	printf(">> %s\n", definition);

	return result;
}
