#include <elf.h>
#include <lib.h>
#include <os1.h>
#include <panic.h>
#include <printk.h>
#include <proc.h>
#include <riscv.h>
#include <sched.h>
#include <sys/types.h>
#include <trap.h>
#include <vm.h>

struct proc procs[NPROCS];
u8 kstacks[NPROCS][PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
u8 pgtbls[NPROCS][PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
struct cpu cpu;
extern void init(void);
extern char _procmgr;

list_t schedq;

void initcpu(void) {
    cpu.rp = NULL;
    memset(&cpu.ctx, 0, sizeof(context_t));
}

void initproc(void) {
    for (int i = 0; i < NPROCS; i++) {
        procs[i].stat = UNUSED;
		procs[i].recv_from = -1;
		procs[i].kstack = kstacks[i];
		procs[i].pgtbl = (pagetable_t)pgtbls[i];
    }
	list_init(&schedq);
}

void userinit(void) {
    struct proc *p;

    // Allocate new proc.
    p = newproc();

    // Load initcode at address 0.
    kvmmap(p->pgtbl, 4096, (u64)init, PAGE_SIZE,
           PTE_V | PTE_W | PTE_R | PTE_X | PTE_U);
    // Set parent to 0.
    p->ppid = 0;

    // Dump proc memory space.
    kvmdump(p->pgtbl, TRAPFRAME);
    kvmdump(p->pgtbl, TRAMPOLINE);
    kvmdump(p->pgtbl, 4096);
}

static struct proc *_newproc(int pid) {
    struct proc *p;

    for (p = &procs[0]; p < &procs[NPROCS]; p++) {
        if (p->stat == UNUSED) {
            p->stat = USED;
            p->pid = pid;
            goto found;
        }
    }
    return NULL;

found:
	list_push_back(&schedq, &p->next);
    // Initialize proc.
    p->stat = RUNNABLE;
    // Allocate memory for trapframe, page table, and kernel stack.
    p->tf = alloc_page();
    p->pgtbl = alloc_page();

    // Initialize trapframe.
    memset(p->tf, 0, sizeof(trapframe_t));
    p->tf->sepc = 4096;
    memset(p->pgtbl, 0, PAGE_SIZE);

    // Initialize context.
    p->ctx.ra = (u64)usertrapret;
    p->ctx.sp = (u64)(p->kstack + PAGE_SIZE);
    p->tf->satp = SATP(p->pgtbl);
    p->tf->ksp = (u64)(p->kstack + PAGE_SIZE);
    p->tf->trap_handler = (u64)(usertrap);
    p->tf->sp = USTACK;
    // Map trapframe and trampoline to proc's memory space.
    kvmmap(p->pgtbl, TRAPFRAME, (u64)p->tf, PAGE_SIZE, PTE_V | PTE_W | PTE_R);
    kvmmap(p->pgtbl, TRAMPOLINE, (u64)trampoline, PAGE_SIZE,
           PTE_V | PTE_X | PTE_R);

    for (u64 nstack = 1; nstack <= NUSTACK; nstack++) {
        u64 stack = USTACK - (nstack * PAGE_SIZE);
        kvmmap(p->pgtbl, stack, (u64)alloc_page(), PAGE_SIZE,
               PTE_V | PTE_W | PTE_R | PTE_U);
        kvmdump(p->pgtbl, stack);
    }
    p->heap = 0x20000000;

    return p;
}
static u64 mpid = 1;
struct proc *newproc(void) {
	return _newproc(mpid++);
}

static void load_embed_elf(struct proc *p, const char *elf) {
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf;

	if(!IS_RISCV_ELF(*ehdr))
		panic("not riscv elf");

	Elf64_Phdr *phdr = (Elf64_Phdr *)(elf + ehdr->e_phoff);

    u64 prot = PTE_V | PTE_U;
	for (int i = 0; i < ehdr->e_phnum; i++) {
		u64 off = phdr[i].p_offset;

		if(phdr[i].p_type != PT_LOAD)
			continue;

		if (phdr[i].p_flags & PF_X) {
			prot |= PTE_X;
		}
		if (phdr[i].p_flags & PF_R) {
			prot |= PTE_R;
		}
		if (phdr[i].p_flags & PF_W) {
			prot |= PTE_W;
		}

		for (u64 va = phdr[i].p_vaddr; va < phdr[i].p_vaddr + phdr[i].p_memsz; va += PAGE_SIZE) {
			char *page = (char *)va2pa(p->pgtbl, va);
			if (page == NULL) {
				page = alloc_page();
				kvmmap(p->pgtbl, va, (u64)page, PAGE_SIZE, prot);
			}
			memcpy(page, elf + off, PAGE_SIZE);
		}
	}
	p->tf->sepc = ehdr->e_entry;
}

struct proc *procmgr(void) {
	struct proc *p = _newproc(PROCMGR);
	if (!p) {
		panic("procmgr: no procs");
	}
	p->ppid = 0;
	strcpy(p->name, "procmgr");
	load_embed_elf(p, &_procmgr);

	LIST_FOR_EACH(p, &schedq, struct proc, next) {
		printk("pid: %d\n", p->pid);
	}

	return p;
}

void sleep(void *wchan) {
    struct proc *rp = this_proc();

    rp->wchan = wchan;
	if(rp->stat == RUNNABLE)
		rp->stat = SLEEP;

    sched();

    rp->wchan = NULL;
}

void wakeup(void *wchan) {
    for (struct proc *rp = &procs[0]; rp < &procs[NPROCS]; rp++) {
        if (rp->wchan == wchan && (rp->stat == SLEEP || rp->stat == RECEIVE)) {
            rp->stat = RUNNABLE;
            rp->wchan = NULL;
        }
    }
}

struct proc *find_proc(u64 pid) {
	for (struct proc *rp = &procs[0]; rp < &procs[NPROCS]; rp++) {
		if (rp->pid == pid) {
			return rp;
		}
	}
	return NULL;
}
