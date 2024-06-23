#ifndef VMSIM_H
#define VMSIM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define PAGE_SIZE           4096  // 4KB
#define PHY_MEM_SIZE        (16 * 1024 * 1024) // 16MB
#define NUM_PAGES           (PHY_MEM_SIZE / PAGE_SIZE)
#define MAX_PROCESSES       5
#define MAX_REGISTERS       8
#define INSTRUCTION_SIZE    32  // 32 bytes

typedef struct {
    int frame_number;   // frame number
    int valid;          // valid is 1 if page is in physical memory 
} PageTableEntry;

typedef struct {
    int pid;                            // process ID 
    int size;                           // total size of process 
    int num_inst;                       // number of instruction in file
    int pc;                             // program counter which holds address of next instruction 
    int temp_reg_set[MAX_REGISTERS];    // Register set for context switching 
    PageTableEntry *page_table;         // Page table 
} Process;

// TODO: You can add more definitions 

#endif