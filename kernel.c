#include <kernel/types.h>
#include <kernel/system.h>
#include <kernel/mm.h>
#include <kernel/fs.h>
#include <kernel/tty.h>
#include <asm/io.h>
#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

#define MAX_IRQ 256
#define MAX_DEVICES 256
#define MAX_FILESYSTEMS 32
#define MAX_MOUNTS 64
#define MAX_PROCESSES 256
#define MAX_FDS 1024
#define MAX_DRIVERS 128
#define HEAP_SIZE (128 * 1024 * 1024)
#define PAGE_SIZE 4096
#define KERNEL_STACK_SIZE 8192
#define USER_STACK_SIZE 32768

#define ENOSYS_VALUE       1
#define EPERM        2
#define ENOENT       3
#define ESRCH        4
#define EINTR        5
#define EIO          6
#define ENXIO        7
#define E2BIG        8
#define ENOEXEC      9
#define EBADF        10
#define ECHILD       11
#define EAGAIN       12
#define ENOMEM       13
#define EACCES       14
#define EFAULT       15
#define ENOTBLK      16
#define EBUSY        17
#define EEXIST       18
#define EXDEV       19
#define ENODEV      20
#define ENOTDIR     21
#define EISDIR      22
#define EINVAL      23
#define ENFILE      24
#define EMFILE      25
#define ENOTTY      26
#define ETXTBSY     27
#define EFBIG       28
#define ENOSPC      29
#define ESPIPE      30
#define EROFS       31
#define EMLINK      32
#define EPIPE       33
#define EDOM        34
#define ERANGE      35
#define EDEADLK     36
#define ENAMETOOLONG 37
#define ENOLCK      38
#define ENOSYS2     39
#define ENOTEMPTY   40
#define ELOOP       41
#define EWOULDBLOCK 42
#define ENOMSG      43
#define EIDRM       44
#define ECHRNG      45
#define EL2NSYNC    46
#define EL3HLT      47
#define EL3RST      48
#define ELNRNG      49
#define EUNATCH     50
#define ENOCSI      51
#define EL2HLT      52
#define EBADE       53
#define EBADR       54
#define EXFULL      55
#define ENOANO      56
#define EBADRQC     57
#define EBADSLT     58
#define EDEADLOCK   59
#define EBFONT      60
#define ENOSTR      61
#define ENODATA     62
#define ETIME       63
#define ENOSR       64
#define ENONET      65
#define ENOPKG      66
#define EREMOTE     67
#define ENOLINK     68
#define EADV        69
#define ESRMNT      70
#define ECOMM       71
#define EPROTO      72
#define EMULTIHOP   73
#define EDOTDOT     74
#define EBADMSG     75
#define EOVERFLOW   76
#define ENOTUNIQ    77
#define EBADFD      78
#define EREMCHG     79
#define ELIBACC     80
#define ELIBBAD     81
#define ELIBSCN     82
#define ELIBMAX     83
#define ELIBEXEC    84
#define EILSEQ      85
#define ERESTART    86
#define ESTRPIPE    87
#define EUSERS      88
#define ENOTSOCK    89
#define EDESTADDRREQ 90
#define EMSGSIZE    91
#define EPROTOTYPE  92
#define ENOPROTOOPT 93
#define EPROTONOSUPPORT 94
#define ESOCKTNOSUPPORT 95
#define EOPNOTSUPP  96
#define EPFNOSUPPORT 97
#define EAFNOSUPPORT 98
#define EADDRINUSE  99
#define EADDRNOTAVAIL 100
#define ENETDOWN    101
#define ENETUNREACH 102
#define ENETRESET   103
#define ECONNABORTED 104
#define ECONNRESET  105
#define ENOBUFS     106
#define EISCONN     107
#define ENOTCONN    108
#define ESHUTDOWN   109
#define ETOOMANYREFS 110
#define ETIMEDOUT   111
#define ECONNREFUSED 112
#define EHOSTDOWN   113
#define EHOSTUNREACH 114
#define EALREADY    115
#define EINPROGRESS 116
#define ESTALE      117
#define EUCLEAN     118
#define ENOTNAM     119
#define ENAVAIL     120
#define EISNAM      121
#define EREMOTEIO   122
#define EDQUOT      123
#define ENOMEDIUM   124
#define EMEDIUMTYPE 125
#define ECANCELED   126
#define ENOKEY      127
#define EKEYEXPIRED 128
#define EKEYREVOKED 129
#define EKEYREJECTED 130
#define EOWNERDEAD  131
#define ENOTRECOVERABLE 132
#define ERFKILL     133
#define EHWPOISON   134

#define O_RDONLY     00
#define O_WRONLY     01
#define O_RDWR       02
#define O_CREAT      0100
#define O_EXCL       0200
#define O_NOCTTY     0400
#define O_TRUNC      01000
#define O_APPEND     02000
#define O_NONBLOCK   04000
#define O_DSYNC      010000
#define FASYNC       020000
#define O_DIRECT     040000
#define O_LARGEFILE  0100000
#define O_DIRECTORY  0200000
#define O_NOFOLLOW   0400000
#define O_NOATIME    01000000
#define O_CLOEXEC    02000000
#define O_SYNC       04010000
#define O_PATH       010000000
#define O_TMPFILE    020000000

#define S_IFMT       0170000
#define S_IFSOCK     0140000
#define S_IFLNK      0120000
#define S_IFREG      0100000
#define S_IFBLK      0060000
#define S_IFDIR      0040000
#define S_IFCHR      0020000
#define S_IFIFO      0010000
#define S_ISUID      0004000
#define S_ISGID      0002000
#define S_ISVTX      0001000
#define S_IRWXU      00700
#define S_IRUSR      00400
#define S_IWUSR      00200
#define S_IXUSR      00100
#define S_IRWXG      00070
#define S_IRGRP      00040
#define S_IWGRP      00020
#define S_IXGRP      00010
#define S_IRWXO      00007
#define S_IROTH      00004
#define S_IWOTH      00002
#define S_IXOTH      00001

#define F_DUPFD      0
#define F_GETFD      1
#define F_SETFD      2
#define F_GETFL      3
#define F_SETFL      4
#define F_GETLK      5
#define F_SETLK      6
#define F_SETLKW     7
#define F_GETOWN     8
#define F_SETOWN     9
#define F_SETSIG     10
#define F_GETSIG     11

#define SEEK_SET     0
#define SEEK_CUR     1
#define SEEK_END     2

#define DT_UNKNOWN   0
#define DT_FIFO      1
#define DT_CHR       2
#define DT_DIR       4
#define DT_BLK       6
#define DT_REG       8
#define DT_LNK       10
#define DT_SOCK      12
#define DT_WHT       14

typedef u64 time_t;
typedef u64 suseconds_t;

struct timeval {
    time_t tv_sec;
    suseconds_t tv_usec;
};

struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
    char domainname[65];
};

typedef u64 dev_t;
typedef u64 ino_t;
typedef u32 nlink_t;
typedef u32 uid_t;
typedef u32 gid_t;
typedef u32 blksize_t;
typedef u64 blkcnt_t;
typedef u64 off_t;
typedef u32 mode_t;

struct stat {
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    off_t st_size;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
    blksize_t st_blksize;
    blkcnt_t st_blocks;
};

struct dirent {
    ino_t d_ino;
    off_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
};

