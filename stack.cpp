#include "stack.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "error_types.h"

static const int kCanareika = 25022007;
static const int kPoison = 525252;
static const int grow_data_coefficient = 2;

#ifdef _DEBUG
    #define ON_DEBUG(code)  code //не заигрываться с этим
#else
    #define ON_DEBUG(code)
#endif

void StackCtor(Stack* stack_pointer, size_t starting_capacity)
{
    assert(stack_pointer); //FIXME ТУТ ПРИШЛОСЬ ОСТАВИТЬ ПОКА АССЕРТ ВМЕСТО ВЕРИФИКАТОРА, Т.К. ИНИЦИАЛИЗИРОВАННАЯ ВЕРСИЯ СОВПАДАЕТ С ОШИБОЧНОЙ
    assert(starting_capacity > 0);

    stack_pointer -> data = (ElementType*) calloc(starting_capacity + ON_DEBUG (2) +0, sizeof(ElementType)); //+0 воспринимается просто как 0
    if (stack_pointer -> data == NULL)
    {
        int errors = ERROR_ALLOCATION_FAILED;
        StackDump(stack_pointer, errors, "Allocating memory in StackCtor failed");
        return;
    }

    stack_pointer -> data[0] = kCanareika;
    stack_pointer -> data[starting_capacity + 1] = kCanareika;

    for (size_t run_index = 1; run_index <= starting_capacity; run_index++)
        stack_pointer -> data[run_index] = kPoison;

    stack_pointer -> data = stack_pointer -> data + 1;
    stack_pointer -> size = 0;
    stack_pointer -> capacity = starting_capacity;
}

void StackDtor(Stack* stack_pointer)
{
    int errors = StackVerification(stack_pointer); //ассерт не нужен, т.к. верификатор проверяет указатель
    if (errors != 0)
    {
        StackDump(stack_pointer, errors, "StackDtor failed");
        return;
    }

    if (stack_pointer -> data)
        free(stack_pointer -> data - 1);
    stack_pointer -> data = NULL;
    stack_pointer -> size = 0;
    stack_pointer -> capacity = 0;
}

void StackPush(Stack* stk, ElementType value)
{
    int errors = StackVerification(stk); //ассерт не нужен, т.к. верификатор проверяет указатель
    if (errors != 0)
    {
        StackDump(stk, errors, "StackPush failed");
        return;
    }
    if (stk->size == stk->capacity)
    {
        errors = ResizeBuffer(stk);
        if (errors != 0)
        {
            StackDump(stk, errors, "Reallocating in StackPush failed");
            return;
        }
    }

    stk -> data[stk->size++] = value;
}

ElementType StackPop(Stack* stk)
{
    int errors = StackVerification(stk);
    if (errors != 0)
    {
        StackDump(stk, errors, "StackPop failed");
        return kPoison;
    }

    size_t current_size = stk->size;
    if (current_size == 0)
        errors += ERROR_POP_WHEN_SIZE_ZERO;
    if (errors != 0)
    {
        StackDump(stk, errors, "There is NO ELEMENT in stack to pop");
        return kPoison;
    }

    ElementType element = stk -> data[current_size - 1];
    stk -> data[current_size - 1] = kPoison;
    stk -> size = current_size - 1;

    errors = StackVerification(stk);
    if (errors != 0)
    {
        StackDump(stk, errors, "cannot Pop the element");
        return kPoison;
    }

    return element;
}

void StackDump(const Stack* stk, int errors, const char* msg)
{
    assert(msg != NULL);

    if (stk == NULL)
    {
        printf("stack [%p] %s (", stk, msg);
        return;
    }

    // левая канарейка находится по data[-1], а правая по data[capacity]
    ElementType* real_data_start = stk->data - 1;
    size_t stack_size_only_elements = stk->size;
    size_t stack_capacity = stk->capacity;

    printf("stack [%p] %s (", stk, msg);
    ErrorsParse(errors);

#ifdef _DEBUG //FIXME написать функцию перевода числа в двоичный вид, чтобы выводить
    printf("Err%d) from %s at %s %d\n", errors, stk->function_name, stk->file_name, stk->line);
#else
    printf("Err%d)\n", errors); //выводить ошибки в двоичном виде
#endif
    printf("    {\n");
    printf("    size = %lu\n", stack_size_only_elements);
    printf("    capacity=%lu\n", stack_capacity);
    printf("    data [%p]\n", real_data_start);
    printf("        {\n");

    printf("         [0] = ");
    PrintElement(real_data_start[0]);
    printf(" (LEFT CANAREIKA)\n");

    for (size_t i = 0; i < stack_size_only_elements; i++)
    {
        printf("        *[%lu] = ", i + 1); // +1 потому что реальные индексы с 1
        PrintElement(stk->data[i]);
        printf("\n");
    }
    for (size_t i = stack_size_only_elements; i < stack_capacity; i++)
    {
        printf("         [%lu] = ", i + 1); // +1 потому что реальные индексы с 1
        PrintElement(stk->data[i]);
        printf(" (POISON)\n");
    }

    printf("         [%lu] = ", stack_capacity + 1);
    PrintElement(stk->data[stack_capacity]);
    printf(" (RIGHT CANAREIKA)\n");

    printf("        }\n");
    printf("}\n");
}

