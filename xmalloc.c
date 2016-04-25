#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "xmalloc.h"

void *xcalloc(size_t number, size_t size)
{
	void *ptr = calloc(number, size);

	if (ptr == NULL)
		fatal("calloc: out of memory");
	return ptr;
}

char *xstrdup(const char *str)
{
	char *str2 = strdup(str);

	if (str2 == NULL)
		fatal("strdup: out of memory");
	return str2;
}
