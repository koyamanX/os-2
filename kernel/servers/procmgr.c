#include <ipc.h>
#include <sys/stat.h>

extern int write(int fd, const void *buf, int count);
extern int open(const char *pathname, int flags, int mode);
extern int execve(const char *filename, char *const argv[], char *const envp[]);
extern int fork(void);

int main(int argc, char **argv) {
	write(1, "I'm the process manager\n", 24);

	while(1);

	return 0;
}
