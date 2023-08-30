# Assemblers

## Windows

### MASM/ml64

On Windows, Visual Studio ships with an assembler, this is located usually in

```
C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.34.31933\bin\Hostx64\x64
```

and is the executable `ml64.exe`.

You can compile an assembly file using the following command

```
ml64.exe <filename>.asm
```

### NASM

You can install NASM for windows and then compile to a .o using

```powershell
& 'C:\Program Files\NASM\nasm.exe' -fwin64 <filename>.asm -o <filename>.o
```
