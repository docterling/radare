; main() {
;   int i = 0;
;   while (i<3) {
;    printf("hello\n");
;    i++;
;   }
; }
.function main
0x3bc  push ebp
0x3bd  mov ebp, esp
0x3bf  sub esp, 0x8
0x3c2  and esp, 0xf0
0x3c5  mov eax, 0x0
0x3ca  add eax, 0xf
0x3cd  add eax, 0xf
0x3d0  shr eax, 0x4
0x3d3  shl eax, 0x4
0x3d6  sub esp, eax
0x3d8  mov dword [ebp-0x4], 0x0
0x3df  cmp dword [ebp-0x4], 0x2
0x3e3  jg 0x3fc
0x3e5  sub esp, 0xc
0x3e8  push dword 0x80484c4
0x3ed  call 0x300
0x3f2  add esp, 0x10
0x3f5  lea eax, [ebp-0x4]
0x3f8  inc dword [eax]
0x3fa  jmp 0x3df
0x3fc  leave
0x3fd  ret
.symbol 0x300 printf
.string 0x80484c4 "hello\n"
