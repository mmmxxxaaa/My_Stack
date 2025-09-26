#include <stdio.h>
#include <stdlib.h>

#include "error_types.h"
#include "stack.h"

static const size_t starting_capacity = 5;

int main()
{
    INIT(stk1);
    StackCtor(&stk1, starting_capacity);
    // printf("%p, %lu, %lu\n", stk1.data, stk1.size, stk1.capacity);

    // StackPush(&stk1, 10);
    // printf("%p, %lu, %lu\n", stk1.data, stk1.size, stk1.capacity);
    // printf("%d\n", stk1.data[0]);
    // StackPush(&stk1, 20);
    // StackPush(&stk1, 30);
    StackPop(&stk1);

    StackDtor(&stk1);
    return 0;
}
