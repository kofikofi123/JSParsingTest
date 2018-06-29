#include "Stream.h"
#include "Utils.h"
#include <stdlib.h>
#include <stdio.h>


static uint8_t getContinuation(uint8_t);
//static uint8_t getNumContinuation(struct Stream*);
static char getByte(struct Stream*);
static uint64_t stringLength(char*);

struct Stream* newStream(char* source){
	struct Stream* stream = malloc(sizeof(struct Stream));

	if (stream == NULL)
		return NULL;

	stream->source = (char*)source;
	//stream->encoding = getEncoding(source);
	stream->index = 0;
	stream->savedIndex = 0;
	stream->size = stringLength(stream->source);

	return stream;
}

uint32_t getCodePoint(struct Stream* stream){
	if (isFinished(stream))
		return 0;

	uint32_t unicode = 0;
	uint8_t input = (uint8_t)getByte(stream);

	uint8_t bits = 0;

	if ((input & 0x80) == 0)
		return input;
	else if ((input & 0xE0) == 0xC0){
		bits = 0;
		unicode = ((input & 0x1F) << 6);
	}else if ((input & 0xF0) == 0xE0){
		bits = 6;
		unicode = ((input & 0x0F) << 12);
	}else if ((input & 0xF8) == 0xF0){
		bits = 12;
		unicode = ((input & 0x07) << 18);
	}else{
		printf("Unknown sequence 1\n");
		exit(EXIT_FAILURE);
	}

	do {
		if (isFinished(stream)){
			printf("Unknown sequence 4\n");
			exit(EXIT_FAILURE);
		}

		input = getContinuation((uint8_t)getByte(stream));
		
	
		
		unicode = unicode | (input << bits);
		bits -= 6;
	}while(bits >= 0);

	return unicode;
}

uint8_t getNumContinuation(struct Stream* stream){
	if (isFinished(stream))
		return 0;
	uint8_t byte = *(uint8_t*)(stream->source + stream->index);

	kassert((byte & 0xC0) != 0x80, "Unknown sequence 3\n");

	if ((byte & 0x80) == 0)
		return 1;
	else if ((byte & 0xE0) == 0xC0){
		return 2;
	}else if ((byte & 0xF0) == 0xE0){
		return 3;
	}else if ((byte & 0xF8) == 0xF0){
		return 4;
	}else{
		kassert(0, "Unknown sequence 2\n");
	}

	return 0;
}

static uint8_t getContinuation(uint8_t byte){
	kassert((byte & 0xC0) == 0x80, "Unknown sequence 2\n");

	return (byte & 0x3F);
}

inline void save(struct Stream* stream){
	stream->savedIndex = stream->index;
}

inline void restore(struct Stream* stream){
	stream->index = stream->savedIndex;
}

inline void reset(struct Stream* stream){
	stream->index = 0;
}

uint8_t isFinished(struct Stream* stream){
	return (stream->index >= stream->size);
}
void advance(struct Stream* stream){
	uint8_t getSize = getNumContinuation(stream);
	stream->index += getSize;
}

static char getByte(struct Stream* stream){
	return *(stream->source + stream->index);
}

uint32_t quickAdvance(struct Stream* stream){
	advance(stream);
	return getCodePoint(stream);
}

static uint64_t stringLength(char* source){
	uint64_t length = 0;

	while (*source++ != 0)
		length++;

	return length;
}

char* getCurrentSource(struct Stream* stream){
	return (stream->source + stream->index);
}