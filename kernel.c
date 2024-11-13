#include "os.h"

extern void uart_init(void);//初始化串口
extern void uart_puts(char *s);//将一个字符串通过串口发出去
extern void page_init(void);//初始化分页

void start_kernel(void)
{
    uart_init();
    uart_puts("Hello,RVOS!\n");
    //向串口发送"Hello,RVOS!\n"

    page_init();

    while(1){};//stop here!
    //这是一个死循环，确保内核执行完初始化代码后不会退出
}