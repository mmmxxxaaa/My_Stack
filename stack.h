#ifndef MY_STACK_H_
#define MY_STACK_H_

#include <stdlib.h>

#include "error_types.h"

typedef int ElementType;

typedef struct {
#ifdef _DEBUG
    const char* function_name;
    int line;
    const char* file_name;
    const char* variable_name;
#endif
    ElementType* data;
    size_t size;
    size_t capacity;

} Stack;

void StackCtor(Stack* stack_pointer, size_t starting_capacity);
void StackDtor(Stack* stack_pointer);

int StackVerification(Stack stack);
int ErrorsParse(int errors); //FIXME парсер ошибки

ErrorType StackPush(Stack* stk, ElementType value);
ElementType StackPop(Stack* stk);
void StackDump(Stack* stk, int errors, const char* msg);


#endif // MY_STACK_H_
