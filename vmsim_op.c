#include "vmsim_op.h"

// Move operation (Opcode=M)
// Move integer value to register 
void op_move(Process *process, char *instruction) {
    char opcode; 
    int reg, value; 
    
    sscanf(instruction, "%c %d %d", &opcode, &reg, &value);
    print_log(process->pid, "Opcode=%c, RegNo=%d, Value=%d", opcode, reg, value);

    if (reg < 0 || reg >= MAX_REGISTERS) {
        fprintf(stderr, "RegisterNo is out of range (%d) \n", reg); 
        exit(1);
    }

    // Move constant tpo operation 
    register_set[reg] = value;    
}

// Add operation (Opcode=A)
// RegDest = RegSrc1 + RegSrc2
void op_add(Process *process, char *instruction) {
    char opcode; 
    int reg_dst, reg_src1, reg_src2; 
    
    sscanf(instruction, "%c %d %d %d", &opcode, &reg_dst, &reg_src1, &reg_src2);
    print_log(process->pid, "Opcode=%c, RegDest=%d, RegSrc1=%d, RegSrc2=%d", 
              opcode, reg_dst, reg_src1, reg_src2);

    if (reg_dst < 0 || reg_dst >= MAX_REGISTERS) {
        fprintf(stderr, "RegDest is out of range (%d) \n", reg_dst); 
        exit(1);
    }

    if (reg_src1 < 0 || reg_src1 >= MAX_REGISTERS) {
        fprintf(stderr, "RegSrc1 is out of range (%d) \n", reg_src1); 
        exit(1);
    }

    if (reg_src2 < 0 || reg_src2 >= MAX_REGISTERS) {
        fprintf(stderr, "RegSrc2 is out of range (%d) \n", reg_src2); 
        exit(1);
    }

    // Add operation 
    register_set[reg_dst] = register_set[reg_src1] + register_set[reg_src2];    
}


// Memory Load operation (Opcode=L)
// Load an integer value from the specified (virtual) address into register
void op_load(Process *process, char *instruction) {
    char opcode; 
    int reg, virt_addr; 
    
    sscanf(instruction, "%c %d %x", &opcode, &reg, &virt_addr);
    print_log(process->pid, "Opcode=%c, RegNo=%d, Addr=0x%04x", 
              opcode, reg, virt_addr);

    if (reg < 0 || reg >= MAX_REGISTERS) {
        fprintf(stderr, "RegNo is out of range (%d) \n", reg); 
        exit(1);
    }

    // Load operation
    char buf[4];
    read_page(process, virt_addr, buf, 4);
    register_set[reg] = *((int *)buf);
}

// Memory Store operation (Opcode=S)
// Store the integer value stored in the register to the (virtual) address
void op_store(Process *process, char *instruction) {
    char opcode; 
    int reg, virt_addr; 
    
    sscanf(instruction, "%c %d %x", &opcode, &reg, &virt_addr);
    print_log(process->pid, "Opcode=%c, RegNo=%d, Addr=0x%04x",
              opcode, reg, virt_addr);

    if (reg < 0 || reg >= MAX_REGISTERS) {
        fprintf(stderr, "RegNo is out of range (%d) \n", reg); 
        exit(1);
    }

    // Store operation
    char buf[4]; 
    *((int *)buf) = register_set[reg];
    write_page(process, virt_addr, buf, 4);
}