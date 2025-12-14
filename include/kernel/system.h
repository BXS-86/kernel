#ifndef KERNEL_SYSTEM_H
#define KERNEL_SYSTEM_H

#include <kernel/types.h>

void pic_remap(void);
void idt_init(void);
void timer_init(u32 frequency);
void timer_handler(void);
void keyboard_handler(void);

#endif
