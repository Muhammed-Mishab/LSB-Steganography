#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>

typedef unsigned int uint;

/* Status return values */
typedef enum {
    e_failure = 0,
    e_success = 1,
    e_unsupported = 2
} Status;

/* Operation type */
typedef enum {
    e_encode = 1,
    e_decode = 2,
    e_unknown = 0
} OperationType;

#endif 
