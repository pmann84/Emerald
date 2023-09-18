# Windows
nasm -fwin64 .\test.asm -o test.obj (--prefix \_)
link /MACHINE:X64 /SUBSYSTEM:CONSOLE /OUT:test.exe /NODEFAULTLIB /ENTRY:start test.obj (requires VS prompt to get link on the path)

# Linux
nasm -felf64 test.asm & ld test.o -o test