typedef struct {
    u64 r15, r14, r13, r12, r11, r10, r9, r8;
    u64 rdi, rsi, rbp, rbx, rdx, rcx, rax;
    u64 interrupt_number, error_code;
    u64 rip, cs, rflags, rsp, ss;
} __attribute__((packed)) interrupt_frame;

typedef struct {
    u32 magic;
    u32 version;
    u64 entry;
    u64 phoff;
    u64 shoff;
    u64 flags;
    u16 ehsize;
    u16 phentsize;
    u16 phnum;
    u16 shentsize;
    u16 shnum;
    u16 shstrndx;
} elf_header;

typedef struct {
    u32 type;
    u32 flags;
    u64 offset;
    u64 vaddr;
    u64 paddr;
    u64 filesz;
    u64 memsz;
    u64 align;
} elf_pheader;

typedef struct process {
    u64 pid;
    u64 ppid;
    u64 state;
    u64 priority;
    u64 rsp;
    u64 cr3;
    u64 entry;
    u64 heap;
    u64 stack;
    u64 kstack;
    char name[32];
    struct process *next;
    struct process *prev;
    u64 signals;
    u64 exit_code;
    u64 brk;
    u64 mmap;
    u64 files[MAX_FDS];
    u64 cwd;
} process_t;

typedef struct {
    u64 start;
    u64 end;
    u64 size;
    u8 type;
    u8 used;
    u64 next;
} memory_block;

typedef struct {
    char name[32];
    u32 type;
    u64 ops;
    u64 data;
    u8 mounted;
    u64 root_inode;
} filesystem_t;

typedef struct file_t file_t;

typedef struct inode_ops {
    u64 (*read)(file_t*, void*, u64, u64);
    u64 (*write)(file_t*, const void*, u64, u64);
    void (*open)(file_t*, u32);
    void (*close)(file_t*);
    u64 (*ioctl)(file_t*, u64, u64);
} inode_ops_t;

typedef struct inode {
    u64 inode;
    u64 size;
    u32 mode;
    u32 uid;
    u32 gid;
    u64 atime;
    u64 mtime;
    u64 ctime;
    u64 blocks;
    u64 links;
    u64 fs_data;
    inode_ops_t *ops;
    char name[256];
} inode_t;

struct file_t {
    inode_t *inode;
    u64 pos;
    u32 flags;
    u32 mode;
    u64 fs_data;
};

typedef struct {
    char name[256];
    u64 inode;
    u32 type;
    u64 next;
} dirent_t;

typedef struct {
    void (*open)(u64, u32);
    void (*close)(u64);
    u64 (*read)(u64, void*, u64);
    u64 (*write)(u64, const void*, u64);
    u64 (*ioctl)(u64, u64, u64);
    void (*mmap)(u64, u64);
} device_ops;

typedef struct {
    u32 major;
    u32 minor;
    char name[32];
    device_ops ops;
    u64 data;
} device_t;

typedef struct {
    void (*mount)(const char*, u64, const char*);
    void (*umount)(u64);
    inode_t* (*lookup)(u64, const char*);
    u64 (*read)(file_t*, void*, u64, u64);
    u64 (*write)(file_t*, const void*, u64, u64);
    void (*create)(u64, const char*, u32);
    void (*unlink)(u64, const char*);
    dirent_t* (*readdir)(u64, u64);
} fs_ops;

typedef struct page_table_entry {
    u64 present : 1;
    u64 writable : 1;
    u64 user : 1;
    u64 write_through : 1;
    u64 cache_disable : 1;
    u64 accessed : 1;
    u64 dirty : 1;
    u64 huge_page : 1;
    u64 global : 1;
    u64 available1 : 3;
    u64 address : 40;
    u64 available2 : 11;
    u64 no_execute : 1;
} page_table_entry;

typedef struct page_table {
    page_table_entry entries[512];
} page_table;

#define PML4_INDEX(vaddr) (((vaddr) >> 39) & 0x1FF)
#define PDP_INDEX(vaddr) (((vaddr) >> 30) & 0x1FF)
#define PD_INDEX(vaddr) (((vaddr) >> 21) & 0x1FF)
#define PT_INDEX(vaddr) (((vaddr) >> 12) & 0x1FF)

static volatile u16 *vga_buffer = (volatile u16*)VGA_MEMORY;
static int cursor_x = 0;
static int cursor_y = 0;

static process_t *process_list = NULL;
static process_t *current_process = NULL;
static u64 next_pid = 1;
static memory_block *memory_map = NULL;
static u64 memory_map_size = 0;
static filesystem_t filesystems[MAX_FILESYSTEMS];
static u32 num_filesystems = 0;
static device_t devices[MAX_DEVICES];
static u32 num_devices = 0;
static file_t *open_files[MAX_FDS];
static u64 heap_base = 0x1000000;
static u64 timer_freq = 1000;
static u64 uptime = 0;
static u8 key_state[128];
static u8 shift_pressed = 0;
static u8 ctrl_pressed = 0;
static u8 alt_pressed = 0;
static u64 pci_devices[256];
static u32 num_pci = 0;
static u64 usb_devices[64];
static u32 num_usb = 0;
static u64 network_devices[16];
static u32 num_net = 0;
static u64 audio_devices[8];
static u32 num_audio = 0;
static u64 storage_devices[32];
static u32 num_storage = 0;
static fs_ops* mounted_fs[MAX_MOUNTS];
static u64 mount_points[MAX_MOUNTS];
static char mount_paths[MAX_MOUNTS][256];
static u32 num_mounts = 0;
static page_table *kernel_pml4 = NULL;
static page_table *free_page_list = NULL;
static u64 next_inode = 1;

static void vga_update_cursor(void) {
    u16 pos = cursor_y * VGA_WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (u8)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (u8)((pos >> 8) & 0xFF));
}

static void vga_putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 8) & ~7;
    } else if (c == 8) {
        if (cursor_x > 0) {
            cursor_x--;
            vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = ' ' | (0x07 << 8);
        }
    } else {
        vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = (0x07 << 8) | c;
        cursor_x++;
    }
    
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= VGA_HEIGHT) {
        for (int i = 0; i < (VGA_HEIGHT-1) * VGA_WIDTH; i++) {
            vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
        }
        for (int i = (VGA_HEIGHT-1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            vga_buffer[i] = ' ' | (0x07 << 8);
        }
        cursor_y = VGA_HEIGHT - 1;
    }
    
    vga_update_cursor();
}

static void vga_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = ' ' | (0x07 << 8);
    }
    cursor_x = cursor_y = 0;
    vga_update_cursor();
}

void kprintf(const char *fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);
    
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 's': {
                    char *str = __builtin_va_arg(args, char*);
                    while (*str) vga_putchar(*str++);
                    break;
                }
                case 'd': {
                    int num = __builtin_va_arg(args, int);
                    char buf[32];
                    char *ptr = buf;
                    
                    if (num < 0) {
                        vga_putchar('-');
                        num = -num;
                    }
                    
                    do {
                        *ptr++ = '0' + (num % 10);
                        num /= 10;
                    } while (num);
                    
                    while (ptr > buf) vga_putchar(*--ptr);
                    break;
                }
                case 'x': {
                    unsigned long num = __builtin_va_arg(args, unsigned long);
                    char hex[] = "0123456789ABCDEF";
                    for (int i = 60; i >= 0; i -= 4) {
                        vga_putchar(hex[(num >> i) & 0xF]);
                    }
                    break;
                }
                case 'p': {
                    void *ptr = __builtin_va_arg(args, void*);
                    kprintf("0x%x", (unsigned long)ptr);
                    break;
                }
                default:
                    vga_putchar(*fmt);
            }
        } else {
            vga_putchar(*fmt);
        }
        fmt++;
    }
    
    __builtin_va_end(args);
}

