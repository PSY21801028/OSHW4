#ifndef VMSIM_OP_H
#define VMSIM_OP_H

#include "vmsim.h"

extern void read_page(Process *process, int virt_addr, void *buf, size_t count);
extern void write_page(Process *process, int virt_addr, const void *buf, size_t count);
extern void print_log(int pid, const char *format, ...);
extern int register_set[MAX_REGISTERS];

#endif 