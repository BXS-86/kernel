ENTRY(_start)

SECTIONS
{
    . = 0x100000;
    .multiboot_header : {
        *(.multiboot_header)
    }
    .text : ALIGN(4K) {
        *(.text)
    }
    .rodata : ALIGN(4K) {
        *(.rodata)
        *(.rodata.*)
    }
    .data : ALIGN(4K) {
        *(.data)
        *(.data.*)
    }
    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }
    /DISCARD/ : {
        *(.comment)
        *(.note.*)
    }
}
