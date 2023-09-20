        ;ARM64

        ;Flags[SingleProEpi] functionLength[24] RegF[0] RegI[0] H[0] frameChainReturn[UnChained] frameSize[16]

|main|  PROC
|$LN3|
        sub         sp,sp,#0x10
        mov         w8,#7
        str         w8,[sp]
        ldr         w0,[sp]
        add         sp,sp,#0x10
        ret

        ENDP  ; |main|