#ifndef HEKET_PARSE_STATE_H
#define HEKET_PARSE_STATE_H

#include <stdbool.h>
#include "node.h"

typedef struct HeketParseState {

	bool allow_partial_match;

} HeketParseState;

bool node_has_remaining_children(HeketNode node, HeketParseState state);
bool node_is_at_last_child(HeketNode node, HeketParseState state);
bool advance_node_to_next_child(HeketNode node, HeketParseState state);
bool revert_node_to_prior_alternative(HeketNode node, HeketParseState state);

int get_child_index_for_node(HeketNode node, HeketParseState state);

HeketNode get_current_child_for_node(HeketNode node, HeketParseState state);

#endif
