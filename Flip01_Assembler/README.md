**Here’s everything related to the assembler for Flip01.**

# THE ASSEMBLER
This program is responsible for **translating mnemonic instructions** (load, sub, add$, etc.) into their corresponding opcodes, while also converting numeric values and memory references into formats understandable by the processor.

![4](https://github.com/user-attachments/assets/6da3b320-6bfd-496b-ae5a-4adff5caa7a8)
> The assembler after startup

The code should be written in the panel that, on startup, displays the message `“write your text here”`.   
The conversion is automatically shown in the space below.     

**To disable the live machine code conversion**, simply press the corresponding button labeled Live analysis enabled.  
At that point, the button and interface will indicate that live translation has been paused.    
**To resume real-time conversion** or to view the corresponding translation, press the button again.   

To:

- **open a text file** (.txt) containing code you want to analyze
- **save the current file**
- **clear the content**
- **start a new file**

just click the corresponding buttons.   

After specifying the name or the path for the current file, it won’t be necessary to do so again, and all subsequent saves will be made to that file.   
This continues until the user initializes a new file via the New Code button.   

The assembler comes with a built-in decimal-hexadecimal-binary converter.   
To access it, just click the **Conversion Table** button, which opens the converter in a new window.   
Simply select the input format, enter the value in the text bar, and the conversions into all three formats will happen automatically.   
A control panel below the results **displays any input errors or format issues** in real time.  

![6](https://github.com/user-attachments/assets/2d74f8f1-4a2c-4a90-9410-63871ffaae67)
> The converter

> [!NOTE]
> Closing the converter window won’t affect the assembler, but closing the assembler will also close the converter window if it’s open.  

Another handy feature of the assembler is the **View Manual** button.   
As the name suggests, it opens the free Flip01 manual in your default browser.   
The manual is incredibly useful for programming, as it includes a full description of the processor, along with opcodes, details, and explanations of all supported CPU instructions.  

The manual is a visually enhanced and easier-to-read version of the information found in this README.   
It’s the same free manual available on Medium, which has been previously mentioned.  

Any changes made in the assembler’s text editor are automatically saved in a _log.txt_ file located in the same folder as the assembler.   
If the assembler crashes for any reason, your unsaved work won’t be lost.   
You can recover it by opening the _log.txt_ file directly from the assembler.   
Think of it as a real-time safety net, creating backup files on the fly to safeguard your progress in case something goes wrong.

**To add a comment to the code**, use the following syntax:
```
// Comment
```
Comments can only be added at the beginning of a new line.   

> [!NOTE]
>The assembler was developed in _C/C++_ using the _Dev C++ IDE_.

We hope that the UI and UX are quite intuitive, but it may be helpful to explicitly outline **all possible errors**, their **corresponding codes**, and **how to resolve them**:   

## Errors
- **E — 000:** The file you are trying to open and read using the designated button may be corrupted or unreachable.   
Please check that the file is intact and in the correct format (.txt).
- **E — 001:** The code you have written is too large for Flip01!   
Try to shorten it and clean it up by using calls to labels for repeated code segments.
- **E — 002:** The instruction you have written is not among those supported by Flip01.   
Double-check that the syntax is correct and remember that the assembler is case-sensitive: instructions must be written in lowercase (except for the R in immediate instructions).
- **E — 003:** The instruction does not have a declared variable as its second argument. This argument must not be a number.
- **E — 004:** The instruction is missing some required parameters.   
Refer to the previous chapter to check how many and which arguments are needed for each instruction.
- **E — 005:** This instruction requires a general-purpose register (AX or BX) as the first argument.   
The parameter written in the first position is likely incorrect or undefined.
- **E — 006:** The instruction has more parameters than required.   
Refer to the previous chapter to check how many and which arguments are needed for each instruction.
- **E — 007:** The argument associated with this direct instruction is too large!   
Remember that acceptable values range from 0 to 255.
- **E — 008:** The argument associated with the direct instruction is not an integer.   
It is likely that an alphanumeric value was entered or that this parameter has not been defined.
- **E — 009:** The variable has not been declared at the beginning of the program.   
Ensure that the variable name is spelled correctly and that the name written in the declaration matches the one used in the instruction.   
Remember that the assembler is case-sensitive, and the declaration must follow the structure:   
`[variable name] = [numeric value]`
- **E — 010:** The value associated with the variable is not an integer.   
An alphanumeric value may have been entered, or this parameter has not been defined.
- **E — 011:** This variable has been defined multiple times within the code.   
Remember that to change the value of a memory cell, you must use processor instructions and not assignment operations; the latter are only for declarations.
- **E — 012:** The value associated with this variable is too large! Acceptable values range from 0 to 255.
- **E — 013:** The label has been called but not declared.   
Each label must be defined with the syntax `[label:]`.   
Check for any spelling errors or issues related to the case-sensitivity of the assembler.
- **E — 014:** This label is defined multiple times within the code.   
A label can have only one definition, and two labels cannot share the same name.
- **E — 015:** You have declared too many input devices.   
Flip01 supports up to 32 different input devices.
- **E — 016:** You have declared too many output devices.   
Flip01 supports up to 32 different output devices.
- **E — 017:** The name assigned to this variable is a number. Variable names must contain at least one letter.
- **E — 018:** You have declared a device as input but then used it as output.   
Ensure that the declaration and usage are consistent _(> input)_.
- **E — 019:** You have declared a device as output but then used it as input.   
Ensure that the declaration and usage are consistent _(< output)_.
- **E — 020:** You are using a device that you have not declared.   
Ensure that the name matches the declaration and remember that Flip01 is case-sensitive.
- **E — 021:** The name assigned to this label is a number.   
Label names must contain at least one letter.
- **E — 022:** The name assigned to this device is a number.   
I/O device names must contain at least one letter.
- **E — 023:** You have not specified which device to use for this input or output operation.
- **E — 024:** The name you have chosen for this I/O device has already been used for another device.
- **E — 025:** The port number you have chosen for this device has already been assigned to another device.   
To avoid conflicts, use different ports for different devices.
- **E — 026:** The field that should contain the port number for the I/O device does not contain an integer.   
Please check the format.
- **E — 027:** The port number assigned to this device is not a valid integer.   
Remember that port numbers must be between 0 and 31.

> [!CAUTION]
> The assembler is only compatible with Windows-based operating systems


Windows might flag it as a virus or some kind of dangerous software because I haven’t yet figured out how to avoid that issue. </br>
If you don’t trust it _(and you’d be totally right, the internet is a weird place)_, you can recompile everything from the source code.

I developed it using **Dev C++** as the IDE, not **Visual Studio**, but there shouldn’t be any compatibility issues.

If you just want to use the assembler, click on the _Flip01_Assembler.exe_ file.
If you want to modify something, you probably know better than we do how and what to change.




For any issues, bugs, or anything else, write to us at _pescettistudio@gmail.com_.
</br>
</br>
</br>
**ASSEMBLER FOR THE FLIP01 CPU**
                                                                            
Copyright (C) 2024 Pescetti Studio (Biasolo Riccardo & Croci Lorenzo)    
                                                                           
This program is free software: you can redistribute it and/or modify     
it under the terms of the GNU General Public License as published by     
the Free Software Foundation, either version 3 of the License, or       
(at your option) any later version.                   
                                                                            
This program is distributed in the hope that it will be useful,       
but WITHOUT ANY WARRANTY; without even the implied warranty of        
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        
GNU General Public License for more details.                 
                                                                            
You should have received a copy of the GNU General Public License      
along with this program.  If not, see <https://www.gnu.org/licenses/>
                                                                         

