#ifndef KERNEL_MM_H
#define KERNEL_MM_H

#include <kernel/types.h>

void mm_init(u64 mem_size);
void *kmalloc(usize_t size);
void *kcalloc(usize_t num, usize_t size);
void *krealloc(void *ptr, usize_t size);
void kfree(void *ptr);

#define PAGE_SIZE 4096

#endif
