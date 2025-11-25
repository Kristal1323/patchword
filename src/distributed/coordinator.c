#include "coordinator.h"
#include "util_fd.h"
#include "util_io.h"
#include "util_ipc.h"
#include "util_log.h"
#include "util_proc.h"
#include "util_signals.h"
#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

//
// ------------------ Coordinator Structures ------------------
//

typedef struct {
    int id;
    char payload[MAX_PAYLOAD];
    int assigned;
    int completed;
} Task;

typedef struct {
    pid_t pid;
    PipePair to_worker;
    PipePair from_worker;
    int alive;
    int busy;
    int current_task;
} Worker;

typedef struct {
    char *task_file;
    int num_workers;
} CoordConfig;


//
// ------------------ Argument Parsing ------------------
//

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

    if (!cfg->task_file || cfg->num_workers <= 0) {
        return -1;
    }

    return 0;
}


//
// ------------------ Task Loading ------------------
//

static Task* load_tasks(const char *path, int *count) {
    FILE *f = fopen(path, "r");
    if (!f) {
        log_error("Cannot open task file %s", path);
        return NULL;
    }

    Task *tasks = NULL;
    int cap = 0;
    int n = 0;
    char line[256];

    while (fgets(line, sizeof(line), f)) {
        if (n >= cap) {
            cap = cap ? cap * 2 : 16;
            tasks = realloc(tasks, sizeof(Task) * cap);
        }
        tasks[n].id = n;
        strncpy(tasks[n].payload, line, sizeof(tasks[n].payload));
        tasks[n].payload[strcspn(tasks[n].payload, "\n")] = '\0';
        tasks[n].assigned = 0;
        tasks[n].completed = 0;
        n++;
    }

    fclose(f);
    *count = n;
    return tasks;
}


//
// ------------------ Assigning Tasks to Workers ------------------
//

static int assign_task_to_worker(Worker *w, Task *t) {
    Message msg;
    msg.type = MSG_TASK;
    msg.id = t->id;
    strncpy(msg.payload, t->payload, MAX_PAYLOAD - 1);
    msg.payload[MAX_PAYLOAD - 1] = '\0';

    char out[MAX_MSG];
    format_message(out, sizeof(out), &msg);
    safe_writeline(w->to_worker.write_fd, out);

    w->busy = 1;
    w->current_task = t->id;
    t->assigned = 1;

    log_info("Assigned task %d to worker pid=%d", t->id, w->pid);
    return 0;
}


//
// ------------------ Coordinator Main ------------------
//

int coordinator_main(int argc, char **argv) {
    CoordConfig cfg;

    if (parse_args(argc, argv, &cfg) < 0) {
        usage();
        return 1;
    }

    log_info("Coordinator starting with %d workers, tasks file: %s",
             cfg.num_workers, cfg.task_file);

    //
    // Load tasks
    //
    int task_count = 0;
    Task *tasks = load_tasks(cfg.task_file, &task_count);
    if (!tasks) return 1;

    log_info("Loaded %d tasks", task_count);

    //
    // Spawn workers
    //
    install_sigchld_handler();
    Worker *workers = calloc(cfg.num_workers, sizeof(Worker));

    const char *worker_path = "./patchword"; // run the same binary from cwd
    for (int i = 0; i < cfg.num_workers; i++) {
        char *wargv[] = { (char *)worker_path, "worker", NULL };

        if (spawn_worker_process(worker_path,
                                wargv,
                                &workers[i].to_worker,
                                &workers[i].from_worker,
                                &workers[i].pid) < 0) {
            log_error("Failed to spawn worker %d", i);
            return 1;
        }

        workers[i].alive = 1;
        workers[i].busy = 0;
        workers[i].current_task = -1;

        log_info("Spawned worker %d pid=%d", i, workers[i].pid);
    }

    //
    // Prepare poll() array
    //
    struct pollfd *pfds = calloc(cfg.num_workers, sizeof(struct pollfd));
    for (int i = 0; i < cfg.num_workers; i++) {
        pfds[i].fd = workers[i].from_worker.read_fd;
        pfds[i].events = POLLIN;
    }

    //
    // ------------------ Main Event Loop ------------------
    //
    while (1) {

        // Check if all tasks done
        int all_done = 1;
        for (int t = 0; t < task_count; t++) {
            if (!tasks[t].completed) {
                all_done = 0;
                break;
            }
        }
        if (all_done) break;

        //
        // Assign tasks to idle workers
        //
        for (int i = 0; i < cfg.num_workers; i++) {
            if (!workers[i].alive || workers[i].busy) continue;

            for (int t = 0; t < task_count; t++) {
                if (!tasks[t].assigned) {
                    assign_task_to_worker(&workers[i], &tasks[t]);
                    break;
                }
            }
        }

        //
        // poll() for messages
        //
        int pr = poll(pfds, cfg.num_workers, 100);
        if (pr < 0) {
            if (child_died()) {
                clear_child_flag();
                pid_t dead;
                while ((dead = reap_any_child()) > 0) {
                    log_warn("Worker pid %d died unexpectedly", dead);
                }
            }
            continue;
        }

        if (pr == 0)
            continue;

        //
        // Read available worker messages
        //
        for (int i = 0; i < cfg.num_workers; i++) {

            if (!workers[i].alive) continue;

            if (pfds[i].revents & POLLIN) {
                char buf[MAX_MSG];
                ssize_t n = safe_readline(workers[i].from_worker.read_fd,
                                          buf, sizeof(buf));
                if (n <= 0) continue;

                Message msg;
                if (parse_message(buf, &msg) < 0) {
                    log_warn("Invalid message from worker %d: %s", i, buf);
                    continue;
                }

                switch (msg.type) {

                case MSG_WORKER_READY:
                    log_info("Worker %d ready", i);
                    break;

                case MSG_RESULT: {
                    int tid = msg.id;
                    tasks[tid].completed = 1;
                    workers[i].busy = 0;
                    workers[i].current_task = -1;

                    log_info("Worker %d completed task %d (%s)",
                             i, tid, msg.payload);
                    break;
                }

                case MSG_EXITING:
                    log_info("Worker %d exiting", i);
                    workers[i].alive = 0;
                    break;

                default:
                    log_warn("Unknown message type %d from worker %d",
                             msg.type, i);
                }
            }

            // Detect hangup
            if (pfds[i].revents & POLLHUP) {
                log_warn("Worker %d hung up", i);
                workers[i].alive = 0;
            }
        }
    }

    //
    // ------------------ Send END to all workers ------------------
    //
    log_info("All tasks complete — sending END to workers");

    for (int i = 0; i < cfg.num_workers; i++) {
        if (!workers[i].alive) continue;

        Message endmsg;
        endmsg.type = MSG_END;

        char out[MAX_MSG];
        format_message(out, sizeof(out), &endmsg);
        safe_writeline(workers[i].to_worker.write_fd, out);
    }

    //
    // Cleanup
    //
    for (int i = 0; i < cfg.num_workers; i++) {
        close_fd(workers[i].to_worker.write_fd);
        close_fd(workers[i].from_worker.read_fd);
    }

    log_info("Coordinator exiting cleanly.");
    return 0;
}
