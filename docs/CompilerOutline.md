# Compilation Overview

```mermaid
flowchart TD
    id1["Source Code"]
    id2([Compiler])
    id3[Assembly]
    id4([Assembler])
    id5("Object File")
    id6([Linker])
    id7(((External Libs)))

    classDef borderless stroke-width:0px
    class id1 borderless
    class id3 borderless
    class id5 borderless

    id1 --> id2 --> id3 --> id4 --> id5 --> id6
    id7 --> id6
```

## Assemblers

For the x86 architecture 2 examples of Assemblers are `NASM` (mostly for Linux) and `MASM` (Ms Macro Assembler for Windows). For windows you can see more details [here](https://learn.microsoft.com/en-us/cpp/assembler/masm/masm-for-x64-ml64-exe?view=msvc-170).
