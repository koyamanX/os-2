#include <ipc.h>

extern int write(int fd, const void *buf, int count);
extern int open(const char *pathname, int flags, int mode);
extern int execve(const char *filename, char *const argv[], char *const envp[]);
extern int fork(void);

int my_strcpy(char *dest, const char *src) {
	int i = 0;
	while(src[i] != '\0') {
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return i;
}

int main(void) {
	int stdin = open("/console", 0, 0);
	int stdout = open("/console", 0, 0);

	message_t msg;
	char str[] = "Hello, World! from remote process\n";
	msg.mtype = 0;
	my_strcpy((char *)msg.mdata, str);
	
	while(1) {
		write(stdout, "I'm Child Process\n", 18);
		if(ipc_send(1, &msg) == IPC_OK) {
			write(stdout, "IPC_OK\n", 11);
		}
		for(int i = 0; i < 10000000; i++);
	}
	(void)(stdin);

	return 0;
}
