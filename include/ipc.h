#ifndef _IPC_H
#define _IPC_H

#include <riscv.h>

#define IPC_ANY 0

typedef struct message {
	u64 mtype;
	u64 msrc;
	u8 mdata[128];
} message_t;

typedef u64 endpoint_t;

#define IPC_INVALID_MESSAGE -3
#define IPC_INVALID_ENDPOINT -2
#define IPC_ERR -1
#define IPC_OK 0
#define IPC_RESEND 1
#define IPC_NOTIFY 2
#define IPC_KERNEL 3

int ipc_send(endpoint_t ep, message_t *msg);
int ipc_recv(endpoint_t ep, message_t *msg);

#endif
