#ifndef __OS_H__
#define __OS_H__

#include "types.h"
#include "riscv.h"
#include "platform.h"

#include <stddef.h>
#include <stdarg.h>
 
/* uart */
extern int uart_putc(char ch);
extern void uart_puts(char *s);
extern int uart_getc(void);

/* printf */
extern int  printf(const char* s, ...);
extern void panic(char *s);

/*实现基于Page的动态内存分配，将内存分页到堆上*/
/*最简单的方法是链表和数组*/
extern void *page_alloc(int npages);
extern void page_free(void *p);


//这一段是对寄存器的上下文的定义，由于x0是固定的所以不需要
//也就是说，保存一个程序在一个时刻所有的寄存器状态
/* task management */
struct context {
	/* ignore x0 */ 
	reg_t ra;
	reg_t sp;
	reg_t gp;
	reg_t tp;
	reg_t t0;
	reg_t t1;
	reg_t t2;
	reg_t s0;
	reg_t s1;
	reg_t a0;
	reg_t a1;
	reg_t a2;
	reg_t a3;
	reg_t a4;
	reg_t a5;
	reg_t a6;
	reg_t a7;
	reg_t s2;
	reg_t s3;
	reg_t s4;
	reg_t s5;
	reg_t s6;
	reg_t s7;
	reg_t s8;
	reg_t s9;
	reg_t s10;
	reg_t s11;
	reg_t t3;
	reg_t t4;
	reg_t t5;
	reg_t t6;

	reg_t pc;
};

//这两个函数用来创建一个任务和延迟一个任务（将CPU留给其他任务）
extern int  task_create(void (*task)(void));
extern void task_delay(volatile int count);
//这个函数使当前任务放弃CPU的控制权，允许调度器选择另一个任务执行
extern void task_yield();

/* plic */
extern int plic_claim(void);//获取硬件中断代码
extern void plic_complete(int irq);//硬件中断处理完成

/* lock */
extern int spin_lock(void);//使用自旋锁
extern int spin_unlock(void);//解除自旋锁

/* software timer */
//软件计时器结构
struct timer {
	void (*func)(void *arg);//计时器到期时调用的函数
	void *arg;//传入上面函数的参数
	uint32_t timeout_tick;//定时器到期的时间，以时钟周期为单位
};
//创建一个新的定时器，并返回指向该定时器的指针。如果创建失败，返回 NULL。
extern struct timer *timer_create(void (*handler)(void *arg), void *arg, uint32_t timeout);
//删除指定的定时器，并释放与之关联的资源。
extern void timer_delete(struct timer *timer);

#endif /* __OS_H__ */
