#include "platform.h"
/*
 * GNU ld 使用Linker Script来描述和控制链接过程
 * Linker Script是简单的纯文本文件，采用特定的脚本描述语言编写
 * gcc -T os.ld
 */

/*告诉编译器使用 -march=rv32ima 和 -mabi=ilp32*/
OUTPUT_ARCH("riscv")

/*设置入口点，也就是从什么地方开始整个程序，比如这里的_start,指的是start.S中的_start这个label*/
ENTRY(_start)

/*这一段用于描述目标机器上内存区域的位置，大小和相关
 *每一行可以认为定义了内存的一个区域，目前说的都是物理地址
 *ORIGIN可以写成org,LENGTH也可以写成l
 *rx就是说，默认将未描述放到哪块内存的只读文件节将被放到这个区域
 */
MEMORY
{
    ram (wxa!ri) : ORIGIN =0x80000000,LENGTH = LENGTH_RAM
}

/*
 *这一段告诉链接器如何将input sections映射到output sections 
 *以及如何将output sections放置在内存中
 */
SECTIONS
{
	/*
	 * We are going to layout all text sections in .text output section,
	 * starting with .text. The asterisk("*") in front of the
	 * parentheses means to match the .text section of ANY object file.
	 */
    /*
     *这段就是说.text这一节会被放到ram里面去
     */
	.text : {
		PROVIDE(_text_start = .);
		*(.text .text.*)
		PROVIDE(_text_end = .);
	} >ram

    /* 
     * PROVIDE段可以在Linker Script中定义符号(symbols)
     * 每个符号包括一个名字(name)和一个对应的地址值(address)
     * 在代码中访问这些符号等同于访问这个地址
     */
	.rodata : {
		PROVIDE(_rodata_start = .);
		*(.rodata .rodata.*)
		PROVIDE(_rodata_end = .);
	} >ram

	.data : {
		/*
		 * . = ALIGN(4096) tells the linker to align the current memory
		 * location to 4096 bytes. This will insert padding bytes until
		 * current location becomes aligned on 4096-byte boundary.
		 * This is because our paging system's resolution is 4,096 bytes.
		 */
		. = ALIGN(4096);
		PROVIDE(_data_start = .);
		/*
		 * sdata and data are essentially the same thing. We do not need
		 * to distinguish sdata from data.
		 */
		*(.sdata .sdata.*)
		*(.data .data.*)
		PROVIDE(_data_end = .);
	} >ram

	.bss :{
		/*
		 * https://sourceware.org/binutils/docs/ld/Input-Section-Common.html
		 * In most cases, common symbols in input files will be placed
		 * in the ‘.bss’ section in the output file.
		 */
		PROVIDE(_bss_start = .);
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		*(COMMON)
		PROVIDE(_bss_end = .);
	} >ram

	PROVIDE(_memory_start = ORIGIN(ram));
	PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));

	PROVIDE(_heap_start = _bss_end);
	PROVIDE(_heap_size = _memory_end - _heap_start);
}
