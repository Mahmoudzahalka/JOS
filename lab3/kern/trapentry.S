/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>



###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.data;                                                          \
		.long name;	                                        \
	.text;                                                          \
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.data;                                                          \
		.long name;	                                        \
	.text;                                                          \
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

//lab3-start-challenge1
.data
	.global handlers_func
	
handlers_func:
//lab3-end-challenge1

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
 /*lab3-start
    TRAPHANDLER_NOEC(handler_divide, T_DIVIDE)  
    TRAPHANDLER_NOEC(handler_debug, T_DEBUG)
    TRAPHANDLER_NOEC(handler_nmi, T_NMI)
    TRAPHANDLER_NOEC(handler_brkpt, T_BRKPT)
    TRAPHANDLER_NOEC(handler_oflow, T_OFLOW)
    TRAPHANDLER_NOEC(handler_bound, T_BOUND)
    TRAPHANDLER_NOEC(handler_illop, T_ILLOP)
    TRAPHANDLER_NOEC(handler_device, T_DEVICE)
    TRAPHANDLER(handler_dblflt, T_DBLFLT)
    // T_COPROC 9 - RESERVED we can define it or no it doesnt effect 
    TRAPHANDLER(handler_tss, T_TSS)
    TRAPHANDLER(handler_segnp, T_SEGNP)
    TRAPHANDLER(handler_stack, T_STACK)
    TRAPHANDLER(handler_gpflt, T_GPFLT)
    TRAPHANDLER(handler_pgflt, T_PGFLT)
    // T_RES 15 - RESERVED we can define it or no it doesnt effect 
    TRAPHANDLER_NOEC(handler_fperr, T_FPERR)
    TRAPHANDLER(handler_align, T_ALIGN)
    TRAPHANDLER_NOEC(handler_mchk, T_MCHK)
    TRAPHANDLER_NOEC(handler_simderr, T_SIMDERR)
 lab3-end*/
 //lab3-start-challenge1
    TRAPHANDLER_NOEC(handlers_func0, T_DIVIDE)  
    TRAPHANDLER_NOEC(handlers_func1, T_DEBUG)
    TRAPHANDLER_NOEC(handlers_func2, T_NMI)
    TRAPHANDLER_NOEC(handlers_func3, T_BRKPT)
    TRAPHANDLER_NOEC(handlers_func4, T_OFLOW)
    TRAPHANDLER_NOEC(handlers_func5, T_BOUND)
    TRAPHANDLER_NOEC(handlers_func6, T_ILLOP)
    TRAPHANDLER_NOEC(handlers_func7, T_DEVICE)
    TRAPHANDLER(handlers_func8, T_DBLFLT)
    TRAPHANDLER_NOEC(handlers_func9, 9)               /* T_COPROC 9 - RESERVED we can define it or no it doesnt effect */
    TRAPHANDLER(handlers_func10, T_TSS)
    TRAPHANDLER(handlers_func11, T_SEGNP)
    TRAPHANDLER(handlers_func12, T_STACK)
    TRAPHANDLER(handlers_func13, T_GPFLT)
    TRAPHANDLER(handlers_func14, T_PGFLT)
    TRAPHANDLER_NOEC(handlers_func15, 15)            /* T_RES 15 - RESERVED we can define it or no it doesnt effect */
    TRAPHANDLER_NOEC(handlers_func16, T_FPERR)
    TRAPHANDLER(handlers_func17, T_ALIGN)
    TRAPHANDLER_NOEC(handlers_func18, T_MCHK)
    TRAPHANDLER_NOEC(handlers_func19, T_SIMDERR)
    //20-31 RESERVED
    //exercise7
    /*20-31 reserved so it doesnt effect if we define or now, 32-37 in the next lab
    but i define it fo cosequtivity to access handles_func[] with the trap number*/
    TRAPHANDLER_NOEC(handlers_func20, 20);
    TRAPHANDLER_NOEC(handlers_func21, 21);
    TRAPHANDLER_NOEC(handlers_func22, 22);
    TRAPHANDLER_NOEC(handlers_func23, 23);
    TRAPHANDLER_NOEC(handlers_func24, 24);
    TRAPHANDLER_NOEC(handlers_func25, 25);
    TRAPHANDLER_NOEC(handlers_func26, 26);
    TRAPHANDLER_NOEC(handlers_func27, 27);
    TRAPHANDLER_NOEC(handlers_func28, 28);
    TRAPHANDLER_NOEC(handlers_func29, 29);
    TRAPHANDLER_NOEC(handlers_func30, 30);
    TRAPHANDLER_NOEC(handlers_func31, 31);
    TRAPHANDLER_NOEC(handlers_func32, 32);
    TRAPHANDLER_NOEC(handlers_func33, 33);
    TRAPHANDLER_NOEC(handlers_func34, 34);
    TRAPHANDLER_NOEC(handlers_func35, 35);
    TRAPHANDLER_NOEC(handlers_func36, 36);
    TRAPHANDLER_NOEC(handlers_func37, 37);
    TRAPHANDLER_NOEC(handlers_func38, 38);
    TRAPHANDLER_NOEC(handlers_func39, 39);
    TRAPHANDLER_NOEC(handlers_func40, 40);
    TRAPHANDLER_NOEC(handlers_func41, 41);
    TRAPHANDLER_NOEC(handlers_func42, 42);
    TRAPHANDLER_NOEC(handlers_func43, 43);
    TRAPHANDLER_NOEC(handlers_func44, 44);
    TRAPHANDLER_NOEC(handlers_func45, 45);
    TRAPHANDLER_NOEC(handlers_func46, 46);
    TRAPHANDLER_NOEC(handlers_func47, 47);
    TRAPHANDLER_NOEC(handlers_func48, T_SYSCALL);
//lab3-end-challenge1


/*
 * Lab 3: Your code here for _alltraps
 */
 /*lab3-start*/
 _alltraps:
	pushl %ds
	pushl %es
	pushal
    	movw $GD_KD, %ax
    	movw %ax, %ds
    	movw %ax, %es
	pushl %esp
	call trap
 /*lab3-end*/




