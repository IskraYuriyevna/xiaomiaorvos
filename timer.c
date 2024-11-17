#include "os.h"

extern void schedule(void);
/* interval ~= 1s */
#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ
//定义了计时器中断的间隔，单位为 CLINT 的时钟周期数。这里设置为 CLINT_TIMEBASE_FREQ，即每秒钟的时钟周期数，大约是 1 秒的间隔。
 
static uint32_t _tick = 0;
//定义了一个静态变量，用于记录系统启动以来的时钟滴答数。

#define MAX_TIMER 10
//定义了系统可以创建的最大定时器数量为 10。
static struct timer timer_list[MAX_TIMER];
//这行代码声明了一个 timer 结构体类型的静态数组 timer_list，其大小由 MAX_TIMER 定义。
//数组中的每个元素都是一个 timer 结构体实例，用于存储定时器的回调函数、参数和超时值。

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

// void timer_init()
// //初始化计时器。
// {
// 	/*
// 	 * On reset, mtime is cleared to zero, but the mtimecmp registers 
// 	 * are not reset. So we have to init the mtimecmp manually.
// 	 */
// 	timer_load(TIMER_INTERVAL);
// 	//调用 timer_load 函数，设置计时器中断的初始间隔。

// 	/* enable machine-mode timer interrupts. */
// 	w_mie(r_mie() | MIE_MTIE);
// 	//启用机器模式下的计时器中断。MIE_MTIE 是 mie 寄存器中用于控制计时器中断的位。

// 	/* enable machine-mode global interrupts. */
// 	//w_mstatus(r_mstatus() | MSTATUS_MIE);
// 	//启用机器模式下的全局中断。MSTATUS_MIE 是 mstatus 寄存器中用于控制全局中断的位。
// }

void timer_init()
//初始化定时器
{
	struct timer *t = &(timer_list[0]);
	//初始化计时器数组，将所有计时器设为NULL，表示所有的计时器都是未使用的
	for (int i = 0; i < MAX_TIMER; i++) {
		t->func = NULL; /* use .func to flag if the item is used */
		t->arg = NULL;
		t++;
	}

	/*
	 * On reset, mtime is cleared to zero, but the mtimecmp registers 
	 * are not reset. So we have to init the mtimecmp manually.
	 */
	 //调用 timer_load(TIMER_INTERVAL) 来设置 CLINT 的 mtimecmp 寄存器，以便定时器中断可以在指定的时间间隔后触发。
	timer_load(TIMER_INTERVAL);

	//使用机器模式下的计时器中断
	/* enable machine-mode timer interrupts. */
	w_mie(r_mie() | MIE_MTIE);
}

struct timer *timer_create(void (*handler)(void *arg), void *arg, uint32_t timeout)
//创建一个新计时器，返回计时器结构体
//handler：定时器到期时调用的函数。arg：传递给 handler 的参数。timeout：定时器到期前的时间（以时钟周期为单位）。
{
	/* TBD: params should be checked more, but now we just simplify this */
	//检查 handler 是否为 NULL 或 timeout 是否为 0，如果是，则返回 NULL。这是一个简化的检查，理论上需要检查更多
	if (NULL == handler || 0 == timeout) {
		return NULL;
	}

	/* use lock to protect the shared timer_list between multiple tasks */
	//使用自旋锁保护对 timer_list 的访问
	spin_lock();

	//遍历 timer_list 找到第一个未使用的定时器（func 为 NULL）。
	struct timer *t = &(timer_list[0]);
	for (int i = 0; i < MAX_TIMER; i++) {
		if (NULL == t->func) {
			break;
		}
		t++;
	}
	//如果没有找到未使用的定时器，解锁并返回 NULL。
	if (NULL != t->func) {
		spin_unlock();
		return NULL;
	}

	//否则，设置定时器的 func、arg 和 timeout_tick，
	t->func = handler;
	t->arg = arg;
	t->timeout_tick = _tick + timeout;
	//然后解锁并返回定时器的指针。
	spin_unlock();

	return t;
}

void timer_delete(struct timer *timer)
//删除计时器，输入指向要删除的定时器的指针。
{
	spin_lock();//启动自旋锁，保护计时器数组

	//便历 timer_list 找到匹配的定时器
	struct timer *t = &(timer_list[0]);
	//如果找到，将 func 和 arg 设置为 NULL，表示该定时器不再使用
	for (int i = 0; i < MAX_TIMER; i++) {
		if (t == timer) {
			t->func = NULL;
			t->arg = NULL;
			break;
		}
		t++;
	}

	spin_unlock();//解锁
}

/* this routine should be called in interrupt context (interrupt is disabled) */
static inline void timer_check()
//检查计时器是否到期，并在到期时执行回调函数
{
	struct timer *t = &(timer_list[0]);
	//遍历 timer_list，检查每个定时器的 timeout_tick 是否小于或等于当前的 _tick
    
    

	for (int i = 0; i < MAX_TIMER; i++) {
		if (NULL != t->func) {
			if (_tick >= t->timeout_tick) {

				//如果定时器到期，调用其 func 回调函数，并传递 arg 作为参数。
				t->func(t->arg);

				/* once time, just delete it after timeout */
				//执行回调函数后，将 func 和 arg 设置为 NULL，表示该定时器已被删除。
				t->func = NULL;
				t->arg = NULL;

				//由于是一次性定时器，找到第一个到期的定时器后即停止检查。
				break;
			}
		}
		t++;
	}
}

void timer_handler() 
//处理计时器中断。
{
	//增加滴答计数器。
	_tick++;
	printf("tick: %d\n", _tick); 
	//打印当前的滴答计数器值。

	timer_check();//检查计时器是否到期

	timer_load(TIMER_INTERVAL);
	//重新加载计时器中断的间隔，以便定时器中断能够周期性地发生。

	schedule();
}
