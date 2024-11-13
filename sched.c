#include "os.h"

/* defined in entry.S */
extern void switch_to(struct context *next);

#define STACK_SIZE 1024
/*
 * In the standard RISC-V calling convention, the stack pointer sp
 * is always 16-byte aligned.
 */
uint8_t __attribute__((aligned(16))) task_stack[STACK_SIZE];
struct context ctx_task;
//以上三句对应一个任务的栈和上下文

static void w_mscratch(reg_t x)
{
	//asm volatile是在C语言中内嵌汇编代码
	//这一句是将x的值写入到mscratch寄存器
	//volatile告诉编译器这句不能优化也不能合并
	//csrw表示控制状态寄存器写入，mscratch是寄存器名称
	//%0是第一个操作数，它会被替换为x的值
	//： ：表示该指令没有输出
	//r(x)制定了输入x应该被当作一个通用寄存器(r表示register)处理
	asm volatile("csrw mscratch, %0" : : "r" (x));
}

void user_task0(void);
void sched_init()
//初始化任务调度
{
	w_mscratch(0);

	ctx_task.sp = (reg_t) &task_stack[STACK_SIZE];
	//把任务的第一条指令的地址保存到ra
	ctx_task.ra = (reg_t) user_task0;
}

void schedule()
//将当前任务的上下文保存并切换到下一个任务
{
	struct context *next = &ctx_task;
	switch_to(next);
}

/*
 * a very rough implementaion, just to consume the cpu
 */
void task_delay(volatile int count)
//让CPU不停算count*=50000减到0来延迟时间
{
	count *= 50000;
	while (count--);
}


void user_task0(void)
//这是第一个（或者说第0个任务），就是每隔一秒不停打印running
{
	uart_puts("Task 0: Created!\n");
	while (1) {
		uart_puts("Task 0: Running...\n");
		task_delay(1000);
	}
}

