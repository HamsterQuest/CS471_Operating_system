#include <types.h>
#include <lib.h>
#include <thread.h>
#include <addrspace.h>
#include <machine/trapframe.h>
#include <curthread.h>
#include <syscall.h>
#include <process_calls.h>
#include <array.h>

int sys_getpid(int *retval) {
	*retval = curthread->pid;

	//never fails
	return 0;
}
pid_t sys_waitpid(pid_t pid, int *status, int options){
	//errer if no thread with pid specified
	if(pidArray[pid] == NULL)
		return EFAULT;
		
	//wake me up before you exit
	array_add(pidArray[pid]->sleepers, curthread);
	thread_sleep(curthread);
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
	int err;
	if(err = as_copy(curthread->t_vmspace, &new_addrspace)){
		return err;
	}
	
	// Initialize new thread and fork
	// Arrange child_after_fork to be called
	struct thread *new_thread;
	err = thread_fork(curthread->t_name, (void *) new_addrspace, 
			(unsigned long) tf, child_after_fork, &new_thread);
	
	// set retval to pid
	*retval = new_thread->pid;
	
	// Return thread_forks's error
	return err;
}

int sys_execv(const char *program, char **args){
	return -1;
}

void sys__exit(int exitcode){
}
