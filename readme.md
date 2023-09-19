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

## Compiler Development
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
Then to configure and build use the provided CMake files in the root of the project. For Linux you can use whatever 
your CMake defaults are but on Windows you will need to install a WSL instance of Ubuntu or something and build it 
on that.