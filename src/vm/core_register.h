#ifndef CORE_REGISTER_H
#define CORE_REGISTER_H

#include <stdint.h>

typedef
struct core_register {
    int32_t value;
    int32_t mode;
}
Register;

#endif