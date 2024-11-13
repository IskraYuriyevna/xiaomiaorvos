#include "os.h"

/* defined in entry.S */
extern void switch_to(struct context *next);

//最多10个任务，一个栈的大小为1024
#define MAX_TASKS 10
#define STACK_SIZE 1024 
/*
 * In the standard RISC-V calling convention, the stack pointer sp
 * is always 16-byte aligned.
 */
uint8_t __attribute__((aligned(16))) task_stack[MAX_TASKS][STACK_SIZE];
struct context ctx_tasks[MAX_TASKS];
//以上几句对应一个任务的栈和上下文

static int _top = 0;
static int _current = -1;

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

//void user_task0(void);
void sched_init()
//初始化任务调度
{
	w_mscratch(0);

	//ctx_task.sp = (reg_t) &task_stack[STACK_SIZE];
	//把任务的第一条指令的地址保存到ra
	//ctx_task.ra = (reg_t) user_task0;
}

void schedule()
//将当前任务的上下文保存并切换到下一个任务
{	
	//检测任务数量是否大于0，如果小于0则报错
	if (_top <= 0) {
		panic("Num -f task should be greater then zero!");
		return;
	}

	//更新全局变量_current,它指向当前正在执行的任务的上下文
	//通过将_current加1并取模_top,实现循环选择下一个任务的索引
	//当_current达到_top-1时，下一个任务的索引会回到0,形成一个循环
	_current = (_current + 1) % _top;
	//通过_current索引从ctx_tasks数组中获取下一个任务的上下文，并将其指针赋给next
	//ctx_tasks存储所有人物的上下文
	struct context *next = &(ctx_tasks[_current]);
	//将下一个任务的上下文指针传入switch_to函数
	//该函数将当前任务的上下文保存并读取下一个任务的上下文
	switch_to(next);
}

int task_create(void (*start_routin)(void))
//这个函数传入一个指向人物的入口函数的指针
{
	if (_top < MAX_TASKS) {
		//如果当前任务数量小于最大任务数量
		//为新任务设置栈指针(sp)，这里将栈顶指针地址赋给任务的栈指针
		ctx_tasks[_top].sp = (reg_t) &task_stack[_top][STACK_SIZE];
		//设置任务的返回地址(ra)，也就是任务入口函数的地址，这样当任务开始执行的时候将会从这里开始执行
		ctx_tasks[_top].ra = (reg_t) start_routin;
		//任务数量加一
		_top++;
		return 0;
	} else {
		//如果任务创建失败（达到最大任务数量）
		return -1;
	}
}

/*
 * DESCRIPTION
 * 	task_yield()  causes the calling task to relinquish the CPU and a new 
 * 	task gets to run.
 */
void task_yield()
//这个函数使当前任务放弃CPU的控制权，允许调度器选择另一个任务执行
{
	schedule();
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


// void user_task0(void)
// //这是第一个（或者说第0个任务），就是每隔一秒不停打印running
// {
// 	uart_puts("Task 0: Created!\n");
// 	while (1) {
// 		uart_puts("Task 0: Running...\n");
// 		task_delay(1000);
// 	}
// }

