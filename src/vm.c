#include "vm.h"
#include "common.h"
#include "compiler.h"
#include "object.h"
#include "memory.h"
#ifdef DEBUG_TRACE_EXECUTION
#include "debug.h"
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

VM vm;

static void resetStack()
{
	vm.stackTop = vm.stack;
}

static void runtimeError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);
	size_t instruction = vm.ip - vm.chunk->code - 1;
	int line = getLine(vm.chunk, instruction);
	fprintf(stderr, "[line %d] in script\n", line);
	resetStack();
}

void initVM()
{
	resetStack();
	vm.objects = NULL;
}

void freeVM()
{
	freeObjects();
}

void push(Value value)
{
	*vm.stackTop = value;
	vm.stackTop++;
}

Value pop()
{
	vm.stackTop--;
	return *vm.stackTop;
}

static Value peek(int distance)
{
	return vm.stackTop[-1 - distance];
}

static bool isFalsey(Value value)
{
	return IS_NULL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate()
{
	ObjString* b = AS_STRING(pop());
	ObjString* a = AS_STRING(pop());
	
	int length = a->length + b->length;
	ObjString* result = makeString(length);
	memcpy(result->chars, a->chars, a->length);
	memcpy(result->chars + a->length, b->chars, b->length);
	result->chars[length] = '\0';
	
	push(OBJ_VAL(result));
}

static InterpretResult run()
{
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

// Trick to ensure those statements end up in the same scope
#define BINARY_OP(valueType, op) \
    do { \
	  if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
		runtimeError("Operands must be numbers."); \
		return INTERPRET_RUNTIME_ERROR; \
	  } \
      double b = AS_NUMBER(pop()); \
      double a = AS_NUMBER(pop()); \
      push(valueType(a op b)); \
    } while (false)

	for (;;)
	{
#ifdef DEBUG_TRACE_EXECUTION
		printf("          ");
		for (Value* slot = vm.stack; slot < vm.stackTop; slot++)
		{
			printf("[ ");
			printValue(*slot);
			printf(" ]");
		}
		printf("\n");
		disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
		uint8_t instruction;
		switch (instruction = READ_BYTE())
		{
			case OP_CONSTANT:
			{
				Value constant = READ_CONSTANT();
				push(constant);
				break;
			}
			case OP_CONSTANT_LONG:
			{
				// TODO: is there a way to do it in one line?
				uint32_t constantIndex = READ_BYTE();
				constantIndex |= READ_BYTE() << 8;
				constantIndex |= READ_BYTE() << 16;
				Value constant = vm.chunk->constants.values[constantIndex];
 				push(constant);
				break;
			}
			case OP_NULL:		push(NULL_VAL); break;
			case OP_TRUE:		push(BOOL_VAL(true)); break;
			case OP_FALSE:		push(BOOL_VAL(false)); break;
			case OP_EQUAL:
			{
				Value b = pop();
				Value a = pop();
				push(BOOL_VAL(valuesEqual(a, b)));
				break;
			}
			case OP_GREATER:	BINARY_OP(BOOL_VAL, > ); break;
			case OP_LESS:		BINARY_OP(BOOL_VAL, < ); break;
			case OP_ADD:
			{
				// TODO: Add implicit type conversion if at least one operand is a string
				// TODO: Add other useful string manipulation functions STRING INTERPOLATION!
				if (IS_STRING(peek(0)) && IS_STRING(peek(1)))
				{
					concatenate();
				}
				else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1)))
				{
					double b = AS_NUMBER(pop());
					double a = AS_NUMBER(pop());
					push(NUMBER_VAL(a + b));
				}
				else
				{
					runtimeError("Operands must be two numbers or two strings.");
					return INTERPRET_RUNTIME_ERROR;
				}
				break;
			}
			case OP_SUBTRACT:	BINARY_OP(NUMBER_VAL, -); break;
			case OP_MULTIPLY:	BINARY_OP(NUMBER_VAL, *); break;
			case OP_DIVIDE:		BINARY_OP(NUMBER_VAL, /); break;
			case OP_NOT:		push(BOOL_VAL(isFalsey(pop()))); break; // TODO: No need to pop and push, just modify the value
			case OP_NEGATE: 
			{
				if (!IS_NUMBER(peek(0)))
				{
					runtimeError("Operand must be a number.\n");
					return INTERPRET_RUNTIME_ERROR;
				}
				push(NUMBER_VAL(-AS_NUMBER(pop()))); // TODO: No need to pop and push, just modify the value
				break;
			}
			case OP_RETURN:
			{
				printValue(pop());
				printf("\n");
				return INTERPRET_OK;
			}
		}
	}

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_CONSTANT_LONG
#undef BINARY_OP
}

InterpretResult interpret(const char* source)
{
	Chunk chunk;
	initChunk(&chunk);

	if (!compile(source, &chunk))
	{
		freeChunk(&chunk);
		return INTERPRET_COMPILE_ERROR;
	}

	vm.chunk = &chunk;
	vm.ip = vm.chunk->code;

	InterpretResult result = run();

	freeChunk(&chunk);
	return result;
}