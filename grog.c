#include "stdio.h"

#define REGISTERS_COUNT 16
#define MEMORY_SIZE 64*1024 // 64K should be enough...

// Instruction set

#define HCF 0x00 // Halt and catch fire

typedef unsigned char byte;
typedef unsigned int address;

struct GrogVM {
    byte registers[REGISTERS_COUNT];
    byte memory[MEMORY_SIZE];
    address pc; // program counter
};

typedef struct GrogVM GrogVM;

void run(GrogVM *vm) {
    printf("Grog Virtual Machine\n");
    printf("--------------------\n");
    printf("%d registers\n", REGISTERS_COUNT);
    printf("%d addressable bytes in memory\n", MEMORY_SIZE);
    printf("\nRunning...\n");
    byte instruction = vm->memory[vm->pc];
    while (instruction != HCF) {

    }
    printf("Halt and catch fire!\n");
}

int main(int argc, char **argv)
{
    GrogVM vm = {};
    vm.memory[0] = HCF;
    run(&vm);
}