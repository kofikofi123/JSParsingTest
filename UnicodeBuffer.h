#ifndef UNICODE_BUFFER
#define UNICODE_BUFFER

#include <stdint.h>

struct UnicodeBuffer {
	uint32_t* buffer;
	uint64_t length;	
	uint64_t position;
};

struct UnicodeBuffer* mallocUBuffer(void);
uint8_t createUBuffer(struct UnicodeBuffer*, uint64_t);
uint8_t resizeUBuffer(struct UnicodeBuffer*, uint64_t);
uint8_t appendUBuffer(struct UnicodeBuffer*, uint32_t);
void resetUBuffer(struct UnicodeBuffer*);
uint32_t* mallocCopyUBuffer(struct UnicodeBuffer*);
uint64_t scanU32Buffer(uint32_t*);
void deleteUBuffer(struct UnicodeBuffer*);
void freeUBuffer(struct UnicodeBuffer*);

#endif