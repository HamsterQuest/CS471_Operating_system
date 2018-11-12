#ifndef _PROCESS_CALLS_
#define _PROCESS_CALLS_

/* Get the next available pid */
pid_t get_next_pid(void);

/* Get the current process pid */
int sys_getpid(int *retval);

/* Waits for there to be no processes running with specified pid */
pid_t sys_waitpid(pid_t pid, int *status, int options);

/* Function child thread jumps to after fork */
void child_after_fork(void *addrspace_addr, unsigned long tf_addr);

/* Creates process identical to the parent */
int sys_fork(struct trapframe *tf, int *retval);

/* Excecutes a program */
int sys_execv(const char *program, char **args);

/* exits current process */
void sys__exit(int exitcode);

#endif