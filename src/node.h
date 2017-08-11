#ifndef HEKET_NODE_H
#define HEKET_NODE_H

typedef struct HeketNode {

	char* parent_rulename;

	struct HeketNode* child_nodes;
	int child_count;

	int optional;

	int min_repeats;
	int max_repeats;

	char* quoted_string;

} HeketNode;

HeketNode heket_node_from_abnf(const char *abnf);

#endif
