#include "stack.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "error_types.h"

static const int kCanareika = 25022007;
static const int kPoison = 525252;
static const int grow_data_coefficient = 2;


void PrintElement(ElementType element)
{
    printf("%d", element);
}

int StackCtor(Stack* stack_pointer, size_t starting_capacity) //FIXME как проинициализировать все элементы data пойзон значением, отличным от 0?
{
    assert(stack_pointer); //FIXME верификатор в начале вместо ассертов и вызывать верификатор по указателю, а в нём проверить указатель на нуль
    assert(starting_capacity > 0);

    stack_pointer -> data = (ElementType*) calloc(starting_capacity + 2, sizeof(ElementType)); //проверить указатель выданный каллокок
    if (stack_pointer -> data == NULL)
        return ERROR_ALLOCATION_FAILED; //FIXME нормально обработать это

    stack_pointer -> data[0] = kCanareika;
    stack_pointer -> data[starting_capacity + 1] = kCanareika;
    for (size_t run_index = 1; run_index <= starting_capacity; run_index++)
        stack_pointer -> data[run_index] = kPoison;
    stack_pointer -> data = stack_pointer -> data + 1;
    stack_pointer -> size = 0;
    stack_pointer -> capacity = starting_capacity;

    return 0;
    // int errors = StackVerification(*stack_pointer); //FIXME норм что передаю разыменованный стек?
    // if (errors != 0)
    // {
        // StackDump(stack_pointer, errors, "construction failed");
    //     // abort();
    //     //FIXME - return ошибки сделай чееееел
    // }
}

void StackDtor(Stack* stack_pointer)
{
// FIXME - вызов верификатора вместо ассерта - везде
    assert(stack_pointer != NULL);

// FIXME - free if not null
    if (stack_pointer -> data)
    {
        free(stack_pointer -> data - 1);
        stack_pointer -> data = NULL;
        stack_pointer -> size = 0;
        stack_pointer -> capacity = 0;
    }
}

int StackPush(Stack* stk, ElementType value)
{
    assert(stk); //пока ассерт, но надо написать верификатор
    //TODO написать реаллокацию data в случае, если переполняем массив

    int errors = StackVerification(stk);
    if (errors != 0)
        return errors;
    if (stk->size == stk->capacity)
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
    }

    stk -> data[stk->size++] = value;
    return 0;
//     if (current_size == current_capacity) //FIXME разобраться с санитайзером
//         {// FIXME - Func Resize
//             size_t new_capacity = current_capacity * grow_data_coefficient;
//             ElementType* check_buf = (ElementType*) realloc(stk->data, new_capacity * sizeof(ElementType));
//             if (check_buf == NULL)
//             {
//                 //шо тут
//                 errors += ERROR_REALLOCATING;
//                 return errors;
//             }
//
//             stk->data = check_buf;
//             stk->capacity = new_capacity;
//         }
//     // FIXME - Canary to the end
}

ElementType StackPop(Stack* stk)//ДЕЛО СДЕЛАНО нужно ли изменять значение с бывшего на kPoison у элемента, который попнули
{
    assert(stk);

    size_t current_size = stk->size;

    int errors = StackVerification(stk);
    if (current_size == 0)  //FIXME я же это никак не внесу в верификатор (когда в массиве только канарейка)
    { //FIXME current_size == 0 должно быть
        // printf("%lu\n", current_size);
        // printf("%d\n\n", errors);
        errors += ERROR_POP_WHEN_SIZE_ZERO;
    }
    // printf("%d\n\n", errors);
    if (errors != 0)
        StackDump(stk, errors, "cannot Pop the element"); //если случилась ошибка (верификатор о ней сообщил), то я вызываю стекдамп и abort()?

    ElementType element = stk -> data[current_size - 1];
    stk -> data[current_size - 1] = kPoison;
    stk -> size = current_size - 1;

    errors = StackVerification(stk);
    if (errors != 0)
        StackDump(stk, errors, "cannot Pop the element"); //если случилась ошибка (верификатор о ней сообщил), то я вызываю стекдамп и abort()?

    return element;
}
// FIXME - Канарейка под условной компиляцией
//ДЕЛО СДЕЛАНО какая-то пизда с size, какого хуя он на 1 больше
void StackDump(const Stack* stk, int errors, const char* msg)
{
    assert(stk != NULL);
    assert(msg != NULL);
    // ВАЖНО: data сдвинут на 1 элемент вправо от начала выделенной памяти
    // Поэтому левая канарейка находится по data[-1], правая - по data[capacity]
    ElementType* real_data_start = stk->data - 1;
    size_t stack_size_only_elements = stk->size;
    size_t stack_capacity = stk->capacity;

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
    printf("    data [%p]\n", real_data_start);
    printf("        {\n");

    printf("         [0] = ");
    PrintElement(real_data_start[0]);
    printf(" (LEFT CANAREIKA)\n");

    for (size_t i = 0; i < stack_size_only_elements; i++)
    {
        // (решил) если поменяем тип (ElementType), то тут вместо %d нужно будет другой спецификатор писать, что делать?
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
{// FIXME - Принимать указатель и проверять на нулл
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
    //FIXME канарейки
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


