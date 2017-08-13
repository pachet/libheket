#ifndef HEKET_UTIL_H
#define HEKET_UTIL_H

#include "string-array.h"

char* str_copy(const char* string);

HeketStringArray str_split(const char* string, const char delimiter);

int count_indents(const char* str);

#endif
