# Grog VM

## The virtual machine you didn't ask for

Grog Virtual Machine is an experimental, minimalistic virtual machine.

## Architecture

* All values are 8 bits.
* 16 registers.
* 64Kb of addressable memory.
* 16 input/output devices.

## Instructions

| Instruction | Name                   | Description                                        |
| ´´´HCF´´´   | Halt and Catch Fire    | Stops the machine.                                 |
| ´´´LOAD´´´  | Load into register     | Load a value into a register.                      |
| ´´´STORE´´´ | Store into register    | Store value into memory.                           |
| ´´´AND´´´   | Boolean AND            | Boolean AND.                                       |
| ´´´OR´´´    | Boolean OR             | Boolean OR.                                        |
| ´´´XOR´´´   | Boolean XOR            | Boolean XOR.                                       |
| ´´´JAL´´´   | Jump                   | Unconditional jump to offset from next value.      |
| ´´´JALR´´´  | Jump                   | Unconditional jump to offset from next register.   |
| ´´´BEQ´´´   | Branch if equal        | Branch if registers are equal.                     |
| ´´´BNEQ´´´  | Branch if not equal    | Branch if registers are not equal.                 |
| ´´´BGT´´´   | Branch if greater than | Branch if register is greater equal.               |
