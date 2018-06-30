#include <stdlib.h>
#include <stdio.h>
#include <unictype.h>
#include <wchar.h>
#include <string.h>
#include "Stream.h"
#include "Tokenizer.h"
#include "UnicodeBuffer.h"
//#include "Unilook.h"
#include "Utils.h"
#include "KString.h"
/*
static struct UnicodeLookup* unilook = NULL;//malloc(sizeof(struct UnicodeLookup));


static char* ID_Start[] = {
	"Lu", "Ll", "Lt", "Lm", "Lo", "Nl"
};*/

static char* ReservedName[] = {
	"await", "break", "case", "catch", "class",
	"const", "continue", "debugger", "default", "delete",
	"do", "else", "export", "extends", "finally", 
	"for", "function", "if", "import", "in",
	"instanceof", "new", "return", "super", "switch",
	"this", "throw", "try", "typeof", "var",
	"void", "while", "with", "yield"
};

static char* FutureReservedName[] = {
	"enum"
};

static char* ReservedLiteralName[] = {
	"true", "false", "null"
};

static void produceSyntaxError(const char*);
static void produceOutOfMemoryError(const char*);

//static uint8_t isWhitespace(uint32_t);
static uint8_t isLineTerminator(uint32_t);
static uint8_t isIdentifierStart(uint32_t);
static uint8_t isIdentifierContinue(uint32_t);
static uint8_t isNumeric(uint32_t);
static uint8_t isHexNumeric(uint32_t);
static uint8_t isReservedName(uint32_t*);
static uint8_t matchList(uint32_t*, char**, uint64_t);

static uint8_t toN(uint8_t);
static uint8_t inRange(uint32_t, uint32_t, uint32_t);


static void appendToken(struct Token*, struct Token*);
static struct Token* getTopToken(struct Token*);

static struct Token* tokenizeLineTerminator(uint32_t);
static struct Token* tokenizeIdentifer(uint32_t*);

static struct Token* createToken(uint8_t);
//static struct Token* createLineTerminatorToken(uint32_t);

static void printTokenChars(struct Token*, uint8_t);
static void printTokens(struct Token*);

struct Token* tokenize(char* source){

	struct Stream* stream = newStream(source);
	struct UnicodeBuffer* buffer = mallocUBuffer();
	struct Token* tokens = createToken(TOKEN_START);
	struct Token* token = NULL;
	
	//unilook = malloc(sizeof(struct UnicodeLookup));

	if (stream == NULL || tokens == NULL || !createUBuffer(buffer, 25)){
		CLEANUP_SEQUENCE()
		return NULL;
	}

	uint32_t currentInput = getCodePoint(stream);
	uint8_t reconsume = 0;
	uint8_t currentState = 0;
	uint8_t isUniEsc = 0;

