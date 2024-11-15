#include "platform.h"

	# size of each hart's stack is 1024 bytes
    # 定义每一个hart的栈大小为1024字节
	.equ	STACK_SIZE, 1024

    # 声明_start为程序的入口点
	.global	_start

	.text
_start:
	# park harts with id != 0
	csrr	t0, mhartid		# read current hart id
	mv	tp, t0			# keep CPU's hartid in its tp for later usage.
	bnez	t0, park		# if we're not on the hart 0
					# we park the hart
    # 读取mhartid寄存器，获取当前hart的id,并存储在寄存器t0中
    # 将t0的值也就是hartID移动到tp寄存器中
    # 如果t0不为0,也就是现在不是hart0则跳转到park标签暂停执行

    # 下面这一段需要初始化BSS段
	# Set all bytes in the BSS section to zero.
	la	a0, _bss_start 
    # 将BSS段的起始地址加载到寄存器a0
	la	a1, _bss_end
    # 将BSS段的终止地址加载到寄存器a1
	bgeu	a0, a1, 2f
1:
	sw	zero, (a0)
    # 将0值存到a0指向的地址
	addi	a0, a0, 4
    # 将a0寄存器的值增加4,移动到下一个4字节的地址
	bltu	a0, a1, 1b
    # 如果 a0小于a1,跳回标签1继续循环
2:
	# Setup stacks, the stack grows from bottom to top, so we put the
	# stack pointer to the very end of the stack range.
	slli	t0, t0, 10		# shift left the hart id by 1024
    # 将t0的值左移10位，相当于乘以1024,为每一个hart计算栈空间的偏移量
	la	sp, stacks + STACK_SIZE	# set the initial stack pointer
					# to the end of the first stack space
    # 将栈的起始地址加上每个栈的大小(STACK_SIZE)加载到栈指针sp
	add	sp, sp, t0		# move the current hart stack pointer
					# to its place in the stack space
    # 将sp寄存器的值加上t0的值，为当前hart设置正确的栈指针

	li	t0, 3 << 11 | 1 << 7 # 加载立即数到t0
	csrs	mstatus, t0 # 将t0的值设置到mstatus

	j	start_kernel		# hart 0 jump to c
    # 如果当前是主hart(hart0)，跳转到C语言编写的start_kernel函数继续执行

park:
    # 这是一个无限循环，wfi执行时cpu核心进入低功耗等待状态，直到中断
    # 如果遇到中断，会无条件跳转到park标签继续执行
	wfi
	j	park

	# In the standard RISC-V calling convention, the stack pointer sp
	# is always 16-byte aligned.
.balign 16
# 将后续数据对齐到16字节的边界
stacks:
	.skip	STACK_SIZE * MAXNUM_CPU # allocate space for all the harts stacks
    # skip用于保留空间，这里给每一个CPU保留了STACK_SIZE的空间
	.end				# End of file
