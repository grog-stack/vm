# Grog VM

## The virtual machine you didn't ask for

Grog Virtual Machine is an experimental, minimalistic virtual machine.

## Architecture

* All values are 8 bits.
* 16 registers.
* 64Kb of addressable memory.
* 16 input/output devices.

## Conventions

Throughout this document we will use some conventions.

### Bits

All values are 8 bits long. Bits are numbered starting with ```0```, least significant bit first. 
For example, the hexadecimal number ```71``` is represented like this:

| Binary representation | 01110001 |
| :-- | :-: |
| Bit indexes           | 76543210 |

## Instruction set summary

The instruction set is loosely based on [RISC-V](https://riscv.org/). Most instructions require 2 bytes.

| Instruction | Description                                         |
| :-:         | :--                                                 |
| ```HCF```   | Halt and Catch Fire. Stops the machine.             |
| ```LOAD```  | Load a value from memory into register.             |
| ```STORE``` | Store a value from a register into memory.          |
| ```ADD```   | Add. Register A = register A + register B           |
| ```SUB```   | Subtract. Register A = register A - register B      |
| ```MUL```   | Multiply. Register A = register A * register B      |
| ```MUL```   | Divide. Register A = register A / register B        |
| ```AND```   | Boolean AND. Register A = register A AND register B |
| ```OR```    | Boolean OR. Register A = register A OR register B   |
| ```XOR```   | Boolean XOR. Register A = register A XOR register B |
| ```JAL```   | Unconditional jump.                                 |
| ```BEQ```   | Branch if register A is equal to register B         |
| ```BNE```   | Branch if register A is not equal to register B     |
| ```BLT```   | Branch if register A is less than register B        |
| ```BGE```   | Branch if register A is greater than or equal to B  |

## Addressing modes

Instructions that operate with memory (such as ```LOAD```and ```STORE```) need a reference to the memory address. 
There are several ways in which the memory address is calculated:

* Absolute: the address is read from the next two memory locations (little endian).
* Immediate: the address is the next in memory, following the current instruction. In other terms: address = pc + 1.
* Offset from immediate location: the address is calculated by adding an 8 bits offset to the PC. The offset is obtained from the next memory location. In other terms: address = pc + (value from pc + 1).
* Offset from register: the address is calculated by adding an 8 bits offset to the PC. The offset is obtained from any of the 16 registers. The register is obtained from the next memory location. In other terms: address = pc + (value from register specified in pc + 1).

We have 4 addressing modes, so we need 2 bits to codify this flavour in the opcode. Instructions will use bits
7 and 6 to codify the addressing mode.

### ```HCF```

Halt and Catch Fire. Stops the virtual machine and exits. Opcode ```00```.  

### Load from and store into memory

* ```LOAD``` loads a value from memory into a register. Opcode ```01```.
* ```STORE``` stores the value of a register into memory. Opcode ```02```.

In both instructions, the addressing mode and registers are read from the next memory value. 
The target register is specified in bits 0 to 3. The adressing mode is specified in bits 4 to 5. Bits 6 to 7 are unused.

In this table we list all the opcode masks for these instructions.

| instruction | immediate | absolute | offset from immediate location | offset from register |
| :-:         |       --: |      --: |                            --: |                  --: |
| ```LOAD```  |  ```  ``` |      --: |                            --: |                  --: |
| ```STORE``` |  ```  ``` |      --: |                            --: |                  --: |


### Arithmetic instructions

| instruction | signed opcode | unsigned opcode |
| :-:         |           --: |             --: |
| ```ADD```   |      ```03``` |        ```83``` |
| ```SUB```   |      ```04``` |        ```84``` |
| ```MUL```   |      ```05``` |        ```85``` |
| ```DIV```   |      ```06``` |        ```86``` |

### Boolean instructions

| instruction |     opcode |
| :-:         |        --: |
| ```AND```   | ```07``` |
| ```OR```    | ```08``` |
| ```XOR```   | ```09``` |

### Jumps

| instruction | immediate | absolute |   offset | register |
| :-:         |       --: |      --: |      --: |      --: |
| ```JAL```   |  ```0A``` | ```4A``` | ```8A``` | ```CA``` |
| ```BEQ```   |  ```0B``` | ```4B``` | ```8B``` | ```CB``` |
| ```BNE```   |  ```0C``` | ```4C``` | ```8C``` | ```CC``` |
| ```BLT```   |  ```0D``` | ```4D``` | ```8D``` | ```CD``` |
| ```BGE```   |  ```0E``` | ```4E``` | ```8E``` | ```CE``` |

All conditional jumps (```BEQ```, ```BNE```, ```BLT```, ```BGE```) require one operand, specifying the pair of
registers to compare. Bits 0 to 3 codify the ```src``` register, and bits 4 to 7 codify the ```dest``` register. 
The second operand depends on the addressing mode. Immediate, offset, and regiter modes require a second 1 byte
operand. Absolute requires a second two bytes operand.

### I/O

There are two I/O instructions.

* ```IN``` inputs a value from a device, and stores it in a register. Opcode ```0F``. The first byte operand
specifies the the register, and the second byte operand specifies the device.
* ```OUT``` outputs the value of a register into a device. Opcode ```10```. The first byte operand
specifies the the register, and the second byte operand specifies the device.

## Devices

### Console

This devices is in address ```00```. It only supports output.