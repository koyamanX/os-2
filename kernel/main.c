/**
 *	@file main.c
 *	@brief Kernel main.
 *	@author ckoyama(koyamanX)
 */
#include <alloc.h>
#include <lib.h>
#include <os1.h>
#include <plic.h>
#include <printk.h>

#include <riscv.h>
#include <sched.h>
#include <sys/types.h>
#include <timer.h>
#include <uart.h>
#include <vm.h>
#include <task.h>

/**
 *	@brief Kernel stack per CPU.
 *	@attention Only 4*PAGE_SIZE.
 */
__attribute__((aligned(16))) char stack[PAGE_SIZE * 4];

void kinit(void) {
    // This code runs in M-mode.
    u64 mstatus;

    // Set mstatus.mpp to S-mode, so that mret enter S-mode.
    mstatus = r_mstatus();
    mstatus &= MSTATUS_MPP_MASK;
    mstatus |= MSTATUS_MPP_S_MODE;
    w_mstatus(mstatus);

    // Disable paging.
    w_satp(0);

    // Setup and enable timer interrupt for M-mode.
    init_timer();

    // Set address where mret returns.
    w_mepc((u64)((u64 *)&kmain));

    // Set kernel trap vector.
    w_stvec((u64)((u64 *)&kernelvec));

    // Set Interrupt enable for S-mode.
    // Set Previous interrupt enable for S-mode.
    // Calling sret enables interrupt.
    w_sstatus(r_sstatus() | SSTATUS_SPIE);
    w_sie(SIE_SEIE | SIE_STIE | SIE_SSIE);

    // Return to mepc, enable interrupt for M-mode.
    asm volatile("mret");
}

void kmain(void) {
    // This code runs in S-mode.
    pagetable_t kpgtbl;

    // Initialize UART.
    uart_init();
    // Now we cau use UART related functions.
    INFO_PRINTK("kernel starts\n");
    // Initialize kernel pagetable.
    kpgtbl = kvminit();
    // Enable paging.
    kvmstart(kpgtbl);
    INFO_PRINTK("enable paging\n");
    // Initialize CPU struct.
    initcpu();
	// Initialize task struct.
	inittask();
    // Initialize PLIC.
    plic_init();
	// Initialize process manager.
	procmgr();
    // Enter scheduler, never returns.
    scheduler();
}
