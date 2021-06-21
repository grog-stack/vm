#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

#define REGISTERS_COUNT 16
#define MEMORY_SIZE 64*1024 // 64K should be enough...
#define LEFT_NIBBLE 0xF0
#define RIGHT_NIBBLE 0x0F
#define READ_BINARY "rb"

// Adressing modes
#define IMMEDIATE 0x00
#define ABSOLUTE 0x40
#define OFFSET 0x80
#define REGISTER 0xC0

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

address addressFromAddressingMode(GrogVM *vm, byte mode, byte register_) {
    switch (mode) {
        case IMMEDIATE: return vm->pc+1;
        case ABSOLUTE: return readAddressFromAbsoluteAddress(vm, vm->pc+1);
        case OFFSET: return vm->pc + vm->memory[vm->pc+1];
        case REGISTER: return vm->pc + vm->registers[register_];
        default: return -1; // Should never happen... right?
    }
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

bool alwaysJump(byte dest, byte src) { return true; }

bool testEquals(byte dest, byte src) { return dest == src; }

bool testNotEquals(byte dest, byte src) { return dest != src; }

bool testLessThan(byte dest, byte src) { return dest < src; }

bool testGreaterOrEqualThan(byte dest, byte src) { return dest >= src; }

void branchIfTest(GrogVM *vm, byte opcode, bool (*test)(byte, byte)) { 
    byte operand1 = vm->memory[vm->pc+1];
    byte register_ = operand1 & RIGHT_NIBBLE;
    byte mode = operand1 & LEFT_NIBBLE;
    byte operand2 = vm->memory[vm->pc+2];
    byte dest = (operand2 & LEFT_NIBBLE) >> 4;
    byte src = operand2 & RIGHT_NIBBLE;
    if (test(vm->registers[dest], vm->registers[src])) {
        vm->pc = addressFromAddressingMode(vm, mode, register_);
    } else {
        vm->pc += 2;
    }
}

// Instruction set

void HCF(GrogVM *vm, byte instr) { vm->running = false; }

// Memory
void LOAD(GrogVM *vm, byte instr) {
    byte operand = vm->memory[vm->pc+1];
    byte register_ = operand & RIGHT_NIBBLE;
    byte mode = operand & LEFT_NIBBLE;
    vm->registers[register_] = vm->memory[addressFromAddressingMode(vm, mode, register_)];
    vm->pc += 2;
}

void STORE(GrogVM *vm, byte instr) {
    byte operand = vm->memory[vm->pc+1];
    byte register_ = operand & RIGHT_NIBBLE;
    byte mode = operand & LEFT_NIBBLE;
    vm->memory[addressFromAddressingMode(vm, mode, register_)] = vm->registers[register_];
    vm->pc += 3;
}

// Arithmetic
void ADD(GrogVM *vm, byte opcode) { instructionOnRegister(vm, &addBytes); }
void SUB(GrogVM *vm, byte opcode) { instructionOnRegister(vm, &subBytes); }
void MUL(GrogVM *vm, byte opcode) { instructionOnRegister(vm, &mulBytes); }
void DIV(GrogVM *vm, byte opcode) { instructionOnRegister(vm, &divBytes); }

// Boolean
void AND(GrogVM *vm, byte opcode) { instructionOnRegister(vm, &andBytes); }
void OR(GrogVM *vm, byte opcode) { instructionOnRegister(vm, &orBytes); }
void XOR(GrogVM *vm, byte opcode) { instructionOnRegister(vm, &xorBytes); }

// Branching
void JAL(GrogVM *vm, byte opcode) { branchIfTest(vm, opcode, &alwaysJump); }
void BEQ(GrogVM *vm, byte opcode) { branchIfTest(vm, opcode, &testEquals); }
void BNE(GrogVM *vm, byte opcode) { branchIfTest(vm, opcode, &testNotEquals); }
void BLT(GrogVM *vm, byte opcode) { branchIfTest(vm, opcode, &testLessThan); }
void BGE(GrogVM *vm, byte opcode) { branchIfTest(vm, opcode, &testGreaterOrEqualThan); }

void EBREAK(GrogVM *vm, byte opcode) { /* Not implemented yet. */ }

void (*INSTRUCTIONS[255])(GrogVM *, byte);

void setBranchInstruction(void (* function)(GrogVM *, byte), byte baseOpcode) {
    INSTRUCTIONS[baseOpcode] = function;
    INSTRUCTIONS[baseOpcode & 0x4F] = function;
    INSTRUCTIONS[baseOpcode & 0x8F] = function;
    INSTRUCTIONS[baseOpcode & 0xCF] = function;
}

void initInstructions() {
    INSTRUCTIONS[0x00] = &HCF;
    INSTRUCTIONS[0x01] = &LOAD;
    INSTRUCTIONS[0x02] = &STORE;
    INSTRUCTIONS[0x03] = &ADD;
    INSTRUCTIONS[0x04] = &SUB;
    INSTRUCTIONS[0x05] = &MUL;
    INSTRUCTIONS[0x06] = &DIV;
    INSTRUCTIONS[0x07] = &AND;
    INSTRUCTIONS[0x08] = &OR;
    INSTRUCTIONS[0x09] = &XOR;
    setBranchInstruction(&JAL, 0x0A);
    setBranchInstruction(&BEQ, 0x0B);
    setBranchInstruction(&BNE, 0x0C);
    setBranchInstruction(&BLT, 0x0D);
    setBranchInstruction(&BGE, 0x0E);
}

void run(GrogVM *vm) {
    printf("\nRunning...\n");
    vm->running = true;
    while (vm->running == true) {
        byte opcode = vm->memory[vm->pc];
        (*INSTRUCTIONS[opcode])(vm, opcode);
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
    initInstructions();
    GrogVM vm = {};
    char *filename = argv[1]; 
    printf("Reading ROM from %s\n", filename);
    loadROM(&vm, filename);
    run(&vm);
    dump(&vm);
}