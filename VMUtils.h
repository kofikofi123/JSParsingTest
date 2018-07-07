#ifndef VM_UTILS
#define VM_UTILS

#include <stdint.h>

//Data types
typedef struct ecmaScriptString_T {
	uint16_t* Buffer;
	uint64_t Length;
}ecmaString;


typedef struct ecmaScriptObject_T {

}ecmaObject;

typedef double ecmaNumber;
typedef uint8_t ecmaBool;

typedef struct ecmaScriptValue {
	uint8_t type;
	union {
		ecmaString* StringValue;
		ecmaNumber NumberValue;
		ecmaBool BoolValue;
	} value;
}ecmaValue;


struct ecmaSciptDataPropertyDesc {
	ecmaValue Value; //will change, maybe
	ecmaBool Writable;
	ecmaBool Enumerable;
	ecmaBool Configurable;
};

struct ecmaScriptAccessPropertyDesc {
	ecmaObject* Get;
	ecmaObject* Set;
	ecmaBool Enumerable;
	ecmaBool Configurable;
};


ecmaString* newString(uint16_t*, uint64_t);
ecmaString*	convertAsciiToString(const char*);
ecmaString* convertUniToString(uint32_t*);

#endif