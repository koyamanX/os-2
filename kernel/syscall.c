#include <fcntl.h>
#include <panic.h>

#include <sched.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <syscall.h>
#include <uart.h>
#include <unistd.h>
#include <vm.h>
#include <ipc.h>
#include <printk.h>
#include <task.h>

u64 syscall(struct task *rp) {
    u64 syscall_num = rp->tf->a7;
    u64 a0 = rp->tf->a0;
    u64 a1 = rp->tf->a1;
    u64 a2 = rp->tf->a2;
    u64 ret = -1;

    switch (syscall_num) {
        case __NR_WRITE:
            ret = write(a0, (void *)va2pa(rp->pgtbl, a1), a2);
            rp->tf->a0 = ret;
            break;
		case __NR_IPC_SEND:
			ret = ipc_send((endpoint_t)a0, (message_t*)a1);
			rp->tf->a0 = ret;
			break;
		case __NR_IPC_RECV:
			ret = ipc_recv((endpoint_t)a0, (message_t*)a1);
			rp->tf->a0 = ret;
			break;
		case TASK_CREATE:
			ret = task_create((char *)va2pa(rp->pgtbl, a0), (u64)a1, (u64 *)a2);
			rp->tf->a0 = ret;
			break;
        default:
            panic("invalid syscall\n");
            break;
    }

    return ret;
}
