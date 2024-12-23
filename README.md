# Flip01: a simple yet versatile 8-bit CPU

# Overview

**Flip01**, short for _First Level Instructional Processor_, is a small **8-bit CPU** with a **16-bit address bus**. <br />
This means it can handle data ranging from 0 to 2⁸–1 (255) and can store up to 2¹⁶ (65536 bit ~ 64 KB) different pieces of data. <br /> 

Data and addresses travel on two separate connection networks called the **data bus** and **address bus**, respectively. <br />

![2](https://github.com/user-attachments/assets/67aaeb13-bdc0-4479-894d-588990f07793)
> The schematic datapath of Flip01: it shows the connections between the components and the two buses (Data and Address).

Flip01 is based on a **Harvard architecture**, meaning that data and instructions (the actions the processor must execute) are stored in two separate memories: **MEM1 for data** and **MEM2 for instructions**. <br />
MEM1 is an 8-bit memory with a capacity of 64 KB, while MEM2 is a 9-bit memory with a capacity of 64 KB. <br /> <br />

![3](https://github.com/user-attachments/assets/ad15f7d3-d63a-4b4a-ba9b-cbd9139cb408)
> Schematic representation of the features of MEM1 and MEM2 and their connections to the rest of the circuit.

To move, temporarily store, and possibly modify these data, a component called register is used. <br />

# REGISTERS
In very simple terms, a register can be described as a standalone memory cell with **fast read and write capabilities**. <br />
In Flip01, there are **10 main registers** (_+ 1: spoiler_), and their size and connection (data or address bus) are defined by their **specific function**. <br />

- **AX:** This is the first of two **general-purpose registers**. <br />
This means it has no specific function within the processor but is used to store data for calculations. <br />
Since it only handles data, it is 8 bits in size and is connected solely to the **data bus**.
- **BX:** The second and final **general-purpose register**, with the same properties as AX. <br />
Like AX, it is 8 bits and connected only to the data bus. 
- **STAUTSAX (SAX):** An 8-bit register designed to **save the contents of AX** when needed. <br />
It can only communicate with AX, so it is not directly connected to either the data bus or the address bus. 
- **STATUSBX (SBX):** A mirror register to STATUSAX. <br />
It is also an 8-bit register but **saves the contents of BX** when required. <br />
Like SAX, it is not connected directly to the data or address buses.
- **DATA REGISTER (DR):** This register **communicates directly with the memory holding data** (MEM1). <br />
Since it handles both data and addresses, it is a 16-bit register connected to **both the data and address buses**. <br />
It uses converters to adapt the size before input and output:
  - On **input**, data is converted **from 8 to 16 bits** by padding with zeros.
  - On **output**, data is converted back **from 16 to 8 bits** by discarding the first 8 bits. <br /> 
  If any discarded bits contain data, an **error LED** will light up as a preventive measure.
- **INSTRUCTION REGISTER (IR):** A mirror register to DR, but it **interfaces with the memory that holds instructions** (MEM2). <br />
Instructions in memory are 9 bits long, divided as follows: <br />
The first bit specifies which general-purpose register (AX or BX) the instruction refers to: <br />
  - **1 for AX, 0 for BX**.
  - The remaining 8 bits contain the **actual instruction**. <br />
  Only the 8 bits representing the instruction are loaded into the IR, so it is an 8-bit register.
- **ALUIN:** Inside the processor, there is a unit responsible for performing calculations, called the ALU. <br />
The ALUIN register **takes and stores the first of the two operands for the ALU**. <br />
This is necessary because both operands cannot enter at the same time (they would conflict on the data bus). <br />
This register **holds the first operand** until the second arrives. <br />
The second operand doesn’t need to be stored since the ALU processes it immediately upon arrival. <br />
This register is also 8 bits, as it only handles data. 
- **ALUOUT:** This register **stores the result of the ALU operation**, then passes it to one of the general-purpose registers if needed for future operations. <br />
ALUOUT is also an 8-bit register.
- **MEMORY ADDRESS REGISTER (MAR):** This register **stores the memory address of the data to be read or rewritten**. <br />
It is useful when an instruction refers to reading data from a specific memory location, which is different from the instruction’s own location. <br />
MAR helps **retrieve the address of the data, locate it in memory, and either read its value or overwrite it with a new one**. <br />
Since it deals only with addresses, it is a 16-bit register connected to the address bus both for input and output. <br />
It is also connected to the data bus for output, with a **16-to-8-bit converter** (similar to the one in DR) that triggers an error LED if data is lost during conversion. 
- **PROGRAM COUNTER (PC):** This register **holds the address of the next instruction to be executed**. <br />
It ensures that, at each step of the program, the processor knows exactly where to read the next instruction. <br />
This is particularly useful when the execution flow is non-linear. <br />
_(foreshadowing jump instructions????)_ <br />
Like MAR, the PC is a 16-bit register connected to the address bus for both input and output and to the data bus for output, with an **intermediary converter**. <br />

All registers update when there is a state change in a common signal that goes to all components that require it. <br />
This signal is called **the clock**, and ideally, it looks like this: <br />

![4](https://github.com/user-attachments/assets/c2511464-eeda-4584-a3a3-90f50d8c3842)
> A diagram showing the behavior of the clock signal over time.

It **oscillates between 0 and 1**, and the instant change from 0 to 1 **updates the values stored in the registers** based on the data circulating in the bus at that exact moment. <br />
<br />
If registers are used to temporarily store data, as previously mentioned, another fundamental component called the **ALU** (_Arithmetic Logic Unit_) is used to modify it. <br />

# ALU
Every processor has a different ALU in terms of **calculation power** or **data handling capabilities**. <br />
The ALU of Flip01 is designed to take **two 8-bit inputs** and produce a single output of the same size. <br />

Based on the configuration and the combination of **6 binary control signals** (0 or 1), the ALU can perform **8 main operations** and **4 derived ones**.<br />
The 6 control signals are as follows:

- **BRin:** Sets the second input of the ALU to 0.
- **s0, s1, s2:** The unique combinations of these signals determine which of the 8 main operations will be executed.
- **Cin, op:** These signals control **additional bits** for operations that require them.
<br />
The 8 main operations are:

- **ADD:** When given two inputs, A and B, this configuration **outputs the sum of the two**: `A + B`
- **SUB:** With the same inputs, A and B, this configuration **outputs the difference**: `A — B`
- **AND:** For the same inputs, this configuration performs a **bitwise AND operation** on A and B: `A AND B` <br />
Following the logical structure of a single-bit AND operator.

![5](https://github.com/user-attachments/assets/bc612089-21f1-4143-a9e3-3dcb9f56d757)
> Logic table of the AND operator (left) and two examples with 8-bit operands (right)

- **OR:** For the same inputs, this configuration performs a **bitwise OR operation** on A and B: `A OR B` <br />
Following the logical structure of a single-bit OR operator.

![6](https://github.com/user-attachments/assets/6055b97d-bff2-442a-bdce-0e82994c1cac)
> Logic table of the OR operator (left) and two examples with 8-bit operands (right)

- **NOT:** In this case, only one operand is considered as input (A). <br />
This configuration outputs the **bitwise inverse of the input operand**: `NOT A` <br />
Following the logical structure of a NOT operator

![7](https://github.com/user-attachments/assets/bb6cedb0-cdf2-448c-a791-0635abfcee20)
> Logic table of the NOT operator (left) and two examples with 8-bit operands (right)

- **SHIFTL:** This configuration **shifts the bits of operand A to the left by B positions**, keeping the result 8 bits long. <br />
The vacated positions on the right are filled with 0 padding.
- **SHIFTR:** This configuration **shifts the bits of operand A to the right by B positions**, keeping the result 8 bits long. <br />
The vacated positions on the left are filled with 0 padding, regardless of the leftmost bit (logical shift).

![8](https://github.com/user-attachments/assets/46f9cf36-4804-4415-861c-f5a69d3c527c)
> Practical example of how left shift (shiftl) and right shift (shiftr) operations work.

- **MUL:** With bits s0, s1, and s2 set this way, **operand A is multiplied by operand B**.
  The operation performed is a standard arithmetic multiplication.

Below are the specific configurations for the 6 contol signals _(BRin, Cin, op, s0, s1, and s2)_ used for each of the 8 main operations.

![12](https://github.com/user-attachments/assets/7855a9cf-6379-47aa-8ed1-4a9af3610faf)
> Control signal combinations that define the various main operations indicated.

By manipulating the ALU’s inputs, you can derive **four additional operations**:

- **INC:** The first input to the ALU remains unchanged, while the second is forced to 1. <br />
Setting the ALU to the add configuration gives `A + 1`, **incrementing the value by one unit**.
- **DEC:** Similarly, the first input remains unchanged, while the second is forced to 1. <br />
However, setting the ALU to the sub configuration gives `A — 1`, **decrementing the value by one unit**.
- **0:** Regardless of the input, this operation **outputs the value 0**. <br />
Given input A, forcing the second input to 0 and performing an AND operation results in 0, no matter the value of A.
- **A:** Finally, you can **output the same value as the first input without modifying it**. <br />
Given input A, forcing the second input to 0 and performing a sub operation results in A: `A — 0 = A`. <br />

![11](https://github.com/user-attachments/assets/ce15d75f-0181-4de0-a23f-fb393ff980c0)
> Control signal combinations that define the four additional operations.

Connected to the ALU there is an additional, **critical register**:

## FLAG REGISTER
This is the third and final register that communicates with the ALU. <br />

It’s an 8-bit register where **each bit indicates a specific aspect of the most recent operation**:

- **X (Shift/Mul Error):** This bit is set **if the number of bits to shift exceeds 8**, surpassing the maximum allowable length for the operands involved in the shift operation, **or if the result of a multiplication operation exceeds the usual 8-bit limit**.
  The bit is updated regardless of the operation type. Therefore, if the operation performed is neither a shift (_shiftl or shiftr_) nor a multiplication (_mul_), the X flag bit can be ignored.
It can be ignored if the executed operation is of a different type.
- **L _(A < B)_:** This bit is set if **operand A is less than operand B** in absolute value.
- **E _(A = B)_:** This bit is set if **operand A is equal to operand B** in absolute value.
- **G _(A > B)_:** This bit is set if **operand A is greater than operand B** in absolute value.
- **C _(Carry)_:** This bit is set if there was a carry during the operation, meaning **the most significant bit had to generate a carry to the previous bit**.
- **V _(Overflow)_:** This bit is set if **the result exceeds the 8-bit limit**, even though both operands are 8 bits. <br />
This indicates that an extra bit would have been required for the result to be correct.
- **Z _(Zero)_:** This bit is set if **the result of the operation is zero**.
- **N _(Negative)_:** This bit is set if **the result of the operation is negative** for any reason.

> [!TIP]
> Quick reminder before we continue: if you enjoy these kinds of projects, feel free to follow us **FOR FREE** on [Patreon](https://www.patreon.com/c/PescettiStudio/posts)!
> You’ll get early updates and exclusive content, and even a “_simple_” follow really helps keep these projects free and open source.
> Thanks :)

# INSTRUCTIONS

Instructions represent the full range of actions the processor can perform. <br />
Flip01 has **48 instructions**, each identified by a **unique operation code** _(op-code)_ in hexadecimal format. <br />

Instructions consist of **micro-instructions**, which are movements of data between registers or arithmetic and logical operations. <br />
For example, the addR instruction, which adds the contents of the two general-purpose registers AX and BX, is made up of the following micro-instructions:

![12](https://github.com/user-attachments/assets/a89c9f5a-5935-4854-8315-a31251f71e5d)
> Example of reading and interpreting microinstructions (using the microinstructions of the add$ instruction as a case study).

If two micro-instructions operate on different buses (data and address) and are sequential, **they can be executed in the same clock cycle**, as no data will conflict. <br />

In MEM1 memory cells, it’s possible to assign a unique name to each cell, allowing access to it during program execution through read and write actions. <br />
To assign a name to a memory cell, the following syntax is used: <br />
```
[name] = [value]
```
The **`=`** operator symbolizes an **assignment operation**. <br />

For the remainder of the manual, this type of memory cell will be referred to as a **VARIABLE**, adopting standard terminology. <br />

In textual form, this can be expressed as: <br />
_“The cell named [name] will initially contain the value [value] at the start of the program.”_ <br />

The **address of the memory cell**, or its location within MEM1, **is assigned procedurally**, typically as the first available cell after those reserved for program parameters. <br />

The value indicated represents only the initial assignment to the cell, and to modify its content later, you must use the CPU instructions. <br />

Currently, the instructions available on Flip01 can be grouped into **five categories**:
1. **Direct addressing instructions**
2. **Single-operand instructions**
3. **Zero-operand instructions**
4. **Immediate instructions**
5. **Jump instructions**

## Direct Addressing Instructions
These instructions consist of **two parameters**: the first is the **general-purpose register** (AX or BX) they refer to, and the second is the memory address containing the value to be considered. <br />
In Flip01’s high-level syntax, the memory address specified as the second parameter is associated with the **unique name of a variable**. <br />
These instructions involve memory access for reading the data. <br />
```
Instruction syntax: Instruction [register] [variable]
```
### **load**
`(syntax: load [register] [variable])` <br />
**_op-code: 0x02_** <br />
This instruction copies the value of the variable specified in the _[variable]_ parameter into the register indicated in the _[register]_ parameter.
  - MEM1 _[variable]_ -> DR
  - DR -> _[register]_
    
The load instruction requires 2 clock cycles to execute.
### **store** 
`(syntax: store [register] [variable])` <br />
op-code: 0x04 <br />
This instruction copies the value in the _[register]_ parameter into the memory cell assigned to the variable specified in the _[variable]_ parameter.
  - _[register]_ -> DR
  - DR -> MEM1 _[variable]_
    
The store instruction requires 2 clock cycles to execute.
### **add** 
`(syntax: add [register] [variable])` <br />
_**op-code: 0x06**_ <br />
This instruction adds the value associated with the _[variable]_ parameter to the value stored in the _[register]_ parameter. The result is saved in the specified register. <br />
_[register]_ = _[register]_ + _[variable]_
  - MEM1 _[variable]_ -> DR, _[register]_ -> ALUA
  - DR + ALUA -> ALUOUT
  - ALUOUT -> _[register]_
    
The add instruction requires 3 clock cycles to execute.
### **sub** 
`(syntax: sub [register] [variable])` <br />
_**op-code: 0x09**_ <br />
This instruction subtracts the value associated with the _[variable]_ parameter from the value stored in the _[register]_ parameter. The result is saved in the specified register. <br />
_[register]_ = _[register]_ — _[variable]_
  - MEM1 _[variable]_ -> DR, _[register]_ -> ALUA
  - DR — ALUA -> ALUOUT
  - ALUOUT -> _[register]_
    
The sub instruction requires 3 clock cycles to execute.
### **and** 
`(syntax: and [register] [variable])` <br />
_**op-code: 0x0C**_ <br />
This instruction performs a bitwise AND operation between the value stored in the _[register]_ and the value associated with the _[variable]_. The result is saved in the specified register. <br />
_[register]_ = _[register]_ AND _[variable]_
  - MEM1 _[variable]_ -> DR, _[register]_ -> ALUA
  - DR AND ALUA -> ALUOUT
  - ALUOUT -> _[register]_
    
The and instruction requires 3 clock cycles to execute.
### **or** 
`(syntax: or [register] [variable])` <br />
_**op-code: 0x0F**_ <br />
This instruction performs a bitwise OR operation between the value stored in the _[register]_ and the value associated with the _[variable]_. The result is saved in the specified register. <br />
_[register]_ = _[register]_ OR _[variable]_
  - MEM1 _[variable]_ -> DR, _[register]_ -> ALUA
  - DR OR ALUA -> ALUOUT
  - ALUOUT -> _[register]_
    
The or instruction requires 3 clock cycles to execute.
### **cmp** 
`(syntax: cmp [register] [variable])` <br />
_**op-code: 0x30**_ <br />
The CMP instruction compares the value in the register specified in the _[register]_ field with the value of the variable indicated in the _[variable]_ field. <br />
The flags are updated upon completion of this operation.
  - MEM1 _[variable]_ -> DR, _[register]_ -> ALUA
  - ALUA — DR -> ALUOUT
    
The cmp instruction requires 2 clock cycles to execute.

### **test** 
`(syntax: test [register] [variable])` </br>
_**op-code: 0x69**_ </br>
This instruction performs an AND operation between the value stored in the _[register]_ and the specified _[variable]_. The result is stored in the specified register. </br>
_[register]_ AND _[variable]_
- MEM1 _[variable]_ -> DR, _[register]_ -> ALUA
- DR AND ALUA -> ALUOUT
  
The test instruction requires 2 clock cycles to execute.

### **mul** 
`(syntax: mul [register][variable])` </br>
_**op-code: 0x60**_ </br>
This instruction multiplies the value stored in the _[register]_ by the value specified in _[variable]_.
_[register]_ = _[register]_ * _[variable]_
- MEM1 _[variable]_ -> DR, _[register]_ -> ALUA
- ALUA * DR -> ALUOUT
- ALUOUT -> _[register]_
  
The mul instruction requires 3 clock cycles to execute.

## Single-Operand Instructions
These instructions involve only one parameter, usually the general-purpose register (AX or BX) they refer to. <br />
```
Instruction syntax: Instruction [register]
```
### **not** 
`(syntax: not [register])` <br />
_**op-code: 0x12**_ <br />
This instruction inverts the bits of the value stored in the _[register]_. The result is saved in the specified register. <br />
_[register]_ = NOT _[register]_
  - DR -> ALUA
  - ALUA — _[register]_ -> ALUOUT
  - ALUOUT -> _[register]_
    
The not instruction requires 3 clock cycles to execute.
### **neg** 
`(syntax: neg [register])` <br />
_**op-code: 0x15**_ <br />
This instruction negates the value stored in the _[register]_. The result is saved in the specified register. <br /> 
_[register]_ = NEG _[register]_
  - DR -> ALUA
  - ALUA — _[register]_ + 1 -> ALUOUT
  - ALUOUT -> _[register]_
    
The neg instruction requires 3 clock cycles to execute.
### **inc** 
`(syntax: inc [register])` <br />
_**op-code: 0x24**_ <br />
This instruction increments the value stored in the _[register]_ by 1. The result is saved in the specified register. <br />
_[register]_ = _[register]_ + 1
  - _[register]_ -> ALUA
  - ALUA + 1 -> ALUOUT
  - ALUOUT -> _[register]_
    
The inc instruction requires 3 clock cycles to execute.
### **dec** 
`(syntax: dec [register])` <br />
_**op-code: 0x27**_ <br />
This instruction decrements the value stored in the _[register]_ by 1. The result is saved in the specified register. <br />
_[register]_ = _[register]_ — 1
  - _[register]_ -> ALUA
  - ALUA — 1 -> ALUOUT
  - ALUOUT -> _[register]_
    
The dec instruction requires 3 clock cycles to execute.
### **rnd** 
`(syntax: rnd [register])` <br />
_**op-code: 0x37**_ <br />
This instruction stores a random value between 0 and 255 in the _[register]_.
  - RND-> _[register]_
    
The rnd instruction requires 1 clock cycle to execute.
### **save** 
`(syntax: save [register])` <br />
_**op-code: 0x5C**_ <br />
This instruction copies the value from the _[register]_ into the corresponding status register.
  - _[register]_ -> STATUS _[register]_
    
The save instruction requires 1 clock cycle to execute.
### **read** 
`(syntax: read [register])` <br />
_**op-code: 0x5D**_ <br />
This instruction copies the value from the corresponding status register into the _[register]_.
  - STATUS _[register]_ -> _[register]_
    
The read instruction requires 1 clock cycle to execute.
### **copy** 
`(syntax: copy _[register]_)` </br>
_**op-code: 0x73**_ </br>
This instruction copies the value in the specified [register] to another register.
- _[register]_ -> ALUA
- ALUA OR 0 -> ALUOUT
- ALUOUT -> AX, ALUOUT -> BX

The copy instruction requires 3 clock cycles to execute.

## Zero-Operand Instructions
These instructions do not take any parameters. <br />
They primarily act on the processor state or perform operations between the two general-purpose registers, without needing to specify a target.
```
Instruction syntax: Instruction
```
### **addR** 
`(syntax: addR)` <br />
_**op-code: 0x18**_ <br />
This instruction adds the values contained in the two general-purpose registers (AX and BX). The result is stored in register AX. <br />
AX = AX + BX
  - AX -> ALUA
  - ALUA + BX -> ALUOUT
  - ALUOUT -> AX
    
The addR instruction requires 3 clock cycles to execute.
### **subR** 
`(syntax: subR)` <br />
_**op-code: 0x1E**_ <br />
This instruction subtracts the value in register BX from the value in register AX. The result is stored in register AX. <br />
AX = AX — BX
  - AX -> ALUA
  - ALUA — BX -> ALUOUT
  - ALUOUT -> AX
    
The subR instruction requires 3 clock cycles to execute.
### **andR** 
`(syntax: andR)` <br />
_**op-code: 0x21**_ <br />
This instruction performs a bitwise AND operation between the values stored in the general-purpose registers. The result is stored in register AX. <br />
AX = AX AND BX
  - AX -> ALUA
  - ALUA AND BX -> ALUOUT
  - ALUOUT -> AX
    
The andR instruction requires 3 clock cycles to execute.
### **orR** 
`(syntax: orR)` <br />
_**op-code: 0x1B**_ <br />
This instruction performs a bitwise OR operation between the values stored in the general-purpose registers. The result is stored in register AX. <br />
AX = AX OR BX
  - AX -> ALUA
  - ALUA OR BX -> ALUOUT
  - ALUOUT -> AX
    
The orR instruction requires 3 clock cycles to execute.
### **cmpR** 
`(syntax: cmpR)` <br />
_**op-code: 0x32**_ <br />
This instruction compares the values stored in the general-purpose registers. The comparison is performed by subtracting the value in register BX from the value in register AX, updating the Flag register accordingly, and discarding the result.
  - AX -> ALUA
  - ALUA — BX -> ALUOUT
    
The cmpR instruction requires 2 clock cycles to execute.
### **flip** 
`(syntax: flip)` <br />
_**op-code: 0x34**_ <br />
This instruction swaps the values stored in the two general-purpose registers (AX and BX).
  - AX -> ALUA
  - BX -> AX, ALUA-> ALUOUT
    
The flip instruction requires 2 clock cycles to execute.
### **stop** 
`(syntax: stop)` <br />
_**op-code: 0x38**_ <br />
This instruction halts the execution of the program.
  - CLKDIS = 1
  - (wait)
    
The stop instruction requires 2 clock cycles to execute.
### **testR** 
`(syntax: testR)` </br>
_**op-code: 0x6B**_ </br>
This instruction performs an AND operation between the two general-purpose registers. The result is stored in the AX register. </br>
AX AND BX
- [register] -> ALUA
- ALUA -1 -> ALUOUT
- ALUOUT -> [register]
  
The testR instruction requires 3clock cycles to execute.
### **nop** 
`(syntax: nop)` </br>
_**op-code: 0x6F**_ </br>
This instruction does nothing.
- nothing
  
The nop instruction requires 1 clock cycle to execute.
### **wait** 
`(syntax: wait)` </br>
_**op-code: 0x70**_ </br>
This instruction does nothing for 3 clock cycles, creating a 5-cycle pause in execution (including 2 fetch cycles).
- nothing
- nothing
- nothing
  
The wait instruction requires 3 clock cycles to execute.

### **mulR**
`(syntax: mulR)` </br>
_**op-code: 0x63**_ </br>
This instruction multiplies the values in the two general-purpose registers. The result is stored in the AX register. </br>
AX = AX * BX
- AX -> ALUA
- ALUA * BX -> ALUOUT
- ALUOUT -> AX
  
The mulR instruction requires 3 clock cycles to execute.

## Immediate Instructions
These instructions consist of two parameters: the first is the general-purpose register (AX or BX) they refer to, and the second is the constant value to be considered.
```
Instruction syntax: Instruction [register] [value]
```
Unlike direct instructions, these instructions do not require memory access to read the data. <br /><br />

### **load$** 
`(syntax: load$ [register] [value])` <br />
_**op-code: 0x59**_ <br />
This instruction copies the _[value]_ parameter into the _[register]_. It is a load instruction.
  - DR -> _[register]_
    
The load$ instruction requires 1 clock cycle to execute.
### **add$** 
`(syntax: add$ [register] [value])` <br />
_**op-code: 0x50**_ <br />
This instruction adds the _[value]_ parameter to the value stored in the _[register]_. The result is saved in the specified register. <br />
_[register]_ = _[register]_ + _[value]_
  - _[register]_ -> ALUA
  - ALUA + DR -> ALUOUT
  - ALUOUT -> _[register]_
    
The add$ instruction requires 3 clock cycles to execute.
### **sub$** 
`(syntax: sub$ [register] [value])` <br />
_**op-code: 0x4D**_ <br />
This instruction subtracts the _[value]_ parameter from the value stored in the _[register]_. The result is saved in the specified register. <br />
_[register]_ = _[register]_ — _[value]_
  - _[register]_ -> ALUA
  - ALUA — DR -> ALUOUT
  - ALUOUT -> _[register]_
    
The sub$ instruction requires 3 clock cycles to execute.
### **and$** 
`(syntax: and$ [register] [value])` <br />
_**op-code: 0x53**_ <br />
This instruction performs a bitwise AND operation between the value stored in the _[register]_ and the _[value]_ parameter. The result is saved in the specified register. <br />
_[register]_ = _[register]_ AND _[value]_
  - _[register]_ -> ALUA
  - ALUA AND DR -> ALUOUT
  - ALUOUT -> _[register]_
    
The and$ instruction requires 3 clock cycles to execute.
### **or$** 
`(syntax: or$ [register] [value])` <br />
_**op-code: 0x56**_ <br />
This instruction performs a bitwise OR operation between the value stored in the _[register]_ and the _[value]_ parameter. The result is saved in the specified register. <br />
_[register]_ = _[register]_ OR _[value]_
  - _[register]_ -> ALUA
  - ALUA OR DR -> ALUOUT
  - ALUOUT -> _[register]_
    
The or$ instruction requires 3 clock cycles to execute.
### **shiftl$** 
`(syntax: shiftl$ [register] [value])` <br />
_**op-code: 0x2A**_ <br />
This instruction performs a logical left shift on the value stored in the _[register]_ by the number of bits specified in the _[value]_ parameter. The result is saved in the specified register. <br />
_[register]_ = _[register]_ shiftl _[value]_
  - _[register]_ -> ALUA
  - ALUA SHIFTL DR -> ALUOUT
  - ALUOUT -> _[register]_
    
The shiftl$ instruction requires 3 clock cycles to execute.
### **shiftr$** 
`(syntax: shiftr$ [register] [value])` <br />
_**op-code: 0x2D**_ <br />
This instruction performs a logical right shift on the value stored in the _[register]_ by the number of bits specified in the [value] parameter. The result is saved in the specified register. <br />
_[register]_ = _[register]_ shiftr _[value]_
  - _[register]_ -> ALUA
  - ALUA SHIFTR DR -> ALUOUT
  - ALUOUT -> _[register]_
    
The shiftr$ instruction requires 3 clock cycles to execute.
### **cmp$** 
`(syntax: cmp$ [register] [value])` <br />
_**op-code: 0x5A**_ <br />
This instruction compares the value stored in the [register] with the _[value]_ parameter. <br />
The comparison is performed by subtracting the _[value]_ from the register’s value, updating the Flag register accordingly, and discarding the result.
  - _[register]_ -> ALUA
  - ALUA OR DR -> ALUOUT
    
The cmp$ instruction requires 2 clock cycles to execute.
### **test$** 
`(syntax: test _[register] [value]_)` </br>
_**op-code: 0x6D**_ </br>
This instruction performs an AND operation between the value stored in the _[register]_ and the specified _[variable]_. The result is stored in the _[register]_. </br>
_[register]_ AND _[variable]_
- _[register]_ -> ALUA
- ALUA — DR-> ALUOUT
- ALUOUT -> _[register]_
  
The test$ instruction requires 3 clock cycles to execute.

### **mul$** 
`(syntax: mul$ [register][value])` </br>
_**op-code: 0x66**_ </br>
This instruction multiplies the value stored in the _[register]_ by the value specified in _[value]_. </br>
_[register]_ = _[register]_ * _[value]_
- _[register]_ -> ALUA
- ALUA * DR -> ALUOUT
- ALUOUT -> _[register]_
  
The mul$ instruction requires 3 clock cycles to execute.

## Jump Instructions
These instructions interrupt the linear execution of the program to execute code segments identified by labels. 
```
Instruction syntax: Instruction [label]
```
### **jmp** 
`(syntax: jmp [label])` <br />
_**op-code: 0x4C**_ <br />
This instruction unconditionally jumps to the specified _[label]_.
  - DR -> PC
    
The jmp instruction requires 1 clock cycle to execute.
### **jc** 
`(syntax: jc [label])` <br />
_**op-code: 0x3A**_ <br />
This instruction jumps to the specified _[label]_ if the carry bit © is set to 1 by the previous instruction.
  - DR -> PC
    
The jc instruction requires 1 clock cycle to execute.
### **jv** 
`(syntax: jv [label])` <br />
_**op-code: 0x3C**_ <br />
This instruction jumps to the specified _[label]_ if the overflow bit (V) is set to 1 by the previous instruction.
  - DR -> PC
    
The jv instruction requires 1 clock cycle to execute.
### **jn** 
`(syntax: jn [label])` <br />
_**op-code: 0x3E**_ <br />
This instruction jumps to the specified _[label]_ if the negative bit (N) is set to 1 by the previous instruction.
  - DR -> PC
    
The jn instruction requires 1 clock cycle to execute.
### **jz** 
`(syntax: jz [label])` <br />
_**op-code: 0x40**_ <br />
This instruction jumps to the specified _[label]_ if the zero bit (Z) is set to 1 by the previous instruction.
  - DR -> PC
    
The jz instruction requires 1 clock cycle to execute.
### **je** 
`(syntax: je [label])` <br />
_**op-code: 0x42**_ <br />
⚠ This instruction must immediately follow a compare instruction (cmp, cmpR, cmp$). <br />
It jumps to the specified _[label]_ if and only if the operands specified by the previous compare instruction are equal; otherwise, the program continues normally.
  - DR -> PC
    
The je instruction requires 1 clock cycle to execute.
### **jg** 
`(syntax: jg [label])` <br />
_**op-code: 0x44**_ <br />
⚠ This instruction must immediately follow a compare instruction (cmp, cmpR, cmp$). <br />
It jumps to the specified _[label]_ if and only if the first operand in the previous compare instruction is greater than the second operand; otherwise, the program continues normally. 
  - DR -> PC
    
The jg instruction requires 1 clock cycle to execute.
### **jl** 
`(syntax: jl [label])` <br />
_**op-code: 0x46**_ <br />
⚠ This instruction must immediately follow a compare instruction (cmp, cmpR, cmp$). <br />
It jumps to the specified _[label]_ if and only if the first operand in the previous compare instruction is less than the second operand; otherwise, the program continues normally.
  - DR -> PC
    
The jl instruction requires 1 clock cycle to execute.
### **jle** 
`(syntax: jle [label])` <br />
_**op-code: 0x48**_ <br />
⚠ This instruction must immediately follow a compare instruction (cmp, cmpR, cmp$). <br />
It jumps to the specified _[label]_ if and only if the first operand in the previous compare instruction is less than or equal to the second operand; otherwise, the program continues normally.
  - DR -> PC
    
The jle instruction requires 1 clock cycle to execute.
### **jge** 
`(syntax: jge [label])` <br />
_**op-code: 0x4A**_ <br />
⚠ This instruction must immediately follow a compare instruction (cmp, cmpR, cmp$). <br />
It jumps to the specified _[label]_ if and only if the first operand in the previous compare instruction is greater than or equal to the second operand; otherwise, the program continues normally.
  - DR -> PC
    
The jge instruction requires 1 clock cycle to execute.

A processor must also be capable of receiving data from external sources, processing it, and displaying it to the user.<br />
To accomplish this, Flip01 utilizes an external circuit called the Input & Output Manager, or simply the: 

# I/O Manager
This circuit is responsible for reading from and writing to **64 unique I/O devices**. <br />
Split into **32 input devices** and **32 output devices**, each port has an identifier ranging from 0 to 31.<br />
During the program, the user will decide which port to connect their devices to and assign them an identifier number accordingly. <br />

In the demo version of Flip01 on Logisim, both in the `“Flip01_Full”` circuit and the `“Playground”` version (which features a compact processor design to facilitate external connections), the I/O Management module **already has devices connected in specific positions**, with ports already assigned. <br />

### input
- Port **I00** — Keyboard Data [7 bits]
- Port **I01** — Keyboard Ready: Indicates if the keyboard buffer contains a character [1 bit]
- Port **I23** — BTN0: A push-button _(1/8)_ [1 bit]
- Port **I24** — BTN1: A push-button _(2/8)_ [1 bit]
- Port **I25** — BTN2: A push-button _(3/8)_ [1 bit]
- Port **I26** — BTN3: A push-button _(4/8)_ [1 bit]
- Port **I27** — BTN4: A push-button _(5/8)_ [1 bit]
- Port **I28** — BTN5: A push-button _(6/8)_ [1 bit]
- Port **I29** — BTN6: A push-button _(7/8)_ [1 bit]
- Port **I30** — BTN7: A push-button _(8/8)_ [1 bit]

The **21 input ports** from **I02 to I22** are unassigned and can be freely used by the user.

### output
- Port **O00** — TTY Data (Teletypewriter, capable of displaying text messages) [7 bits]
- Port **O01** — RGB Video X Coordinate [8 bits]
- Port **O02** — RGB Video Y Coordinate [8 bits]
- Port **O03** — RGB Video Write Enable: When set to 1, it writes to the cursor position defined by X and Y [1 bit]
- Port **O04** — LED State 0: Turns on if it receives 1, and stays on until it receives 2, and vice versa _(1/4)_ [2 bits]
- Port **O05** — LED State 1: Turns on if it receives 1, and stays on until it receives 2, and vice versa _(2/4)_ [2 bits]
- Port **O06** — LED State 2: Turns on if it receives 1, and stays on until it receives 2, and vice versa _(3/4)_ [2 bits]
- Port **O07** — LED State 3: Turns on if it receives 1, and stays on until it receives 2, and vice versa _(4/4)_ [2 bits]
- Port **O23** — LED Matrix 8x8 Row 0 _(1/8)_ [8 bits]
- Port **O24** — LED Matrix 8x8 Row 1 _(2/8)_ [8 bits]
- Port **O25** — LED Matrix 8x8 Row 2 _(3/8)_ [8 bits]
- Port **O26** — LED Matrix 8x8 Row 3 _(4/8)_ [8 bits]
- Port **O27** — LED Matrix 8x8 Row 4 _(5/8)_ [8 bits]
- Port **O28** — LED Matrix 8x8 Row 5 _(6/8)_ [8 bits]
- Port **O29** — LED Matrix 8x8 Row 6 _(7/8)_ [8 bits]
- Port **O30** — LED Matrix 8x8 Row 7 _(8/8)_ [8 bits]
- Port **O31** — Reset Bit: When set to 1, it resets the connected device. <br />
If multiple devices are in use, it might be helpful to assign different reset bits to reset devices at different times.

The **15 output ports** from **O08 to O22** are unassigned and can be freely used by the user. <br /><br />

If you need to change the reference ports for certain components, you will have to **update the corresponding port** entered as a constant within the module. <br />
The port inside the module must match the selected port. <br />
This step is also crucial if **additional components of the same type** are added. <br />
The devices that require this change are: <br />

+ **The 8x8 LED matrix controller**: Each row corresponds to an output, meaning each port must be synchronized with the I/O Manager.
+ **The RGB video controller**: The X and Y coordinate inputs correspond to two ports that also need to be synchronized with the I/O Manager.

## I/O Instructions
These instructions **handle the communication between external devices and the CPU**. <br />
They require **two parameters**: the first is the register that will be affected by the instruction, while the second is the name of the device that will interact with that register.
```
Instruction syntax: Instruction [register][device]
```
### **input** 
`(syntax: input [register] [device])` <br />
_**op-code: 0x5E**_ <br />
This instruction reads the value transmitted from the input device _[device]_ connected to the port _[port number]_ and copies it into the specified register _[register]_. <br />
Each input device must be declared using the syntax: <br />
_> [device] [port number]_
  - DR -> I/O Manager, _[device]_ > _[register]_
    
The input instruction requires 1 clock cycle to execute.
### **output** 
`(syntax: output [register] [device])` <br />
_**op-code: 0x5F**_ <br />
This instruction reads the value stored in the specified register _[register]_ and transmits it to the output device _[device]_ connected to the port _[port number]_. <br />
Each output device must be declared using the syntax: <br />
_< [device] [port number]_
  - DR -> I/O Manager, _[register]_ > _[device]_
    
The output instruction requires 1 clock cycle to execute. <br />

Writing programs and manually replacing op-codes, hexadecimal values, and addresses can be tiresome, so we have created a small **assembler**, which acts as a **translator from high-level language**.

# THE ASSEMBLER
This program is responsible for **translating mnemonic instructions** (load, sub, add$, etc.) into their corresponding opcodes, while also converting numeric values and memory references into formats understandable by the processor.

![4](https://github.com/user-attachments/assets/6da3b320-6bfd-496b-ae5a-4adff5caa7a8)
> The assembler after startup

The code should be written in the panel that, on startup, displays the message `“write your text here”`. <br />
The conversion is automatically shown in the space below. <br /><br />

**To disable the live machine code conversion**, simply press the corresponding button labeled Live analysis enabled.<br />
At that point, the button and interface will indicate that live translation has been paused.<br /><br />
**To resume real-time conversion** or to view the corresponding translation, press the button again. <br />

To:

- **open a text file** (.txt) containing code you want to analyze
- **save the current file**
- **clear the content**
- **start a new file**

just click the corresponding buttons. <br />

After specifying the name or the path for the current file, it won’t be necessary to do so again, and all subsequent saves will be made to that file. <br />
This continues until the user initializes a new file via the New Code button. <br />

The assembler comes with a built-in decimal-hexadecimal-binary converter. </br>
To access it, just click the **Conversion Table** button, which opens the converter in a new window. </br>
Simply select the input format, enter the value in the text bar, and the conversions into all three formats will happen automatically. </br>
A control panel below the results **displays any input errors or format issues** in real time.  

![6](https://github.com/user-attachments/assets/2d74f8f1-4a2c-4a90-9410-63871ffaae67)
> The converter

> [!NOTE]
> Closing the converter window won’t affect the assembler, but closing the assembler will also close the converter window if it’s open.  

Another handy feature of the assembler is the **View Manual** button. </br>
As the name suggests, it opens the free Flip01 manual in your default browser. </br>
The manual is incredibly useful for programming, as it includes a full description of the processor, along with opcodes, details, and explanations of all supported CPU instructions.  

The manual is a visually enhanced and easier-to-read version of the information found in this README. </br>
It’s the same free manual available on Medium, which has been previously mentioned.  

Any changes made in the assembler’s text editor are automatically saved in a _log.txt_ file located in the same folder as the assembler. </br>
If the assembler crashes for any reason, your unsaved work won’t be lost. </br>
You can recover it by opening the _log.txt_ file directly from the assembler. </br>
Think of it as a real-time safety net, creating backup files on the fly to safeguard your progress in case something goes wrong.

**To add a comment to the code**, use the following syntax:
```
// Comment
```
Comments can only be added at the beginning of a new line. <br />

> [!NOTE]
>The assembler was developed in _C/C++_ using the _Dev C++ IDE_.

We hope that the UI and UX are quite intuitive, but it may be helpful to explicitly outline **all possible errors**, their **corresponding codes**, and **how to resolve them**: <br />

## Errors
- **E — 000:** The file you are trying to open and read using the designated button may be corrupted or unreachable. <br />
Please check that the file is intact and in the correct format (.txt).
- **E — 001:** The code you have written is too large for Flip01! <br />
Try to shorten it and clean it up by using calls to labels for repeated code segments.
- **E — 002:** The instruction you have written is not among those supported by Flip01. <br />
Double-check that the syntax is correct and remember that the assembler is case-sensitive: instructions must be written in lowercase (except for the R in immediate instructions).
- **E — 003:** The instruction does not have a declared variable as its second argument. This argument must not be a number.
- **E — 004:** The instruction is missing some required parameters. <br />
Refer to the previous chapter to check how many and which arguments are needed for each instruction.
- **E — 005:** This instruction requires a general-purpose register (AX or BX) as the first argument. <br />
The parameter written in the first position is likely incorrect or undefined.
- **E — 006:** The instruction has more parameters than required. <br />
Refer to the previous chapter to check how many and which arguments are needed for each instruction.
- **E — 007:** The argument associated with this direct instruction is too large! <br />
Remember that acceptable values range from 0 to 255.
- **E — 008:** The argument associated with the direct instruction is not an integer. <br />
It is likely that an alphanumeric value was entered or that this parameter has not been defined.
- **E — 009:** The variable has not been declared at the beginning of the program. <br />
Ensure that the variable name is spelled correctly and that the name written in the declaration matches the one used in the instruction. <br />
Remember that the assembler is case-sensitive, and the declaration must follow the structure: <br />
`[variable name] = [numeric value]`
- **E — 010:** The value associated with the variable is not an integer. <br />
An alphanumeric value may have been entered, or this parameter has not been defined.
- **E — 011:** This variable has been defined multiple times within the code. <br />
Remember that to change the value of a memory cell, you must use processor instructions and not assignment operations; the latter are only for declarations.
- **E — 012:** The value associated with this variable is too large! Acceptable values range from 0 to 255.
- **E — 013:** The label has been called but not declared. <br />
Each label must be defined with the syntax `[label:]`. <br />
Check for any spelling errors or issues related to the case-sensitivity of the assembler.
- **E — 014:** This label is defined multiple times within the code. <br />
A label can have only one definition, and two labels cannot share the same name.
- **E — 015:** You have declared too many input devices. <br />
Flip01 supports up to 32 different input devices.
- **E — 016:** You have declared too many output devices. <br />
Flip01 supports up to 32 different output devices.
- **E — 017:** The name assigned to this variable is a number. Variable names must contain at least one letter.
- **E — 018:** You have declared a device as input but then used it as output. <br />
Ensure that the declaration and usage are consistent _(> input)_.
- **E — 019:** You have declared a device as output but then used it as input. <br />
Ensure that the declaration and usage are consistent _(< output)_.
- **E — 020:** You are using a device that you have not declared. <br />
Ensure that the name matches the declaration and remember that Flip01 is case-sensitive.
- **E — 021:** The name assigned to this label is a number. <br />
Label names must contain at least one letter.
- **E — 022:** The name assigned to this device is a number. <br />
I/O device names must contain at least one letter.
- **E — 023:** You have not specified which device to use for this input or output operation.
- **E — 024:** The name you have chosen for this I/O device has already been used for another device.
- **E — 025:** The port number you have chosen for this device has already been assigned to another device. <br />
To avoid conflicts, use different ports for different devices.
- **E — 026:** The field that should contain the port number for the I/O device does not contain an integer. <br />
Please check the format.
- **E — 027:** The port number assigned to this device is not a valid integer. <br />
Remember that port numbers must be between 0 and 31.

> [!CAUTION]
> The assembler is only compatible with Windows-based operating systems

# MICRO PROGRAM COUNTER
The micro program counter is a section of Flip01 that, upon receiving the op-code of the currently active function, **sequentially produces all the control signals** that regulate the state of each component of the circuit and define the operation of the micro-instructions associated with that instruction. <br />

To achieve this, the following steps are executed in sequence:

1. The micro program counter **reads the first micro-instruction** from a third memory (MEM3) at the address corresponding to the op-code of the instruction.
2. If there is another micro-instruction linked to that instruction, it executes it; otherwise, the address returns to 0, executing the fetch instruction, which retrieves the next instruction to be executed. **This process is repeated until the described condition becomes true**.

![9](https://github.com/user-attachments/assets/3addefe3-ba99-456a-81e7-1aa8c4228804)
> The mechanism that outlines the sequential operation of Flip01’s micro program counter.

The memory accessed by the micro program counter (MEM3) is non-volatile, and **the set of control signal instructions is referred to as the ISA** (_Instruction Set Architecture_).

> [!TIP]
> One last note before wrapping up : if you enjoy these kinds of projects, feel free to follow us **FOR FREE** on [Patreon](https://www.patreon.com/c/PescettiStudio/posts)!
> You’ll get early updates and exclusive content, and even a “_simple_” follow really helps keep these projects free and open source.
> Thanks :)

# How to Use the Processor (Beginner’s Tutorial)
The first step is to open the file **[Flip01_Circuit.circ](Flip01_Circuit.circ)** with **[Logisim Evolution](https://github.com/logisim-evolution/logisim-evolution)**. <br />
At this point, the file containing the entire circuit should open, but for now, we'll set it aside since we first need to generate the code for the CPU to interpret. <br />

To do this, you will need to open the **[assembler](Flip01_Assembler)** and write the code, or open a previously prepared file for this purpose _(you can find examples in the [Code Examples](Code_Examples) folder)_. <br />
If the code is correct and contains no errors, the assembler will directly produce the code readable by the CPU. <br />

It should look like this: </br>
![5](https://github.com/user-attachments/assets/a290e868-8bca-40de-9aa2-5e11f9cc6cc9)
> Program successfully assembled: No errors or warnings detected.

At this point, simply copy the first line of hexadecimal values _(the one corresponding to the data)_ and paste it into the **RAM MEM1** component in the Logisim file. To do this, right-click on the component and select **`Edit Contents`**. <br />

You will need to repeat the same process with the second line of hexadecimal values _(the one corresponding to the instructions)_, pasting it into the **RAM MEM2** component in the Logisim file. Again, right-click on the component and select **`Edit Contents`**. <br />

Now everything is ready to start the simulation. In the main menu at the top, click on **`Simulate`** to open a dropdown menu. <br />
You can modify the "simulation speed" by changing the **clock frequency** in the **`Auto-Tick Frequency`** section _(a higher value corresponds to a greater execution speed)_. <br />

Once you have made this choice, in the same dropdown menu, just press **`Auto-Tick Enabled`**, or use the shortcut `Ctrl + K`to start the simulation. <br /> <br />

As mentioned at the beginning of the guide, all the referenced files and additional materials are available here on **GitHub** and on **[Patreon](https://www.patreon.com/PescettiStudio/posts)** for **free**. <br /><br />

Flip01 has been in development for a long time, perhaps too long for what it is, and yet it is still far from being a complete project. <br />
There are still many possible implementations to explore, which is why I’ve made it open source. <br />
Now Flip01 is in your hands; use your creativity, play with it, break it, and push beyond every limit we have designed. <br />
Have fun! :) <br /> <br />
 
Flip01 is a project by **Biasolo Riccardo** and **Croci Lorenzo**, developed for the **Pescetti Studio** collective. <br /><br />

If you come across any errors, inaccuracies, or typos, feel free to reach out to us! <br />
Send us an email at _**pescettistudio@gmail.com**_ with **[bug]** at the beginning of the subject line. <br />

# Updates

## Upgrade 1) FliPGA01
![Flipga01v2](https://github.com/user-attachments/assets/b26b4ea7-9e27-4e5a-aa8a-b5ccd1575914)
The basic version of Flip01 now also has an FPGA implementation! </br>
The project, called FlipGA01, is of course free and open source. </br>
You can find all the files here on [GitHub](https://github.com/pescetti-studio/FliPGA01), and there’s a detailed guide available on [Medium](https://medium.com/@crocilorenzo01/flipga01-a-simple-8-bit-cpu-on-a-fpga-db3e0fb82fe6).

## Upgrade 2) It’s Dangerous to Go Alone! Take This
![IDTGATT](https://github.com/user-attachments/assets/f199704e-40fe-4e39-a582-2b33414bb2bc)
This one’s all about the assembler, and it introduces three handy tools to make your life a little easier:
1. **Conversion Table** </br>
Now you can quickly switch between binary, decimal, and hexadecimal values using a dedicated button. </br>
The converter opens in a separate window, leaving the main assembler workspace untouched.

2. **View Manual** </br>
A new button lets you open the free Flip01 manual in your default browser. </br>
This free detailed guide covers not just how to program Flip01, but everything you need to know about the processor itself.

3. **Auto-Save Feature** </br>
This one's a bit behind-the-scenes: every change you make in the assembler is now automatically saved to a file named _log.txt_ in the same folder as the assembler. </br>
If the program crashes unexpectedly, you won’t lose your work! </br>
Simply click _Open File_ after restarting, select _log.txt_, and pick up right where you left off.

## Upgrade 3) Instruction Bonanza

We’ve added six new instructions

![bonanza](https://github.com/user-attachments/assets/ee728237-8559-4e89-9a90-73274d34ff0c)

- **test**: Performs an AND operation between the specified general-purpose register and the indicated variable, but doesn’t save the result.
- **testR**: Performs an AND operation between two registers, again without saving the result.
- **test$**: Performs an AND operation between the specified general-purpose register and the given value, but doesn’t save the result.
- **nop**: Does nothing for 1 clock cycle.
- **wait**: Does nothing for 3 clock cycles. Combined with the 2 fetch cycles, this creates a visible 5-cycle pause in execution.
- **copy**: Copies the value from the specified general-purpose register into another general-purpose register.
  
> [!NOTE]
> All the details on how to use these new instructions can be found in the dedicated section of this manual.

> [!IMPORTANT]
> The previously named **`pause`** instruction has been renamed to **`stop`** to avoid confusion.
> _All the sample programs here on GitHub and the various manuals online have already been updated._

## Upgrade 4) X Factor
We've expanded Flip01's capabilities by adding a module for multiplication and updating all the necessary logic structures accordingly.

![mul](https://github.com/user-attachments/assets/b20f7099-e1ad-4a9f-8fb8-4722eefda3b6)

To support this, we've introduced three new instructions specifically for handling multiplication operations:
- **mul:** Multiplies the value in the specified general-purpose register with the indicated variable. The result is saved in the original register.
- **mulR:** Multiplies the values in two general-purpose registers. The result is saved in the AX register.
- **mul$:** Multiplies the value in the specified general-purpose register with the given constant. The result is saved in the original register.

> [!NOTE]
> All the details on how to use these new instructions can be found in the dedicated section of this manual.

> [!IMPORTANT]
> The flag bit previously named X, which was used solely to check the validity of shift operations, has now been repurposed to also ensure that multiplication results stay within the 8-bit limit.

Do you like this stuff? </br>
Support us with a [donation on PayPal](https://paypal.me/PescettiStudio?country.x=IT&locale.x=it_IT)! </br>
It helps us keep everything free and open source for everyone. </br>
