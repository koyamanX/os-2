#include <ipc.h>
#include <sys/stat.h>

extern int write(int fd, const void *buf, int count);
extern int open(const char *pathname, int flags, int mode);

int main(int argc, char **argv) {
	message_t msg;

    mknod("/console", S_IFCHR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, 0);
	open("/console", 0, 0);
	open("/console", 0, 0);

	write(1, "I'm the process manager\n", 24);

	while(1) {
		ipc_recv(IPC_ANY, &msg);

		switch(msg.mtype) {
			case 0:
				// Create process
				write(1, "Creating process\n", 17);
				break;
			case 1:
				// Kill process
				write(1, "Killing process\n", 16);
				break;
			case 2:
				// Get process status
				write(1, "Getting process status\n", 23);
				break;
			case 3:
				// Get process list
				write(1, "Getting process list\n", 21);
				break;
			default:
				write(1, "Unknown message type\n", 21);
				break;
		}
	}
	return 0;
}
