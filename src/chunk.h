#pragma once

#include "common.h"
#include "value.h"

/**
 * @brief Operation Codes that the lox language supports
 */
typedef enum
{
    OP_CONSTANT,
    OP_NULL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_UPVALUE,
	OP_SET_UPVALUE,
    OP_GET_PROPERTY,
	OP_SET_PROPERTY,
    OP_GET_SUPER,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,
    OP_CALL,
    OP_INVOKE,
    OP_SUPER_INVOKE,
    OP_CLOSURE,
	OP_CLOSE_UPVALUE,
    OP_RETURN,
    OP_CLASS,
    OP_INHERIT,
    OP_METHOD
} OpCode;

typedef struct
{
    int line;
    int count;
} Line;

/**
 * @brief Chunks are a sequence of bytecode
 */
typedef struct {
    int count; ///< Number of elements allocated in code array
    int capacity; ///< Maximum size of code array
    uint8_t* code; ///< The array of bytes of code
    int linesCount;
	int linesCapacity;
    Line* lines; ///< Array of lines to relate to source code, mirrors the code array and only stores the line number for the code
    ValueArray constants; ///< Array of constants used for bytecode
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);
int getLine(Chunk* chunk, int offset);