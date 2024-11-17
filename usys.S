#include "syscall.h"
# 包含头文件，这里面提供了系统调用的标号1

.global gethid
gethid:
	# 将系统调用编号 SYS_gethid 加载到寄存器 a7 中。
	# 在 RISC-V 中，a7 通常用于传递系统调用编号。
	li a7, SYS_gethid
	# 触发系统调用
	ecall
	ret
 