	do {
		if (currentState == STATE_NORMAL){
			if (currentInput == 0x2F){
				reconsume = 1;
				currentState = STATE_COMMENT;
			}else if (isLineTerminator(currentInput)){
				token = tokenizeLineTerminator(currentInput);
				appendToken(tokens, token);
			}else if (isIdentifierStart(currentInput)){
				reconsume = 1;
				currentState = STATE_IDENTIFIER_START;
				resetUBuffer(buffer);
			}else if (currentInput == 0x5C){
				reconsume = 1;
				currentState = STATE_IDENTIFIER_START;
			}else if (isNumeric(currentInput)){
				reconsume = 1;
				currentState = STATE_NUMERIC_LITERAL;
			}
		}else if (currentState == STATE_COMMENT){
			//save(stream);
			if (currentInput == 0x2F)
				currentState = STATE_SINGLE_COMMENT;
			else if (currentInput == 0x2F)
				currentState = STATE_MULTI_COMMENT;
		}else if (currentState == STATE_SINGLE_COMMENT){
			if (isLineTerminator(currentInput)){
				reconsume = 1;
				currentState = STATE_NORMAL;
			}else if (isFinished(stream))
				break;
		}else if (currentState == STATE_IDENTIFIER_START){
			if (isIdentifierStart(currentInput))
				appendUBuffer(buffer, currentInput);
			else if (isIdentifierContinue(currentInput)){
				reconsume = 1;
				currentState = STATE_IDENTIFIER_PART;
			}else if (currentInput == 0x5C){
				isUniEsc = 1;
				currentInput = quickAdvance(stream);

				if (currentInput != 0x75){
					produceSyntaxError("eok");
					CLEANUP_SEQUENCE()
					return NULL;
				}

				currentInput = quickAdvance(stream);

				uint32_t temp_uni = 0;
				uint32_t temp = 0;
				uint8_t tempB = 0;

				if (currentInput == 0x7B){
					currentInput = quickAdvance(stream);
					do {
						if (isFinished(stream))
							break;

						if (currentInput == 0x7D){
							if (tempB > 0)
								break;
							else{
								printf("U+%x\n", currentInput);
								produceSyntaxError("Unknown unicode sequence 4");
								CLEANUP_SEQUENCE()
								return NULL;
							}
						}

						temp = toN(currentInput);
						if (temp == 0xFF){
							printf("U+%x\n", currentInput);
							produceSyntaxError("Unknown unicode sequence 3");
							CLEANUP_SEQUENCE()
							return NULL;
						}

						tempB++;

						temp_uni = (temp_uni << 4) | temp;

						currentInput = quickAdvance(stream);

					}while(1);
				}else{
					for (uint8_t temp_counter = 0; temp_counter < 4; temp_counter++){
						temp = toN(currentInput);
						if (temp == 0xFF){
							produceSyntaxError("Unknown unicode sequence 5");
							CLEANUP_SEQUENCE()
							return NULL;
						}
						temp_uni = (temp_uni << 4) | temp;
						currentInput = quickAdvance(stream);

					}
				}

				if (!isIdentifierStart(temp_uni)){
					produceSyntaxError("Unknown unicode sequence 2");
					CLEANUP_SEQUENCE()
					return NULL;
				}


				reconsume = 1;
				appendUBuffer(buffer, temp_uni);
			}else{
				token = tokenizeIdentifer(mallocCopyUBuffer(buffer));
				if (token == NULL){
					produceOutOfMemoryError("Tokens");
					CLEANUP_SEQUENCE()
					return NULL;
				}

				reconsume = 1;
				currentState = STATE_IDENTIFIER;
			}
		}else if (currentState == STATE_IDENTIFIER_PART){
			if (isIdentifierContinue(currentInput)){
				appendUBuffer(buffer, currentInput);
			}else if (currentInput == 0x5C){
				isUniEsc = 1;
				currentInput = quickAdvance(stream);

				if (currentInput != 0x75){
					produceSyntaxError("eok");
					CLEANUP_SEQUENCE()
					return NULL;
				}

				currentInput = quickAdvance(stream);

				uint32_t temp_uni = 0;
				uint32_t temp = 0;
				uint8_t tempB = 0;
				if (currentInput == 0x7B){
					currentInput = quickAdvance(stream);
					do {
						if (isFinished(stream))
							break;

						if (currentInput == 0x7D){
							if (tempB > 0)
								break;
							else{
								produceSyntaxError("Unknown unicode sequence 4");
								CLEANUP_SEQUENCE()
								return NULL;
							}
						}
						temp = toN(currentInput);
						if (temp == 0xFF){
							produceSyntaxError("Unknown unicode sequence 3");
							CLEANUP_SEQUENCE()
							return NULL;
						}

						tempB++;

						temp_uni = (temp_uni << 4) | temp;

						currentInput = quickAdvance(stream);

					}while(1);
				}else{
					for (uint8_t temp_counter = 0; temp_counter < 4; temp_counter++){
						temp = toN(currentInput);
						if (temp == 0xFF){
							produceSyntaxError("Unknown unicode sequence 5");
							CLEANUP_SEQUENCE()
							return NULL;
						}
						temp_uni = (temp_uni << 4) | temp;
						currentInput = quickAdvance(stream);

					}
				}

				if (!isIdentifierContinue(temp_uni)){
					produceSyntaxError("Unknown unicode sequence 6");
					CLEANUP_SEQUENCE()
					return NULL;
				}
				reconsume = 1;
				appendUBuffer(buffer, temp_uni);
			}else{
				token = tokenizeIdentifer(mallocCopyUBuffer(buffer));
				if (token == NULL){
					produceOutOfMemoryError("Tokens");
					CLEANUP_SEQUENCE()
					return NULL;
				}

				reconsume = 1;
				currentState = STATE_IDENTIFIER;
			}
		}else if (currentState == STATE_IDENTIFIER){
			if (isReservedName(token->source)){
				if (isUniEsc){
					produceSyntaxError("No unicode escape characters in reserved name\n");
					CLEANUP_SEQUENCE()
					return NULL;
				}
				setTokenFlag(token, FLAG_RESERVED);
			}

			appendToken(tokens, token);
			token = NULL;
			currentState = STATE_NORMAL;
			isUniEsc = 0;
		}else if (currentState == STATE_NUMERIC_LITERAL){
			resetUBuffer(buffer);
			save(stream);
			reconsume = 1;
			if (currentInput == 0x30){
				currentInput = quickAdvance(stream);
				if (currentInput == 0x78 || currentInput == 0x58){
					currentState = STATE_HEX_LITERAL;
				}else if (currentInput == 0x62 || currentInput == 0x42){
					currentState = STATE_BIN_LITERAL;
				}else if (currentInput == 0x6F || currentInput == 0x4F){
					currentState = STATE_OCTAL_LITERAL;
				}else if (currentInput == 0x08 || currentInput == 0x09){
					currentState = STATE_DECIMAL_LEGACY_LITERAL;
				}else{
					currentState = STATE_OCTAL_LEGACY_LITERAL;
				}
			}else{
				restore(stream);
				currentState = STATE_DECIMAL_LITERAL;
			}
		}else if (currentState == STATE_HEX_LITERAL){
			printf("GOKTTTI\n");
			currentInput = quickAdvance(stream);
			printf("U+%x test\n", currentInput);
			CLEANUP_SEQUENCE();
			return NULL;
		}

		if (reconsume)
			reconsume = 0;
		else
			currentInput = quickAdvance(stream);

		if (isFinished(stream)){
			printf("Sniped U+%x\n", currentInput);
			break;
		}
	}while(1);

