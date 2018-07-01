#include "UnicodeBuffer.h"
#include <string.h>
#include <stdlib.h>

static uint8_t isFull(struct UnicodeBuffer*);

struct UnicodeBuffer* mallocUBuffer(void){
	struct UnicodeBuffer* buffer = malloc(sizeof(struct UnicodeBuffer));

	if (buffer == NULL)
		return NULL;

	buffer->buffer = NULL;
	buffer->length = 0;
	buffer->position = 0;

	return buffer;
}


uint8_t createUBuffer(struct UnicodeBuffer* buffer, uint64_t length){
	if (buffer == NULL)
		return 0;
	uint32_t* ubuffer = malloc(sizeof(uint32_t) * length);

	if (ubuffer == NULL)
		return 0;

	buffer->buffer = ubuffer;
	buffer->length = length;

	return 1;
}


uint8_t resizeUBuffer(struct UnicodeBuffer* buffer, uint64_t newLength){
	uint32_t* oldBuffer = buffer->buffer;
	if (oldBuffer == NULL) return 0;
	uint32_t* newBuffer = realloc(oldBuffer, newLength);

	if (newBuffer == NULL)
		return 0;

	uint64_t oldLength = buffer->length;

	if (newLength > oldLength)
		memset(newBuffer + oldLength, 0, newLength-oldLength);

	buffer->buffer = newBuffer;
	buffer->length = newLength;

	return 1;
}


void resetUBuffer(struct UnicodeBuffer* buffer){
	uint32_t* ubuffer = buffer->buffer;

	if (ubuffer == NULL)
		return;

	memset(ubuffer, 0, buffer->length);
	buffer->position = 0;
}
uint32_t* mallocCopyUBuffer(struct UnicodeBuffer* buffer){
	uint32_t real_size = 0;
	uint32_t* ubuffer = buffer->buffer;
	uint32_t* temp_ubuffer = ubuffer;

	if (ubuffer == NULL)
		return NULL;

	while (*temp_ubuffer++ != 0)
		real_size++;

	uint32_t* newBuffer = malloc(sizeof(uint32_t) * (real_size + 1));

	if (newBuffer == NULL)
		return NULL;

	memcpy(newBuffer, ubuffer, real_size * sizeof(uint32_t));

	newBuffer[real_size] = 0;
	

	return newBuffer;
}

uint8_t appendUBuffer(struct UnicodeBuffer* buffer, uint32_t unicode){
	uint64_t length = buffer->length;
	uint64_t pos = buffer->position;
	if (isFull(buffer)){
		if (!resizeUBuffer(buffer, length * 2))
			return 0;
	}

	buffer->buffer[pos] = unicode;
		buffer->position = pos + 1;

	return 1;
}

void deleteUBuffer(struct UnicodeBuffer* buffer){
	free(buffer->buffer);
	buffer->buffer = NULL;
}

void freeUBuffer(struct UnicodeBuffer* buffer){
	if (buffer->buffer != NULL)
		deleteUBuffer(buffer);
	free(buffer);
}

uint64_t scanU32Buffer(uint32_t* items){
	uint64_t length = 0;

	while (*items++ != 0)
		length++;

	return length;
}

static uint8_t isFull(struct UnicodeBuffer* buffer){
	return (buffer->position >= buffer->length);
}