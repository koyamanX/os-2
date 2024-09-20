#include <ipc.h>

#include <riscv.h>
#include <vm.h>
#include <printk.h>
#include <task.h>

int ipc_send(endpoint_t ep, message_t __user *msg) {
	endpoint_t dest = ep;
	endpoint_t src = this_proc()->pid;
	struct task *dest_p = find_proc(dest);

	if (dest < 0 || dest >= NPROCS) {
		return IPC_INVALID_ENDPOINT;
	}

	if (dest_p->stat == RECEIVE && ((dest_p->recv_from == src) || (dest_p->recv_from == IPC_ANY))) {
		if (copyin(msg, &dest_p->msg, sizeof(message_t)) < 0) {
			return IPC_ERROR;
		}
		wakeup(&dest_p->msg);
		return IPC_OK;
	}

	return IPC_RESEND;
}

int ipc_recv(endpoint_t ep, message_t __user *msg) {
	endpoint_t src = ep;

	if (src < 0 || src >= NPROCS) {
		return IPC_INVALID_ENDPOINT;
	}

	this_proc()->stat = RECEIVE;
	this_proc()->recv_from = src;
	sleep(&this_proc()->msg);

	if (this_proc()->recv_from != src && this_proc()->recv_from != IPC_ANY) {
		return IPC_ERROR;
	}

	this_proc()->recv_from = IPC_INVALID_ENDPOINT;
	if (copyout(&this_proc()->msg, msg, sizeof(message_t)) < 0) {
		return IPC_ERROR;
	}

	return IPC_OK;
}
