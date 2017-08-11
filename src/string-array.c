#include "string-array.h"

HeketStringArray make_heket_string_array(char** strings, int len)
{
	HeketStringArray result = {
		len:    len,
		values: strings
	};
}
