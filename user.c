#include "os.h"
#include "user_api.h"

//#define USE_LOCK
#define DELAY 4000

void user_task0(void)
{
	uart_puts("Task 0: Created!\n");

	unsigned int hid = -1;

	/*
	 * if syscall is supported, this will trigger exception, 
	 * code = 2 (Illegal instruction)
	 */
	//hid = r_mhartid();
	//printf("hart id is %d\n", hid); 

#ifdef CONFIG_SYSCALL
	int ret = -1;
	ret = gethid(&hid);
	//ret = gethid(NULL);
	if (!ret) {
		printf("system call returned!, hart id is %d\n", hid);
	} else {
		printf("gethid() failed, return: %d\n", ret);
	}
#endif

	while (1){
		uart_puts("Task 0: Running... \n");
		task_delay(DELAY);
	}
}


//extern void trap_test(void);

// struct userdata {
// 	int counter;//一个整数变量，用于计数回调函数被调用的次数。
// 	char *str;//一个字符指针，指向一个字符串，表示回调函数的身份标识。
// };

// /* Jack must be global */
// //userdata 类型的全局变量，初始化 counter 为 0，str 指向字符串 "Jack"。
// struct userdata person = {0, "Jack"};

// void timer_func(void *arg)
// //这个函数被用作定时器到期时的回调函数。它接受一个 void* 类型的参数 arg，该参数被转换为 userdata 类型的指针。
// {
// 	//如果 arg 为 NULL，则直接返回。
// 	if (NULL == arg) 
// 		return;

// 	//增加 param 的 counter 并打印一条消息，显示定时器到期的次数和对应的字符串。
// 	struct userdata *param = (struct userdata *)arg;

// 	param->counter++;
// 	printf("======> TIMEOUT: %s: %d\n", param->str, param->counter);
// }

// void user_task0(void)
// //第一个任务，死循环打印，其中含有定时器
// {
// 	uart_puts("Task 0: Created!\n");
// 	//创建三个定时器 t1、t2 和 t3，
// 	//每个定时器都使用 timer_func 作为回调函数，并且都使用全局变量 person 作为参数。
// 	//定时器的超时时间分别为 3、5 和 7 个时间单位。
// 	struct timer *t1 = timer_create(timer_func, &person, 3);
// 	if (NULL == t1) {
// 		printf("timer_create() failed!\n");
// 	}
// 	struct timer *t2 = timer_create(timer_func, &person, 5);
// 	if (NULL == t2) {
// 		printf("timer_create() failed!\n");
// 	}
// 	struct timer *t3 = timer_create(timer_func, &person, 7);
// 	if (NULL == t3) {
// 		printf("timer_create() failed!\n");
// 	}
// 	while (1) {
// 		uart_puts("Task 0: Running... \n");
// 		task_delay(DELAY);
// 	}
// }

void user_task1(void)
//这是第二个任务，死循环打印
{
	uart_puts("Task 1: Created!\n");
	while (1) {
		uart_puts("Task 1: Running... \n");
		task_delay(DELAY);
	}
}



// void user_task0(void)
// //这是第一个任务，死循环打印
// {
// 	uart_puts("Task 0: Created!\n");
// 	while (1) {
// 		//下面是一个条件编译的宏，如果定义了锁才会编译这一段代码
// #ifdef USE_LOCK 
// //调用 spin_lock 函数获取自旋锁。
// //自旋锁是一种简单的同步机制，用于保护共享资源，防止多个任务同时访问同一资源。
// 		spin_lock(); 
// #endif
// 		uart_puts("Task 0: Begin ... \n");
// 		for (int i = 0; i < 5; i++) {
// 			uart_puts("Task 0: Running... \n");

// 			task_delay(DELAY);
// 		}
// 		uart_puts("Task 0: End ... \n");
// #ifdef USE_LOCK
// 		//任务运行完成后释放自旋锁，允许其他任务访问共享资源。
// 		spin_unlock();
// #endif
// 	}
// }

// void user_task0(void)
// //这是第一个任务，死循环打印
// {
// 	uart_puts("Task 0: Created!\n");

// 	task_yield();
// 	uart_puts("Task 0: I'm back!\n");
// 	while (1) {
// 		uart_puts("Task 0: Running...\n");
// 		//trap_test();//中断测试代码
// 		task_delay(DELAY);
// 		//task_yield();
// 	}
// } 

// void user_task1(void)
// {
// 	uart_puts("Task 1: Created!\n");
// 	while (1) {
// 		uart_puts("Task 1: Begin ... \n");
// 		for (int i = 0; i < 5; i++) {
// 			uart_puts("Task 1: Running... \n");
// 			task_delay(DELAY);
// 		}
// 		uart_puts("Task 1: End ... \n");
// 	}
// }

/* NOTICE: DON'T LOOP INFINITELY IN main() */ 
void os_main(void)
//os主函数，创建上面两个任务
{
	task_create(user_task0);
	task_create(user_task1);
}