	printTokens(tokens);
	
	//freeUBuffer(buffer);
	//free(stream);
	
	return tokens;
}

/*
static uint8_t isWhitespace(uint32_t unicode){
	if (unicode == 0x09 || unicode == 0x0B || unicode == 0x0C || unicode == 0x20 || unicode == 0xA0 || unicode == 0xFEFF)
		return 1;

	//if (!unicodeLookup(unicode, unilook))
	//	return 0;


	return uc_is_property_white_space(unicode);//stringCompareRAW(unilook->generalCategory, "Zs", 3);
}*/

static uint8_t isLineTerminator(uint32_t unicode){
	return (unicode == 0x0A || unicode == 0x0D || unicode == 0x2028 || unicode == 0x2029);
}

static uint8_t isIdentifierStart(uint32_t unicode){
	//save(stream);
	if (unicode == 0x5F || unicode == 0x24){
		//restore(stream);
		return 1;
	}

	return uc_is_property_id_start(unicode);
}

static uint8_t isIdentifierContinue(uint32_t unicode){
	if (unicode == 0x24 || unicode == 0x200C || unicode == 0x200D)
		return 1;

	return uc_is_property_id_continue(unicode);
}

static uint8_t isNumeric(uint32_t unicode){
	return inRange(unicode, 0x30, 0x39);
}

static uint8_t isHexNumeric(uint32_t unicode){
	return isNumeric(unicode) || inRange(unicode, 0x41, 0x46) || inRange(unicode, 0x61, 0x66);
}

static inline uint8_t inRange(uint32_t unicode, uint32_t min, uint32_t max){
	return (unicode >= min && unicode <= max);
}

static uint8_t toN(uint8_t input){
	if (input >= 0x30 && input <= 0x39)
		return (input - 0x30);
	else if (inRange(input, 0x41, 0x46))//(input >= 0x41 && input <= 0x46)
		return (10 + (input - 0x41));
	else if (inRange(input, 0x61, 0x66))//(input >= 0x61 && input <= 0x46)
		return (10 + (input - 0x61));
	return 0xFF;
}


static uint8_t isReservedName(uint32_t* buffer){
	uint8_t c_size = sizeof(char*);
	if (matchList(buffer, ReservedName, sizeof(ReservedName)/c_size))
		return 1;

	return matchList(buffer, FutureReservedName, sizeof(FutureReservedName)/c_size) || matchList(buffer, ReservedLiteralName, sizeof(ReservedLiteralName)/c_size);
}

static uint8_t compareBuffer(uint32_t* buffer, char* item){
	uint32_t tempA = 0, tempB = 0;
	while (*item != 0 && *buffer != 0){
		tempA = (uint32_t)*item++;
		tempB = *buffer++;

		if (tempA != tempB)
			return 0;
	}

	return 1;
}

