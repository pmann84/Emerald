# Project Emerald

## Overview
An educational attempt to invent a programming language called Emerald. This repository will include everything related 
this. The main part of this is the Emerald compiler. This may expand to include tooling also.

## Language Overview
Support for features is slow as this is an educational project. You can see the language grammar in the [`docs` folder of this repo](docs/Grammar.md). Currently, Emerald supports the following programming constructs:
- Single and Multiline comments (using `#` and `#*...*#` respectively)
- Integer data types ONLY
- Variable assignment and reassignment (assignment using `let`)
- Basic mathematical expressions using `+`, `-`, `*`, `/`
- If/else-if/else statements (`if (...) {} else if (...) {} else {}`)

### Features in progress
- While loop (`while (...) {}`)
- Relational and equality expressions (`==`, `!=`, `<`, `>`, `<=`, `>=`)

### Up-and-Coming Features
- For loop
- Basic Functions
- Variable assignment in if/while expression (`if (let a = ...) {}` etc)
- External code
- and more...

### Sample
There is a full sample in the 

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

### Setup on Windows
I use CLion to develop this so far and have generally used the MinGW/MSVC toolchains. You need to make sure you have the following installed
- MSVC SDK 2019 (specifically 14.29.30133)
- CMake (if your IDE doesn't support it Out of the Box)
- Nasm (install from the website - once installed just add it to your path)

### Setup on Linux
I use CLion to develop this so far. To setup your Linux environment do the following:
- Setup your WSL instance for C++ development
  - C++ and tools `sudo apt-get install build-essential gdb`
  - CMake `sudo apt-get install cmake`
  - nasm `sudo apt install nasm`
- Update `gcc` and `g++` to a version that supports C++23 (I use gcc/++13), run the following (note the last command change the path `/home/linuxbrew/.linuxbrew/bin` to wherever your gcc/g++ symlinks are)
  - `sudo add-apt-repository ppa:ubuntu-toolchain-r/test`
  - `sudo apt-get install gcc-13 g++-13`
  - `sudo update-alternatives --install /home/linuxbrew/.linuxbrew/bin/gcc gcc /usr/bin/gcc-13 60 --slave /home/linuxbrew/.linuxbrew/bin/g++ g++ /usr/bin/g++-13`

> [!NOTE]
> You can repeat the following steps on Windows usin WSL, just install a WSL instance (I use Ubuntu but whatever you like). Then use your IDE to set up a WSL toolchain (CLion does this out of the box - although you may need to check it finds the correct compiler version in the toolchain settings).