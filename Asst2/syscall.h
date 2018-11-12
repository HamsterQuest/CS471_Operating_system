//os161-1.11\kern\include
#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);

//system call functions for PA2
int sys_getpid(int code);
int sys_fork(int code);
int sys_execv(int code);
int sys_waitpid(int code);
int sys_exit(int code);

#endif /* _SYSCALL_H_ */
