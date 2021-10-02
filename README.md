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

## Example