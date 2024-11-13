# 这个文件中列举了需要编译的文件
SRCS_ASM = \
	start.S \
	mem.S \

SRCS_C = \
	kernel.c \
	uart.c \
	printf.c \
	page.c \

include ./common.mk