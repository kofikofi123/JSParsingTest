#include <stdint.h>

struct Stream {
	char* source;
	uint8_t encoding;
	uint64_t index;
	uint64_t size;
	uint64_t savedIndex;
};



struct Stream* newStream(char*);

uint32_t getCodePoint(struct Stream*);
void advance(struct Stream*);
void save(struct Stream*);
void restore(struct Stream*);
uint8_t isFinished(struct Stream*);
void reset(struct Stream*);
uint32_t quickAdvance(struct Stream*);
char* getCurrentSource(struct Stream*);
uint8_t getNumContinuation(struct Stream*);
//void compare(struct Stream*, const char*);