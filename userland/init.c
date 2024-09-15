#include <sys/stat.h>
extern int write(int fd, const void *buf, int count);
extern int open(const char *pathname, int flags, int mode);

int main(void) {
    mknod("/console", S_IFCHR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, 0);
	int stdin = open("/console", 0, 0);
	int stdout = open("/console", 0, 0);

    while(1) {
		write(stdout, "Hello, World!\n", 14);
		for(int i = 0; i < 10000000; i++);
    }
	(void)(stdin);

    return 0;
}
