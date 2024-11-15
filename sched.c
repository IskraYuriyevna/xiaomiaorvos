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



//void user_task0(void);
void sched_init()
//初始化任务调度
{
	w_mscratch(0);

	w_mie(r_mie() | MIE_MSIE);
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
//这个函数传入一个指向任务的入口函数的指针
{
	if (_top < MAX_TASKS) {
		//如果当前任务数量小于最大任务数量
		//为新任务设置栈指针(sp)，这里将栈顶指针地址赋给任务的栈指针
		ctx_tasks[_top].sp = (reg_t) &task_stack[_top][STACK_SIZE];
		//设置任务的返回地址(ra)，也就是任务入口函数的地址，这样当任务开始执行的时候将会从这里开始执行
		//如果你问这里为什么多了这么多调试代码，就看看下一行：
		//由于之前的例子中这里是ra,而第八课中是pc,我没有发现这里不同
		//所以说没有修改，导致多任务跳转时就会出错
		//因此我花了3个多小时查找错误原因
		//发现是因为切换时会跳转到非法的地址
		//于是我寻找了很久是否是硬件的定义出了问题
		//最后在这里发现有一个寄存器发生了变化
		//ctx_tasks[_top].ra = (reg_t) start_routin;
		ctx_tasks[_top].pc = (reg_t) start_routin;
		//任务数量加一
		_top++;
		return 0;
	} else {
		//如果任务创建失败（达到最大任务数量）
		return -1;
	}
}

// int task_create(void (*start_routin)(void))
// {
//     if (_top < MAX_TASKS) {
//         printf("Creating task %d\n", _top);
//         printf("Entry address: %p\n", start_routin);
        
//         // 获取上下文指针
//         struct context *ctx = &ctx_tasks[_top];
        
//         // 手动清零整个上下文
//         for(int i = 0; i < sizeof(struct context)/sizeof(reg_t); i++) {
//             ((reg_t*)ctx)[i] = 0;
//         }
        
//         // 设置必要的字段
//         ctx->sp = (reg_t) &task_stack[_top][STACK_SIZE];
//         ctx->ra = (reg_t) start_routin;
        
//         // 直接访问 PC 字段，使用偏移量确保正确访问
//         reg_t *pc_ptr = (reg_t*)((char*)ctx + 124);  // 使用确认的偏移量
//         *pc_ptr = (reg_t)start_routin;
        
//         printf("Context ra: %p\n", ctx->ra);
//         printf("Context sp: %p\n", ctx->sp);
//         printf("Context pc: %p\n", *pc_ptr);  // 打印设置的 PC 值
        
//         _top++;
//         return 0;
//     }
//     return -1;
// }

// int task_create(void (*start_routin)(void))
// {
//     if (_top < MAX_TASKS) {
//         printf("Creating task %d\n", _top);
//         printf("Entry address: %p\n", start_routin);
//         printf("Stack address: %p\n", &task_stack[_top][STACK_SIZE]);
        
//         ctx_tasks[_top].sp = (reg_t) &task_stack[_top][STACK_SIZE];
// 		ctx_tasks[_top].ra = (reg_t) start_routin;
        
//         printf("Context ra: %p\n", ctx_tasks[_top].ra);
//     	printf("Context sp: %p\n", ctx_tasks[_top].sp);
        
//         _top++;
//         return 0;
//     }
//     return -1;
// }

// void schedule()
// {   
//     if (_top <= 0) {
//         panic("Num of task should be greater then zero!");
//         return;
//     }
    
//     printf("Scheduling: current=%d, top=%d\n", _current, _top);
//     _current = (_current + 1) % _top;
//     printf("Switching to task %d\n", _current);
    
//     struct context *next = &(ctx_tasks[_current]);
//     printf("Next task PC = 0x%08x\n", next->pc);  // 添加这行
    
//     switch_to(next); 
// }

// void check_context_layout(void) 
// {
//     struct context ctx;
//     printf("Context layout check:\n");
//     printf("Size of reg_t: %d bytes\n", sizeof(reg_t));
//     printf("Size of context: %d bytes\n", sizeof(struct context));
//     printf("Offset of ra: %d bytes\n", (char*)&ctx.ra - (char*)&ctx);
//     printf("Offset of sp: %d bytes\n", (char*)&ctx.sp - (char*)&ctx);
//     printf("Offset of t6: %d bytes\n", (char*)&ctx.t6 - (char*)&ctx);  // 添加这行
//     printf("Offset of pc: %d bytes\n", (char*)&ctx.pc - (char*)&ctx);
//     printf("Expected PC offset in asm: %d bytes\n", 31 * 4);

//     // 添加任务0的上下文检查
//     if (_top > 0) {
//         printf("\nTask 0 context check:\n");
//         printf("ra = 0x%08x\n", ctx_tasks[0].ra);
//         printf("sp = 0x%08x\n", ctx_tasks[0].sp);
//         printf("pc = 0x%08x\n", ctx_tasks[0].pc);
//     }
// }

/*
 * DESCRIPTION
 * 	task_yield()  causes the calling task to relinquish the CPU and a new 
 * 	task gets to run.
 */
void task_yield()
//这个函数使当前任务放弃CPU的控制权，允许调度器选择另一个任务执行
{
	//schedule();	
	/* trigger a machine-level software interrupt */
	int id = r_mhartid();
	*(uint32_t*)CLINT_MSIP(id) = 1;
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


