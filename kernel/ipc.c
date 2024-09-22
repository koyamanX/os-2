#include <ipc.h>

#include <riscv.h>
#include <vm.h>
#include <printk.h>
#include <task.h>
#include <panic.h>
#include <sched.h>
#include <lib.h>

int ipc_send(endpoint_t ep, message_t __user *msg) {
	task_t *sender = this_proc();
	task_t *receiver = task_lookup(ep);

	if(receiver == NULL) {
		return IPC_INVALID_ENDPOINT;
	}
	if(sender->pid == receiver->pid) {
		return IPC_INVALID_ENDPOINT;
	}

	if(receiver->stat == SLEEP && (receiver->recv_from == sender->pid || receiver->recv_from == IPC_ANY)) {
		if(copyin(msg, &receiver->msg, sizeof(message_t)) != 0) {
			return IPC_INVALID_MESSAGE;
		}
		task_resume(receiver);

		return IPC_OK;
	} else if (receiver->stat != SLEEP) {
		if(copyin(msg, &sender->msg, sizeof(message_t)) != 0) {
			return IPC_INVALID_MESSAGE;
		}
		list_push_back(&receiver->senderwq, &sender->senderwq_next);
		sender->stat = SENDING;
		sched();

		return IPC_OK;
	}

	return IPC_ERR;
}

int ipc_recv(endpoint_t ep, message_t __user *msg) {
	task_t *receiver = this_proc();
	task_t *sender = NULL;

	if(receiver->recv_from == IPC_ANY && receiver->notification) {
		message_t notification;

		notification.msrc = IPC_KERNEL;
		notification.mtype = IPC_NOTIFY;
		memcpy(&notification.mdata[0], &receiver->notification, sizeof(u64));
		if(copyout(&notification, msg, sizeof(message_t)) != 0) {
			return IPC_INVALID_MESSAGE;
		}
		receiver->notification = 0;

		return IPC_OK;
	}

	if(!list_is_empty(&receiver->senderwq)) {
		sender = LIST_POP_FRONT(&receiver->senderwq, task_t, senderwq_next);
		if(copyout(&sender->msg, msg, sizeof(message_t)) != 0) {
			return IPC_INVALID_MESSAGE;
		}
		task_resume(sender);

		return IPC_OK;
	} else {
		receiver->recv_from = ep;

		task_suspend(receiver);

		if(copyout(&receiver->msg, msg, sizeof(message_t)) != 0) {
			return IPC_INVALID_MESSAGE;
		}
		task_resume(sender);
		return IPC_OK;
	}
	return IPC_ERR;
}
