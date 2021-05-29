#include "stdio.h"

#define REGISTERS_COUNT 16
#define MEMORY_SIZE 64*1024 // 64K should be enough...

typedef unsigned char byte;

struct GrogVM {
    byte registers[REGISTERS_COUNT];
    byte memory[MEMORY_SIZE];
    byte pc; // program counter
};

typedef struct GrogVM GrogVM;

void run(GrogVM *vm) {
    printf("Grog Virtual Machine\n");
    printf("--------------------\n");
    printf("%d registers\n", REGISTERS_COUNT);
    printf("%d addressable bytes in memory\n", MEMORY_SIZE);
}

int main(int argc, char **argv)
{
    GrogVM vm = {};
    run(&vm);
}