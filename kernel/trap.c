#include <os1.h>
#include <panic.h>
#include <plic.h>
#include <printk.h>

#include <riscv.h>
#include <sched.h>
#include <stddef.h>
#include <sys/types.h>
#include <trap.h>
#include <uart.h>
#include <vm.h>
#include <task.h>
#include <lib.h>

void kerneltrap(void) {
    asm volatile("nop");
}

void usertrapret(void) {
    struct task *p;

    w_stvec(TRAMPOLINE);
    p = this_proc();
    p->tf->satp = r_satp();
    p->tf->ksp = (u64)(p->kstack + PAGE_SIZE);
    w_sstatus(((r_sstatus() & ~SSTATUS_SPP) | SSTATUS_SPIE));
    w_sepc(p->tf->sepc);

    // Call userret using userspace virtual address.
    ((void (*)(u64))(TRAMPOLINE + (((u64)userret) - ((u64)trampoline))))(
        (u64)SATP(p->pgtbl));
}

u64 syscall(struct task *rp);
void usertrap(void) {
    struct task *rp;
    rp = this_proc();
    u64 scause = r_scause();

    switch (scause) {
#define SMODE_SOFTWARE_INTERRUPT 0x8000000000000001
        case SMODE_SOFTWARE_INTERRUPT: {
            w_sip(0x0);
            rp->stat = RUNNABLE;
            sched();
            break;
        }
#define ECALL_FROM_U_MODE 8
        case ECALL_FROM_U_MODE: {
            rp->tf->sepc += 4;
            syscall(rp);
            if (rp->stat == RUNNING) {
                rp->stat = RUNNABLE;
				enqueue(rp);
            }
            sched();
            break;
        }
#define SUPERVISOR_MODE_EXTERNAL_INTERRUPT 0x8000000000000009
        case SUPERVISOR_MODE_EXTERNAL_INTERRUPT:
            DEBUG_PRINTK("supervisor_mode_external_interrupt\n");
            uart_intr();
            plic_complete(plic_claim());
            break;
		case LOAD_PAGE_FAULT:
		case STORE_AMO_PAGE_FAULT:
		case INSTRUCTION_PAGE_FAULT: {
			task_t *task = this_proc();
			
			message_t msg;
			msg.mtype = IPC_PAGE_FAULT;
			msg.page_fault.addr = r_stval();
			msg.page_fault.type = scause;
			msg.page_fault.task = task->pid;

			memmove(&task->pager->notification_msg, &msg, sizeof(message_t));
			task->pager->notification = KERNEL_NOTIFICATION;
			PANIC_ON(task->pager == NULL, "Pager is NULL\n");
			task_resume(task->pager);

			break;
		}
        default: {
            printk("trap: fault: cause: %x, epc:%x, tval:%x\n",
                         r_scause(), r_sepc(), r_stval());
            panic("trap: Unimplemented trap");
            break;
        }
    }
    usertrapret();
}
