#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "server.h"


#define DEFAULT_PORT  7890


static int parse_port(
    const char *text,
    uint16_t *port
);

int main(int argc, char *argv[])
{
    uint16_t port = DEFAULT_PORT;

    if (argc > 2) {
        fprintf(
            stderr,
            "Использование: %s [порт]\n",
            argv[0]
        );

        return EXIT_FAILURE;
    }

    if (argc == 2) {
        if (parse_port(argv[1], &port) == -1) {
            fprintf(
                stderr,
                "Некорректный порт: %s\n",
                argv[1]
            );

            return EXIT_FAILURE;
        }
    }

    return server_run(port);
}

static int parse_port(const char *text, uint16_t *port)
{
    if ((text == NULL) || (port == NULL)) {
        return -1;
    }

    errno = 0;

    char *end = NULL;

    unsigned long value = strtoul(text, &end, 10);

    if (errno != 0) {
        return -1;
    }

    if ((end == text) || (*end != '\0')) {
        return -1;
    }

    if ((value == 0) || (value > 65535)) {
        return -1;
    }

    *port = (uint16_t)value;

    return 0;
}