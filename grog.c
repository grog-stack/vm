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

// Register instructions

address readAddressFromAbsoluteAddress(GrogVM *vm, address from) {
    return ((address) vm->memory[from] << 8) & (address) vm->memory[from+1];
}
byte decodeRegister(byte instruction) {
    return (instruction & RIGHT_NIBBLE);
}

byte addBytes(byte dest, byte src) { return dest + src; } 
byte subBytes(byte dest, byte src) { return dest - src; }
byte mulBytes(byte dest, byte src) { return dest * src; }
byte divBytes(byte dest, byte src) { return dest / src; }
byte andBytes(byte dest, byte src) { return dest & src; }
byte orBytes(byte dest, byte src) { return dest | src; }
byte xorBytes(byte dest, byte src) { return dest ^ src; }

void instructionOnRegister(GrogVM *vm, byte (*op)(byte, byte)) {
    byte operand = vm->memory[vm->pc+1];
    byte dest = (operand & LEFT_NIBBLE) >> 4;
    byte src = operand & RIGHT_NIBBLE;
    vm->registers[dest] = op(vm->registers[dest], vm->registers[src]);
    vm->pc += 2;
}

// Instruction set

void hcf(GrogVM *vm, byte instr) {}

void load(GrogVM *vm, byte instr) {
    vm->registers[decodeRegister(instr)] = vm->memory[vm->pc+1];
    vm->pc += 2;
}

void store(GrogVM *vm, byte instr) {
    vm->memory[readAddressFromAbsoluteAddress(vm, vm->pc+1)] = vm->registers[decodeRegister(instr)];
    vm->pc += 3;
}

void add(GrogVM *vm, byte instr) { instructionOnRegister(vm, &addBytes); }

void sub(GrogVM *vm, byte instr) { instructionOnRegister(vm, &subBytes); }

void mul(GrogVM *vm, byte instr) { instructionOnRegister(vm, &mulBytes); }

void div(GrogVM *vm, byte instr) { instructionOnRegister(vm, &divBytes); }

void and(GrogVM *vm, byte instr) { instructionOnRegister(vm, &andBytes); }

void or(GrogVM *vm, byte instr) { instructionOnRegister(vm, &orBytes); }

void xor(GrogVM *vm, byte instr) { instructionOnRegister(vm, &xorBytes); }

void (*instructions[9])(GrogVM *, byte) = {&hcf, &load, &store, &add, &sub, &mul, &div, &and, &or};

void run(GrogVM *vm) {
    printf("\nRunning...\n");
    byte instr = vm->memory[vm->pc];
    while (instr != HCF) {
        byte opcode = (instr & LEFT_NIBBLE) >> 4;
        (*instructions[opcode])(vm, instr);
        instr = vm->memory[vm->pc];
    }
    printf("Halt and catch fire!\n");
}

void loadROM(GrogVM *vm, char *filename) {
    FILE *fileptr = fopen(filename, READ_BINARY);
    fseek(fileptr, 0, SEEK_END);                     // Jump to the end of the file
    long filelen = ftell(fileptr);                   // Get the current byte offset in the file
    printf("%ld bytes in ROM\n", filelen);
    rewind(fileptr);                                 // Jump back to the beginning of the file

    fread(&(vm->memory), filelen, 1, fileptr);       // Read in the entire file
    printf("ROM loaded into memory.\n");
    fclose(fileptr);
}

void dump(GrogVM *vm) {
    printf("Registers:");
    for (int i = 0; i < REGISTERS_COUNT; i++) {
        printf(" %x=%x",i, vm->registers[i]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    printf("Grog Virtual Machine: %d registers, %d addressable bytes in memory.\n", REGISTERS_COUNT, MEMORY_SIZE);
    GrogVM vm = {};
    char *filename = argv[1]; 
    printf("Reading ROM from %s\n", filename);
    loadROM(&vm, filename);
    run(&vm);
    dump(&vm);
}

