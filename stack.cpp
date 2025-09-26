#include "stack.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "error_types.h"
//FIXME должен ли выводиться результат в консоль?
static const int kCanareika = 25022007;
static const int kPoison = 525252;


void PrintElement(ElementType element)
{
    printf("%d", element);
}

void StackCtor(Stack* stack_pointer, size_t starting_capacity) //FIXME как проинициализировать все элементы data пойзон значением, отличным от 0?
{
    assert(stack_pointer);
    assert(starting_capacity > 0);

    stack_pointer -> data = (ElementType*) calloc(starting_capacity + 2, sizeof(ElementType)); //проверить указатель выданный каллокок
    if (stack_pointer -> data == NULL)
        return; //FIXME нормально обработать это

    stack_pointer -> data[0] = kCanareika;
    for (size_t run_index = 1; run_index <= starting_capacity; run_index++)
        stack_pointer -> data[run_index] = kPoison;
    stack_pointer -> data[starting_capacity + 1] = kCanareika;
    stack_pointer -> size = 1;
    stack_pointer -> capacity = starting_capacity;
    int errors = StackVerification(*stack_pointer); //норм что передаю разыменованный стек?
    if (errors != 0)
    {
        StackDump(stack_pointer, errors, "construction failed");
        // abort();
    }
}

void StackDtor(Stack* stack_pointer)
{
    assert(stack_pointer != NULL);

    free(stack_pointer -> data);
    stack_pointer -> data = NULL;
    stack_pointer -> size = 0;
    stack_pointer -> capacity = 0;
}

ErrorType StackPush(Stack* stk, ElementType value)
{
    assert(stk); //пока ассерт, но надо написать верификатор
    //TODO написать реаллокацию data в случае, если переполняем массив

    stk -> data[stk->size++] = value;

    return ERROR_NO;
}

ElementType StackPop(Stack* stk)//ДЕЛО СДЕЛАНО нужно ли изменять значение с бывшего на kPoison у элемента, который попнули
{
    assert(stk);

    size_t current_size = stk->size;

    int errors = StackVerification(*stk);
    if (current_size == 1)  //FIXME я же это никак не внесу в верификатор (когда в массиве только канарейка)
    {
        // printf("%lu\n", current_size);
        // printf("%d\n\n", errors);
        errors += ERROR_POP_WHEN_SIZE_ZERO;
    }
    // printf("%d\n\n", errors);
    if (errors != 0)
        StackDump(stk, errors, "cannot Pop the element"); //если случилась ошибка (верификатор о ней сообщил), то я вызываю стекдамп и abort()?

    ElementType element = stk -> data[current_size - 1];
    stk -> data[current_size - 1] = kPoison;
    stk->size = current_size - 1;

    errors = StackVerification(*stk);
    if (errors != 0)
        StackDump(stk, errors, "cannot Pop the element"); //если случилась ошибка (верификатор о ней сообщил), то я вызываю стекдамп и abort()?

    return element;
}
//ДЕЛО СДЕЛАНО какая-то пизда с size, какого хуя он на 1 больше
void StackDump(const Stack* stk, int errors, const char* msg)
{
    assert(stk != NULL);
    assert(msg != NULL);

    size_t stack_size_with_left_canareika =  stk->size;
    size_t stack_size_only_elements =  stack_size_with_left_canareika - 1;
    size_t stack_capacity = stk->capacity + 2; //почему тут +2, а не в конструкторе

    printf("stack [%p] %s (", stk, msg);
    ErrorsParse(errors);

#ifdef _DEBUG
    printf("Err%d) from %s at %s %d\n", errors, stk->function_name, stk->file_name, stk->line);
#else
    printf("Err%d)\n", errors); //выводить ошибки в двоичном виде
#endif
    printf("    {\n");
    printf("    size = %lu\n", stack_size_only_elements);
    printf("    capacity=%lu\n", stack_capacity);
    printf("    data [%p]\n", stk->data);
    printf("        {\n");

    size_t i = 0;
    printf("         [%lu] = ", i);
    PrintElement(stk->data[i++]);
    printf(" (LEFT CANAREIKA)\n");

    for (; i < stack_size_with_left_canareika; i++)
    {
        // (решил) если поменяем тип (ElementType), то тут вместо %d нужно будет другой спецификатор писать, что делать?
        printf("        *[%lu] = ", i);
        PrintElement(stk->data[i]);
        printf("\n"); //putc?
    }
    for (; i>= stack_size_with_left_canareika && i < stack_capacity - 1; i++)
    {
        printf("         [%lu] = ", i);
        PrintElement(stk->data[i]);
        printf(" (POISON)\n");
    }

    printf("         [%lu] = ", i);
    PrintElement(stk->data[i]);
    printf(" (RIGHT CANAREIKA)\n");

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
    else
    {
        if (stack.data[stack.capacity + 1] != kCanareika) //если data = null, то эти проверки положат программу, надо бы их внутрь проверки на null засунуть
        errors += ERROR_RIGHT_CANAREIKA_DAMAGED;

        if (stack.data[0] != kCanareika)
        errors += ERROR_LEFT_CANAREIKA_DAMAGED;
    }

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
    if (errors & ERROR_POP_WHEN_SIZE_ZERO)
        printf("ERROR_POP_WHEN_SIZE_ZERO ");
    if (errors & ERROR_RIGHT_CANAREIKA_DAMAGED)
        printf("ERROR_RIGHT_CANAREIKA_DAMAGED ");
    if (errors & ERROR_LEFT_CANAREIKA_DAMAGED)
        printf("ERROR_LEFT_CANAREIKA_DAMAGED ");
    return 1;
#undef CHECKING_ERROR
}


