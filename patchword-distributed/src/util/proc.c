#include "proc.h"
#include <sys/wait.h>
#include <unistd.h>

pid_t reap_any_child(void) {
    return waitpid(-1, NULL, WNOHANG);
}
