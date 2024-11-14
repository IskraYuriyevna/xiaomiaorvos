#include "os.h"

#define DELAY 1000

// extern void trap_test(void);

void user_task0(void)
//这是第一个任务，死循环打印
{
	uart_puts("Task 0: Created!\n");
	while (1) {
		uart_puts("Task 0: Running...\n");
		// trap_test();//中断测试代码
		task_delay(DELAY);
		task_yield();
	}
} 

void user_task1(void)
//这是第二个任务，也是死循环打印
{
	uart_puts("Task 1: Created!\n");
	while (1) {
		uart_puts("Task 1: Running...\n");
		task_delay(DELAY);
		task_yield();
	}
}

/* NOTICE: DON'T LOOP INFINITELY IN main() */ 
void os_main(void)
//os主函数，创建上面两个任务
{
	task_create(user_task0);
	task_create(user_task1);
}

