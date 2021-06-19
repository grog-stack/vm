# Grog VM

## The virtual machine you didn't ask for

Grog Virtual Machine is an experimental, minimalistic virtual machine.

## Architecture

* All values are 8 bits.
* 16 registers.
* 64Kb of addressable memory.
* 16 input/output devices.

## Instruction set

The instruction set is loosely based on [RISC-V](https://riscv.org/).

| Instruction | Name                            | Description                                         |
| :-:         | :--                             | :--                                                 |
| ```HCF```   | Halt and Catch Fire             | Stops the machine.                                  |
| ```LOAD```  | Load into register              | Load a value into a register.                       |
| ```STORE``` | Store into register             | Store value into memory.                            |
| ```AND```   | Boolean AND                     | Boolean AND.                                        |
| ```OR```    | Boolean OR                      | Boolean OR.                                         |
| ```XOR```   | Boolean XOR                     | Boolean XOR.                                        | 
| ```JAL```   | Jump                            | Unconditional jump to offset from next value.       |
| ```JALR```  | Jump                            | Unconditional jump to offset from next register.    |
| ```BEQ```   | Branch if equal                 | Branch if registers are equal.                      |
| ```BNE```   | Branch if not equal             | Branch if registers are not equal.                  |
| ```BLT```   | Branch if less than             | Branch if register is less than other.              |
| ```BGE```   | Branch if greater or equal than | Branch if register is greater or equals than other. |
