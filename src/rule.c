#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "string-array.h"
#include "rule.h"
#include "parse-result.h"
#include "parse-state.h"

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

HeketRule heket_rule_from_lines(HeketStringArray lines)
{
	HeketRule result;

	HeketStringArray cleaned_lines = remove_line_comments(lines);

	char* definition = heket_string_array_to_string(cleaned_lines);

	int i = 0;
	int definition_len = strlen(definition);
	int name_start_idx = -1;
	int name_end_idx = -1;
	int body_start_idx = -1;

	while (i < definition_len) {
		char character = definition[i];

		if (character != 0x09 && character != 0x20) {
			if (name_start_idx == -1) {
				name_start_idx = i;
			} else {
				name_end_idx = i;
			}
		}

		if (character == 0x3D) {
			body_start_idx = i + 1;
			break;

		}

		i++;
	}

	assert(body_start_idx != -1);
	assert(name_start_idx != -1);
	assert(name_end_idx > name_start_idx);

	int name_len = name_end_idx - name_start_idx;
	int name_bytes = name_len * sizeof(char);
	char* name = malloc(name_bytes);
	name = strncpy(name, definition + name_start_idx, name_len);

	result.name = name;

	int body_len = definition_len - body_start_idx;
	int body_bytes = body_len * sizeof(char);
	char* body = malloc(body_bytes);
	body = strncpy(body, definition + body_start_idx, body_len);

	HeketNode node = heket_node_from_abnf(body);
	node.parent_rulename = name;

	result.node = node;

	return result;
}

HeketParseResult parse_text_with_rule(
	const char* text,
	HeketRule rule,
	bool allow_partial_match
)
{
	HeketParseState state = {
		allow_partial_match: allow_partial_match
	};

	return parse_text_with_node(text, rule.node, state);
}

