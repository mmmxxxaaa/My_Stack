#ifndef MY_STACK_H_
#define MY_STACK_H_

#include <stdlib.h>

#include "error_types.h"

#ifdef _DEBUG
    #define INIT(name) Stack name = {__func__, __LINE__, __FILE__, #name, NULL, 0, 0}
    // INIT(stk1); //Всё время ссылается на эту строку при дампе, как будто херня какая-то
#else
    #define INIT(name) Stack name = {};
#endif

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


void PrintElement(ElementType element);
void StackCtor(Stack* stack_pointer, size_t starting_capacity);
void StackDtor(Stack* stack_pointer);

int StackVerification(Stack* stack);
int ErrorsParse(int errors);

void StackPush(Stack* stk, ElementType value);
ElementType StackPop(Stack* stk);
void StackDump(const Stack* stk, int errors, const char* msg);
int ResizeBuffer(Stack* stk);


#endif // MY_STACK_H_
