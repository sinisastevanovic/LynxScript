#include "lynxscript.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static LynxErrorFn errorCallback = NULL;

LYNX_API void lxSetErrorCallback(LynxErrorFn callback)
{
	errorCallback = callback;
}

LYNX_API void lxInitVM()
{
	initVM();
}

LYNX_API void lxExecuteScript(const char* source)
{
	InterpretResult result = interpret(source);
	//free(source);

	if (errorCallback != NULL && result == INTERPRET_COMPILE_ERROR)
		errorCallback(65);
	if (errorCallback != NULL && result == INTERPRET_RUNTIME_ERROR)
		errorCallback(70);
}

static char* readFile(const char* path)
{
	FILE* file = fopen(path, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "Could not open file \"%s\".\n", path);
		exit(74);
	}
	fseek(file, 0L, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);
	char* buffer = (char*)malloc(fileSize + 1);
	if (buffer == NULL)
	{
		fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
		exit(74);
	}
	size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
	if (bytesRead < fileSize)
	{
		fprintf(stderr, "Could not read file \"%s\".\n", path);
		exit(74);
	}
	buffer[bytesRead] = '\0';
	fclose(file);
	return buffer;
}

static void runFile(const char* path)
{
	char* source = readFile(path);
	lxExecuteScript(source);
}

LYNX_API void lxExecuteFile(const char* path)
{
	runFile(path);
}

LYNX_API void lxFreeVM()
{
	freeVM();
}
