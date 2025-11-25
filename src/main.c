#include "patchword_cli.h"
#include "coordinator.h"
#include "worker.h"
#include "dictionary.h"
#include "util_str.h"
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv) {

    // Load the dictionary into memory once for all modes
    if (load_dictionary("data/words_alpha.txt") != 0) {
        fprintf(stderr, "Failed to load dictionary (data/words_alpha.txt)\n");
        return 1;
    }

    // Distributed mode
    if (argc > 1 && strcmp(argv[1], "--distributed") == 0) {
        return coordinator_main(argc - 1, argv + 1);
    }

    // Worker mode (spawned by coordinator)
    if (argc > 1 && strcmp(argv[1], "worker") == 0) {
        return worker_main(argc - 1, argv + 1);
    }

    // Default = CLI mode
    return patchword_cli_main(argc, argv);
}
