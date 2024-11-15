#include "os.h"

void plic_init(void)
//平台级中断控制器（Platform-Level Interrupt Controller , PLIC）初始化
{
	int hart = r_tp();//读取当前硬件线程的ID
  
	/* 
	 * Set priority for UART0.
	 *
	 * Each PLIC interrupt source can be assigned a priority by writing 
	 * to its 32-bit memory-mapped priority register.
	 * The QEMU-virt (the same as FU540-C000) supports 7 levels of priority. 
	 * A priority value of 0 is reserved to mean "never interrupt" and 
	 * effectively disables the interrupt. 
	 * Priority 1 is the lowest active priority, and priority 7 is the highest. 
	 * Ties between global interrupts of the same priority are broken by 
	 * the Interrupt ID; interrupts with the lowest ID have the highest 
	 * effective priority.
	 */
	// 设置UART0 中断优先级(0~7),0表示禁止中断
	*(uint32_t*)PLIC_PRIORITY(UART0_IRQ) = 1; 
 
	/*
	 * Enable UART0
	 *
	 * Each global interrupt can be enabled by setting the corresponding 
	 * bit in the enables registers.
	 */
	// 在PLIC的使能寄存器中设置对应位，启用UART0中断
	*(uint32_t*)PLIC_MENABLE(hart, UART0_IRQ)= (1 << (UART0_IRQ % 32));

	/* 
	 * Set priority threshold for UART0.
	 *
	 * PLIC will mask all interrupts of a priority less than or equal to threshold.
	 * Maximum threshold is 7.
	 * For example, a threshold value of zero permits all interrupts with
	 * non-zero priority, whereas a value of 7 masks all interrupts.
	 * Notice, the threshold is global for PLIC, not for each interrupt source.
	 */
	//设置PLIC的中断优先级阈值为0,也就是说，大于0的都可以中断
	*(uint32_t*)PLIC_MTHRESHOLD(hart) = 0;

	/* enable machine-mode external interrupts. */
	//通过设置mie寄存器中的MIE_MEIE位，启用机器模式下的外部中断
	w_mie(r_mie() | MIE_MEIE);

	/* enable machine-mode global interrupts. */
	//通过设置mstatus寄存器中的MSTATUS_MIE位，启用机器模式下的全局中断
	//w_mstatus(r_mstatus() | MSTATUS_MIE);
}

/* 
 * DESCRIPTION:
 *	Query the PLIC what interrupt we should serve.
 *	Perform an interrupt claim by reading the claim register, which
 *	returns the ID of the highest-priority pending interrupt or zero if there 
 *	is no pending interrupt. 
 *	A successful claim also atomically clears the corresponding pending bit
 *	on the interrupt source.
 * RETURN VALUE:
 *	the ID of the highest-priority pending interrupt or zero if there 
 *	is no pending interrupt.
 */
int plic_claim(void)
//查询PLIC,获取当前硬件线程应该对应服务的中断号，通过读取PLIC的claim寄存器，返回最高优先级的待处理中断的ID,如果没有则返回0
{
	int hart = r_tp();
	int irq = *(uint32_t*)PLIC_MCLAIM(hart);//读取PLIC的claim寄存器，获取中断号
	return irq;
}

/* 
 * DESCRIPTION:
  *	Writing the interrupt ID it received from the claim (irq) to the 
 *	complete register would signal the PLIC we've served this IRQ. 
 *	The PLIC does not check whether the completion ID is the same as the 
 *	last claim ID for that target. If the completion ID does not match an 
 *	interrupt source that is currently enabled for the target, the completion
 *	is silently ignored.
 * RETURN VALUE: none
 */
void plic_complete(int irq)
//通知PLIC已经完成了对某个中断的处理，通过向PLIC的complete寄存器写入中断号，表示该中断已经被处理
{
	int hart = r_tp();
	*(uint32_t*)PLIC_MCOMPLETE(hart) = irq;//向PLIC的complete寄存器写入中断号，表示该中断已经被处理。
}
