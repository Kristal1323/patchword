#ifndef PATCHWORD_SIGNALS_H
#define PATCHWORD_SIGNALS_H

void install_sigchld_handler(void);
int child_died(void);
void clear_child_flag(void);

#endif
