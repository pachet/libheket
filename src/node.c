#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "node.h"

typedef struct ParseResult {

	int len;
	HeketNode* node;

} ParseResult;

static int add_child_node_to_node(HeketNode child_node, HeketNode parent_node)
{
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
	ParseResult result = {
		node: NULL,
		len:  0
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
	ParseResult result = {
		node: NULL,
		len:  0
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
			add_child_node_to_node(*parse_result.node, node);
		}
	}

	return node;
}
