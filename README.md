# libheket
### *An ABNF parser / unparser generator written in C*
**libheket** is a C port of my JavaScript [ABNF parsing library](https://github.com/burninggarden/heket).

This is obviously still a work in progress.

Usage:

`````c
#include <heket.h>

HeketRuleset = heket.ruleset_from_abnf("foo = \"bar\" | \"baz\"");
`````
