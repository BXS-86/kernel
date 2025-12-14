bits 32
global _start
extern kmain

section .multiboot_header
align 4
multiboot_header:
    dd 0x1BADB002
    dd 0x00000003
    dd -(0x1BADB002 + 0x00000003)

section .text
_start:
    cli
    mov esp, stack_top
    push ebx
    push eax
    call check_cpuid
    call check_long_mode
    call setup_page_tables
    call enable_paging
    lgdt [gdt64.pointer]
    jmp gdt64.code:long_mode_start

check_cpuid:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 0x200000
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    xor eax, ecx
    jz no_cpuid
    ret

no_cpuid:
    hlt
    jmp no_cpuid

check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb no_long_mode
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz no_long_mode
    ret

no_long_mode:
    hlt
    jmp no_long_mode

setup_page_tables:
    mov eax, p3_table
    or eax, 0b11
    mov dword [p4_table], eax
    mov eax, p2_table
    or eax, 0b11
    mov dword [p3_table], eax
    mov ecx, 0
.map_p2_table:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011
    mov [p2_table + ecx * 8], eax
    inc ecx
    cmp ecx, 512
    jne .map_p2_table
    ret

enable_paging:
    mov eax, p4_table
    mov cr3, eax
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax
    ret

section .bss
align 4096
p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096
stack_bottom:
    resb 4096 * 4
stack_top:

section .rodata
gdt64:
    dq 0
.code: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

bits 64
long_mode_start:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov rsp, stack_top
    pop rdi
    pop rsi
    call kmain
.hang:
    cli
    hlt
    jmp .hang
