#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "node.h"
#include "parse-result.h"

typedef struct ParseResult {

	int len;
	HeketNode* node;

} ParseResult;

static NumericBase base_from_char(char token)
{
	switch (token) {
	case 0x62: // b
		return BASE_BINARY;

	case 0x64: // d
		return BASE_DECIMAL;

	case 0x78: // x
		return BASE_HEX;
	default:
		return BASE_UNKNOWN;
	}
}

static HeketNode last_child_of_node(HeketNode parent_node)
{
	int child_count = parent_node.child_count;

	return parent_node.child_nodes[child_count - 1];
}

static bool character_within_range(char character, char start, char end)
{
	if (character < start) {
		return false;
	}

	if (character > end) {
		return false;
	}

	return true;
}

static bool character_is_alphanumeric(char character)
{
	return (
		   character_within_range(character, 0x30, 0x39) // A-Z
		|| character_within_range(character, 0x41, 0x5A) // 0-9
		|| character_within_range(character, 0x61, 0x7A) // a-z
	);
}

static int add_child_to_node(HeketNode child_node, HeketNode parent_node)
{

	// If the last child node added to the parent was a repeat node,
	// the new child being added should actually be added to the repeat node
	// instead of being directly added to the current parent. This was the
	// most elegant way of dealing with the forward-affecting nature of
	// ABNF repetition clauses, and is unique to that node type.
	if (parent_node.child_count > 0) {
		HeketNode last_child = last_child_of_node(parent_node);

		if (last_child.type == NODE_TYPE_REPEATING) {
			return add_child_to_node(child_node, last_child);
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

	HeketNode child_node = {
		type: NODE_TYPE_OPTIONAL,
		child_count: 0
	};

	char* abnf_subset = slice_abnf_subset(abnf, offset, i);
	HeketNode nested_node = heket_node_from_abnf(abnf_subset);

	add_child_to_node(nested_node, child_node);

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
	int found_end_quotes = false;

	while (i < len) {
		char token = abnf[i++];

		if (token == 0x22) {
			found_end_quotes = true;
			break;
		}
	}

	assert(found_end_quotes == true);

	char* abnf_subset = slice_abnf_subset(abnf, offset, i);

	HeketNode child_node = {
		type: NODE_TYPE_STRING,
		quoted_string: abnf_subset
	};

	ParseResult result = {
		node: &child_node,
		len:  i - offset + 1
	};

	return result;
}

static HeketNode create_numeric_value_node(NumericBase base, char* segment)
{
	char* pointer;
	int numeric_value = strtol(segment, &pointer, 10);

	assert(numeric_value != *pointer);

	HeketNode node = {
		type: NODE_TYPE_NUMERIC_VALUE,
		numeric_value: numeric_value
	};

	return node;
}

static ParseResult parse_numeric(const char* abnf, int offset)
{
	int i = offset;
	int len = strlen(abnf);

	// Advance past the initial % signifier:
	i++;

	// Determine the base to use (binary, decimal, or hex) based on the
	// leading token:
	NumericBase base = base_from_char(abnf[i]);

	assert(base != BASE_UNKNOWN);

	// Advance past the base signifier:
	i++;

	HeketNode container_node = {
		type: NODE_TYPE_UNSPECIFIED,
		child_count: 0
	};

	int segment_start = i;

	while (i < len) {
		char token = abnf[i++];

		// If we encounter a slash (option separator) or a space, break
		// the iteration cycle -- those tokens signify that the numeric
		// clause was exceeded.
		if (
			   token == 0x2F // /
			|| token == 0x20 // space
		) {
			break;
		}

		if (
			   token != 0x2D // -
			&& token != 0x2E // .
		) {
			// For all characters except hyphens and periods, we want to
			// continue reading additional characters.
			continue;
		}

		// If we encounter a hyphen, it means the numeric clause should be
		// treated as a numeric range of values.
		if (token == 0x2D) {
			// The ABNF specification doesn't allow specifing numeric ranges
			// and numeric options (ie, mixing the "-" and "." delimiters)
			// within the same numeric clause. Check to make sure the node
			// type for the container node hasn't been set to a numeric set
			// (in other words, make sure that no "." set delimiter was
			// previously encountered).
			assert(container_node.type != NODE_TYPE_NUMERIC_SET);
			container_node.type = NODE_TYPE_NUMERIC_RANGE;
		} else if (token == 0x2E) {
			// Same note as above regarding "-" vs "." delimiters, but in
			// reverse.
			assert(container_node.type != NODE_TYPE_NUMERIC_RANGE);
			container_node.type = NODE_TYPE_NUMERIC_SET;
		}

		// Make sure at least one character occurred before the "-" or "."
		// delimiter:
		assert(i > segment_start + 1);

		// Regardless of whether we encountered a slash or a period, we want
		// to parse the prior segment as a numeric value for the given base,
		// and add it to the container.
		char* segment = slice_abnf_subset(abnf, segment_start, i);
		segment_start = i;
		HeketNode node = create_numeric_value_node(base, segment);

		add_child_to_node(node, container_node);
	}

	// Numeric clauses will *always* end with a numeric value, regardless
	// of whether they contain slash delimiters denoting ranges, period
	// delimiters denoting sets, or no delimiters at all.
	char* segment = slice_abnf_subset(abnf, segment_start, i);
	HeketNode node = create_numeric_value_node(base, segment);
	add_child_to_node(node, container_node);

	// If no delimiters were encountered, treat the container node as a
	// numeric set containing only one child:
	if (container_node.type == NODE_TYPE_UNSPECIFIED) {
		container_node.type = NODE_TYPE_NUMERIC_SET;
	}

	ParseResult result = {
		node: &container_node,
		len:  i - offset
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
		type: NODE_TYPE_REPEATING,
		min_repeats: min_repeats,
		max_repeats: max_repeats,
		child_count: 0
	};

	ParseResult result = {
		len:  subset_len,
		node: &node
	};

	return result;
}

static ParseResult parse_alternative(const char* abnf, int offset)
{
	int i = offset;
	int len = strlen(abnf);

	// Advance past the initial "/" signifier:
	i++;

	int start_pos = i;

	// We need to keep track of whether the current string position is located
	// within a quoted string literal. If so, we need to ignore the appearance
	// of any "/" tokens, as they're part of the literal and not meant to
	// signify the next alternative.
	int within_quoted_str = 0;

	// Same with groups. Except with groups, we need to keep track of the level
	// of nesting, since we can have groups within groups.
	int nested_group_level = 0;

	// Walk along the ABNF string, token by token. Break if/when we encounter
	// another "/" alternative signifier.

	while (i < len) {
		char token = abnf[i++];

		// If we hit a "/" signifier and we're not currently within a string
		// literal or group, bail out.
		if (
			   token == 0x2F
			&& within_quoted_str == 0
			&& nested_group_level == 0
		) {
			break;
		}

		// If we encounter a double quote, it means that a string literal
		// is either starting or ending, so flip the flag.
		if (token == 0x22) {
			within_quoted_str = !within_quoted_str;
		}

		if (!within_quoted_str) {
			if (token == 0x28) {
				nested_group_level++;
			} else if (token == 0x29) {
				nested_group_level--;
			}
		}
	}

	char* abnf_subset = slice_abnf_subset(abnf, start_pos, i);

	HeketNode child_node = heket_node_from_abnf(abnf_subset);

	ParseResult result = {
		node: &child_node,
		len:  i - offset
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
	int i = offset;
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
	assert(character_is_alphanumeric(first_token));


	// Iterate over the ABNF string and pull out the rule name.
	while (i < len) {
		char token = abnf[i++];

		// If the character isn't supported, we assume the rule name is
		// complete.
		if (
			   !character_is_alphanumeric(first_token)
			&& first_token != 0x2D // -
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
		type: NODE_TYPE_RULE,
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
	HeketNode node = {
		type: NODE_TYPE_SEQUENTIAL,
		child_count: 0
	};

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
			node.type = NODE_TYPE_ALTERNATIVE;
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

		if (parse_result.node) {
			add_child_to_node(*parse_result.node, node);
		}
	}

	return node;
}

