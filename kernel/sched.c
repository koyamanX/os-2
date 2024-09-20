#include <printk.h>

#include <sched.h>
#include <task.h>

extern void swtch(context_t *old, context_t *new);

void sched(void) {
    swtch(&this_proc()->ctx, &this_cpu().ctx);
}

void scheduler(void) {
    while (1) {
		task_t *task = dequeue();
		if(!task) {
			continue;
		}
		if(task->stat != RUNNABLE) {
			continue;
		}
		this_proc() = task;
		task->stat = RUNNING;
		swtch(&this_cpu().ctx, &task->ctx);
		this_proc() = NULL;
		if(task->stat == RUNNING) {
			task->stat = RUNNABLE;
		}
		enqueue(task);
    }
}
