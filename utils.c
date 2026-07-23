#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

void fatal(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void dump(
    const unsigned char *data,
    size_t length
)
{
    if (data == NULL) {
        return;
    }

    for (size_t i = 0; i < length; i++) {
        printf("%02x ", data[i]);

        if ((i % 16 == 15) || (i == length - 1)) {
            size_t bytes_in_line = (i % 16) + 1;

            for (size_t j = bytes_in_line; j < 16; j++) {
                printf("   ");
            }

            printf("| ");

            size_t line_start = i - bytes_in_line + 1;

            for (size_t j = line_start; j <= i; j++) {
                unsigned char byte = data[j];

                if (isprint(byte)) {
                    printf("%c", byte);
                } else {
                    printf(".");
                }
            }

            printf("\n");
        }
    }
}