int StackVerification(Stack* stack)
{
    if (stack == NULL)
        return ERROR_NULL_PTR;

    int errors = 0;
#ifdef _DEBUG
    if (stack -> function_name == NULL)
        errors += ERROR_PTR_FUNCTION_NAME;

    if (stack -> line == 0)
        errors += ERROR_PTR_NUMBER_LINE;

    if (stack -> file_name == NULL)
        errors += ERROR_PTR_FILE_NAME;

    if (stack -> variable_name == NULL)
        errors += ERROR_PTR_VARIABLE_NAME;
#endif
    if (stack -> data == NULL)
    {
        errors += ERROR_PTR_DATA;
        return errors; //дальше нечего проверять
    }

    if (stack -> capacity == 0)
    {
        errors += ERROR_CAPACITY_NUMBER;
        return errors; //дальше нечего проверять
    }
    //FIXME канарейки в условную компиляцию
    if (stack -> data[stack->capacity] != kCanareika) //если data = null, то эти проверки положат программу, надо бы их внутрь проверки на null засунуть
        errors += ERROR_RIGHT_CANAREIKA_DAMAGED;

    if (stack -> data[-1] != kCanareika) //ваще пиздец))
        errors += ERROR_LEFT_CANAREIKA_DAMAGED;

    if (stack->size > stack->capacity)
        errors += ERROR_SIZE_NUMBER;

    return errors;
}

int ErrorsParse(int errors)
{
#define CHECKING_ERROR(s)  // FIXME заменяет две строки с ифом на одну
    if (errors == 0)
        return 0;

    if (errors & ERROR_PTR_FUNCTION_NAME)  printf("ERROR_PTR_FUNCTION_NAME ");
    if (errors & ERROR_PTR_NUMBER_LINE)    printf("ERROR_PTR_NUMBER_LINE ");
    if (errors & ERROR_PTR_FILE_NAME)      printf("ERROR_PTR_FILE_NAME ");
    if (errors & ERROR_PTR_VARIABLE_NAME)  printf("ERROR_PTR_VARIABLE_NAME ");
    if (errors & ERROR_PTR_DATA)           printf("ERROR_PTR_DATA ");
    if (errors & ERROR_SIZE_NUMBER)        printf("ERROR_SIZE_NUMBER ");
    if (errors & ERROR_CAPACITY_NUMBER)    printf("ERROR_CAPACITY_NUMBER ");
    if (errors & ERROR_POP_WHEN_SIZE_ZERO) printf("ERROR_POP_WHEN_SIZE_ZERO ");

    if (errors & ERROR_RIGHT_CANAREIKA_DAMAGED)
        printf("ERROR_RIGHT_CANAREIKA_DAMAGED ");
    if (errors & ERROR_LEFT_CANAREIKA_DAMAGED)
        printf("ERROR_LEFT_CANAREIKA_DAMAGED ");
    return 1;
#undef CHECKING_ERROR
}


int ResizeBuffer(Stack* stk)
{
    size_t new_capacity = (stk->capacity == 0) ? 1 : stk->capacity * 2;

    size_t total_size = (new_capacity + 2) * sizeof(ElementType);
    ElementType* new_memory = (ElementType*) realloc(stk->data - 1, total_size);
    if (new_memory == NULL)
        return ERROR_REALLOCATION;

    new_memory[0] = kCanareika;
    new_memory[new_capacity + 1] = kCanareika;

    // ИНИЦИАЛИЗИРУЕМ НОВЫЕ POISONЫ
    for (size_t i = stk->capacity + 1; i <= new_capacity; i++)
        new_memory[i] = kPoison;

    stk->data = new_memory + 1;
    stk->capacity = new_capacity;
    return 0;
}

void PrintElement(ElementType element)
{
    printf("%d", element);
}

//ПОБИТОВЫЕ ОПЕРАЦИИ ВМЕСТО +=
//написать мейн, показывающий различные ошибки
//доделать условную компиляцию канареек
//сделать хэши
//поменяться кодом с Умаром
