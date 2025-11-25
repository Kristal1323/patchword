#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "coordinator.h"
#include "worker.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "Usage: patchword <coord|worker> [args...]\n");
        return 1;
    }

    if (strcmp(argv[1], "coord") == 0) {
        return coordinator_main(argc - 1, argv + 1);
    }

    if (strcmp(argv[1], "worker") == 0) {
        return worker_main(argc - 1, argv + 1);
    }

    fprintf(stderr,
        "patchword: unknown subcommand '%s'\n", argv[1]);
    return 1;
}
