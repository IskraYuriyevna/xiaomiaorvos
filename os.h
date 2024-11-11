#ifndef __OS_H__
#define __OS_H__

#include "types.h"
#include "platform.h"

#include <stddef.h>
#include <stdarg.h>

/* uart */
extern int uart_putc(char ch);
extern void uart_puts(char *s);

/* printf */
extern int  printf(const char* s, ...);
extern void panic(char *s);

/* memory management */
/*实现基于Page的动态内存分配，将内存分页到堆上*/
/*最简单的方法是链表和数组*/
extern void *page_alloc(int npages);
extern void page_free(void *p);

#endif /* __OS_H__ */
