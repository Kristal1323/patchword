#include "util_signals.h"
#include <signal.h>
#include <stddef.h>
#include <stdatomic.h>

static volatile sig_atomic_t child_flag = 0;

static void sigchld_handler(int sig) {
    (void)sig;
    child_flag = 1;
}

void install_sigchld_handler(void) {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);
}

int child_died(void) {
    return child_flag != 0;
}

void clear_child_flag(void) {
    child_flag = 0;
}
