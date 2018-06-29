#ifndef STATIC_SEM_H
#define STATIC_SEM_H

#include <stdint.h>

void kassert(uint8_t, const char*);
char* readFile(const char*);

#endif