# Project Emerald

## Overview
An educational attempt to invent a programming language called Emerald. This repository will include everything related 
this. The main part of this is the Emerald compiler. This may expand to include tooling also.

## Language Overview

## Tech Stack & Dependencies
The compiler itself will start in C++ (in the far-flung future maybe we will build the compiler in Emerald but this 
is a far off dream for now), and will use various homebrew C++ libraries of my own for certain aspects. The main 
thing here is that we most of the compiler sections will be written from scratch not using any lexing or parsing 
frameworks. 

Currently, this supports linux only and uses `nasm` and `ld` to assemble and link the final programs. The compiler 
uses the CMake build system.

## Compiling a file
To compile a simple Emerald file, launch a WSL terminal and simply run
```bash
Emerald <path/to/emd file> -out test
```
here the argument passed to `-out` will be the name of the executable. You can then run your program with the command 
```bash
./test; echo $?
```
which will also print out the error code.

## Compiler Development
### Checkout
To build the compiler checkout the source code use
```shell
git clone --recursive https://github.com/pmann84/Emerald.git
```
to pull down the submodule dependencies. If you forget you can use
```shell
git clone https://github.com/pmann84/Emerald.git
cd Emerald
git submodule update --init --recursive
```
Then to configure and build use the provided CMake files in the root of the project.

### Setting up for Windows
I use CLion on Windows to develop this so far, but since I'm only supporting Linux you need to setup an appropriate Linux 
environment to develop on. Do the following
- Install a WSL instance (I use Ubuntu but whatever you like)
- Setup your WSL instance for C++ development
  - C++ and tools `sudo apt-get install build-essential gdb`
  - CMake `sudo apt-get install cmake`
  - nasm `sudo apt install nasm`
- Open CLion and create build configurations using the WSL toolchain (should auto-detect everything).