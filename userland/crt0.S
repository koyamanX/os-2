.globl _start
_start:
	# clear bss
	la t0, _bss_start
	la t1, _bss_end
_clear_bss:
	sd zero, 0(t0)
	add t0, t0, 8
	bltu t0, t1, _clear_bss

.extern main
	call main
exit:
	mv a0, zero
	j _exit

_exit:
	li a7, 8
	ecall

.globl write
write:
	li a7, 0
	ecall
	ret

.globl open
open:
	li a7, 2
	ecall
	ret

.globl mknod
mknod:
	li a7, 4
	ecall
	ret

.globl ipc_send
ipc_send:
	li a7, 17
	ecall
	ret

.globl ipc_recv
ipc_recv:
	li a7, 18
	ecall
	ret

.globl fork
fork:
	li a7, 7
	ecall
	ret

.globl execve
execve:
	li a7, 1
	ecall
	ret

