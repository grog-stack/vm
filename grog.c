#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

#define REGISTERS_COUNT 16
#define MEMORY_SIZE 64*1024 // 64K should be enough...
#define LEFT_NIBBLE 0xF0
#define RIGHT_NIBBLE 0x0F
#define READ_BINARY "rb"

// Errors

#define ROM_TOO_LARGE 1

typedef unsigned char byte;
typedef unsigned int address;

struct GrogVM {
    byte registers[REGISTERS_COUNT];
    byte memory[MEMORY_SIZE];
    address pc; // program counter
    bool running;
};

typedef struct GrogVM GrogVM;

// Register instructions

address readAddressFromAbsoluteAddress(GrogVM *vm, address from) {
    return ((address) vm->memory[from] << 8) + (address) vm->memory[from+1];
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

bool testEquals(byte dest, byte src) { return dest == src; }

bool testNotEquals(byte dest, byte src) { return dest != src; }

bool testLessThan(byte dest, byte src) { return dest < src; }

bool testGreaterOrEqualThan(byte dest, byte src) { return dest >= src; }

void branchIfTest(GrogVM *vm, bool (*test)(byte, byte)) { 
    byte operand = vm->memory[vm->pc+1];
    byte dest = (operand & LEFT_NIBBLE) >> 4;
    byte src = operand & RIGHT_NIBBLE;
    if (test(vm->registers[dest], vm->registers[src])) {
        address jumpAddress = readAddressFromAbsoluteAddress(vm, vm->pc+2);
        vm->pc = jumpAddress;
    } else {
        vm->pc += 4;
    }
 }

// Instruction set

void HCF(GrogVM *vm, byte instr) { printf("Halt and catch fire!\n"); vm->running = false; }

void LOAD(GrogVM *vm, byte instr) {
    vm->registers[decodeRegister(instr)] = vm->memory[vm->pc+1];
    vm->pc += 2;
}

void STORE(GrogVM *vm, byte instr) {
    vm->memory[readAddressFromAbsoluteAddress(vm, vm->pc+1)] = vm->registers[decodeRegister(instr)];
    vm->pc += 3;
}

void ADD(GrogVM *vm, byte instr) { instructionOnRegister(vm, &addBytes); }

void SUB(GrogVM *vm, byte instr) { instructionOnRegister(vm, &subBytes); }

void MUL(GrogVM *vm, byte instr) { instructionOnRegister(vm, &mulBytes); }

void DIV(GrogVM *vm, byte instr) { instructionOnRegister(vm, &divBytes); }

void AND(GrogVM *vm, byte instr) { instructionOnRegister(vm, &andBytes); }

void OR(GrogVM *vm, byte instr) { instructionOnRegister(vm, &orBytes); }

void XOR(GrogVM *vm, byte instr) { instructionOnRegister(vm, &xorBytes); }

// This is not exactly what the RISC-V JAL instruction does
void JAL(GrogVM *vm, byte instr) { vm->pc = vm->pc + vm->memory[vm->pc+1]; }

// This is not exactly what the RISC-V JALR instruction does
void JALR(GrogVM *vm, byte instr) { vm->pc = vm->pc + decodeRegister(instr); }

void BEQ(GrogVM *vm, byte instr) { branchIfTest(vm, &testEquals); }

void BNE(GrogVM *vm, byte instr) { branchIfTest(vm, &testNotEquals); }

void BLT(GrogVM *vm, byte instr) { branchIfTest(vm, &testLessThan); }

void BGE(GrogVM *vm, byte instr) { branchIfTest(vm, &testGreaterOrEqualThan); }

void (*instructions[16])(GrogVM *, byte) = {
    &HCF,   // 0x00
    &LOAD,  // 0x01
    &STORE, // 0x02
    &ADD,   // 0x03
    &SUB,   // 0x04
    &MUL,   // 0x05
    &DIV,   // 0x06
    &AND,   // 0x07
    &OR,    // 0x08
    &XOR,   // 0x09
    &JAL,   // 0x0A
    &JALR,  // 0x0B
    &BEQ,   // 0x0C
    &BNE,   // 0x0D
    &BLT,   // 0x0E
    &BGE,   // 0x0F
};

void run(GrogVM *vm) {
    printf("\nRunning...\n");
    byte instr = vm->memory[vm->pc];
    vm->running = true;
    while (vm->running == true) {
        byte opcode = instr & RIGHT_NIBBLE;
        (*instructions[opcode])(vm, instr);
        instr = vm->memory[vm->pc];
    }
}

long vmMemorySize(GrogVM *vm) {
    return sizeof(vm->memory);
}

void loadROM(GrogVM *vm, char *filename) {
    FILE *fileptr = fopen(filename, READ_BINARY);
    fseek(fileptr, 0, SEEK_END);                     // Jump to the end of the file
    long filelen = ftell(fileptr);                   // Get the current byte offset in the file
    long maxLength = vmMemorySize(vm);
    if( filelen > maxLength) {
        printf("ROM size is %ld bytes, max is %ld.\n", filelen, maxLength);
        exit(ROM_TOO_LARGE);
    }
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


