#include <ipc.h>
#include <stdint.h>
#include <sys/stat.h>

extern int write(int fd, const void *buf, int count);
extern int task_create(char *path, uint64_t pager, uint64_t *entry);

int main(int argc, char **argv) {
	write(1, "I'm the process manager\n", 24);
	int ret = task_create("/bin/hello", 0, 0);
	if (ret < 0) {
		write(1, "Failed to create task\n", 23);
		return -1;
	} else {
		write(1, "Task created\n", 13);
	}

	while(1) {
		write(1, "Waiting for message\n", 20);
		message_t msg;
		ipc_recv(IPC_ANY, &msg);

		write(1, "Received message\n", 17);
#define MSG_TYPE_PAGE_FAULT IPC_PAGE_FAULT
		switch(msg.mtype) {
			case MSG_TYPE_PAGE_FAULT:
				write(1, "PAGE_FAULT\n", 11);
				break;
			default:
				write(1, "UNKNOWN\n", 8);
				break;
		}
	}

	return 0;
}
