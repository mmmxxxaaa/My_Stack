#ifndef ERROR_TYPES_H_
#define ERROR_TYPES_H_

typedef enum {
    ERROR_NO                    = 0, //нахуй надо
    ERROR_PTR_FUNCTION_NAME     = 1 << 1,
    ERROR_PTR_NUMBER_LINE       = 1 << 2,
    ERROR_PTR_FILE_NAME         = 1 << 3,
    ERROR_PTR_VARIABLE_NAME     = 1 << 4,
    ERROR_PTR_DATA              = 1 << 5,
    ERROR_SIZE_NUMBER           = 1 << 6,
    ERROR_CAPACITY_NUMBER       = 1 << 7
} ErrorType;

#endif // ERROR_TYPES_H_
