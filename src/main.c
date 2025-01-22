#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: support string maniputlation: "${drink} will be ready in ${steep + cool} minutes."

static void repl()
{
	char line[1024];
	for (;;)
	{
		printf("> ");
		if (!fgets(line, sizeof(line), stdin))
		{
			printf("\n");
			break;
		}

		// Ensure the input ends with a newline character
		size_t length = strlen(line);
		if (length > 0 && line[length - 1] != '\n')
		{
			if (length < sizeof(line) - 1)
			{
				line[length] = '\n';
				line[length + 1] = '\0';
			}
		}
		interpret(line);
	}
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
	InterpretResult result = interpret(source);
	free(source);

	if (result == INTERPRET_COMPILE_ERROR)
		exit(65);
	if (result == INTERPRET_RUNTIME_ERROR)
		exit(70);
}

int main(int argc, const char* argv[]) 
{
    initVM();

    if (argc == 1)
        repl();
	else if (argc == 2)
		runFile(argv[1]);
    else
	{
		fprintf(stderr, "Usage: clox [path]\n");
		exit(64);
	}

    freeVM();
    return 0;
}