#ifndef STRING_H
#define STRING_H

#include <stdint.h>

typedef int64_t strpos_t;

uint32_t stringLength(const char*);
strpos_t stringFind(const char*, const char*, strpos_t);
strpos_t stringFindC(const char*, char, strpos_t);
strpos_t stringRegex(const char*, const char*);
uint8_t stringCompare(const char*, const char*);
uint8_t stringCompareRAW(const char*, const char*, uint32_t);


#endif