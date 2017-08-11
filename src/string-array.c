#include <stdlib.h>
#include <string.h>
#include "string-array.h"

HeketStringArray make_heket_string_array(char** strings, int len)
{
	HeketStringArray result = {
		len:    len,
		values: strings
	};
}

const int get_total_length(HeketStringArray string_array)
{
	int i = 0;
	int total_len = 0;

	while (i < string_array.len) {
		char* string = string_array.values[i];

		total_len += strlen(string);

		i++;
	}

	return total_len;
}

char* heket_string_array_to_string(HeketStringArray string_array)
{
	int bytes = get_total_length(string_array) + 1;
	char* result = malloc(bytes);
	int i = 0;

	while (i < string_array.len) {
		strcat(result, string_array.values[i]);
		i++;
	}

	result[bytes - 1] = 0x00;

	return result;
}
