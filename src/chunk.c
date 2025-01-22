#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) 
{
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lineBuff.count = 0;
	chunk->lineBuff.capacity = 0;
    chunk->lineBuff.lines = NULL;
	initValueArray(&chunk->constants);
}

void freeChunk(Chunk *chunk) 
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	FREE_ARRAY(LineEnc, chunk->lineBuff.lines, chunk->capacity);
	freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void writeChunk(Chunk *chunk, uint8_t byte, int line)
{
    if (chunk->lineBuff.count > 0 && chunk->lineBuff.lines[chunk->lineBuff.count - 1].line == line)
    {
        chunk->lineBuff.lines[chunk->lineBuff.count - 1].count++;
    }
    else
    {
        if (chunk->lineBuff.capacity < chunk->lineBuff.count + 1) {
            int oldCapacity = chunk->lineBuff.capacity;
            chunk->lineBuff.capacity = GROW_CAPACITY(oldCapacity);
            chunk->lineBuff.lines = GROW_ARRAY(LineEnc, chunk->lineBuff.lines, oldCapacity, chunk->lineBuff.capacity);
        }

        chunk->lineBuff.lines[chunk->lineBuff.count].line = line;
        chunk->lineBuff.lines[chunk->lineBuff.count].count = 1;
		chunk->lineBuff.count++;
    }

    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->count++;
}

void writeConstant(Chunk* chunk, Value value, int line)
{
    int index = addConstant(chunk, value);
    if (index < 256)
    {
		writeChunk(chunk, OP_CONSTANT, line);
		writeChunk(chunk, (uint8_t)index, line);
    }
    else
    {
		writeChunk(chunk, OP_CONSTANT_LONG, line);
		writeChunk(chunk, (uint8_t)(index & 0xff), line);
		writeChunk(chunk, (uint8_t)((index >> 8) & 0xff), line);
		writeChunk(chunk, (uint8_t)((index >> 16) & 0xff), line);
    }
}

int addConstant(Chunk* chunk, Value value)
{
	writeValueArray(&chunk->constants, value);
	return chunk->constants.count - 1;
}

int getLine(Chunk* chunk, int offset)
{
	for (int i = 0; i < chunk->lineBuff.count; i++)
	{
		if (offset < chunk->lineBuff.lines[i].count)
		{
			return chunk->lineBuff.lines[i].line;
		}
		offset -= chunk->lineBuff.lines[i].count;
	}

	return -1;
}
