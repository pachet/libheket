#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "string-array.h"


HeketStringArray str_split(const char* str, const char delim)
{
	int i = 0;
	int len = strlen(str);
	int substr_count = 1;

	// Walk over the array to determine how many substrings there will be.
	// This is more performant than calling realloc(substrs) when we
	// determine that another substring needs to be added to the result.
	while (i < len) {
		if (str[i] == delim) {
			substr_count++;
		}

		i++;
	}

	int bytes = substr_count * sizeof(char*);
	char** substrs = malloc(bytes);

	HeketStringArray result;

	if (!substrs) {
		return result;
	}

	i = 0;
	int offset = 0;
	int substr_idx = 0;

	while (i < len) {
		if (str[i] != delim && i + 1 < len) {
			i++;
			continue;
		}

		int substr_len = i - offset;

		if (i + 1 == len) {
			substr_len++;
		}

		char* substr = malloc(substr_len);

		memcpy(substr, str + offset, substr_len);

		substrs[substr_idx] = substr;
		substr_idx++;
		offset = i + 1;
		i++;
	}

	result.len = substr_idx;
	result.values = substrs;

	return result;
}


int count_indents(const char* str)
{
	int i = 0;
	int len = strlen(str);
	int indents = 0;

	while (i < len) {
		if (str[i] == 0x09) {
			// Eh...
			indents += 4;
		} else if (str[i] == 0x20) {
			indents++;
		} else {
			return indents;
		}

		i++;
	}

	return indents;
}

