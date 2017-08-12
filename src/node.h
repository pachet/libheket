#ifndef HEKET_NODE_H
#define HEKET_NODE_H

#include <stdbool.h>

typedef enum {
	NODE_TYPE_SEQUENTIAL_CHILDREN,
	NODE_TYPE_ALTERNATIVE_CHILDREN,
	NODE_TYPE_OPTIONAL,
	NODE_TYPE_STRING,
	NODE_TYPE_NUMERIC_RANGE,
	NODE_TYPE_NUMERIC_SET,
	NODE_TYPE_NUMERIC_VALUE,
	NODE_TYPE_REPEATING,
	NODE_TYPE_RULE
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

	enum HeketNodeType type;

	struct HeketNode* child_nodes;
	int child_count;

	int min_repeats;
	int max_repeats;

	char* quoted_string;

	enum NumericBase numeric_base;
	int numeric_value;

} HeketNode;



HeketNode heket_node_from_abnf(const char *abnf);

#endif
