#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "node.h"

typedef struct ParseResult {

	int len;
	HeketNode* node;

} ParseResult;

static HeketNode last_child_of_node(HeketNode parent_node)
{
	int child_count = parent_node.child_count;

	return parent_node.child_nodes[child_count - 1];
}

static int node_is_repeating(HeketNode node)
{
	if (node.min_repeats == 0 && node.max_repeats == 0) {
		return 0;
	}

	return 1;
}

static int character_within_range(char character, char start, char end)
{
	if (character < start) {
		return 0;
	}

	if (character > end) {
		return 0;
	}

	return 1;
}

static int add_child_to_node(HeketNode child_node, HeketNode parent_node)
{
	if (parent_node.child_count > 0) {
		HeketNode last_child = last_child_of_node(parent_node);

		if (node_is_repeating(last_child)) {
			child_node.min_repeats = last_child.min_repeats;
			child_node.max_repeats = last_child.max_repeats;

			parent_node.child_nodes[parent_node.child_count - 1] = child_node;
			free(&last_child);

			return 1;
		}
	}


	int bytes = (parent_node.child_count + 1) * sizeof(HeketNode);
	parent_node.child_nodes = realloc(parent_node.child_nodes, bytes);
	parent_node.child_nodes[parent_node.child_count] = child_node;
	parent_node.child_count++;

	return 0;
}

static char* slice_abnf_subset(const char* abnf, int start_pos, int end_pos)
{
	int subset_len = end_pos - start_pos;
	int subset_bytes = (subset_len + 1) * sizeof(char);
	char* abnf_subset = malloc(subset_bytes);
	strncpy(abnf_subset, abnf + start_pos, subset_len);

	return abnf_subset;
}

static ParseResult parse_group(const char* abnf, int offset)
{
	// Advance past the initial "(" starting parenthesis:
	offset++;

	int i = offset;
	int len = strlen(abnf);
	int parens_count = 0;

	while (i < len) {
		char token = abnf[i++];

		if (token == 0x28) {
			parens_count++;
		} else if (token == 0x29) {
			if (parens_count == 0) {
				break;
			}

			parens_count--;
		}
	}

	assert(parens_count == 0);

	char* abnf_subset = slice_abnf_subset(abnf, offset, i);

	HeketNode child_node = heket_node_from_abnf(abnf_subset);

	ParseResult result = {
		node: &child_node,
		len:  strlen(abnf_subset)
	};

	return result;
}

static ParseResult parse_optional(const char* abnf, int offset)
{
	// Advance past the initial "[" starting bracket:
	offset++;

	int i = offset;
	int len = strlen(abnf);
	int bracket_count = 0;

	while (i < len) {
		char token = abnf[i++];

		if (token == 0x5B) {
			bracket_count++;
		} else if (token == 0x5D) {
			if (bracket_count == 0) {
				break;
			}

			bracket_count--;
		}
	}

	assert(bracket_count == 0);

	char* abnf_subset = slice_abnf_subset(abnf, offset, i);
	HeketNode child_node = heket_node_from_abnf(abnf_subset);

	child_node.optional = 1;

	ParseResult result = {
		node: &child_node,
		len:  strlen(abnf_subset)
	};

	return result;
}

static ParseResult parse_string(const char* abnf, int offset)
{
	// Advance past the initial double quotes:
	offset++;

	int i = offset;
	int len = strlen(abnf);
	int found_end_quotes = 0;

	while (i < len) {
		char token = abnf[i++];

		if (token == 0x22) {
			found_end_quotes = 1;
			break;
		}
	}

	assert(found_end_quotes == 1);

	char* abnf_subset = slice_abnf_subset(abnf, offset, i);

	HeketNode child_node = {
		quoted_string: abnf_subset
	};

	ParseResult result = {
		node: &child_node,
		len:  0
	};

	return result;
}

static ParseResult parse_numeric(const char* abnf, int offset)
{
	ParseResult result = {
		node: NULL,
		len:  0
	};

	return result;
}

