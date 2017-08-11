#ifndef HEKET_NODE_H
#define HEKET_NODE_H

typedef struct HeketNode {

	char* parent_rulename;

	HeketNodeList child_nodes;

} HeketNode;

HeketNode heket_node_from_abnf(const char *abnf);

#endif
