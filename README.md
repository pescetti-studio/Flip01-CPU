# Flip01-CPU
_A small &amp; simple 8-bit CPU developed in [Logisim Evolution](https://github.com/logisim-evolution/logisim-evolution)_.<br />
A longform covering everything about the processor is available for free on _[Medium](https://medium.com/@biasolo.riccardo/flip01-a-simple-yet-versatile-8-bit-cpu-fc01c36d5922)_.

![circuit](https://github.com/user-attachments/assets/ae60af9d-e89c-4f0f-9f27-20bd5aaf34da)

> [!NOTE]
> Both the assembler and the entire Flip01 project are still under development and will receive numerous updates and expansions in the coming months. Follow us on _[Patreon](https://www.patreon.com/PescettiStudio/posts)_ (it’s free, of course) to stay updated.

### Overview

**Flip01**, _short for First Level Instructional Processor_, is a small **8-bit CPU** with a **16-bit address bus**. <br />
This means it can handle data ranging from 0 to 2 <sup>8</sup> - 1 (255) and can store up to 2 <sup>16</sup> (65536 bit ~ 64 KB) different pieces of data. <br />
Data and addresses travel on two separate connection networks called the **data bus** and **address bus**, respectively. <br />

![datapath](https://github.com/user-attachments/assets/dc22cab1-69b5-413c-aa09-b63efe2d0078)

Flip01 is based on a **Harvard architecture**, meaning that data and instructions (the actions the processor must execute) are stored in two separate memories: **MEM1** for data and **MEM2** for instructions. <br />
MEM1 is an _8-bit memory_ with a capacity of 64 KB, while MEM2 is a _9-bit memory_ with the same capacity of 64 KB. <br />

## Registers

In Flip01, there are 10 main registers (+ 1), and their size and connection (data or address bus) are defined by their specific function. <br />

+ **AX**: general purpose register -> 8 bit 
+ **BX**: general purpose register -> 8 bit 
+ **SAX**: Status AX -> 8 bit 
+ **SBX**: Status BX -> 8 bit
+ **DR**: Data Register -> 16 bit
+ **IR**: Instruction Register -> 8 bit 
+ **ALUIN**: ALU input register -> 8 bit
+ **ALUOUT**: ALU output register -> 8 bit
+ **MAR**: Memory Address Register -> 16 bit
+ **PC**: Program Counter -> 16 bit
+ **(Flag)**: The Flags register -> 8 bit
  - _X_: Active if the number of bits to be shifted exceeds 8
  - _L_: Active if A < B
  - _E_: Active if A = B
  - _G_: Active if A > B
  - _V_: Active if an overflow occurs
  - _C_: Active if a carry bit has been used
  - _Z_: Active if the result = 0
  - _N_: Active if the result < 0

## ALU

The ALU of Flip01 is designed to take two 8-bit inputs and produce a single output of the same size. <br />
Based on the configuration and the combination of 6 binary control signals (0 or 1), the ALU can perform 7 main operations and 4 derived ones. <br />

### Main operations

+ **ADD**: ALUOUT = A + B
+ **SUB**: ALUOUT = A - B
+ **AND**: ALUOUT = A AND B
+ **OR**: ALUOUT = A OR B
+ **NOT**: ALUOUT = !A
+ **SHIFTL**: << A
+ **SHIFTR**: >> A

### Derived operations

+ **INC**: A + 1
+ **DEC**: A - 1
+ **0**: ALUOUT = 0
+ **A**: ALUOUT = A

## Instructions

Currently, the instructions available on Flip01 can be grouped into five categories: <br />

+ Direct addressing instructions
+ Single-operand instructions
+ Zero-operand instructions
+ Immediate instructions
+ Jump instructions

Instruction     |    op-code    |               Type              |
 :------------: | :-----------: |          :-----------:          |
**load**        |      0x02     | _Direct addressing instruction_ |
**store**       |      0x04     | _Direct addressing instruction_ |
**add**         |      0x06     | _Direct addressing instruction_ |
**sub**         |      0x09     | _Direct addressing instruction_ |
**and**         |      0x0C     | _Direct addressing instruction_ |
**or**          |      0x0F     | _Direct addressing instruction_ |
**cmp**         |      0x30     | _Direct addressing instruction_ |
**not**         |      0x12     |   _Single-operand instruction_  |
**neg**         |      0x15     |   _Single-operand instruction_  |
**inc**         |      0x24     |   _Single-operand instruction_  |
**dec**         |      0x27     |   _Single-operand instruction_  |
**rnd**         |      0x37     |   _Single-operand instruction_  |
**save**        |      0x5C     |   _Single-operand instruction_  |
**read**        |      0x5D     |   _Single-operand instruction_  |
**addR**        |      0x18     |    _Zero-operand instruction_   |
**subR**        |      0x1E     |    _Zero-operand instruction_   |
**andR**        |      0x21     |    _Zero-operand instruction_   |
**orR**         |      0x1B     |    _Zero-operand instruction_   |
**cmpR**        |      0x32     |    _Zero-operand instruction_   |
**flip**        |      0x34     |    _Zero-operand instruction_   |
**pause**       |      0x38     |    _Zero-operand instruction_   |
**load$**       |      0x59     |      _Immediate instruction_    |
**add$**        |      0x50     |      _Immediate instruction_    |
**sub$**        |      0x4D     |      _Immediate instruction_    |
**and$**        |      0x53     |      _Immediate instruction_    |
**or$**         |      0x56     |      _Immediate instruction_    |
**shiftl$**     |      0x2A     |      _Immediate instruction_    |
**shiftr$**     |      0x2D     |      _Immediate instruction_    |
**cmp$**        |      0x5A     |      _Immediate instruction_    |
**jmp**         |      0x4c     |         _Jump instruction_      |
**jc**          |      0x3A     |         _Jump instruction_      |
**jv**          |      0x3C     |         _Jump instruction_      |
**jn**          |      0x3E     |         _Jump instruction_      |
**jz**          |      0x40     |         _Jump instruction_      |
**je**          |      0x42     |         _Jump instruction_      |
**jg**          |      0x44     |         _Jump instruction_      |
**jl**          |      0x46     |         _Jump instruction_      |
**jle**         |      0x48     |         _Jump instruction_      |
**jge**         |      0x4A     |         _Jump instruction_      |

### I/O Manager

This circuit is responsible for reading from and writing to 64 unique I/O devices. <br />
Split into 32 input devices and 32 output devices, each port has an identifier ranging from 0 to 31. 

### I/O Instructions

These instructions handle the communication between external devices and the CPU. 

Instruction     |    op-code    |               Type              |
 :------------: | :-----------: |          :-----------:          |
**input**       |      0x5E     |         _I/O Instruction_       |
**output**      |      0x5F     |         _I/O Instruction_       |

> [!IMPORTANT]
> This is an excerpt from the manual available in the repository. <br />
For a more detailed overview of the Flip01 CPU, please refer to the [Flip01 manual](Flip01_manual.pdf).

# The assembler

This program is responsible for **translating mnemonic instructions** _(load, sub, add$, etc.)_ **into their corresponding opcodes**, while also converting numeric values and memory references into formats understandable by the processor. <br />

![assembler](https://github.com/user-attachments/assets/f566485a-f58b-4197-93c4-3c9407546eac)

The code should be written in the panel that, on startup, displays the message `write your text here` <br />
The conversion is automatically shown in the space below.<br />
**To disable the live machine code conversion**, simply press the corresponding button labeled `Live analysis enabled`. <br /> 
At that point, the button and interface will indicate that live translation has been paused. <br />
**To resume real-time conversion** or to view the corresponding translation, press the button again. <br /> 
<br />
To: 
+ **open a text file _(.txt)_ containing code you want to analyze**
+ **save the current file**
+ **clear the content and start a new file**
just click the corresponding buttons. <br />
After specifying the name or the path for the current file, it won’t be necessary to do so again, and all subsequent saves will be made to that file. This continues until the user initializes a new file via the `New Code` button. <br /> <br />

**To add a comment to the code**, use the following syntax:
```
 // Comment
```
Comments can only be added at the beginning of a new line.
<br />

For a detailed analysis of the possible **errors and warnings** generated by the assembler during the analysis of user-written code, please refer to the [Flip01 manual](Flip01_manual.pdf).

The assembler was developed in **C/C++** using the **Dev C++ IDE**.

<br />

> [!WARNING]
> The assembler is only compatible with Windows-based operating systems

# How to Use the Processor (Beginner's Tutorial)

The first step is to open the file **[Flip01_Circuit.circ](Flip01_Circuit.circ)** with **Logisim Evolution**. <br />
At this point, the file containing the entire circuit should open, but for now, we'll set it aside since we first need to generate the code for the CPU to interpret. <br />

To do this, you will need to open the [assembler](Flip01_Assembler) and write the code, or open a previously prepared file for this purpose _(you can find examples in the [Code Examples](Code_Examples) folder)_. 
<br /> If the code is correct and contains no errors, the assembler will directly produce the code readable by the CPU. <br />

It should look something like this:

![assembler2](https://github.com/user-attachments/assets/774da8e1-f1df-4821-8f86-33ee60bcf0b0)

At this point, simply copy the first line of hexadecimal values _(the one corresponding to the data)_ and paste it into the **RAM MEM1** component in the Logisim file. To do this, right-click on the component and select **`Edit Contents`**. <br />

You will need to repeat the same process with the second line of hexadecimal values _(the one corresponding to the instructions)_, pasting it into the **RAM MEM2** component in the Logisim file. Again, right-click on the component and select **`Edit Contents`**. <br />

Now everything is ready to start the simulation. In the main menu at the top, click on **`Simulate`** to open a dropdown menu. <br />
You can modify the "simulation speed" by changing the **clock frequency** in the **`Auto-Tick Frequency`** section _(a higher value corresponds to a greater execution speed)_. <br />

Once you have made this choice, in the same dropdown menu, just press **`Auto-Tick Enabled`**, or use the shortcut `Ctrl + K`to start the simulation. <br />
