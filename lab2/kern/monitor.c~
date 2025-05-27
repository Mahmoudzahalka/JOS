// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>

#define CMDBUF_SIZE	80	// enough for one VGA text line

//lab2-challeng start
#include <kern/pmap.h>
extern pde_t * kern_pgdir;
//lab2-challenge end

struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
	{ "backtrace", "mon_backtrace", mon_backtrace },
	{ "showmappings", "usage: showmmapings 0xstartAdrr 0xendAddr", showmappings },  //lab2-challeng start
	{ "setperms", "usage: setperms 0xaddr set/clear perms-up to 3 P W U", setperms },
	{ "dump", "usage: dump va/pa 0xlowaddr 0xupperaddr", dumpAddr },                    //lab2-challenge end
};
#define NCOMMANDS (sizeof(commands)/sizeof(commands[0]))

/***** Implementations of basic kernel monitor commands *****/

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < NCOMMANDS; i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	// Your code here.


	uint32_t* ebp;
	uint32_t eip;
	uint32_t stop = 0;
	ebp= (uint32_t*)read_ebp();
	cprintf("Stack backtrace:\n");
	struct Eipdebuginfo info;
	while(ebp)
	{
		cprintf("ebp %08x eip %08x args %08x %08x %08x %08x %08x\n",ebp,*(ebp+1),*(ebp+2),*(ebp+3),*(ebp+4),*(ebp+5),*(ebp+6));
		debuginfo_eip(*(ebp+1),&info);
		cprintf("\t%s:%d: %.*s+%d\n",info.eip_file,info.eip_line,info.eip_fn_namelen,info.eip_fn_name,*(ebp+1)-info.eip_fn_addr);
		ebp=(uint32_t*) (*(ebp));
	}
		//cprintf("ebp %08x eip %08x args %08x %08x %08x %08x %08x\n",ebp,*(ebp+1),*(ebp+2),*(ebp+3),*(ebp+4),*(ebp+5),*(ebp+6));

	return 0;
}



/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < NCOMMANDS; i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void
monitor(struct Trapframe *tf)
{
	char *buf;

	cprintf("Welcome to the JOS kernel monitor!\n");
	cprintf("Type 'help' for a list of commands.\n");


	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}


//-----------------------------------------------------------------------------------
//lab2-challenge start

int showmappings(int argc, char**argv, struct Trapframe *tf)
{ //can they be not aligned to pages?

	//usage is : "showmmapings 0x3000 0x5000"
	//so agrv[1] is showmappings
	//argv[2] is lower address of the range
	//argv[3] is higher address of the range
	//output will be: address 0x<> is mapped to physical page <> with permission bits ...
	
	if(argc<=1 || argc>3)
	{
		cprintf("wrong usage, see help command for more info\n");
		return -1;
	}
	//long strtol(const char *s, char **endptr, int base);
	
	uintptr_t lowerBound= strtol(argv[1],NULL,16); //here change the type to waht represents va in our code
	uintptr_t upperBound= strtol(argv[2],NULL,16);
	if(upperBound<lowerBound) //add if any of these is higher than 4gb or 4gb-thereserved soemthing
	{
		cprintf("wrong usage, see help command for more info\n");
		return -1;
	}
	//here check what happens with unmapped pages and their page tbale in pgdir_walk
	//i think we made it return null if unmapped and create is zero(those were the
	//instruction for it i think)
	pte_t* pageTableEntry;
	//align lowerBound and upperBound to PGSIZE
	//lowerbound = ROUNDDOWN(lowerBound,PGSIZE);
	//upperBound = ROUNDUP(upperBound,PGSIZE); //here need to handle some edge cases aswell
	while(lowerBound<=upperBound) //might have the same problem in map_region
	{
		//make sure that pgdir_walk handles this case as it should
		pageTableEntry = pgdir_walk(kern_pgdir,(void*)lowerBound,0);
		if(!pageTableEntry)
		{
			cprintf("address 0x%x is unmapped\n",lowerBound);
			lowerBound+=PGSIZE; //check this aswell
			continue;
		}
		cprintf("address %x is mapped to physical page %x with permissions : W: %x U:%x\n",lowerBound,*pageTableEntry&0xffff000,*pageTableEntry&PTE_W,*pageTableEntry&PTE_U);
		lowerBound+=PGSIZE; //check what this should be 
	}
	return 0;
}

