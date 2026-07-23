#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

void fatal(const char *message);

void dump(
    const unsigned char *data,
    size_t length
);

#endif