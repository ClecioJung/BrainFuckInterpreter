# BrainFuckInterpreter

## Overview

**BrainFuckInterpreter** is a simple BrainFuck language interpreter written in C. It posseses the following characteristics:

- Written in C99;
- It uses the library [arguments](https://github.com/ClecioJung/arguments) to parse command line arguments;
- The option `--Version` or `-v` displays the software version;
- The option `--debug` or `-d` activates the the debug mode, which allows the use of the commands `#` and `@`. In this version of BrainFuck, the instruction `#` shows the current cell and its value, while the instruction `@` shows all used cells and its values. When the debug mode is off this instructions are ignorated;
- The option `--language` or `-l` shows the language instructions;
- Use the option `--memory=%d` or `-m=%d` to specify the program buffer size to be used. The default value is 30000;

## BrainFuck

**Brainfuck** is an minimalistic esoteric programming language created in 1993 by Urban Müller (check out this article [article](https://en.wikipedia.org/wiki/Brainfuck) for more information). The language consists of only eight simple commands, a data pointer and an instruction pointer:

| Instruction | Description |
| ----------- | ----------- |
| `>`         | Increment the data pointer |
| `<`         | Decrement the data pointer |
| `+`         | Increment the byte at the data pointer |
| `-`         | Decrement the byte at the data pointer |
| `.`         | Output the byte at the data pointer |
| `,`         | Get one byte from the input and stores it at the data pointer |
| `[`         | If the byte at the data pointer is non zero, then loop the instructions inside the brackets `[ ]` |
| `]`         | End of loop |

Below we see an hello world program in this language:

```brainfuck
++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.
```

As you can see, this code is very difficult to understand, however, this language was not designed for practical use, but rather to inspire and provide challenges for programmers, such as the development of this interpreter =).

## Usage

Download this project and compile it by typing the command `make` in its folder. Next, just run the executable `BrainFuckInterpreter` passing as an argument the BrainFuck code file you wish to parse. Here is an example:

```
./BrainFuckInterpreter BrainFuck/helloWorld.b 
Hello World!
```