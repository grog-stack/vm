#include "stdio.h"

#define REGISTERS_COUNT 16
#define MEMORY_SIZE 64*1024 // 64K should be enough...

// Instruction set

#define HCF 0x00 // Halt and catch fire
#define LEFT_NIBBLE 0xF0
#define RIGHT_NIBBLE 0x0F
#define READ_BINARY "rb"

typedef unsigned char byte;
typedef unsigned int address;

struct GrogVM {
    byte registers[REGISTERS_COUNT];
    byte memory[MEMORY_SIZE];
    address pc; // program counter
};

typedef struct GrogVM GrogVM;

void add(GrogVM *vm, byte instr) { 
    byte operand = vm->memory[vm->pc+1];
    byte dest = (operand & LEFT_NIBBLE) >> 4;
    byte src = operand & RIGHT_NIBBLE;
    vm->registers[dest] = vm->registers[dest] + vm->registers[src];
    vm->pc += 2;
};

void sub(GrogVM *vm, byte instr) { 
    byte operand = vm->memory[vm->pc+1];
    byte dest = (operand & LEFT_NIBBLE) >> 4;
    byte src = operand & RIGHT_NIBBLE;
    vm->registers[dest] = vm->registers[dest] - vm->registers[src];
    vm->pc += 2;
};

void mul(GrogVM *vm, byte instr) { 
    byte operand = vm->memory[vm->pc+1];
    byte dest = (operand & LEFT_NIBBLE) >> 4;
    byte src = operand & RIGHT_NIBBLE;
    vm->registers[dest] = vm->registers[dest] * vm->registers[src];
    vm->pc += 2;
};

void div(GrogVM *vm, byte instr) { 
    byte operand = vm->memory[vm->pc+1];
    byte dest = (operand & LEFT_NIBBLE) >> 4;
    byte src = operand & RIGHT_NIBBLE;
    vm->registers[dest] = vm->registers[dest] / vm->registers[src];
    vm->pc += 2;
};

void (*instructions[4])(GrogVM *, byte) = {&add, &sub, &mul, &div};

void run(GrogVM *vm) {
    printf("\nRunning...\n");
    byte instr = vm->memory[vm->pc];
    while (instr != HCF) {
        (*instructions[instr])(vm, instr);
    }
    printf("Halt and catch fire!\n");
}

void loadROM(GrogVM *vm, char *filename) {
    FILE *fileptr = fopen(filename, READ_BINARY);
    fseek(fileptr, 0, SEEK_END);                     // Jump to the end of the file
    long filelen = ftell(fileptr);                        // Get the current byte offset in the file
    printf("%ld bytes in ROM\n", filelen);
    rewind(fileptr);                                 // Jump back to the beginning of the file

    fread(&(vm->memory), filelen, 1, fileptr);       // Read in the entire file
    printf("ROM loaded into memory.\n");
    fclose(fileptr);
}

int main(int argc, char **argv)
{
    printf("Grog Virtual Machine: %d registers, %d addressable bytes in memory.\n", REGISTERS_COUNT, MEMORY_SIZE);
    GrogVM vm = {};
    char *filename = argv[1]; 
    printf("Reading ROM from %s\n", filename);
    loadROM(&vm, filename);
    run(&vm);
}

