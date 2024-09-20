#include <ker_calls.h>
#include <riscv.h>
#include <task.h>

int handle_ker_calls(uint64_t a7, uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6) {
	switch (a7) {
		case VM_MAP:
			break;
		case VM_UNMAP:
			break;
		case TASK_CREATE:
			break;
		case TASK_EXIT:
			break;
		case TASK_DESTROY:
			break;
		case TASK_SCHEDULE:
			break;
		case IPC_SEND:
			break;
		case IPC_RECEIVE:
			break;
		case IPC_REPLY:
			break;
		case IPC_NOTIFY:
			break;
		case IRQ_AQUIRE:
			break;
		case IRQ_RELEASE:
			break;
		default:
			return K_FAILURE;
	}
	return K_SUCCESS;
}
