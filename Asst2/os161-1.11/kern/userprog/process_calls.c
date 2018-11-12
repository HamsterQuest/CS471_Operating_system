#include <types.h>
#include <lib.h>
#include <thread.h>
#include <addrspace.h>
#include <machine/trapframe.h>
#include <curthread.h>
#include <addrspace.h>
#include <syscall.h>
#include <process_calls.h>

pid_t get_next_pid(void) {
	// find the next available pid
		
	// too many processes
	panic("No pids available!\n");        
	return -1;
}

int sys_getpid(int *retval) {
	//something like this.
	*retval = curthread->pid;

	//never fails
	return 0;
}
pid_t sys_waitpid(pid_t pid, int *status, int options){
	return pid;
}

void child_after_fork(void *addrspace_addr, unsigned long tf_addr) {
	struct addrspace *addrspace;
	struct trapframe *frame;

	// Cast to appropriate types
	addrspace = (struct addrspace *) addrspace_addr;
	frame = (struct trapframe *) tf_addr;

	// Child needs it's address space and trap frame
	curthread->t_vmspace = addrspace;

	// Fork returns 0 for child process
	frame->tf_v0 = 0;

	// No error
	frame->tf_a3 = 1;

	// Increment the PC to go to the next instruction.
	frame->tf_epc += 4;

	// Use the trapframe to go back to user mode
	md_forkentry(frame);
}

int sys_fork(struct trapframe *tf, int *retval) {
	// Copy address space
	struct addrspace *new_addrspace;
	as_copy(curthread->t_vmspace, &new_addrspace);
	
	// Initialize new thread and fork
	// Arrange child_after_fork to be called
	struct thread *new_thread;
	int err = thread_fork(curthread->t_name, (void *) new_addrspace, 
			(unsigned long) tf, child_after_fork, &new_thread);
	
	// something something retval should be set to the pid?
	
	// Return -1 to the system call handler if there was an error
	if(err) {
		return -1;
	} else {
		return 0;
	}
}

int sys_execv(const char *program, char **args){
	return -1;
}

void sys__exit(int exitcode){
}
