# Linkers

## Windows

### Link.exe

On Windows, Visual Studio ships with a linker, this is located usually in

```
C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.34.31933\bin\Hostx64\x64
```

and is the executable `link.exe`.

You can compile an assembly file using the following command (easiest way is to use a VS developer command prompt)

```powershell
link /MACHINE:X64 /SUBSYSTEM:CONSOLE /OUT:<filename>.exe /NODEFAULTLIB /ENTRY:main <filename>.o
```
