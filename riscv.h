#ifndef __RISCV_H__
#define __RISCV_H__

#include "types.h"
 
//这段头文件用于访问和操作RISC-V的控制状态寄存器(CSR)。这些寄存器在操作系统内核中用于管理硬件状态和处理中断
/*
 * ref: https://github.com/mit-pdos/xv6-riscv/blob/riscv/kernel/riscv.h
 */


static inline reg_t r_tp()
//读取tp寄存器（线程指针）的值
{
	reg_t x;
	asm volatile("mv %0, tp" : "=r" (x) );
	return x;
}

/* which hart (core) is this? */
static inline reg_t r_mhartid()
//读取 mhartid 寄存器的值，该寄存器存储当前硬件线程（hart）的 ID。
{
	reg_t x;
	asm volatile("csrr %0, mhartid" : "=r" (x) );
	return x;
}

//下面定义了mstatus寄存器中不同位的掩码，用于控制和查询处理器状态
/* Machine Status Register, mstatus */
#define MSTATUS_MPP (3 << 11)
#define MSTATUS_SPP (1 << 8)

#define MSTATUS_MPIE (1 << 7)
#define MSTATUS_SPIE (1 << 5)
#define MSTATUS_UPIE (1 << 4)

#define MSTATUS_MIE (1 << 3)
#define MSTATUS_SIE (1 << 1)
#define MSTATUS_UIE (1 << 0)

static inline reg_t r_mstatus()
//读取 mstatus 寄存器的值，该寄存器包含机器模式下的状态信息。
{
	reg_t x;
	asm volatile("csrr %0, mstatus" : "=r" (x) );
	return x;
}

static inline void w_mstatus(reg_t x)
//向 mstatus 寄存器写入值。
{
	asm volatile("csrw mstatus, %0" : : "r" (x));
}

/*
 * machine exception program counter, holds the
 * instruction address to which a return from
 * exception will go.
 */
static inline void w_mepc(reg_t x)
//向 mepc 寄存器写入值，该寄存器存储异常处理返回时的程序计数器地址。
{
	asm volatile("csrw mepc, %0" : : "r" (x));
}

static inline reg_t r_mepc()
//读取 mepc 寄存器的值。
{
	reg_t x;
	asm volatile("csrr %0, mepc" : "=r" (x));
	return x;
}

/* Machine Scratch register, for early trap handler */
static inline void w_mscratch(reg_t x)
//向 mscratch 寄存器写入值，该寄存器用于在上下文切换时保存临时数据。
{
	asm volatile("csrw mscratch, %0" : : "r" (x));
}

/* Machine-mode interrupt vector */
static inline void w_mtvec(reg_t x)
//向 mtvec 寄存器写入值，该寄存器定义了机器模式下的中断向量。
{
	asm volatile("csrw mtvec, %0" : : "r" (x));
}

/* Machine-mode Interrupt Enable */
#define MIE_MEIE (1 << 11) // external
#define MIE_MTIE (1 << 7)  // timer
#define MIE_MSIE (1 << 3)  // software

static inline reg_t r_mie()
//读取 mie 寄存器的值，该寄存器用于启用或禁用不同类型的中断。
{
	reg_t x;
	asm volatile("csrr %0, mie" : "=r" (x) );
	return x;
}

static inline void w_mie(reg_t x)
//向 mie 寄存器写入值。
{
	asm volatile("csrw mie, %0" : : "r" (x));
}

/* Machine-mode Cause Masks */
#define MCAUSE_MASK_INTERRUPT	(reg_t)0x80000000
#define MCAUSE_MASK_ECODE	(reg_t)0x7FFFFFFF

static inline reg_t r_mcause()
//读取 mcause 寄存器的值，该寄存器存储最近一次异常或中断的原因。
{
	reg_t x;
	asm volatile("csrr %0, mcause" : "=r" (x) );
	return x;
}

#endif /* __RISCV_H__ */
