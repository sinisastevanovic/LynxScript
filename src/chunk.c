#include "chunk.h"
#include "memory.h"
#include "vm.h"

#include <stdlib.h>

/**
 * @brief Initalize a chunk to zero
 * @param chunk the existing chunk to zero out
 */
void initChunk(Chunk* chunk) 
{
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    chunk->linesCapacity = 0;
	chunk->linesCount = 0;
    initValueArray(&chunk->constants);
}

/// @brief 
/// @param chunk 
/// @param byte 
/**
 * @brief Write a byte to the chunk array, resizing if we're at capcity
 * @param chunk chunk to append to
 * @param byte byte to append to chunk
 * @param line the source code line number the chunk is associated with. Used for printing where errors are.
 */
void writeChunk(Chunk* chunk, uint8_t byte, int line) 
{
    if (chunk->linesCount > 0 && chunk->lines[chunk->linesCapacity - 1].line == line)
    {
		chunk->lines[chunk->linesCount - 1].count++;
	}
    else
    {
        if (chunk->linesCapacity < chunk->linesCount + 1)
        {
            int oldCapacity = chunk->linesCapacity;
            chunk->linesCapacity = GROW_CAPACITY(oldCapacity);
            chunk->lines = GROW_ARRAY(Line, chunk->lines, oldCapacity, chunk->linesCapacity);
        }
        chunk->lines[chunk->linesCount].line = line;
        chunk->lines[chunk->linesCount].count = 1;
        chunk->linesCount++;
    }

    if (chunk->capacity < chunk->count + 1) 
    {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->count++;
}

/**
 * @brief Add a constant number to a bytecode
 * @param chunk The bytecode to add the constant to
 * @param value The value to add to the bytecode
 * @return Index of where the value was added in the chunk
 */
int addConstant(Chunk* chunk, Value value) 
{
    push(value);
    writeValueArray(&chunk->constants, value);
    pop();
    // Return the index where the constant was appended so we can located it later
    return chunk->constants.count - 1;
}

int getLine(Chunk* chunk, int offset)
{
	int line = 0;
	for (int i = 0; i < chunk->linesCount; i++)
	{
		line += chunk->lines[i].count;
		if (line > offset)
		{
			return chunk->lines[i].line;
		}
	}
	return 0;
}

/**
 * @brief Free a bytecode
 * @param chunk The bytecode to free
 */
void freeChunk(Chunk* chunk) 
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}