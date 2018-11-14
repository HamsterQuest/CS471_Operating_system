#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);

//system call functions for PA2
int sys_getpid(int32_t *retval);
__DEAD void sys__exit(int code);
int sys_execv(const char *prog, char *const *args);
pid_t sys_fork(int32_t *retval);
int sys_waitpid(pid_t pid, int *returncode, int flags);

#endif /* _SYSCALL_H_ */
