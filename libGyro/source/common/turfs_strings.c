#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

char *turfs__strdup(const char *str)
{
    size_t len = 1 + strlen(str);
    char *p = malloc(len);
    return p ? memcpy(p, str, len) : NULL;
}
