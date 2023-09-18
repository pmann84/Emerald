# How to run the test
Compile and run with this (only on WSL or linux)
```wsl
nasm -felf64 test.asm && ld test.o -o test && ./test
```
Get the return code with
```wsl
echo $?
```
