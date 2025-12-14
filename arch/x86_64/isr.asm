bits 64
section .text

global default_isr
global isr_handler
global irq_handler

default_isr:
    iretq

isr_handler:
    iretq

irq_handler:
    iretq
