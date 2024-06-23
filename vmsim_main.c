#include "vmsim_main.h"

// Define process list structure
typedef struct Node {
    Process *process;
    struct Node *next;
} Node;

Node *process_list = NULL;
Node *current_process = NULL;

int main(int argc, char *argv[]) 
{
    if (argc < 2 || argc > MAX_PROCESSES + 1) 
    {
        fprintf(stderr, "Usage: %s <process image files... up to %d files>\n", 
                argv[0], MAX_PROCESSES);
        exit(EXIT_FAILURE);
    }

    // Initialize
    initialize();

    // Load processes 
    for (int i = 1; i < argc; i++) {
        load(argv[i], i - 1);
    }

    // Execute processes 
    simulate();

    // Free memory
    free(phy_memory);
    while (process_list) {
        Node *temp = process_list;
        process_list = process_list->next;
        free(temp->process->page_table);
        free(temp->process);
        free(temp);
    }

    return 0;
}


// Initialization 
void initialize() 
{
    int i;

    // Physical memory 
    phy_memory = (char *) malloc(PHY_MEM_SIZE);

    // Register set 
    for (i = 0; i < MAX_REGISTERS; i++) {
        register_set[i] = 0;
    }
    // Initialize process list
    process_list = NULL;
}

// Load process from file
void load(const char *filename, int pid)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    Process *process = (Process *)malloc(sizeof(Process));
    process->pid = pid;
    fscanf(file, "%d %d", &process->size, &process->num_inst);

    process->pc = 0;
    int num_pages = (process->size + PAGE_SIZE - 1) / PAGE_SIZE;
    process->page_table = (PageTableEntry *)malloc(num_pages * sizeof(PageTableEntry));
    for (int i = 0; i < num_pages; i++)
    {
        process->page_table[i].frame_number = -1;
        process->page_table[i].valid = 0;
    }

    for (int i = 0; i < MAX_REGISTERS; i++)
    {
        process->temp_reg_set[i] = 0;
    }

    char instruction[INSTRUCTION_SIZE];
    for (int i = 0; i < process->num_inst; i++)
    {
        fscanf(file, " %[^\n]", instruction);
        int virt_addr = i * INSTRUCTION_SIZE;

        // Store instruction in memory, handle page faults, etc.
        write_page(process, virt_addr, instruction, strlen(instruction) + 1);
    }

    // Add process to the process list
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->process = process;
    new_node->next = NULL;

    if (process_list == NULL)
    {
        process_list = new_node;
    }
    else
    {
        Node *temp = process_list;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }

    fclose(file);
}

// Simulation
// TODO: Repeat simulation until the process list is empty
// while loop
// TODO: Select a process from process list using round-robin scheme 
// TODO: Execue a processs 
        // TODO: If the process is terminated then 
        //       call print_register_set(), 
        //       reclaim allocated frames, 
        //       and remove process from process list 
void simulate() 
{
    while (process_list != NULL) {
        current_process = process_list;

        while (current_process != NULL) {
            Process *process = current_process->process;
            int finish = execute(process);

            if (finish) {
                print_register_set(process->pid);
                // Reclaim allocated frames
                for (int i = 0; i < process->size / PAGE_SIZE; i++) {
                    process->page_table[i].frame_number = -1;
                    process->page_table[i].valid = 0;
                }

                // Remove process from process list
                Node *temp = process_list;
                if (temp->process == process) {
                    process_list = temp->next;
                } else {
                    while (temp->next != NULL && temp->next->process != process) {
                        temp = temp->next;
                    }
                    if (temp->next != NULL) {
                        Node *to_remove = temp->next;
                        temp->next = to_remove->next;
                        free(to_remove);
                    }
                }
                free(process->page_table);
                free(process);
            }
            current_process = current_process->next;
            clock++;
        }
    }
}


// Execute an instruction using program counter 
int execute(Process *process) 
{
    char instruction[INSTRUCTION_SIZE];
    char opcode;

    // Restore register set
    for (int i = 0; i < MAX_REGISTERS; i++)
    {
        register_set[i] = process->temp_reg_set[i];
    }

    // Fetch instruction and update program counter
    read_page(process, process->pc, instruction, INSTRUCTION_SIZE);
    process->pc += INSTRUCTION_SIZE;

    // Execute instruction according to opcode
    opcode = instruction[0];
    switch (opcode)
    {
    case 'M':
        op_move(process, instruction);
        break;
    case 'A':
        op_add(process, instruction);
        break;
    case 'L':
        op_load(process, instruction);
        break;
    case 'S':
        op_store(process, instruction);
        break;
    default:
        printf("Unknown Opcode (%c) \n", opcode);
    }

    // Store register set
    for (int i = 0; i < MAX_REGISTERS; i++) {
        process->temp_reg_set[i] = register_set[i];
    }

    // Check if the last instruction executed
    // TODO: When the last instruction is executed return 1, otherwise return 0 
    return process->pc >= process->num_inst * INSTRUCTION_SIZE;

}


// Read up to 'count' bytes from the 'virt_addr' into 'buf' 
// TODO: Get a physical address from virtual address     
// TODO: handle page fault -> just allocate page     
// TODO: Read up to 'count' bytes from the physical address into 'buf' 
void read_page(Process *process, int virt_addr, void *buf, size_t count)
{
    int page_number = virt_addr / PAGE_SIZE;
    int offset = virt_addr % PAGE_SIZE;

    if (!process->page_table[page_number].valid) {
        // Handle page fault
        for (int i = 0; i < NUM_PAGES; i++) {
            if (/* frame is free */ 1 /* Placeholder condition */) {
                process->page_table[page_number].frame_number = i;
                process->page_table[page_number].valid = 1;
                break;
            }
        }
    }

    int frame_number = process->page_table[page_number].frame_number;
    int phy_addr = frame_number * PAGE_SIZE + offset;
    memcpy(buf, &phy_memory[phy_addr], count);
}


// Write 'buf' up to 'count' bytes at the 'virt_addr'
// TODO: Get a physical address from virtual address 
// TODO: handle page fault -> just allocate page
// TODO: Write 'buf' up to 'count' bytes at the physical address 
void write_page(Process *process, int virt_addr, const void *buf, size_t count)
{
    int page_number = virt_addr / PAGE_SIZE;
    int offset = virt_addr % PAGE_SIZE;

    if (!process->page_table[page_number].valid) {
        // Handle page fault
        for (int i = 0; i < NUM_PAGES; i++) {
            if (/* frame is free */ 1 /* Placeholder condition */) {
                process->page_table[page_number].frame_number = i;
                process->page_table[page_number].valid = 1;
                break;
            }
        }
    }

    int frame_number = process->page_table[page_number].frame_number;
    int phy_addr = frame_number * PAGE_SIZE + offset;
    memcpy(&phy_memory[phy_addr], buf, count);
}


// Print log with format string 
void print_log(int pid, const char *format, ...)
{
    va_list args; 
    va_start(args, format);
    printf("[Clock=%2d][PID=%d] ", clock, pid); 
    vprintf(format, args);
    printf("\n");
    fflush(stdout);
    va_end(args);
}


// Print values in the register set 
void print_register_set(int pid) 
{
    int i;
    char str[256], buf[16]; 
    strcpy(str, "[RegisterSet]:");
    for (i = 0; i < MAX_REGISTERS; i++) {
        sprintf(buf, " R[%d]=%d", i, register_set[i]); 
        strcat(str, buf);
        if (i != MAX_REGISTERS-1)
            strcat(str, ",");
    }
    print_log(pid, "%s", str);
}
