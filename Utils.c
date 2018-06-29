#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>

void kassert(uint8_t condition, const char* msg){
	if (!condition){
		printf("%s\n", msg);
		exit(EXIT_FAILURE);
	}

}

char* readFile(const char* filename){
	FILE* pFile = fopen(filename, "r");

	if (pFile == NULL) 
		return NULL;

	size_t file_length = 0;
	char* buffer = NULL;

	fseek(pFile, 0, SEEK_END);
	file_length = ftell(pFile) ;
	rewind(pFile);

	buffer = malloc(sizeof(char) * (file_length + 1));

	if (buffer == NULL){
		fclose(pFile);
		return NULL;
	}

	if (fread(buffer, sizeof(char), file_length, pFile) != file_length){
		printf("Bytes missing/mysteriously added\n");
	}

	buffer[file_length] = 0;
	

	fclose(pFile);
	return buffer;
}