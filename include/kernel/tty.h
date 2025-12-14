#ifndef KERNEL_TTY_H
#define KERNEL_TTY_H

void tty_init(void);
void tty_putchar(char c);
int tty_getchar(void);
void tty_switch(int num);

#endif
