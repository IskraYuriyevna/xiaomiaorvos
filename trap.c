#include "os.h"

extern void trap_vector(void);
extern void uart_isr(void);
extern void timer_handler(void);
extern void schedule(void);

void trap_init()
//初始化中断向量的基地址 
{
	/*
	 * set the trap-vector base-address for machine-mode
	 */
	w_mtvec((reg_t)trap_vector);
}


void external_interrupt_handler()
//处理外部中断的中断服务历程
{
	int irq = plic_claim();//中断号变量，返回最高优先级的待处理中断的ID,如果没有待处理中断则返回0

	if (irq == UART0_IRQ){//判断中断源，检查返回的中断号是否表示UART0的中断，如果是UART0中断，执行uart_isr()
      		uart_isr();//处理UART0 的中断服务的函数
	} else if (irq) {//如果中断号不是 UART0 中断，但也不为 0（表示有其他中断发生），则打印一条消息表示遇到了意外的中断。
		printf("unexpected interrupt irq = %d\n", irq);
	}
	
	if (irq) {
		//如果存在有效的中断号（即 irq 不为 0），调用 plic_complete 函数，传入中断号，通知 PLIC 当前中断已处理完成。
		//这一步是必要的，因为它告诉 PLIC 该中断已经被处理，可以继续处理其他中断。
		plic_complete(irq);
	} 
}

 
reg_t trap_handler(reg_t epc, reg_t cause)
//epc：异常程序计数器（Exception Program Counter），表示发生异常时的程序计数器值。
//cause：表示异常原因的寄存器值。
{
	reg_t return_pc = epc;
	reg_t cause_code = cause & MCAUSE_MASK_ECODE;
	//初始化返回程序计数器为异常发生时的程序计数器值,并提取异常原因代码
	
	if (cause & MCAUSE_MASK_INTERRUPT) {
		/* Asynchronous trap - interrupt */
		//检查是否为异步中断
		switch (cause_code) {
		case 3://软件中断
			uart_puts("software interruption!\n");
			/*
			 * acknowledge the software interrupt by clearing
    			 * the MSIP bit in mip.
			 */
			int id = r_mhartid();//读取硬件线程ID
			//这行代码通过计算 CLINT（Core Local Interruptor）的 MSIP（Machine Software Interrupt Pending）寄存器地址，并将其值设置为 0，来清除特定硬件线程的软件中断标志
    			*(uint32_t*)CLINT_MSIP(id) = 0;
			/* CLINT_MSIP(id) 宏定义计算 MSIP 寄存器的地址，该地址是基于硬件线程 ID 的。CLINT_BASE 加上 id 再乘以 4（因为每个 MSIP 寄存器占 4 个字节）。
			 * (uint32_t*) 将计算出的地址转换为 uint32_t 类型的指针，这样就可以通过解引用这个指针来读写寄存器的值。
			 * *(uint32_t*)CLINT_MSIP(id) = 0; 将 MSIP 寄存器的值设置为 0，这通常用于清除或禁用软件中断。
			 */
			 
			schedule();

			break;
		case 7://定时器中断
			uart_puts("timer interruption!\n");
			timer_handler();
			break;
		case 11://外部中断
			uart_puts("external interruption!\n");
			external_interrupt_handler();
			break;
		default://未知中断
			printf("Unknown async exception! Code = %ld\n", cause_code);
			break;
		}
	// }else {
    //     /* Synchronous trap - exception */
    //     printf("Sync exceptions! Code = %d\n", cause_code);
    //     printf("Exception PC = 0x%08x\n", epc);  // 添加这行，打印发生异常时的程序计数器值
        
    //     panic("OOPS! What can I do!");
    // }

	} else {
		//打印同步异常的原因代码，并调用panic函数
		/* Synchronous trap - exception */
		printf("Sync exceptions! Code = %ld\n", cause_code);
		panic("OOPS! What can I do!");
		//可以不进行panic强制进行程序的下一步
		//return_pc += 4;
	}

	//返回处理后的程序计数器值 return_pc。
	return return_pc;
}

void trap_test()
{
	/*
	 * Synchronous exception code = 7
	 * Store/AMO access fault
	 */
	*(int *)0x00000000 = 100;

	/*
     * *(int *)0x00000000 = 100;：尝试将值写入地址 0x00000000，这将导致一个同步异常（存储/AMO 访问故障）。
     * 注释掉的代码 //int a = *(int *)0x00000000;：尝试从地址 0x00000000 读取值，这也会导致同步异常（加载访问故障）。
     * uart_puts("Yeah! I'm return back from trap!\n");：在异常处理后，打印返回信息。
	 */

	/*
	 * Synchronous exception code = 5
	 * Load access fault
	 */
	//int a = *(int *)0x00000000;

	uart_puts("Yeah! I'm return back from trap!\n");
}