static ParseResult parse_repeat(const char* abnf, int offset)
{
	// NOTICE: We don't advance past the initial token in this case.

	int i = offset;
	int len = strlen(abnf);
	int asterisk_idx = 0;

	while (i < len) {
		char token = abnf[i++];

		if (token == 0x2A) {
			assert(asterisk_idx == -1);
			asterisk_idx = i;
		} else if (token < 0x30 || token > 0x39) {
			break;
		}
	}

	int subset_len = i - offset;
	int min_repeats;
	int max_repeats;

	if (asterisk_idx != -1) {
		if (asterisk_idx == offset) {
			min_repeats = 0;
		} else {
			char* min_repeat_str = slice_abnf_subset(abnf, offset, asterisk_idx);
			min_repeats = strtol(min_repeat_str, NULL, 10);
		}

		if (asterisk_idx == i) {
			max_repeats = -1;
		} else {
			char* max_repeat_str = slice_abnf_subset(abnf, asterisk_idx, i);
			max_repeats = strtol(max_repeat_str, NULL, 10);
		}
	} else {
		// If no asterisk was encountered, the clause can be repeated
		// only the specified number of times; no more, no less.
		char* abnf_subset = slice_abnf_subset(abnf, offset, i);
		min_repeats = strtol(abnf_subset, NULL, 10);
		max_repeats = min_repeats;
	}

	HeketNode node = {
		min_repeats: min_repeats,
		max_repeats: max_repeats
	};

	ParseResult result = {
		len:  subset_len,
		node: &node
	};

	return result;
}

static ParseResult parse_alternative(const char* abnf, int offset)
{
	ParseResult result = {
		node: NULL,
		len:  0
	};

	return result;
}

static ParseResult parse_whitespace(const char* abnf, int offset)
{
	int i = offset;
	int len = strlen(abnf);

	while (i < len) {
		char token = abnf[i];

		if (token != 0x20 && token != 0x09) {
			break;
		}

		i++;
	}

	ParseResult result = {
		node: NULL,
		len:  i - offset
	};

	return result;
}

static ParseResult parse_rulename(const char* abnf, int offset)
{
	int i = 0;
	int len = strlen(abnf);

	char first_token = abnf[i];

	// The ABNF specification allows the wrapping of rule names in < ... >.
	// These delimiters are entirely optional, and only exist to improve
	// readability. If we encounter one as the first character in the rule
	// name, we can safely bypass it.
	if (first_token == 0x3C) {
		i++;
	}

	int start_pos = i;

	// Slightly different rules for the first character in a rule name --
	// no hyphen allowed here. Note that we don't need to perform this
	// check *after* iterating over the rule name, because we'll only
	// add characters to the rule name string if the character is allowed.
	assert(
		   character_within_range(first_token, 0x30, 0x39)
		|| character_within_range(first_token, 0x41, 0x5A)
		|| character_within_range(first_token, 0x61, 0x7A)
	);

	// Iterate over the ABNF string and pull out the rule name.
	while (i < len) {
		char token = abnf[i++];

		// If the character isn't supported, we assume the rule name is
		// complete.
		if (
			   !character_within_range(first_token, 0x30, 0x39)
			&& !character_within_range(first_token, 0x41, 0x5A)
			&& !character_within_range(first_token, 0x61, 0x7A)
			&& !(first_token == 0x2D)
		) {
			break;
		}
	}

	char* abnf_subset = slice_abnf_subset(abnf, start_pos, i);

	char token = abnf[i];

	// Strip off the trailing ">", if it was specified. (See note above for
	// more context)
	if (token == 0x3E) {
		i++;
	}

	HeketNode child_node = {
		rulename: abnf_subset
	};

	ParseResult result = {
		node: &child_node,
		len:  i - offset
	};

	return result;
}


HeketNode heket_node_from_abnf(const char* abnf)
{
	HeketNode node;

	int i = 0;
	int len = strlen(abnf);

	while (i < len) {
		char token = abnf[i];

		ParseResult parse_result;

		switch (token) {
		case 0x28: // (
			parse_group(abnf, i);
			break;

		case 0x5B: // [
			parse_result = parse_optional(abnf, i);
			break;

		case 0x22: // "
			parse_result = parse_string(abnf, i);
			break;

		case 0x25: // %
			parse_result = parse_numeric(abnf, i);
			break;

		case 0x2A: // *
		case 0x30: // 0
		case 0x31: // 1
		case 0x32: // 2
		case 0x33: // 3
		case 0x34: // 4
		case 0x35: // 5
		case 0x36: // 6
		case 0x37: // 7
		case 0x38: // 8
		case 0x39: // 9
			parse_result = parse_repeat(abnf, i);
			break;

		case 0x2F: // /
			parse_result = parse_alternative(abnf, i);
			break;

		case 0x20: // single space
		case 0x09: // tab
			parse_result = parse_whitespace(abnf, i);
			break;

		default:
			parse_result = parse_rulename(abnf, i);
			// parse rulename
			break;
		}

		i += parse_result.len;

		if (parse_result.node != 0) {
			add_child_to_node(*parse_result.node, node);
		}
	}

	return node;
}
