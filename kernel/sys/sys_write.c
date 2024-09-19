#include <panic.h>
#include <proc.h>
#include <uart.h>

ssize_t write(int fd, const void *buf, size_t count) {
	/*
    struct proc *rp;
    struct file *fp;

    rp = this_proc();
    fp = rp->ofile[fd];
    if (fp == NULL) {
        panic("No file opened\n");
    }
    ret = writei(fp->ip, (char *)buf, fp->offset, count);
    fp->offset += count;
	*/

	for(int i = 0; i < count; i++) {
		uart_putchar(((char *)buf)[i]);
	}

    return count;
}
