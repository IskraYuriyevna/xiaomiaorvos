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

#ifdef CONFIG_SYSCALL
	# 对 QEMU 版本 6.0 及以上的适配，因为在这些版本中，如果没有配置 PMP 规则，会引发异常。因此，这段代码是一个临时的解决方案，直到 QEMU 的问题被解决。
	# PMP 配置允许对整个物理地址空间的访问，这在开发和测试阶段可能是有用的，但在生产环境中可能需要更精细的控制。
	# https://lore.kernel.org/qemu-devel/20201223192553.332508-1-atish.patra@wdc.com/
	# For qemu version >= 6.0, exception would be raised if no PMP enty is
	# configured. So just configure one entny, which allows all the whole
	# 32-bits physical address range is R/W/X.
	# FIXME: I say it is a temporary workaroud due to I think the patch
	# above contains bug, and I have raised new issue to qemu but it has not
	# been rootcaused till now. Details please refer to
	# https://gitlab.com/qemu-project/qemu/-/issues/585 or
	# https://gitee.com/unicornx/riscv-operating-system-mooc/issues/I441IC (in chinese)
	# So it's just a temporary workaround till now to not block people who
	# want to try newer qemu (>= 6.0).
	# 这些指令配置了 PMP（Physical Memory Protection）寄存器，以允许对整个 32 位物理地址空间的读写执行（R/W/X）访问。
	li      t0, 0xffffffff
	csrw    pmpaddr0, t0 # 将 t0 的值写入 pmpaddr0 寄存器，设置 PMP 规则的基址。
	li      t0, 0xf # 将立即数 0xf 加载到寄存器 t0，其中 0xf 表示允许读（R）、写（W）和执行（X）。
	csrw    pmpcfg0, t0 # 将 t0 的值写入 pmpcfg0 寄存器，设置 PMP 规则的配置。
#endif

	# At the end of start_kernel, schedule() will call MRET to switch
	# to the first task, so we parepare the mstatus here.
	# Notice: It is best not to assume that the initial value of mstatus is
	# zero.
#ifdef CONFIG_SYSCALL
	# Set mstatus.MPP as 0, so we will run in User mode after MRET.
	# No need to set mstatus.MPIE to 1 explicitly, because according to ISA
	# specification: interrupts for M-mode, which is higher than U-mode, are
	# always globally enabled regardless of the setting of the global MIE bit.
	li	t0, 3 << 11 # 这里 3 << 11 表示将 3 左移 11 位，设置 MSTATUS_MPP 为 3，即用户模式（User Mode）。
	csrc	mstatus, t0 # 这里 csrc 指令是清除并设置寄存器的指令，它清除 mstatus 寄存器的 MPP 位，并将 t0 的值写入，设置为用户模式。

#else
	li	t0, 3 << 11 | 1 << 7 # 加载立即数到t0
	csrs	mstatus, t0 # 将t0的值设置到mstatus

# endif
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
