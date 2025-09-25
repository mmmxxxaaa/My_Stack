#include "stack.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "error_types.h"

void StackCtor(Stack* stack_pointer, size_t starting_capacity)
{
    assert(stack_pointer);

    stack_pointer -> data = (ElementType*) calloc(starting_capacity, sizeof(ElementType)); //проверить указатель выданный каллокок
    stack_pointer -> size = 0;
    stack_pointer -> capacity = starting_capacity;
}

void StackDtor(Stack* stack_pointer)
{
    free(stack_pointer -> data);
    stack_pointer -> data = NULL;
    stack_pointer -> size = 0;
    stack_pointer -> capacity = 0;
}

ErrorType StackPush(Stack* stk, ElementType value)
{
    assert(stk); //пока ассерт, но надо написать верификатор
    //TODO написать реаллокацию data в случае, если переполняем массив
    //TODO написать реаллокацию data в случае, если уже слишком много свободных ячеек в массиве (мб сделать это в StackPop)


    stk -> data[stk->size++] = value;

    return ERROR_NO;
}

ElementType StackPop(Stack* stk)
{
    assert(stk);//TODO проверка на size>0

    size_t current_size = stk->size;
    stk->size = current_size - 1;

    int errors = StackVerification(*stk);
    // printf("ERRORS: %d\n", errors);
    StackDump(stk, errors, "govno zalupa penis her");

    ElementType element = stk -> data[current_size - 1];

    return element;
}

void StackDump(Stack* stk, int errors, const char* msg)
{
    size_t stack_size =  stk->size;
    size_t stack_capacity = stk->capacity;

    printf("stack [%p] %s (", stk, msg);
    ErrorsParse(errors);

#ifdef _DEBUG
    printf("Err%d) from %s at %s %d\n", errors, stk->function_name, stk->file_name, stk->line);
#else
    printf("Err%d)\n", errors);
#endif
    printf("    {\n");
    printf("    size = %lu\n", stack_size);
    printf("    capacity=%lu\n", stack_capacity);
    printf("    data [%p]\n", stk->data);
    printf("        {\n");
    size_t i = 0;
    for (; i < stack_size; i++)
    {
        printf("        *[%lu] = %d\n", i, stk->data[i]); //если поменяем тип (ElementType), то тут вместо %d нужно будет другой спецификатор писать, что делать?
    }
    for (; i>= stack_size && i < stack_capacity; i++)
    {
        printf("         [%lu] = %d (POISON)\n", i, stk->data[i]);
    }
    printf("        }\n");
    printf("}\n");
}

int StackVerification(Stack stack)
{
    int errors = 0;
#ifdef _DEBUG
    if (stack.function_name == NULL)
        errors += ERROR_PTR_FUNCTION_NAME;

    if (stack.line == 0)
        errors += ERROR_PTR_NUMBER_LINE;

    if (stack.file_name == NULL)
        errors += ERROR_PTR_FILE_NAME;

    if (stack.variable_name == NULL)
        errors += ERROR_PTR_VARIABLE_NAME;
#endif
    if (stack.data == NULL)
        errors += ERROR_PTR_DATA;

    if (stack.capacity == 0)
        errors += ERROR_CAPACITY_NUMBER;

    if (stack.size > stack.capacity)
        errors += ERROR_SIZE_NUMBER;

    return errors;
}

int ErrorsParse(int errors)
{
#define CHECKING_ERROR(s)  // заменяет две строки с ифом на одну
    if (errors == 0)
        return 0;
    if (errors & ERROR_PTR_FUNCTION_NAME)
        printf("ERROR_PTR_FUNCTION_NAME ");
    if (errors & ERROR_PTR_NUMBER_LINE)
        printf("ERROR_PTR_NUMBER_LINE ");
    if (errors & ERROR_PTR_FILE_NAME)
        printf("ERROR_PTR_FILE_NAME ");
    if (errors & ERROR_PTR_VARIABLE_NAME)
        printf("ERROR_PTR_VARIABLE_NAME ");
    if (errors & ERROR_PTR_DATA)
        printf("ERROR_PTR_DATA ");
    if (errors & ERROR_SIZE_NUMBER)
        printf("ERROR_SIZE_NUMBER ");
    if (errors & ERROR_CAPACITY_NUMBER)
        printf("ERROR_CAPACITY_NUMBER ");
    return 1;
#undef CHECKING_ERROR
}
