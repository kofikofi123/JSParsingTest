//Depreciated

/*#include "Unilook.h"
#include "Utils.h"
#include "Stream.h"
#include "KString.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>*/

/*
struct Stream* unicodeData = NULL;

static void nextLine(void);
static void resetLine(void);
//static uint8_t compare(struct Stream*, const char*);
static uint8_t toN(uint8_t);

uint8_t unicodeLookupInit(void){
	char* c_unicodeData = readFile("UnicodeData.txt");

	if (c_unicodeData == NULL)
		return 0;

	unicodeData = newStream(c_unicodeData);

	if (unicodeData == NULL){
		free(c_unicodeData);
		return 0;
	}

	return 1;
}

void unicodeLookupDeInit(void){
	if (unicodeData == NULL) return;
	free((void*)unicodeData->source);
	free(unicodeData);
}


uint8_t unicodeLookup(uint32_t codepoint, struct UnicodeLookup* unilookup){
	uint32_t unicode = 0;
	uint32_t input = 0;
	uint8_t x = 0;
	reset(unicodeData);
	while (1){
		input = getCodePoint(unicodeData);

		if (input == 0x3B){
			if (unicode == codepoint){
				break;
			}
			unicode = 0;
			nextLine();
		}else{
			x = toN(input);
			unicode = ((unicode << 4) | x);
			input = quickAdvance(unicodeData);
		}
		if (isFinished(unicodeData)){
			return 0;
		}
	}

	free(unilookup->characterName);
	unilookup->characterName = NULL;
	memset(unilookup->generalCategory, 0, 3);


	input = quickAdvance(unicodeData);
	uint64_t temp = unicodeData->index;

	while (input != 0x3B)
		input = quickAdvance(unicodeData);

	uint64_t size = (unicodeData->index) - temp;

	char* src = malloc(sizeof(char) * (size + 1));

	if (src == NULL){
		return 0;
	}
	memcpy(src, unicodeData->source + temp, size);

	src[size] = 0;

	unilookup->characterName = src;

	input = quickAdvance(unicodeData);

	memcpy(unilookup->generalCategory, unicodeData->source + unicodeData->index, 2);


	return 1;
}

uint8_t compareUnicodeCategory(struct UnicodeLookup* unilook, char** list, uint8_t size){
	//char ** list = *klist;

	for (uint8_t i = 0; i < size; i++){
		if (stringCompareRAW(unilook->generalCategory, list[i], 3))
			return 1;
	}

	return 0;
}

static uint8_t toN(uint8_t input){
	if (input >= 0x30 && input <= 0x39)
		return (input - 0x30);
	else if (input >= 0x41 && input <= 0x46)
		return (10 + (input - 0x41));
	return 0;
}

static void nextLine(void){
	uint8_t input = getCodePoint(unicodeData);
	do {
		input = quickAdvance(unicodeData);
	}while(input != 0x0A && input != 0x00);

	if (input == 0x00) return;

	advance(unicodeData);
}*/
