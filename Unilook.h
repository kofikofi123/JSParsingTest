//Depreciated
/*#ifndef UNILOOK_H
#define UNILOOK_H

#include <stdint.h>

struct UnicodeLookup {
	char* characterName;
	uint32_t codePoint;
	char generalCategory[3];
};

uint8_t unicodeLookupInit(void);
void unicodeLookupDeInit(void);
uint8_t unicodeLookup(uint32_t, struct UnicodeLookup*);
uint8_t compareUnicodeCategory(struct UnicodeLookup*, char**, uint8_t);



#endif*/