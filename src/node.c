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

static ParseResult parse_group(const char* abnf, int offset)
{
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

	int subset_len = i - offset;
	int subset_bytes = (subset_len + 1) * sizeof(char);
	char* abnf_subset = malloc(subset_bytes);
	strncpy(abnf_subset, abnf + offset, subset_len);

	HeketNode child_node = heket_node_from_abnf(abnf_subset);

	ParseResult result = {
		node: &child_node,
		len:  subset_len
	};

	return result;
}

static ParseResult parse_optional(const char* abnf, int offset)
{
	ParseResult result = {
		node: NULL,
		len:  0
	};

	return result;
}

static ParseResult parse_string(const char* abnf, int offset)
{
	ParseResult result = {
		node: NULL,
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
	ParseResult result = {
		node: NULL,
		len:  0
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
