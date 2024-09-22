#include <task.h>
#include <elf.h>
#include <panic.h>
#include <sched.h>
#include <list.h>
#include <vm.h>
#include <riscv.h>
#include <os1.h>
#include <lib.h>
#include <printk.h>

struct task tasks[NTASKS];
static u8 kstacks[NTASKS][PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
static u8 pgtbls[NTASKS][PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
static u8 trapframe[NTASKS][PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
list_t ready_queue;
struct cpu cpu;
extern char _procmgr;

void inittask(void) {
	for (int i = 0; i < NTASKS; i++) {
		tasks[i].stat = UNUSED;
		tasks[i].kstack = kstacks[i];
		tasks[i].pgtbl = (pagetable_t)pgtbls[i];
		tasks[i].tf = (trapframe_t *)trapframe[i];
		memset(kstacks[i], 0, PAGE_SIZE);
		memset(pgtbls[i], 0, PAGE_SIZE);
		memset(trapframe[i], 0, PAGE_SIZE);
	}
	list_init(&ready_queue);
}

task_t *task_create(char *name, task_t *pager, void (*entry)(void)) {
	task_t *task = NULL;
	static int pid = 0;

	for (int i = 0; i < NTASKS; i++) {
		if (tasks[i].stat == UNUSED) {
			pid++;
			task = &tasks[i];
			break;
		}
	}

	if(task == NULL) {
		return NULL;
	}

	if(pager == NULL) {
		PANIC_ON(task->stat != UNUSED, "No available pager");
		pager = &tasks[0];
	}

	task->stat = RUNNABLE;
	task->pid = pid;
	task->pager = pager;
	//task->ppid = ?;
	strcpy(task->name, name);

	//TODO:
	{
		task->tf->sepc = 4096;
		task->ctx.ra = (u64)usertrapret;
		task->ctx.sp = (u64)(task->kstack + PAGE_SIZE);
		task->tf->satp = SATP(task->pgtbl);
		task->tf->ksp = (u64)(task->kstack + PAGE_SIZE);
		task->tf->trap_handler = (u64)(usertrap);
		task->tf->sp = USTACK;
		// Map trapframe and trampoline to proc's memory space.
		kvmmap(task->pgtbl, TRAPFRAME, (u64)task->tf, PAGE_SIZE, PTE_V | PTE_W | PTE_R);
		kvmmap(task->pgtbl, TRAMPOLINE, (u64)trampoline, PAGE_SIZE,
			   PTE_V | PTE_X | PTE_R);

		for (u64 nstack = 1; nstack <= NUSTACK; nstack++) {
			u64 stack = USTACK - (nstack * PAGE_SIZE);
			kvmmap(task->pgtbl, stack, (u64)alloc_page(), PAGE_SIZE,
				   PTE_V | PTE_W | PTE_R | PTE_U);
			kvmdump(task->pgtbl, stack);
		}
		task->heap = 0x20000000;
	}

	list_init(&task->senderwq);
	list_init(&task->next);

	list_push_back(&ready_queue, &task->next);

	printk("task_create: %s(%x) created\n", name, pid);

	return task;
}

void task_destroy(task_t *task) {
	task->stat = UNUSED;
}

void task_exit(void) {
	return ;
}

void enqueue(task_t *task) {
	list_push_back(&ready_queue, &task->next);
}

task_t *dequeue(void) {
	return LIST_POP_FRONT(&ready_queue, task_t, next);
}

void task_suspend(task_t *task) {
	task->stat = SLEEP;
	sched();
}

void task_resume(task_t *task) {
	task->stat = RUNNABLE;
	enqueue(task);
	sched();
}

void initcpu(void) {
    cpu.rp = NULL;
    memset(&cpu.ctx, 0, sizeof(context_t));
}


static void task_load_embed_elf(task_t *p, const char *elf) {
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

		PANIC_ON((u64)&_end <= phdr[i].p_vaddr && phdr[i].p_vaddr < (u64)PHYEND, "Invalid load address");

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

struct task *procmgr(void) {
	task_t *task = task_create("procmgr", NULL, NULL);

	task_load_embed_elf(task, &_procmgr);
	delegate_memory(task->pgtbl, (u64)PHYEND);

	return task;
}

void sleep(void *wchan) {
    struct task *rp = this_proc();

    rp->wchan = wchan;
    rp->stat = SLEEP;

    sched();

    rp->wchan = NULL;
}

void wakeup(void *wchan) {
    for (struct task *rp = &tasks[0]; rp < &tasks[NTASKS]; rp++) {
        if (rp->wchan == wchan && rp->stat == SLEEP) {
            rp->stat = RUNNABLE;
            rp->wchan = NULL;
        }
    }
}

struct task *task_lookup(u64 pid) {
	for (struct task *rp = &tasks[0]; rp < &tasks[NTASKS]; rp++) {
		if (rp->pid == pid) {
			return rp;
		}
	}
	return NULL;
}
