BITS 64

section .text
    global _start

_start:
    xor     rax, rax
    mov     al, 0x39 ; fork
    syscall
    test    eax, eax
    jnz     _parent
    mov     al, 0x1 ; write
    xor     rdi, rdi
    inc     rdi
    lea     rsi, [rel str]
    xor     rdx, rdx
    mov     edx, str.len
    syscall
    mov     al, 0x3c
    xor     rdi, rdi
    syscall

    str: db "I got the control", 10, 0
        .len equ $ - str

_parent:
    int3

