#ifndef UTF_H
#define UTF_H

#include <stdint.h>

/*Token Types*/
#define TOKEN_START 0
#define TOKEN_LINETERMINATOR 0x01
#define TOKEN_IDENTIFIER 0x02

/*Tokenizer States*/
#define STATE_NORMAL 0
//Comment States
#define STATE_COMMENT 0x1
#define STATE_SINGLE_COMMENT 0x2
#define STATE_MULTI_COMMENT 0x3
//Identifier States
#define STATE_IDENTIFIER_START 0x4
#define STATE_IDENTIFIER_PART 0x5
#define STATE_IDENTIFIER 0x6
//Numeric Liteal States
#define STATE_NUMERIC_LITERAL 0x7
#define STATE_HEX_LITERAL 0x8
#define STATE_BIN_LITERAL 0x9
#define STATE_OCTAL_LITERAL 0xA
#define STATE_DECIMAL_LITERAL 0xB
#define STATE_DECIMAL_LEGACY_LITERAL 0xC
#define STATE_OCTAL_LEGACY_LITERAL 0xD

//Token Flags
#define FLAG_RESERVED 0
#define FLAG_LEGACY 1

//Temporary or perm clean up solution
#define CLEANUP_SEQUENCE() \
	freeUBuffer(buffer); \
	free(tokens); \
	free(stream); \
/*
[Moved to Stream.h]
struct Stream {
	const char* source;
	uint8_t encoding;
	uint64_t index;
	uint64_t savedIndex;
};
*/

struct Token {
	uint32_t* source;
	uint64_t length;
	uint32_t flags;
	uint16_t type;
	struct Token* next;
	struct Token* prev;
};


//struct Stream* newStream(char*);
struct Token* tokenize(char*);
void releaseTokens(struct Token*);
uint32_t tokenCodePoint(struct Token*);
void setTokenFlag(struct Token*, uint32_t);
void clearTokenFlag(struct Token*, uint32_t);
uint8_t getTokenFlag(struct Token*, uint32_t);
/*
[Moved to Stream.h]
uint32_t getCodePoint(struct Stream*);
void advance(struct Stream*);
void save(struct Stream*);
void restore(struct Stream*);
uint8_t isFinished(struct Stream*);

*/

#endif