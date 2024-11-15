#include "os.h"

extern void uart_init(void);//初始化串口
extern void uart_puts(char *s);//将一个字符串通过串口发出去
extern void page_init(void);//初始化分页
extern void sched_init();//初始化任务调度
extern void schedule(void);//任务调度函数
extern void os_main(void);//OS主函数
extern void trap_init(void);//初始化中断
extern void plic_init(void);//初始化硬件中断
extern void timer_init(void);//初始化计时器
// extern void check_context_layout(void);

void start_kernel(void)
{
    uart_init();
    uart_puts("Hello,RVOS!\n");
    //向串口发送"Hello,RVOS!\n"

    // check_context_layout();
    //uart_puts("Layout check done\n");
  
    page_init();
    //uart_puts("Page init done\n");

    trap_init();
    //uart_puts("Trap init done\n");

    plic_init();
    //art_puts("PLIC init done\n");

    timer_init(); 
    // uart_puts("Timer init done\n");

    sched_init();
    // uart_puts("Scheduler init done\n");

    os_main();
    // uart_puts("Tasks created\n");

    schedule();

    uart_puts("Would not go here!\n");
    while(1){};//stop here!
    //这是一个死循环，确保内核执行完初始化代码后不会退出
}