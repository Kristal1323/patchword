#ifndef UTIL_FD_H
#define UTIL_FD_H

int close_fd(int fd);
int set_fd_nonblocking(int fd);
int set_fd_blocking(int fd);

#endif
