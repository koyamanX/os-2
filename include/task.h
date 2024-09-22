#ifndef _TASK_H
#define _TASK_H

#include <riscv.h>
#include <ipc.h>
#include <list.h>
#include <trap.h>

typedef struct {
    u64 ra;
    u64 sp;
    u64 gp;
    u64 tp;
    u64 t0;
    u64 t1;
    u64 t2;
    u64 s0;
    u64 s1;
    u64 a0;
    u64 a1;
    u64 a2;
    u64 a3;
    u64 a4;
    u64 a5;
    u64 a6;
    u64 a7;
    u64 s2;
    u64 s3;
    u64 s4;
    u64 s5;
    u64 s6;
    u64 s7;
    u64 s8;
    u64 s9;
    u64 s10;
    u64 s11;
    u64 t3;
    u64 t4;
    u64 t5;
    u64 t6;
    u64 sepc;          //!< supervisour exception PC, trapped address.
    u64 satp;          //!< satp of a process.
    u64 ksp;           //!< stack pointer for per-process kernel stack.
    u64 trap_handler;  //!< Supervisor mode trap handler.
} trapframe_t;

/**
 * @brief Structure for execution context of process.
 * @details It is used for saving callee-saved registers.
 */
typedef struct {
    u64 ra;
    u64 sp;
    u64 s0;
    u64 s1;
    u64 s2;
    u64 s3;
    u64 s4;
    u64 s5;
    u64 s6;
    u64 s7;
    u64 s8;
    u64 s9;
    u64 s10;
    u64 s11;
} context_t;


typedef struct task {
    u64 stat;                    //!< Execution status of process.
    u64 pid;                     //!< Process IDs.
    trapframe_t *tf;             //!< Trapframe.
    context_t ctx;               //!< Context.
    char name[16];               //!< Name of process.
    pagetable_t pgtbl;           //!< Pagetable of process.
    u64 heap;                    //!< Start address of heap.
    u8 *kstack;		        	 //!< Pointer to per-process kernel stack.
    void *wchan;                 //!< Waiting channel.
    u64 ppid;                    //!< Parent process.
	message_t msg;				 //!< Message buffer. only valid if status is SENDING.
	pid_t recv_from;
	list_t senderwq;			 //!< Sender wait queue.
	list_t senderwq_next;		 //!< Next process in wait queue who wants to send message.
	list_t next;
	struct task *pager;
	void (*pf_handler)(struct task *task, u64 addr, u64 cause);
	context_t *pager_ctx;
	u64 notification;
	message_t notification_msg;
} task_t;

#define NTASKS 64

void inittask(void);
int task_create(char *name, u64 pager, u64 *entry);
void task_destroy(task_t *task);
void task_exit(void);

void enqueue(task_t *task);
task_t *dequeue(void);

struct cpu {
    task_t *rp;  //!< Running process.
    context_t ctx;    //!< Context of scheduler.
};
extern struct cpu cpu;  //!< Processors kernel can run.

#define this_cpu() (cpu)      //!< This processor's corresponding cpu struct.
#define this_proc() (cpu.rp)  //!< This process's corresponding proc struct.

#define NPROCS 16          //!< Maximum number of process.
#define NOFILE 8           //!< Maximum number of open file per process.
#define USTACK 0x80000000  //!< Initial stack pointer for userland.
#define NUSTACK 16         //!< Number of user stack in page.

#define UNUSED 0    //!< Proc struct is unused.
#define USED 1      //!< Proc struct is used.
#define RUNNING 2   //!< Proc is running.
#define RUNNABLE 3  //!< Proc is runnable.
#define ZOMBIE 4    //!< Proc is zombie.
#define SLEEP 5     //!< Proc is sleeping.
#define SENDING 6   //!< Proc is sending message.

/**
 * @brief Sleep for wait channel.
 * @details Sleep for wait channel, process will be SLEEP.
 * @param[in] wchan address to wait for.
 */
void sleep(void *wchan);

/**
 * @brief Wakeup process waitting for wchan.
 * @details Wakeup process waitting for wchan, set RUNNABLE.
 * @param[in] wchan address to wakeup.
 */
void wakeup(void *wchan);

/**
 * @brief switch context.
 * @details swtich context, used for switching process and scheduler, written in
 * asm.
 * @param[in] old old context.
 * @param[in] new new context.
 */
extern void swtch(context_t *old, context_t *new);

struct task *find_proc(u64 pid);

struct task *procmgr(void);

#define PROCMGR 0

void initcpu(void);

void task_suspend(task_t *task);
void task_resume(task_t *task);
struct task *task_lookup(u64 pid);

void enqueue_task(task_t *task);


#endif // _TASK_H
