#include <utils/string.h>
#include <mm/malloc.h>

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
			return 1;
		}

		a++;
		b++;
	}

	return 0;
}

int memcmp(const void * a, const void * b, size_t n) {
	size_t i;
	unsigned char * ca = (unsigned char *)a;
    unsigned char * cb = (unsigned char *)b;

    unsigned char cca, ccb;

	for(i = 0; i < n; i++) {
		cca = *ca;
		ccb = *cb;

		if(cca < ccb) {
			return -1;
		}

		if(cca > ccb) {
			return 1;
		}

		ca++;
		cb++;
	}

	return 0;
}

char * strdup(const char *s1) {
    size_t i;
    size_t len = strlen(s1) + 1;
    char * result;

    result = (char*)malloc(len);
    for(i = 0; i < len; i++) {
        result[i] = s1[i];
    }

    return result;
}

char * strndup(const char *s1, size_t n) {
    size_t i;
    char * result;

    for(i = 0; i < n; i++) {
        if(s1[i] == 0) {
            n = i + 1;
            break;
        }
    }

    result = malloc(n + 1);

    for(i = 0; i < n; i++) {
        result[i] = s1[i];
    }

    result[n] = 0;

    return result;
}

char * strcpy(char * dst, const char * src) {
    size_t i = 0;
    while(src[i]) {
        dst[i] = src[i];
        i++;
    }
    return dst;
}

char * strncpy(char * dst, const char * src, size_t n) {
    size_t i = 0;
    while(src[i] && i < n) {
        dst[i] = src[i];
        i++;
    }
    while(i < n) {
        dst[i] = 0;
        i++;
    }
    return dst;
}

char * strcat(char * dst, const char * src) {
    size_t i, j = 0;
    i = strlen(dst);
    while(src[j]) {
        dst[i] = src[j];
        i++;
        j++;
    }
    dst[i] = 0;
    return dst;
}

char * strncat(char * dst, const char * src, size_t n) {
    size_t i, j = 0;
    i = strlen(dst);
    while(src[j] && j < n) {
        dst[i] = src[j];
        i++;
        j++;
    }
    dst[i] = 0;
    return dst;
}