static uint8_t matchList(uint32_t* buffer, char** list, uint64_t size){
	for (uint64_t i = 0; i < size; i++){
		if (compareBuffer(buffer, list[i]))
			return 1;
	}

	return 0;
}
/*
static uint8_t isIdentifierUnicodeEscape(struct Stream* stream){
	uint32_t input = getCodePoint(stream);
	uint64_t pos = stream->index;

	if (input != 0x5C){
		restore(stream);
		return 0;
	}

	input = quickAdvance(stream);

	if (input != 0x75){
		produceSyntaxError("eok");
		exit(EXIT_FAILURE);
	}

	input = quickAdvance(stream);
	
	if (!isHex4(stream))

	stream->index = pos;
	return 1;

}*/

static void produceSyntaxError(const char* msg){
	printf("Syntax error: %s\n", msg);
}

static void produceOutOfMemoryError(const char* msg){
	printf("Out of memory : %s\n", msg);
}


void releaseTokens(struct Token* token){
	struct Token* temp = token;
	struct Token* tempB = temp;
	while (temp != NULL){
		tempB = temp;
		temp = temp->next;
		free(tempB->source);
		free(tempB);
	}

}

const char* getTokenCategory(struct Token* token){
	uint16_t type = token->type;
	if (type == TOKEN_START)
		return "<START>";
	else if (type == TOKEN_LINETERMINATOR)
		return "<LINE_TERMINATOR>";
	else if (type == TOKEN_IDENTIFIER){
		if (getTokenFlag(token, FLAG_RESERVED))
			return "<IDENIFIER (RESERVED)>";
		else
			return "<IDENIFIER>";
	}
	return "<>";
}
static struct Token* createToken(uint8_t tokenType){
	//uint32_t temp = sizeof(struct Token);
	struct Token* token = malloc(sizeof(struct Token));

	if (token == NULL)
		return NULL;

	memset(token, 0, sizeof(sizeof(struct Token)));

	token->type = tokenType;
	//token->source = NULL;
	/*token->length = 0;
	token->reserved = 0;*/


	return token;
}

static struct Token* tokenizeLineTerminator(uint32_t thing){
	struct Token* token = createToken(TOKEN_LINETERMINATOR);
	uint32_t* items = malloc(sizeof(uint32_t));

	*items = thing;
	if (token == NULL)
		return NULL;

	token->source = items;
	token->length = 1;

	return token;
}

inline void setTokenFlag(struct Token* token, uint32_t flag){
	token->flags |= (1 << flag);
}

inline void clearTokenFlag(struct Token* token, uint32_t flag){
	token->flags &= (~(1 << flag));
}

inline uint8_t getTokenFlag(struct Token* token, uint32_t flag){
	return (token->flags & (1 << flag)) == (1 << flag);
}

static struct Token* tokenizeIdentifer(uint32_t* items){
	struct Token* token = createToken(TOKEN_IDENTIFIER);
	if (token == NULL || items == NULL){
		free(token);
		return NULL;
	}

	token->source = items;
	token->length = scanU32Buffer(items);


	return token;
}

static void appendToken(struct Token* mainToken, struct Token* sub){

	if (mainToken == NULL || sub == NULL)
		return;
	struct Token* top = getTopToken(mainToken);

	if (top == NULL) return;

	top->next = sub;
	sub->prev = top;
	sub->next = NULL;
}

static struct Token* getTopToken(struct Token* token){
	struct Token* temp = token;

	if (temp == NULL) return NULL;
	while (temp->next != NULL)
		temp = temp->next;

	return temp;
}
static void printTokens(struct Token* tokens){
	struct Token* temp = tokens;
	while (temp != NULL){
		printf("{\n\tTokenType: \"%s\"", getTokenCategory(temp));
		if (temp->type != TOKEN_LINETERMINATOR){
			printf("\n\tCharacters={");
			printTokenChars(temp, 0);
			printf("}\n\tStr={");
			printTokenChars(temp, 1);
			printf("}\n}\n");
		}else
			printf("\n}\n");
		temp = temp->next;
	}
}

static void printTokenChars(struct Token* token, uint8_t type){
	uint64_t length = token->length;
	uint32_t* source = token->source;
	uint32_t temp = 0;

	if (source == NULL) return;

	for (uint64_t i = 0; i < length; i++){
		temp = source[i];
		if (type == 0)
			printf("U+%x(%lc) ", temp, (wint_t)temp);
		else if (type == 1)
			printf("%lc", (wint_t)temp);
	}
}
