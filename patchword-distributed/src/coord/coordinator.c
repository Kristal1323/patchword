#include "coordinator.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *task_file;
    int num_workers;
} CoordConfig;

static void usage(void) {
    fprintf(stderr,
        "Usage: patchword coord --tasks FILE --workers N\n");
}

static int parse_args(int argc, char **argv, CoordConfig *cfg) {
    cfg->task_file = NULL;
    cfg->num_workers = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--tasks") == 0) {
            if (++i >= argc) return -1;
            cfg->task_file = argv[i];
        } else if (strcmp(argv[i], "--workers") == 0) {
            if (++i >= argc) return -1;
            cfg->num_workers = atoi(argv[i]);
        } else {
            return -1;
        }
    }

    if (!cfg->task_file || cfg->num_workers <= 0) return -1;
    return 0;
}

int coordinator_main(int argc, char **argv) {
    CoordConfig cfg;

    if (parse_args(argc, argv, &cfg) < 0) {
        usage();
        return 1;
    }

    log_info("Coordinator starting with %d workers, tasks=%s",
             cfg.num_workers, cfg.task_file);

    // To be implemented further
    return 0;
}
