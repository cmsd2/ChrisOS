
#include <utils/string.h>

const char * true_str = "true";
const char * false_str = "false";

size_t strlen(const char *str)
{
	size_t ret = 0;
	while (str[ret])
		ret++;
	return ret;
}

int strcmp(const char * a, const char * b) {
	char ca, cb;
	ca = *a;
	cb = *b;
	while(ca && cb) {
		if(ca < cb) {
			return -1;
		}
		if(ca > cb) {
			return 1;
		}
		a++;
		b++;
	}

	if(ca) {
		return 1;
	}

	if(cb) {
		return -1;
	}

	return 0;
}

int strncmp(const char * a, const char * b, size_t n) {
	size_t i;
	char ca, cb;

	for(i = 0; i < n; i++) {
		ca = *a;
		cb = *b;

		if(ca == 0 && cb == 0) {
			return 0;
		}

		if(ca == 0) {
			return -1;
		}

		if(cb == 0) {
			return 1;
		}

		if(ca < cb) {
			return -1;
		}

		if(ca > cb) {
			return -1;
		}

		a++;
		b++;
	}

	return 0;
}