static void *kernel_memset(void *ptr, int value, usize_t num) {
    unsigned char *p = (unsigned char*)ptr;
    while (num--) *p++ = value;
    return ptr;
}

static void *kernel_memcpy(void *dest, const void *src, usize_t num) {
    unsigned char *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;
    while (num--) *d++ = *s++;
    return dest;
}

static usize_t kernel_strlen(const char *str) {
    usize_t len = 0;
    while (str[len]) len++;
    return len;
}

static char *kernel_strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

static int kernel_strcmp(const char *str1, const char *str2) {
    while (*str1 && *str1 == *str2) {
        str1++; str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

static int kernel_strncmp(const char *str1, const char *str2, usize_t n) {
    while (n-- && *str1 && *str2) {
        if (*str1 != *str2) return *(unsigned char*)str1 - *(unsigned char*)str2;
        str1++; str2++;
    }
    if (n == (usize_t)-1) return 0;
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

static char *kernel_strncpy(char *dest, const char *src, usize_t n) {
    usize_t i;
    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = '\0';
    return dest;
}

static char *kernel_strstr(const char *haystack, const char *needle) {
    if (!*needle) return (char*)haystack;
    
    for (; *haystack; haystack++) {
        const char *h = haystack;
        const char *n = needle;
        
        while (*h && *n && *h == *n) {
            h++;
            n++;
        }
        
        if (!*n) return (char*)haystack;
    }
    
    return NULL;
}

static page_table *alloc_page_table(void) {
    if (free_page_list) {
        page_table *table = free_page_list;
        free_page_list = (page_table*)(*(u64*)free_page_list);
        kernel_memset(table, 0, PAGE_SIZE);
        return table;
    }
    
    for (u64 i = 0; i < memory_map_size; i++) {
        if (!memory_map[i].used && memory_map[i].size >= PAGE_SIZE) {
            memory_map[i].used = 1;
            page_table *table = (page_table*)(memory_map[i].start);
            kernel_memset(table, 0, PAGE_SIZE);
            memory_map[i].start += PAGE_SIZE;
            memory_map[i].size -= PAGE_SIZE;
            return table;
        }
    }
    return NULL;
}

static void free_page_table(page_table *table) {
    *(u64*)table = (u64)free_page_list;
    free_page_list = table;
}

static void map_page(u64 vaddr, u64 paddr, u64 flags) {
    page_table *pml4 = kernel_pml4;
    page_table_entry *pml4e = &pml4->entries[PML4_INDEX(vaddr)];
    
    if (!pml4e->present) {
        page_table *pdp = alloc_page_table();
        if (!pdp) return;
        kernel_memset(pml4e, 0, sizeof(page_table_entry));
        pml4e->present = 1;
        pml4e->writable = 1;
        pml4e->user = (flags & 1) ? 1 : 0;
        pml4e->address = ((u64)pdp >> 12);
    }
    
    page_table *pdp = (page_table*)((u64)(pml4e->address << 12));
    page_table_entry *pdpe = &pdp->entries[PDP_INDEX(vaddr)];
    
    if (!pdpe->present) {
        page_table *pd = alloc_page_table();
        if (!pd) return;
        kernel_memset(pdpe, 0, sizeof(page_table_entry));
        pdpe->present = 1;
        pdpe->writable = 1;
        pdpe->user = (flags & 1) ? 1 : 0;
        pdpe->address = ((u64)pd >> 12);
    }
    
    page_table *pd = (page_table*)((u64)(pdpe->address << 12));
    page_table_entry *pde = &pd->entries[PD_INDEX(vaddr)];
    
    if (!pde->present) {
        page_table *pt = alloc_page_table();
        if (!pt) return;
        kernel_memset(pde, 0, sizeof(page_table_entry));
        pde->present = 1;
        pde->writable = 1;
        pde->user = (flags & 1) ? 1 : 0;
        pde->address = ((u64)pt >> 12);
    }
    
    page_table *pt = (page_table*)((u64)(pde->address << 12));
    page_table_entry *pte = &pt->entries[PT_INDEX(vaddr)];
    
    kernel_memset(pte, 0, sizeof(page_table_entry));
    pte->present = 1;
    pte->writable = (flags & 2) ? 1 : 0;
    pte->user = (flags & 1) ? 1 : 0;
    pte->no_execute = (flags & 4) ? 1 : 0;
    pte->address = (paddr >> 12);
    
    asm volatile("invlpg (%0)" : : "r" (vaddr) : "memory");
}

static void unmap_page(u64 vaddr) {
    page_table *pml4 = kernel_pml4;
    page_table_entry *pml4e = &pml4->entries[PML4_INDEX(vaddr)];
    
    if (!pml4e->present) return;
    
    page_table *pdp = (page_table*)((u64)(pml4e->address << 12));
    page_table_entry *pdpe = &pdp->entries[PDP_INDEX(vaddr)];
    
    if (!pdpe->present) return;
    
    page_table *pd = (page_table*)((u64)(pdpe->address << 12));
    page_table_entry *pde = &pd->entries[PD_INDEX(vaddr)];
    
    if (!pde->present) return;
    
    page_table *pt = (page_table*)((u64)(pde->address << 12));
    page_table_entry *pte = &pt->entries[PT_INDEX(vaddr)];
    
    kernel_memset(pte, 0, sizeof(page_table_entry));
    
    asm volatile("invlpg (%0)" : : "r" (vaddr) : "memory");
}

static page_table *create_address_space(void) {
    page_table *pml4 = alloc_page_table();
    if (!pml4) return NULL;
    
    kernel_memcpy(pml4, kernel_pml4, PAGE_SIZE);
    
    return pml4;
}

static void destroy_address_space(page_table *pml4) {
    for (u64 i = 256; i < 512; i++) {
        if (pml4->entries[i].present) {
            page_table *pdp = (page_table*)((u64)(pml4->entries[i].address << 12));
            for (u64 j = 0; j < 512; j++) {
                if (pdp->entries[j].present) {
                    page_table *pd = (page_table*)((u64)(pdp->entries[j].address << 12));
                    for (u64 k = 0; k < 512; k++) {
                        if (pd->entries[k].present) {
                            page_table *pt = (page_table*)((u64)(pd->entries[k].address << 12));
                            free_page_table(pt);
                        }
                    }
                    free_page_table(pd);
                }
            }
            free_page_table(pdp);
        }
    }
    free_page_table(pml4);
}

static u64 virt_to_phys(u64 vaddr) {
    page_table *pml4 = kernel_pml4;
    page_table_entry *pml4e = &pml4->entries[PML4_INDEX(vaddr)];
    
    if (!pml4e->present) return 0;
    
    page_table *pdp = (page_table*)((u64)(pml4e->address << 12));
    page_table_entry *pdpe = &pdp->entries[PDP_INDEX(vaddr)];
    
    if (!pdpe->present) return 0;
    
    page_table *pd = (page_table*)((u64)(pdpe->address << 12));
    page_table_entry *pde = &pd->entries[PD_INDEX(vaddr)];
    
    if (!pde->present) return 0;
    
    page_table *pt = (page_table*)((u64)(pde->address << 12));
    page_table_entry *pte = &pt->entries[PT_INDEX(vaddr)];
    
    if (!pte->present) return 0;
    
    return (pte->address << 12) | (vaddr & 0xFFF);
}

static void vmm_init(void) {
    kernel_pml4 = alloc_page_table();
    
    for (u64 i = 0; i < 0x100000000; i += PAGE_SIZE) {
        map_page(i, i, 3);
    }
    
    u64 kernel_start = 0xFFFFFFFF80000000;
    for (u64 i = 0; i < 0x40000000; i += PAGE_SIZE) {
        map_page(kernel_start + i, i, 3);
    }
    
    asm volatile("mov %0, %%cr3" : : "r" (kernel_pml4));
}

void pic_remap(void) {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}

void idt_init(void) {
}

static volatile u64 system_ticks = 0;

void timer_handler(void) {
    system_ticks++;
    uptime++;
}

void timer_init(u32 frequency) {
    timer_freq = frequency;
    u32 divisor = 1193180 / frequency;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

static u32 pci_read(u32 bus, u32 slot, u32 func, u32 offset) {
    u32 address = (1 << 31) | (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC);
    outl(0xCF8, address);
    return inl(0xCFC);
}

static void pci_scan(void) {
    for (u32 bus = 0; bus < 256; bus++) {
        for (u32 slot = 0; slot < 32; slot++) {
            for (u32 func = 0; func < 8; func++) {
                u32 vendor = pci_read(bus, slot, func, 0) & 0xFFFF;
                if (vendor != 0xFFFF) {
                    pci_devices[num_pci++] = (bus << 16) | (slot << 8) | func;
                    if (num_pci >= 256) return;
                }
            }
        }
    }
}

static void usb_init(void) {
    for (u32 i = 0; i < num_pci; i++) {
        u32 dev = pci_devices[i];
        u32 bus = (dev >> 16) & 0xFF;
        u32 slot = (dev >> 8) & 0xFF;
        u32 func = dev & 0xFF;
        u32 class = pci_read(bus, slot, func, 0x08) >> 24;
        if (class == 0x0C) {
            usb_devices[num_usb++] = dev;
        }
    }
}

static void network_init(void) {
    for (u32 i = 0; i < num_pci; i++) {
        u32 dev = pci_devices[i];
        u32 bus = (dev >> 16) & 0xFF;
        u32 slot = (dev >> 8) & 0xFF;
        u32 func = dev & 0xFF;
        u32 class = pci_read(bus, slot, func, 0x08) >> 24;
        if (class == 0x02) {
            network_devices[num_net++] = dev;
        }
    }
}

static void audio_init(void) {
    for (u32 i = 0; i < num_pci; i++) {
        u32 dev = pci_devices[i];
        u32 bus = (dev >> 16) & 0xFF;
        u32 slot = (dev >> 8) & 0xFF;
        u32 func = dev & 0xFF;
        u32 class = pci_read(bus, slot, func, 0x08) >> 24;
        if (class == 0x04) {
            audio_devices[num_audio++] = dev;
        }
    }
}

static void storage_init(void) {
    for (u32 i = 0; i < num_pci; i++) {
        u32 dev = pci_devices[i];
        u32 bus = (dev >> 16) & 0xFF;
        u32 slot = (dev >> 8) & 0xFF;
        u32 func = dev & 0xFF;
        u32 class = pci_read(bus, slot, func, 0x08) >> 24;
        if (class == 0x01 || class == 0x05) {
            storage_devices[num_storage++] = dev;
        }
    }
}

#define KERNEL_HEAP_SIZE (64 * 1024 * 1024)

static void* physical_alloc(u64 pages) {
    for (u64 i = 0; i < memory_map_size; i++) {
        if (!memory_map[i].used && memory_map[i].size >= pages * PAGE_SIZE) {
            memory_map[i].used = 1;
            void* addr = (void*)memory_map[i].start;
            memory_map[i].start += pages * PAGE_SIZE;
            memory_map[i].size -= pages * PAGE_SIZE;
            return addr;
        }
    }
    return NULL;
}

static void physical_free(void* addr, u64 pages) {
    (void)pages;
    for (u64 i = 0; i < memory_map_size; i++) {
        if ((u64)addr >= memory_map[i].start && (u64)addr < memory_map[i].end) {
            memory_map[i].used = 0;
            return;
        }
    }
}

void mm_init(u64 mem_size) {
    memory_map_size = mem_size / PAGE_SIZE / 64;
    memory_map = (memory_block*)0x100000;
    
    memory_map[0].start = 0x200000;
    memory_map[0].end = mem_size;
    memory_map[0].size = mem_size - 0x200000;
    memory_map[0].type = 1;
    memory_map[0].used = 0;
    memory_map[0].next = 0;
    
    vmm_init();
    
    kprintf("Memory: %d MB total\n", (int)(mem_size / 1024 / 1024));
    kprintf("VMM: 64-bit paging initialized\n");
}

void *kmalloc(usize_t size) {
    static u8 kernel_heap[KERNEL_HEAP_SIZE];
    static usize_t heap_ptr = 0;
    
    if (heap_ptr + size > KERNEL_HEAP_SIZE) {
        return NULL;
    }
    
    void *ptr = &kernel_heap[heap_ptr];
    heap_ptr += size;
    
    return ptr;
}

void *kcalloc(usize_t num, usize_t size) {
    void *ptr = kmalloc(num * size);
    if (ptr) kernel_memset(ptr, 0, num * size);
    return ptr;
}

void *krealloc(void *ptr, usize_t size) {
    (void)ptr;
    return kmalloc(size);
}

void kfree(void *ptr) {
    (void)ptr;
}

static void process_add(process_t *proc) {
    if (!process_list) {
        process_list = proc;
        proc->next = proc;
        proc->prev = proc;
    } else {
        proc->next = process_list;
        proc->prev = process_list->prev;
        process_list->prev->next = proc;
        process_list->prev = proc;
    }
}

u64 sys_fork(void) {
    process_t *child = (process_t*)kmalloc(sizeof(process_t));
    if (!child) return -ENOMEM;
    
    kernel_memcpy(child, current_process, sizeof(process_t));
    child->pid = next_pid++;
    child->ppid = current_process->pid;
    child->state = 0;
    
    child->cr3 = (u64)create_address_space();
    if (!child->cr3) {
        kfree(child);
        return -ENOMEM;
    }
    
    child->kstack = (u64)kmalloc(KERNEL_STACK_SIZE);
    child->stack = (u64)kmalloc(USER_STACK_SIZE);
    
    process_add(child);
    return child->pid;
}

void sched_init(void) {
    current_process = (process_t*)kmalloc(sizeof(process_t));
    kernel_memset(current_process, 0, sizeof(process_t));
    current_process->pid = next_pid++;
    current_process->state = 1;
    current_process->cr3 = (u64)kernel_pml4;
    kernel_strcpy(current_process->name, "kernel");
    process_add(current_process);
}

u64 fork(void) {
    return sys_fork();
}

void exit(int code) {
    current_process->exit_code = code;
    current_process->state = 2;
    if (current_process->cr3 != (u64)kernel_pml4) {
        destroy_address_space((page_table*)current_process->cr3);
    }
}

void schedule(void) {
    if (!process_list) return;
    
    process_t *next = current_process->next;
    while (next != current_process) {
        if (next->state == 0) {
            current_process = next;
            asm volatile("mov %0, %%cr3" : : "r" (current_process->cr3));
            return;
        }
        next = next->next;
    }
}

void yield(void) {
    schedule();
}

static u64 syscall_table[512];

static u64 sys_unimplemented(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg1; (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    return -ENOSYS_VALUE;
}

static u64 sys_read(u64 fd, u64 buf, u64 count, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg4; (void)arg5; (void)arg6;
    if (fd >= MAX_FDS) return -EBADF;
    file_t *file = open_files[fd];
    if (!file) return -EBADF;
    
    if (file->inode && file->inode->ops && file->inode->ops->read) {
        return file->inode->ops->read(file, (void*)buf, count, file->pos);
    } else {
        char *cbuf = (char*)buf;
        for (u64 i = 0; i < count; i++) {
            int c = tty_getchar();
            if (c == -1) return i;
            cbuf[i] = c;
        }
        return count;
    }
}

static u64 sys_write(u64 fd, u64 buf, u64 count, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg4; (void)arg5; (void)arg6;
    if (fd >= MAX_FDS) return -EBADF;
    file_t *file = open_files[fd];
    if (!file) return -EBADF;
    
    if (file->inode && file->inode->ops && file->inode->ops->write) {
        return file->inode->ops->write(file, (void*)buf, count, file->pos);
    } else {
        const char *cbuf = (const char*)buf;
        for (u64 i = 0; i < count; i++) {
            tty_putchar(cbuf[i]);
        }
        return count;
    }
}

static u64 sys_open(u64 pathname, u64 flags, u64 mode, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg4; (void)arg5; (void)arg6;
    for (int i = 0; i < MAX_FDS; i++) {
        if (!open_files[i]) {
            file_t *file = (file_t*)kmalloc(sizeof(file_t));
            if (!file) return -ENOMEM;
            kernel_memset(file, 0, sizeof(file_t));
            
            file->flags = flags;
            file->mode = mode;
            open_files[i] = file;
            
            for (u32 j = 0; j < num_mounts; j++) {
                if (kernel_strncmp((char*)pathname, mount_paths[j], kernel_strlen(mount_paths[j])) == 0) {
                    if (mounted_fs[j]->lookup) {
                        file->inode = mounted_fs[j]->lookup(mount_points[j], (char*)pathname + kernel_strlen(mount_paths[j]));
                    }
                    break;
                }
            }
            
            return i;
        }
    }
    return -EMFILE;
}

static u64 sys_close(u64 fd, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    if (fd >= MAX_FDS) return -EBADF;
    file_t *file = open_files[fd];
    if (!file) return -EBADF;
    
    if (file->inode && file->inode->ops && file->inode->ops->close) {
        file->inode->ops->close(file);
    }
    
    kfree(file);
    open_files[fd] = NULL;
    return 0;
}

static u64 sys_execve(u64 pathname, u64 argv, u64 envp, u64 arg4, u64 arg5, u64 arg6) {
    (void)envp; (void)arg4; (void)arg5; (void)arg6;
    elf_header *header = (elf_header*)pathname;
    if (header->magic != 0x464C457F) return -ENOEXEC;
    
    for (u16 i = 0; i < header->phnum; i++) {
        elf_pheader *phdr = (elf_pheader*)((u8*)header + header->phoff + i * header->phentsize);
        if (phdr->type == 1) {
            u64 addr = phdr->vaddr;
            u64 offset = phdr->offset;
            void *dest = (void*)addr;
            kernel_memcpy(dest, (u8*)header + offset, phdr->filesz);
            if (phdr->memsz > phdr->filesz) {
                kernel_memset((u8*)dest + phdr->filesz, 0, phdr->memsz - phdr->filesz);
            }
        }
    }
    
    current_process->entry = header->entry;
    current_process->rsp = (u64)kmalloc(USER_STACK_SIZE) + USER_STACK_SIZE;
    
    u64 *stack = (u64*)current_process->rsp;
    
    int argc = 0;
    char **args = (char**)argv;
    while (args[argc]) argc++;
    
    for (int i = argc - 1; i >= 0; i--) {
        *--stack = (u64)args[i];
    }
    
    *--stack = argc;
    current_process->rsp = (u64)stack;
    
    return header->entry;
}

static u64 sys_getpid(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg1; (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    return current_process->pid;
}

static u64 sys_getppid(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg1; (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    return current_process->ppid;
}

static u64 sys_sched_yield(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg1; (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    yield();
    return 0;
}

static u64 sys_brk(u64 addr, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    if ((u64)addr > heap_base + HEAP_SIZE) return current_process->brk;
    current_process->brk = (u64)addr;
    return current_process->brk;
}

static u64 sys_mmap(u64 addr, u64 length, u64 prot, u64 flags, u64 fd, u64 offset) {
    (void)prot; (void)flags; (void)fd; (void)offset;
    u64 start = (u64)addr;
    if (start == 0) {
        start = current_process->mmap;
        current_process->mmap += length;
    }
    
    u64 pages = (length + PAGE_SIZE - 1) / PAGE_SIZE;
    for (u64 i = 0; i < pages; i++) {
        void *phys = physical_alloc(1);
        if (!phys) return -ENOMEM;
        map_page(start + i * PAGE_SIZE, (u64)phys, 3);
    }
    
    return start;
}

static u64 sys_munmap(u64 addr, u64 length, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    u64 pages = (length + PAGE_SIZE - 1) / PAGE_SIZE;
    for (u64 i = 0; i < pages; i++) {
        u64 phys = virt_to_phys(addr + i * PAGE_SIZE);
        if (phys) {
            physical_free((void*)phys, 1);
        }
        unmap_page(addr + i * PAGE_SIZE);
    }
    return 0;
}

static u64 sys_mprotect(u64 addr, u64 length, u64 prot, u64 arg4, u64 arg5, u64 arg6) {
    (void)addr; (void)length; (void)prot; (void)arg4; (void)arg5; (void)arg6;
    return 0;
}

static u64 sys_nanosleep(u64 req, u64 rem, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    struct timespec *ts_req = (struct timespec*)req;
    u64 ticks = ts_req->tv_sec * timer_freq + ts_req->tv_nsec * timer_freq / 1000000000;
    u64 target = uptime + ticks;
    while (uptime < target) {
        asm volatile("pause");
    }
    if (rem) {
        struct timespec *ts_rem = (struct timespec*)rem;
        ts_rem->tv_sec = 0;
        ts_rem->tv_nsec = 0;
    }
    return 0;
}

static u64 sys_gettimeofday(u64 tv, u64 tz, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    if (tv) {
        struct timeval *timeval = (struct timeval*)tv;
        timeval->tv_sec = uptime / timer_freq;
        timeval->tv_usec = (uptime % timer_freq) * 1000000 / timer_freq;
    }
    if (tz) {
        struct timezone *timezone = (struct timezone*)tz;
        timezone->tz_minuteswest = 0;
        timezone->tz_dsttime = 0;
    }
    return 0;
}

static u64 sys_time(u64 tloc, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    time_t now = uptime / timer_freq;
    if (tloc) *(time_t*)tloc = now;
    return now;
}

static u64 sys_kill(u64 pid, u64 sig, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    process_t *proc = process_list;
    do {
        if (proc->pid == pid) {
            proc->signals |= (1ULL << sig);
            return 0;
        }
        proc = proc->next;
    } while (proc != process_list);
    return -ESRCH;
}

static u64 sys_uname(u64 buf, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    struct utsname *uts = (struct utsname*)buf;
    kernel_strcpy(uts->sysname, "BXKernel");
    kernel_strcpy(uts->nodename, "localhost");
    kernel_strcpy(uts->release, "1.0");
    kernel_strcpy(uts->version, "64-bit");
    kernel_strcpy(uts->machine, "x86_64");
    kernel_strcpy(uts->domainname, "");
    return 0;
}

static u64 sys_fcntl(u64 fd, u64 cmd, u64 arg, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg4; (void)arg5; (void)arg6;
    if (fd >= MAX_FDS) return -EBADF;
    file_t *file = open_files[fd];
    if (!file) return -EBADF;
    
    switch (cmd) {
        case F_DUPFD:
            for (u64 i = arg; i < MAX_FDS; i++) {
                if (!open_files[i]) {
                    open_files[i] = file;
                    return i;
                }
            }
            return -EMFILE;
        case F_GETFD:
            return file->flags;
        case F_SETFD:
            file->flags = arg;
            return 0;
        case F_GETFL:
            return file->mode;
        case F_SETFL:
            file->mode = arg;
            return 0;
        default:
            return -EINVAL;
    }
}

static u64 sys_dup(u64 oldfd, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    if (oldfd >= MAX_FDS) return -EBADF;
    file_t *file = open_files[oldfd];
    if (!file) return -EBADF;
    
    for (u64 i = 0; i < MAX_FDS; i++) {
        if (!open_files[i]) {
            open_files[i] = file;
            return i;
        }
    }
    return -EMFILE;
}

static u64 sys_dup2(u64 oldfd, u64 newfd, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    if (oldfd >= MAX_FDS || newfd >= MAX_FDS) return -EBADF;
    file_t *oldfile = open_files[oldfd];
    if (!oldfile) return -EBADF;
    
    if (open_files[newfd]) {
        kfree(open_files[newfd]);
    }
    open_files[newfd] = oldfile;
    return newfd;
}

static u64 sys_chdir(u64 path, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    char *cpath = (char*)path;
    for (u32 i = 0; i < num_mounts; i++) {
        if (kernel_strncmp(cpath, mount_paths[i], kernel_strlen(mount_paths[i])) == 0) {
            if (mounted_fs[i]->lookup) {
                inode_t *inode = mounted_fs[i]->lookup(mount_points[i], cpath + kernel_strlen(mount_paths[i]));
                if (inode) {
                    current_process->cwd = (u64)inode;
                    return 0;
                }
            }
        }
    }
    return -ENOENT;
}

static u64 sys_getcwd(u64 buf, u64 size, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    if (!current_process->cwd) {
        kernel_strncpy((char*)buf, "/", size);
        return 1;
    }
    inode_t *inode = (inode_t*)current_process->cwd;
    kernel_strncpy((char*)buf, inode->name, size);
    return kernel_strlen((char*)buf);
}

static u64 sys_mkdir(u64 pathname, u64 mode, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    char *cpath = (char*)pathname;
    for (u32 i = 0; i < num_mounts; i++) {
        if (kernel_strncmp(cpath, mount_paths[i], kernel_strlen(mount_paths[i])) == 0) {
            if (mounted_fs[i]->create) {
                mounted_fs[i]->create(mount_points[i], cpath + kernel_strlen(mount_paths[i]), mode | S_IFDIR);
                return 0;
            }
        }
    }
    return -ENOENT;
}

static u64 sys_rmdir(u64 pathname, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    char *cpath = (char*)pathname;
    for (u32 i = 0; i < num_mounts; i++) {
        if (kernel_strncmp(cpath, mount_paths[i], kernel_strlen(mount_paths[i])) == 0) {
            if (mounted_fs[i]->unlink) {
                mounted_fs[i]->unlink(mount_points[i], cpath + kernel_strlen(mount_paths[i]));
                return 0;
            }
        }
    }
    return -ENOENT;
}

static u64 sys_unlink(u64 pathname, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    char *cpath = (char*)pathname;
    for (u32 i = 0; i < num_mounts; i++) {
        if (kernel_strncmp(cpath, mount_paths[i], kernel_strlen(mount_paths[i])) == 0) {
            if (mounted_fs[i]->unlink) {
                mounted_fs[i]->unlink(mount_points[i], cpath + kernel_strlen(mount_paths[i]));
                return 0;
            }
        }
    }
    return -ENOENT;
}

static u64 sys_rename(u64 oldpath, u64 newpath, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)oldpath; (void)newpath; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    return -EROFS;
}

static u64 sys_truncate(u64 path, u64 length, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)path; (void)length; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    return -EROFS;
}

static u64 sys_ftruncate(u64 fd, u64 length, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    if (fd >= MAX_FDS) return -EBADF;
    file_t *file = open_files[fd];
    if (!file) return -EBADF;
    
    if (file->inode) {
        file->inode->size = length;
        return 0;
    }
    return -EBADF;
}

static u64 sys_stat(u64 path, u64 statbuf, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    char *cpath = (char*)path;
    struct stat *st = (struct stat*)statbuf;
    
    for (u32 j = 0; j < num_mounts; j++) {
        if (kernel_strncmp(cpath, mount_paths[j], kernel_strlen(mount_paths[j])) == 0) {
            if (mounted_fs[j]->lookup) {
                inode_t *inode = mounted_fs[j]->lookup(mount_points[j], cpath + kernel_strlen(mount_paths[j]));
                if (inode) {
                    st->st_dev = j;
                    st->st_ino = inode->inode;
                    st->st_mode = inode->mode;
                    st->st_nlink = inode->links;
                    st->st_uid = inode->uid;
                    st->st_gid = inode->gid;
                    st->st_rdev = 0;
                    st->st_size = inode->size;
                    st->st_blksize = 512;
                    st->st_blocks = inode->blocks;
                    st->st_atime = inode->atime;
                    st->st_mtime = inode->mtime;
                    st->st_ctime = inode->ctime;
                    return 0;
                }
            }
        }
    }
    return -ENOENT;
}

static u64 sys_fstat(u64 fd, u64 statbuf, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    if (fd >= MAX_FDS) return -EBADF;
    file_t *file = open_files[fd];
    if (!file) return -EBADF;
    
    if (file->inode) {
        struct stat *st = (struct stat*)statbuf;
        st->st_dev = 0;
        st->st_ino = file->inode->inode;
        st->st_mode = file->inode->mode;
        st->st_nlink = file->inode->links;
        st->st_uid = file->inode->uid;
        st->st_gid = file->inode->gid;
        st->st_rdev = 0;
        st->st_size = file->inode->size;
        st->st_blksize = 512;
        st->st_blocks = file->inode->blocks;
        st->st_atime = file->inode->atime;
        st->st_mtime = file->inode->mtime;
        st->st_ctime = file->inode->ctime;
        return 0;
    }
    return -EBADF;
}

static u64 sys_lseek(u64 fd, u64 offset, u64 whence, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg4; (void)arg5; (void)arg6;
    if (fd >= MAX_FDS) return -EBADF;
    file_t *file = open_files[fd];
    if (!file) return -EBADF;
    
    switch (whence) {
        case SEEK_SET:
            file->pos = offset;
            break;
        case SEEK_CUR:
            file->pos += offset;
            break;
        case SEEK_END:
            if (file->inode) {
                file->pos = file->inode->size + offset;
            } else {
                return -EBADF;
            }
            break;
        default:
            return -EINVAL;
    }
    return file->pos;
}

static u64 sys_getdents(u64 fd, u64 dirp, u64 count, u64 arg4, u64 arg5, u64 arg6) {
    (void)count; (void)arg4; (void)arg5; (void)arg6;
    if (fd >= MAX_FDS) return -EBADF;
    file_t *file = open_files[fd];
    if (!file || !file->inode) return -ENOTDIR;
    
    struct dirent *dent = (struct dirent*)dirp;
    
    if (file->pos == 0) {
        dent->d_ino = 1;
        dent->d_off = 0;
        dent->d_reclen = sizeof(struct dirent);
        kernel_strcpy(dent->d_name, ".");
        dent->d_type = DT_DIR;
        file->pos++;
        return sizeof(struct dirent);
    } else if (file->pos == 1) {
        dent->d_ino = 2;
        dent->d_off = 0;
        dent->d_reclen = sizeof(struct dirent);
        kernel_strcpy(dent->d_name, "..");
        dent->d_type = DT_DIR;
        file->pos++;
        return sizeof(struct dirent);
    }
    return 0;
}

static u64 sys_mount(u64 source, u64 target, u64 fstype, u64 flags, u64 data, u64 arg6) {
    (void)source; (void)flags; (void)data; (void)arg6;
    if (num_mounts >= MAX_MOUNTS) return -ENOMEM;
    
    char *cfstype = (char*)fstype;
    char *ctarget = (char*)target;
    
    for (u32 i = 0; i < num_filesystems; i++) {
        if (kernel_strcmp(filesystems[i].name, cfstype) == 0) {
            mounted_fs[num_mounts] = (fs_ops*)filesystems[i].ops;
            mount_points[num_mounts] = filesystems[i].data;
            kernel_strcpy(mount_paths[num_mounts], ctarget);
            num_mounts++;
            return 0;
        }
    }
    return -ENODEV;
}

static u64 sys_umount(u64 target, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    char *ctarget = (char*)target;
    for (u32 i = 0; i < num_mounts; i++) {
        if (kernel_strcmp(mount_paths[i], ctarget) == 0) {
            for (u32 j = i; j < num_mounts - 1; j++) {
                mounted_fs[j] = mounted_fs[j + 1];
                mount_points[j] = mount_points[j + 1];
                kernel_strcpy(mount_paths[j], mount_paths[j + 1]);
            }
            num_mounts--;
            return 0;
        }
    }
    return -EINVAL;
}

static u64 sys_getuid(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg1; (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    return 0;
}

static u64 sys_getgid(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg1; (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    return 0;
}

static u64 sys_geteuid(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg1; (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    return 0;
}

static u64 sys_getegid(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6) {
    (void)arg1; (void)arg2; (void)arg3; (void)arg4; (void)arg5; (void)arg6;
    return 0;
}

static void register_filesystem(const char *name, fs_ops *ops, u64 data) {
    if (num_filesystems >= MAX_FILESYSTEMS) return;
    
    kernel_strcpy(filesystems[num_filesystems].name, name);
    filesystems[num_filesystems].ops = (u64)ops;
    filesystems[num_filesystems].data = data;
    num_filesystems++;
}

static void register_device(u32 major, u32 minor, const char *name, device_ops *ops) {
    if (num_devices >= MAX_DEVICES) return;
    
    devices[num_devices].major = major;
    devices[num_devices].minor = minor;
    kernel_strcpy(devices[num_devices].name, name);
    if (ops) {
        devices[num_devices].ops = *ops;
    }
    num_devices++;
}

static inode_t* procfs_lookup(u64 root, const char *path) {
    (void)root;
    static inode_t proc_inode;
    kernel_memset(&proc_inode, 0, sizeof(inode_t));
    proc_inode.inode = next_inode++;
    proc_inode.mode = S_IFDIR | 0755;
    proc_inode.links = 1;
    
    if (kernel_strcmp(path, "/") == 0) {
        kernel_strcpy(proc_inode.name, "/proc");
        return &proc_inode;
    }
    
    return NULL;
}

static dirent_t* procfs_readdir(u64 root, u64 offset) {
    (void)root;
    static dirent_t dirent;
    kernel_memset(&dirent, 0, sizeof(dirent_t));
    
    if (offset == 0) {
        kernel_strcpy(dirent.name, ".");
        dirent.type = DT_DIR;
        dirent.inode = 1;
    } else if (offset == 1) {
        kernel_strcpy(dirent.name, "..");
        dirent.type = DT_DIR;
        dirent.inode = 1;
    } else if (offset == 2) {
        kernel_strcpy(dirent.name, "cpuinfo");
        dirent.type = DT_REG;
        dirent.inode = next_inode++;
    } else if (offset == 3) {
        kernel_strcpy(dirent.name, "meminfo");
        dirent.type = DT_REG;
        dirent.inode = next_inode++;
    } else if (offset == 4) {
        kernel_strcpy(dirent.name, "version");
        dirent.type = DT_REG;
        dirent.inode = next_inode++;
    } else {
        return NULL;
    }
    
    return &dirent;
}

static u64 procfs_read(file_t *file, void *buf, u64 count, u64 offset) {
    static char cpuinfo[] = "processor: 0\nvendor_id: BXKernel\ncpu family: 6\nmodel: 158\nmodel name: BXKernel Virtual CPU\n";
    static char meminfo[] = "MemTotal: 1024000 kB\nMemFree: 819200 kB\n";
    static char version[] = "BXKernel x86_64\n";
    
    char *data = NULL;
    u64 len = 0;
    
    if (kernel_strstr(file->inode->name, "cpuinfo")) {
        data = cpuinfo;
        len = sizeof(cpuinfo) - 1;
    } else if (kernel_strstr(file->inode->name, "meminfo")) {
        data = meminfo;
        len = sizeof(meminfo) - 1;
    } else if (kernel_strstr(file->inode->name, "version")) {
        data = version;
        len = sizeof(version) - 1;
    }
    
    if (!data) return 0;
    
    if (offset >= len) return 0;
    if (offset + count > len) count = len - offset;
    
    kernel_memcpy(buf, data + offset, count);
    return count;
}

static void tmpfs_create(u64 root, const char *name, u32 mode) {
    (void)root;
    (void)name;
    (void)mode;
}

static inode_t* tmpfs_lookup(u64 root, const char *path) {
    (void)root;
    (void)path;
    return NULL;
}

static fs_ops procfs_ops = {
    .mount = NULL,
    .umount = NULL,
    .lookup = procfs_lookup,
    .read = procfs_read,
    .write = NULL,
    .create = NULL,
    .unlink = NULL,
    .readdir = procfs_readdir
};

static fs_ops tmpfs_ops = {
    .mount = NULL,
    .umount = NULL,
    .lookup = tmpfs_lookup,
    .read = NULL,
    .write = NULL,
    .create = tmpfs_create,
    .unlink = NULL,
    .readdir = NULL
};

static void syscall_init(void) {
    for (int i = 0; i < 512; i++) {
        syscall_table[i] = (u64)sys_unimplemented;
    }
    
    syscall_table[0] = (u64)sys_read;
    syscall_table[1] = (u64)sys_write;
    syscall_table[2] = (u64)sys_open;
    syscall_table[3] = (u64)sys_close;
    syscall_table[4] = (u64)sys_stat;
    syscall_table[5] = (u64)sys_fstat;
    syscall_table[6] = (u64)sys_uname;
    syscall_table[7] = (u64)sys_getdents;
    syscall_table[8] = (u64)sys_lseek;
    syscall_table[9] = (u64)sys_mmap;
    syscall_table[10] = (u64)sys_mprotect;
    syscall_table[11] = (u64)sys_munmap;
    syscall_table[12] = (u64)sys_brk;
    syscall_table[24] = (u64)sys_sched_yield;
    syscall_table[32] = (u64)sys_dup;
    syscall_table[33] = (u64)sys_dup2;
    syscall_table[35] = (u64)sys_nanosleep;
    syscall_table[39] = (u64)sys_getpid;
    syscall_table[57] = (u64)sys_fork;
    syscall_table[59] = (u64)sys_execve;
    syscall_table[62] = (u64)sys_kill;
    syscall_table[63] = (u64)sys_uname;
    syscall_table[72] = (u64)sys_fcntl;
    syscall_table[76] = (u64)sys_truncate;
    syscall_table[77] = (u64)sys_ftruncate;
    syscall_table[78] = (u64)sys_getdents;
    syscall_table[79] = (u64)sys_getcwd;
    syscall_table[80] = (u64)sys_chdir;
    syscall_table[82] = (u64)sys_rename;
    syscall_table[83] = (u64)sys_mkdir;
    syscall_table[84] = (u64)sys_rmdir;
    syscall_table[87] = (u64)sys_unlink;
    syscall_table[96] = (u64)sys_gettimeofday;
    syscall_table[102] = (u64)sys_getuid;
    syscall_table[104] = (u64)sys_getgid;
    syscall_table[107] = (u64)sys_geteuid;
    syscall_table[108] = (u64)sys_getegid;
    syscall_table[110] = (u64)sys_getppid;
    syscall_table[164] = (u64)sys_mount;
    syscall_table[165] = (u64)sys_umount;
    syscall_table[185] = (u64)sys_time;
}

#define MAX_TTYS 12
static int active_tty = 0;

void tty_init(void) {
}

void tty_putchar(char c) {
    vga_putchar(c);
}

int tty_getchar(void) {
    return -1;
}

void tty_switch(int num) {
    if (num >= 0 && num < MAX_TTYS) {
        active_tty = num;
        vga_clear();
        kprintf("Active TTY: %d\n", num + 1);
    }
}

int vfs_open(const char *pathname, int flags, mode_t mode) {
    (void)pathname; (void)flags; (void)mode;
    return 0;
}

ssize_t vfs_read(int fd, void *buf, usize_t count) {
    (void)fd; (void)buf; (void)count;
    return 0;
}

ssize_t vfs_write(int fd, const void *buf, usize_t count) {
    if (fd == 1 || fd == 2) {
        const char *str = (const char*)buf;
        for (usize_t i = 0; i < count; i++) {
            tty_putchar(str[i]);
        }
        return (ssize_t)count;
    }
    return 0;
}

int vfs_close(int fd) {
    (void)fd;
    return 0;
}

void keyboard_handler(void) {
    u8 scancode = inb(0x60);
    
    key_state[scancode & 0x7F] = !(scancode & 0x80);
    
    if (scancode == 0x1D) ctrl_pressed = !(scancode & 0x80);
    else if (scancode == 0x38) alt_pressed = !(scancode & 0x80);
    else if (scancode == 0x2A || scancode == 0x36) shift_pressed = !(scancode & 0x80);
    
    if (ctrl_pressed && alt_pressed) {
        if (scancode >= 0x3B && scancode <= 0x44 && !(scancode & 0x80)) {
            tty_switch(scancode - 0x3B);
            return;
        }
    }
    
    if (!(scancode & 0x80)) {
        char keymap_lower[] = {
            0, 0, '1','2','3','4','5','6','7','8','9','0','-','=',8,
            0,'q','w','e','r','t','y','u','i','o','p','[',']',10,
            0,'a','s','d','f','g','h','j','k','l',';',39,'`',0,
            92,'z','x','c','v','b','n','m',',','.','/',0,0,0,' '
        };
        
        char keymap_upper[] = {
            0, 0, '!','@','#','$','%','^','&','*','(',')','_','+',8,
            0,'Q','W','E','R','T','Y','U','I','O','P','{','}',10,
            0,'A','S','D','F','G','H','J','K','L',':','"','~',0,
            '|','Z','X','C','V','B','N','M','<','>','?',0,0,0,' '
        };
        
        if (scancode < 128) {
            char c = shift_pressed ? keymap_upper[scancode] : keymap_lower[scancode];
            if (c) tty_putchar(c);
        }
    }
}

static void apic_init(void) {
}

static void smp_init(void) {
}

static void acpi_init(void) {
}

static void pci_init(void) {
    pci_scan();
    kprintf("PCI: %d devices found\n", num_pci);
}

static void usb_detect(void) {
    usb_init();
    kprintf("USB: %d controllers found\n", num_usb);
}

static void network_detect(void) {
    network_init();
    kprintf("Network: %d adapters found\n", num_net);
}

static void audio_detect(void) {
    audio_init();
    kprintf("Audio: %d devices found\n", num_audio);
}

static void storage_detect(void) {
    storage_init();
    kprintf("Storage: %d devices found\n", num_storage);
}

static void filesystem_init(void) {
    register_filesystem("procfs", &procfs_ops, 0);
    register_filesystem("tmpfs", &tmpfs_ops, 0);
    register_filesystem("ext2", NULL, 0);
    register_filesystem("ext3", NULL, 0);
    register_filesystem("ext4", NULL, 0);
    register_filesystem("fat", NULL, 0);
    register_filesystem("fat32", NULL, 0);
    register_filesystem("ntfs", NULL, 0);
    register_filesystem("iso9660", NULL, 0);
}

static void driver_init(void) {
    device_ops tty_ops = {0};
    register_device(4, 0, "tty0", &tty_ops);
    register_device(4, 1, "tty1", &tty_ops);
    
    device_ops null_ops = {0};
    register_device(1, 3, "null", &null_ops);
    
    device_ops zero_ops = {0};
    register_device(1, 5, "zero", &zero_ops);
    
    device_ops random_ops = {0};
    register_device(1, 8, "random", &random_ops);
    
    device_ops urandom_ops = {0};
    register_device(1, 9, "urandom", &urandom_ops);
}

void kmain(unsigned long magic, unsigned long mb_info) {
    (void)magic;
    (void)mb_info;
    
    vga_clear();
        
    kprintf("Booted in 64-bit long mode\n");
    
    tty_init();
    pic_remap();
    idt_init();
    timer_init(1000);
    syscall_init();
    apic_init();
    smp_init();
    acpi_init();
    pci_init();
    usb_detect();
    network_detect();
    audio_detect();
    storage_detect();
    filesystem_init();
    driver_init();
    
    asm volatile("sti");
    
    while (1) {
        if (inb(0x64) & 1) {
            keyboard_handler();
        }
        
        asm volatile("hlt");
    }
}
