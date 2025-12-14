#ifndef KERNEL_FS_H
#define KERNEL_FS_H

#include <kernel/types.h>

int vfs_open(const char *pathname, int flags, mode_t mode);
ssize_t vfs_read(int fd, void *buf, usize_t count);
ssize_t vfs_write(int fd, const void *buf, usize_t count);
int vfs_close(int fd);

#endif
