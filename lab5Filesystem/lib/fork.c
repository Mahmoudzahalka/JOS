// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	//lab4-start-ex12
	if(!(err&FEC_WR))
	{
		panic("pgfault: fault isnt write that mean - err != FEC_WR \n");
	}
	if(!(uvpd[PDX(addr)]&PTE_P) || !(uvpt[PGNUM(addr)]&PTE_P) || !(uvpt[PGNUM(addr)]&PTE_COW))
	{
		panic("pgfault: not copy-on-write \n");
	} 
	//lab4-end-ex12

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	//lab4-start-ex12
	if ((r = sys_page_alloc(sys_getenvid(), (void*)PFTEMP, PTE_P|PTE_U|PTE_W)) < 0)
		panic("pgfault: sys_page_alloc: %e \n", r);
	memcpy(PFTEMP, ROUNDDOWN(addr, PGSIZE), PGSIZE);
	if ((r = sys_page_map(sys_getenvid(), (void*)PFTEMP, sys_getenvid(), ROUNDDOWN(addr, PGSIZE), PTE_P|PTE_U|PTE_W)) < 0)
		panic("pgfault: sys_page_map: %e \n", r);
	if ((r = sys_page_unmap(sys_getenvid(), (void*)PFTEMP)) < 0)
		panic("pgfault: sys_page_unmap: %e \n", r);
	return;
	//lab4-end-ex12
	//panic("pgfault not implemented");
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	//lab4-start-ex12
	void* addr_pn = (void*)(pn*PGSIZE);
	//lab5-start-ex8
	if(uvpt[pn] & PTE_SHARE)
	{
		if ((r = sys_page_map(sys_getenvid(), addr_pn, envid, addr_pn, uvpt[pn]&PTE_SYSCALL)) < 0)
			panic("duppage: sys_page_map: %e \n", r);
	}
	//lab5-end-ex8
	else if((uvpt[pn] & PTE_W) || (uvpt[pn] & PTE_COW))
	{
		if ((r = sys_page_map(sys_getenvid(), addr_pn, envid, addr_pn, PTE_P|PTE_U|PTE_COW)) < 0)
			panic("duppage: sys_page_map: %e \n", r);
		if ((r = sys_page_map(sys_getenvid(), addr_pn, sys_getenvid(), addr_pn, PTE_P|PTE_U|PTE_COW)) < 0)
			panic("duppage: sys_page_map: %e \n", r);
	}
	else
	{
		if ((r = sys_page_map(sys_getenvid(), addr_pn, envid, addr_pn, PTE_P|PTE_U)) < 0)
			panic("duppage: sys_page_map: %e \n", r);
	}
	//lab4-end-ex12

	//panic("duppage not implemented");
	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	//lab4-statr-ex12
	set_pgfault_handler(pgfault);
	envid_t env_id = sys_exofork() ;
	if(env_id < 0)
	{
		panic("fork: sys_exofork: %e \n", env_id);
	}
	if(env_id == 0)   //son
	{
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}
	if(env_id > 0)  //parent
	{
		//Copy our address space to the chiled without the ex.stack so we run until USTACKTOP
		uint32_t address = 0;
		for(address = 0; address < USTACKTOP; address+=PGSIZE)
		{
			if((uvpd[PDX(address)]&PTE_P) && (uvpt[PGNUM(address)]&PTE_P) && (uvpt[PGNUM(address)] & PTE_U))
				duppage(env_id, PGNUM(address));
		}
		int r;
		if((r=sys_page_alloc(env_id, (void*)UXSTACKTOP-PGSIZE, PTE_U|PTE_P|PTE_W)) < 0 )
		{
			panic("fork: sys_page_alloc error: %e \n", r);
		}
		//page fault handler setup to the child
		extern void _pgfault_upcall();
		sys_env_set_pgfault_upcall(env_id, _pgfault_upcall);
		//mark the child as runnable
		if ((r = sys_env_set_status(env_id, ENV_RUNNABLE)) < 0)
			panic("fork: sys_env_set_status: %e", r);
		return env_id;
	}
	//sholdnt arrive to here
	return -1;
	//lab4-end-ex12

	//panic("fork not implemented");
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}

