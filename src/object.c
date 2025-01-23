#include "object.h"
#include "memory.h"
#include "value.h"
#include "vm.h"
#include "table.h"

#include <stdio.h>
#include <string.h>

#define ALLOCATE_OBJ(type, objectType) \
	(type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type)
{
	Obj* object = (Obj*)reallocate(NULL, 0, size);
	object->type = type;

	object->next = vm.objects;
	vm.objects = object;
	return object;
}

//static ObjString* allocateString(char* chars, int length)
//{
//	ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
//	string->length = length;
//	string->chars = chars;
//	return string;
//}

static uint32_t hashString(const char* key, int length)
{
	uint32_t hash = 2166136261u;
	for (int i = 0; i < length; i++)
	{
		hash ^= (uint8_t)key[i];
		hash *= 16777619;
	}
	return hash;
}

ObjString* makeString(int length)
{
	ObjString* string = (ObjString*)allocateObject(sizeof(ObjString) + length + 1, OBJ_STRING);
	string->length = length;
	return string;
}

ObjString* copyString(const char* chars, int length)
{
	uint32_t hash = hashString(chars, length);
	ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
	if (interned != NULL)
		return interned;

	ObjString* string = makeString(length);
	memcpy(string->chars, chars, length);
	string->chars[length] = '\0';
	string->hash = hash;
	tableSet(&vm.strings, string, NULL_VAL);
	return string;
}

void printObject(Value value)
{
	switch (OBJ_TYPE(value))
	{
	case OBJ_STRING:
		printf("%s", AS_CSTRING(value));
		break;
	}
}
