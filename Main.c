#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "Utils.h"
#include "Tokenizer.h"
#include "VMUtils.h"

char* readFile(const char*);


int main(int argc, char* argv[]){
	if (argc < 2){
		printf("Not enogh args\n");
		return -1;
	}
	
	char* content = readFile(argv[1]);

	struct Token* tokens = tokenize(content);

	ecmaString* test = convertAsciiToString("This is a test");

	if (test != NULL){
		printf("size %d\n", test->Length);
		freeString(test);
	}

	if (!tokens)
		exit(EXIT_FAILURE);

	releaseTokens(tokens);
	free(content);
	return 0;
}
