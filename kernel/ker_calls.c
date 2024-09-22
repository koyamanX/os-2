#include <ker_calls.h>
#include <riscv.h>
#include <task.h>
#include <ipc.h>

int handle_ker_calls(uint64_t a7, uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6) {
	switch (a7) {
		case IPC_SEND:
			break;
		case IPC_RECEIVE:
			ipc_recv(a0, (message_t *)a1);
			break;
		case IPC_REPLY:
			break;
		default:
			return K_FAILURE;
	}
	return K_SUCCESS;
}
