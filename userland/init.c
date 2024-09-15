#include <sys/stat.h>
#include <ipc.h>

extern int write(int fd, const void *buf, int count);
extern int open(const char *pathname, int flags, int mode);
extern int execve(const char *filename, char *const argv[], char *const envp[]);
extern int fork(void);

int main(void) {
    mknod("/console", S_IFCHR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, 0);
	int stdin = open("/console", 0, 0);
	int stdout = open("/console", 0, 0);

	if(fork() == 0) {
		execve("/usr/sbin/ipc_echo", NULL, NULL);
	}

	message_t msg;

    while(1) {
		ipc_recv(2, &msg);
		write(stdout, (char *)msg.mdata, 30);
		for(int i = 0; i < 10000000; i++);
    }
	(void)(stdin);

    return 0;
}
