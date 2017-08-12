#ifndef HEKET_NODE_H
#define HEKET_NODE_H

#include <stdbool.h>
#include "parse-result.h"

typedef enum {
	NODE_TYPE_SEQUENTIAL,
	NODE_TYPE_ALTERNATIVE,
	NODE_TYPE_OPTIONAL,
	NODE_TYPE_STRING,
	NODE_TYPE_NUMERIC_RANGE,
	NODE_TYPE_NUMERIC_SET,
	NODE_TYPE_NUMERIC_VALUE,
	NODE_TYPE_REPEATING,
	NODE_TYPE_RULE,
	NODE_TYPE_UNSPECIFIED
} HeketNodeType;

typedef enum {
	BASE_BINARY = 2,
	BASE_DECIMAL = 10,
	BASE_HEX = 16,
	BASE_UNKNOWN = -1
} NumericBase;

typedef struct HeketNode {

	char* rulename;
	char* parent_rulename;

	HeketNodeType type;

	struct HeketNode* child_nodes;
	int child_count;

	int min_repeats;
	int max_repeats;

	char* quoted_string;

	NumericBase numeric_base;
	int numeric_value;

} HeketNode;



HeketNode heket_node_from_abnf(const char* abnf);

HeketParseResult parse_text_with_node(const char* text, HeketNode node);

#endif
