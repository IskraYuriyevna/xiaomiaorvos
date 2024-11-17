#include "os.h"

int spin_lock()
//启用自旋锁
{
	w_mstatus(r_mstatus() & ~MSTATUS_MIE);
	//这条指令首先调用 r_mstatus 函数读取当前的 mstatus 寄存器的值，然后使用位运算符 & 和 ~ 清除 MSTATUS_MIE 位（机器模式中断使能位）。
	//MSTATUS_MIE是一个预设的掩码，在对应mstatus的mie这一位的位置为1.在这里是(1<<3)
	//这样，中断被禁用，当前任务进入临界区。
	return 0;
}

int spin_unlock()
//解除自旋锁
{
	w_mstatus(r_mstatus() | MSTATUS_MIE);
	//这条指令调用 r_mstatus 函数读取当前的 mstatus 函数的值，然后使用位运算符 | 设置 MSTATUS_MIE 位，重新启用中断。
	//
	return 0;
}
 