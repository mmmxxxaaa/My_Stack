#include <stdio.h>
#include <stdlib.h>

#include "error_types.h"
#include "stack.h"

#define INIT(name) Stack name = {__func__, __LINE__, __FILE__, #name, NULL, 0, 0}

static const size_t starting_capacity = 5;

int main()
{
#ifdef _DEBUG
    INIT(stk1);

#else
    Stack stk1 = {};
#endif

    StackCtor(&stk1, starting_capacity);
    // printf("%p, %lu, %lu\n", stk1.data, stk1.size, stk1.capacity);

    StackPush(&stk1, 10);
    // printf("%p, %lu, %lu\n", stk1.data, stk1.size, stk1.capacity);
    // printf("%d\n", stk1.data[0]);
    StackPush(&stk1, 20);
    StackPush(&stk1, 30);
    StackPop(&stk1);

    StackDtor(&stk1);
    return 0;
}
