BITS 64

%define SYS_EXECVE  0x3b

section .text
    global _start

_start:
    xor     rax, rax
    mov     al, 0x39 ; fork
    syscall
    test    eax, eax
    jnz     _parent

    mov     rdi, 1
    lea     rsi, [rel confirm_text]
    mov     rdx, confirm_text.len
    mov     al, 1 ; write
    syscall

    lea     rdi, [rel filename]
    xor     rsi, rsi
    xor     rdx, rdx
    mov     eax, SYS_EXECVE
    syscall

    mov     rdi, 1
    lea     rsi, [rel confirm_text]
    mov     rdx, confirm_text.len
    mov     al, 1
    syscall

    confirm_text: db "Shellcode launched", 10, 0
    .len equ $ - confirm_text
    filename: TIMES 20 db 0
_parent:
    int3

