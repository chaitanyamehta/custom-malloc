#ifndef _MEMORY_H
#define _MEMORY_H

#include <sys/types.h>

void *malloc(size_t size);
void free(void *ptr);

#endif