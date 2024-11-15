#include "os.h"

extern void schedule(void);
/* interval ~= 1s */
#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ
//定义了计时器中断的间隔，单位为 CLINT 的时钟周期数。这里设置为 CLINT_TIMEBASE_FREQ，即每秒钟的时钟周期数，大约是 1 秒的间隔。
 
static uint32_t _tick = 0;
//定义了一个静态变量，用于记录系统启动以来的时钟滴答数。

/* load timer interval(in ticks) for next timer interrupt.*/
void timer_load(int interval)
//用于加载计时器中断的触发间隔。
{
	/* each CPU has a separate source of timer interrupts. */
	//读取当前硬件线程（hart）的 ID。
	int id = r_mhartid();
	
	//计算下一个计时器中断的触发时间，并写入到 CLINT 的比较寄存器中。这样，当 CLINT 的 mtime 寄存器值达到这个比较值时，就会触发计时器中断。
	*(uint64_t*)CLINT_MTIMECMP(id) = *(uint64_t*)CLINT_MTIME + interval;
}

void timer_init()
//初始化计时器。
{
	/*
	 * On reset, mtime is cleared to zero, but the mtimecmp registers 
	 * are not reset. So we have to init the mtimecmp manually.
	 */
	timer_load(TIMER_INTERVAL);
	//调用 timer_load 函数，设置计时器中断的初始间隔。

	/* enable machine-mode timer interrupts. */
	w_mie(r_mie() | MIE_MTIE);
	//启用机器模式下的计时器中断。MIE_MTIE 是 mie 寄存器中用于控制计时器中断的位。

	/* enable machine-mode global interrupts. */
	//w_mstatus(r_mstatus() | MSTATUS_MIE);
	//启用机器模式下的全局中断。MSTATUS_MIE 是 mstatus 寄存器中用于控制全局中断的位。
}

void timer_handler() 
//处理计时器中断。
{
	//增加滴答计数器。
	_tick++;
	printf("tick: %d\n", _tick); 
	//打印当前的滴答计数器值。

	timer_load(TIMER_INTERVAL);
	//重新加载计时器中断的间隔，以便定时器中断能够周期性地发生。

	schedule();
}
