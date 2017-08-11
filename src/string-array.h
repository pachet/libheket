#ifndef HEKET_STRING_ARRAY_H
#define HEKET_STRING_ARRAY_H

typedef struct HeketStringArray {
	int len;
	char** values;
} HeketStringArray;

char* heket_string_array_to_string(HeketStringArray array);

#endif