//usage: setperms 0xaddr set/clear perms-up to 3 P W U
int setperms(int argc, char**argv, struct Trapframe *tf)
{
	if(argc<=3 || argc>6)//wrote 6 assuming we deal with 3 perm bits
	{
		cprintf("wrong usage, see help command for more info\n");
		return -1;
	}
	pte_t* pageTableEntry = pgdir_walk(kern_pgdir,(void*)strtol(argv[1],NULL,16),0); //create?
	if(!pageTableEntry)
	{
		cprintf("given address not mapped\n"); //make sure walk works this way
		return -1;
	}
	int numOfPerms = argc - 3; 
	uint32_t mask=0;
	if(numOfPerms>=1)
	{
		if(argv[3][0]=='P')
			mask=mask|PTE_P;
		if(argv[3][0]=='W')
			mask=mask|PTE_W;
		if(argv[3][0]=='U')
			mask=mask|PTE_U;
	}
	if(numOfPerms>=2)
	{
		if(argv[4][0]=='P')
			mask=mask|PTE_P;
		if(argv[4][0]=='W')
			mask=mask|PTE_W;
		if(argv[4][0]=='U')
			mask=mask|PTE_U;
	}
	if(numOfPerms>=3)
	{
		if(argv[5][0]=='P')
			mask=mask|PTE_P;
		if(argv[5][0]=='W')
			mask=mask|PTE_W;
		if(argv[5][0]=='U')
			mask=mask|PTE_U;
	}
	//else
	//{
	//	cprintf("wrong usage, see help");
	//	return -1;
	//}
	if(strcmp("clear",argv[2])==0) //make sure strcmp returns accordingly
	{
		*pageTableEntry=*pageTableEntry & ~mask;
	}
	else if(strcmp("set",argv[2])==0)
		*pageTableEntry=*pageTableEntry|mask;
	else
	{
		cprintf("wrong usage, see help\n");
		return -1;
	}
	return 0;
}


//usage: dump va/pa 0xlowaddr 0xupperaddr
int dumpAddr(int argc, char**argv, struct Trapframe *tf)
{
	if(argc!=4)
	{
		cprintf("wrong usage, see help command for more info\n");
		return -1;
	}
	uintptr_t lowerBound= strtol(argv[2],NULL,16); //here change the type to waht represents va in our code
	uintptr_t upperBound= strtol(argv[3],NULL,16);
	if(upperBound<lowerBound) //add if any of these is higher than 4gb or 4gb-thereserved soemthing
	{
		cprintf("wrong usage, see help command for more info\n");
		return -1;
	}
	if(strcmp(argv[1],"pa") && strcmp(argv[1],"va"))
	{
		cprintf("wrong usage, see help command for more info\n");
		return -1;
	}
	if(strcmp(argv[1],"pa")==0)
	{
		lowerBound+=KERNBASE;
		upperBound+=KERNBASE;
	}
	pte_t* pageTableEntry;
	while(lowerBound<=upperBound) //might have the same problem in map_region
	{
		pageTableEntry = pgdir_walk(kern_pgdir,(void*)lowerBound,0);
		if(!pageTableEntry)
		{
			cprintf("address 0x%x is unmapped\n",lowerBound);
			lowerBound+=4; //check this aswell
			continue;
		}
		cprintf("address %x content is: %x\n",lowerBound,*((uintptr_t*)lowerBound));
		lowerBound+=4; //check what this should be 
	}
	return 0;
}

//lab2-challenge end
//-----------------------------------------------------------------------------------
