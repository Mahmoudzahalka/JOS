#include <inc/assert.h>
#include <inc/x86.h>
#include <kern/spinlock.h>
#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/monitor.h>

void sched_halt(void);

// Choose a user environment to run and run it.
void
sched_yield(void)
{
	struct Env *idle;

	// Implement simple round-robin scheduling.
	//
	// Search through 'envs' for an ENV_RUNNABLE environment in
	// circular fashion starting just after the env this CPU was
	// last running.  Switch to the first such environment found.
	//
	// If no envs are runnable, but the environment previously
	// running on this CPU is still ENV_RUNNING, it's okay to
	// choose that environment.
	//
	// Never choose an environment that's currently running on
	// another CPU (env_status == ENV_RUNNING). If there are
	// no runnable environments, simply drop through to the code
	// below to halt the cpu.

	// LAB 4: Your code here.

	//lab4-start-ex6
	/*int curenv_index;
	if(curenv == NULL)
	{
		curenv_index = -1;
	}
	else
	{
		curenv_index = ENVX(curenv->env_id);
	}

	int i = curenv_index + 1;
	while(i%NENV != curenv_index)
	{
		if(envs[i%NENV].env_status == ENV_RUNNABLE)
			env_run(&envs[i%NENV]);
		i++;
	}
	if(curenv && curenv->env_status == ENV_RUNNING)
	{
		env_run(curenv);
	}
	//lab4-end-ex6

	// sched_halt never returns
	sched_halt();*/
	
		int env_counter = 0;
	int env_id  = 0;
	if(curenv) {
		env_id  = ENVX(curenv->env_id) + 1;
	}
	while(true) {
		if(env_counter == NENV-1) {
			break;
		}
		if(env_id >= NENV)
			env_id = 0;
		if(envs[env_id].env_status == ENV_RUNNABLE) {
			envid_t waiting_id = envs[env_id].env_ipc_sendwait_id;
			if(waiting_id == envs[env_id].env_id ||
				envs[ENVX(waiting_id)].env_ipc_recving)
				env_run(&envs[env_id]); //doesnt return to this point.
		}
		env_id++;
		env_counter++;
	}
	if(curenv && curenv->env_status == ENV_RUNNING){
		if(curenv->env_ipc_sendwait_id == curenv->env_id || envs[ENVX(curenv->env_ipc_sendwait_id)].env_ipc_recving)
			env_run(curenv);
		else
			curenv->env_status = ENV_RUNNABLE;
	}
	// sched_halt never returns
	sched_halt();
}

// Halt this CPU when there is nothing to do. Wait until the
// timer interrupt wakes it up. This function never returns.
//
void
sched_halt(void)
{
	int i;

	// For debugging and testing purposes, if there are no runnable
	// environments in the system, then drop into the kernel monitor.
	for (i = 0; i < NENV; i++) {
		if ((envs[i].env_status == ENV_RUNNABLE ||
		     envs[i].env_status == ENV_RUNNING ||
		     envs[i].env_status == ENV_DYING))
			break;
	}
	if (i == NENV) {
		cprintf("No runnable environments in the system!\n");
		while (1)
			monitor(NULL);
	}

	// Mark that no environment is running on this CPU
	curenv = NULL;
	lcr3(PADDR(kern_pgdir));

	// Mark that this CPU is in the HALT state, so that when
	// timer interupts come in, we know we should re-acquire the
	// big kernel lock
	xchg(&thiscpu->cpu_status, CPU_HALTED);

	// Release the big kernel lock as if we were "leaving" the kernel
	unlock_kernel();

	// Reset stack pointer, enable interrupts and then halt.
	asm volatile (
		"movl $0, %%ebp\n"
		"movl %0, %%esp\n"
		"pushl $0\n"
		"pushl $0\n"
		"sti\n"
		"1:\n"
		"hlt\n"
		"jmp 1b\n"
	: : "a" (thiscpu->cpu_ts.ts_esp0));
}

