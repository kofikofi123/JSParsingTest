#include "KString.h"
#include <stdio.h>

//static struct Pattern* _stringCompileRegex(const char*);

uint32_t stringLength(const char* string){
    uint32_t len = 0;
    while (*string++ != 0)
        len++;
    return len;
}

uint8_t stringCompareRAW(const char* stringA, const char* stringB, uint32_t length){
    char* sA = (char*)stringA;
    char* sB = (char*)stringB;
    char* sEnd = sA + length - 1;
    
    while (sA <= sEnd){
        if (*sA++ != *sB++)
            return 0;
    }
    
    return 1;
}

strpos_t stringFindC(const char* string, char f, strpos_t start){
    uint32_t len = stringLength(string);
    if (start > len)
        start = 0;
    
    char* cString = (char*)string;
    char* cEnd = cString + len - 1;
    
    while (cString <= cEnd){
        if (*cString == f)
            return (strpos_t)(cString - string);
        cString++;
    }
    
    return -1;
}
strpos_t stringFind(const char* string, const char* f, strpos_t start){
    uint32_t len = stringLength(string);
    if (start > len)
        start = 0;
        
    char* cString = (char*)string;
    char* cF = (char*)f;
    char* cEnd = cF + stringLength(cF) - 1;
    char* marker = NULL;
    while (cF != cEnd ){
        if (*cString == 0) return -1;
        if (*cF++ == *cString){
            if (marker == NULL)
                marker = cString;
        }else{
            cF = (char*)f;
            marker = NULL;
        }
        cString++;
    }
    
    if (marker == NULL){
        return -1;
    }else{
        return (strpos_t)(marker - string);
    }
}

uint8_t stringCompare(const char* stringA, const char* stringB){
    uint32_t lenA = stringLength(stringA);
    
    return stringCompareRAW(stringA, stringB, lenA);
}
