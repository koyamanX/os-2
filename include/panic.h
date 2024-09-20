#ifndef PANIC_H
#define PANIC_H

#define _STR(x) #x
#define STR(x) _STR(x)
#define PANIC_IF(cond) if (cond) panic("Panic: " #cond " at " __FILE__ ":" STR(__LINE__))

void panic(char *msg);

#endif
