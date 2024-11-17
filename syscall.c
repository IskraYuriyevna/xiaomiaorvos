#include "os.h"
#include "syscall.h"

int sys_gethid(unsigned int *ptr_hid)
//获取当前硬件线程（hart）的 ID，并将其存储在提供的指针指向的位置。
{
	printf("--> sys_gethid, arg0 = %p\n", ptr_hid);
	if (ptr_hid == NULL) {
		//如果 ptr_hid 是 NULL，返回 -1 表示错误。
		return -1;
	} else {
		//调用 r_mhartid() 函数获取当前硬件线程的 ID，并将这个值存储在 *ptr_hid 指向的地址。
		*ptr_hid = r_mhartid();
		return 0;
	}
}

void do_syscall(struct context *cxt)
//这是一个系统调用的处理函数
{
	//从上下文的a7读取调用编号，riscv中通常使用这个寄存器来传入系统调用编号
	uint32_t syscall_num = cxt->a7;

	switch (syscall_num) {
	case SYS_gethid:
		//如果 syscall_num 是 SYS_gethid，调用 sys_gethid 函数，并将结果存储在 cxt->a0 中。
		cxt->a0 = sys_gethid((unsigned int *)(cxt->a0));
		break;
	default: 
		//如果 syscall_num 不是已知的系统调用编号，在默认情况下打印错误消息，并将 cxt->a0 设置为 -1 表示错误。
		printf("Unknown syscall no: %d\n", syscall_num);
		cxt->a0 = -1;
	}

	return;
}