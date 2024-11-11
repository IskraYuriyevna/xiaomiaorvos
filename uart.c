#include "types.h"
#include "platform.h"
// UART Universal Asynchronous Receiver and Transmitter
// 串行：按位发送和接收。波特率（baud rate）是每秒传输的二进制位数，单位为bps
// 异步：不需要时钟线，直接发送数据，但是需要约定通讯协议格式
// 全双工：可以同时进行收发两方向的数据传递

//UART的通讯协议：
//空闲位：总线处于空闲的信号线状态为‘1’
//起始位：先发出一个‘0’
//数据位，数据长度可以是5/6/7/8/9位，一般是8位，先低后高发送
//奇偶校验位，需要设置为无校验，奇校验，偶校验和始终为0或1
//停止位：数据结束标志，可以是1,1.5，2位的高电平

/*
 * The UART control registers are memory-mapped at address UART0. 
 * This macro returns the address of one of the registers.
 */
#define UART_REG(reg) ((volatile uint8_t *)(UART0 + reg))

/* 
 * Reference
 * [1]: TECHNICAL DATA ON 16550, http://byterunner.com/16550.html
 */

/*
 * UART control registers map. see [1] "PROGRAMMING TABLE"
 * note some are reused by multiple functions
 * 0 (write mode): THR/DLL
 * 1 (write mode): IER/DLM
 */
#define RHR 0	// Receive Holding Register (read mode)
#define THR 0	// Transmit Holding Register (write mode)
#define DLL 0	// LSB of Divisor Latch (write mode)
#define IER 1	// Interrupt Enable Register (write mode)
#define DLM 1	// MSB of Divisor Latch (write mode)
#define FCR 2	// FIFO Control Register (write mode)
#define ISR 2	// Interrupt Status Register (read mode)
#define LCR 3	// Line Control Register
#define MCR 4	// Modem Control Register
#define LSR 5	// Line Status Register
#define MSR 6	// Modem Status Register
#define SPR 7	// ScratchPad Register

/*
 * POWER UP DEFAULTS
 * IER = 0: TX/RX holding register interrupts are both disabled
 * ISR = 1: no interrupt penting
 * LCR = 0
 * MCR = 0
 * LSR = 60 HEX
 * MSR = BITS 0-3 = 0, BITS 4-7 = inputs
 * FCR = 0
 * TX = High
 * OP1 = High
 * OP2 = High
 * RTS = High
 * DTR = High
 * RXRDY = High
 * TXRDY = Low
 * INT = Low
 */

/*
 * LINE STATUS REGISTER (LSR)
 * LSR BIT 0:
 * 0 = no data in receive holding register or FIFO.
 * 1 = data has been receive and saved in the receive holding register or FIFO.
 * ......
 * LSR BIT 5:
 * 0 = transmit holding register is full. 16550 will not accept any data for transmission.
 * 1 = transmitter hold register (or FIFO) is empty. CPU can load the next character.
 * ......
 */
#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE  (1 << 5)

#define uart_read_reg(reg) (*(UART_REG(reg)))
#define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v))

void uart_init()
//初始化uart,设置波特率和奇偶校验位等东西
{   
    //这一段是关闭中段
    uart_write_reg(IER,0x00);

    //这一段是波特率
    uint8_t lcr = uart_read_reg(LCR);
    uart_write_reg(LCR, lcr | (1 << 7));
	uart_write_reg(DLL, 0x03);
	uart_write_reg(DLM, 0x00);

    //这一段是奇偶校验位
    lcr = 0;
    uart_write_reg(LCR,lcr | (3 << 0));
}

int uart_putc(char ch)
{
    while((uart_read_reg(LSR)&LSR_TX_IDLE)==0);
    return uart_write_reg(THR,ch);
}

void uart_puts(char *s)
{
    while(*s){
        uart_putc(*s++);
    }
}