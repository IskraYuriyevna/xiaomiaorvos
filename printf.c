#include "os.h"

/*
 * ref: https://github.com/cccriscv/mini-riscv-os/blob/master/05-Preemptive/lib.c
 */

static int _vsnprintf(char * out, size_t n, const char* s, va_list vl)
{
	int format = 0;//跟踪是否在处理占位符
	int longarg = 0;//跟踪是否将下一个参数作为long处理
	size_t pos = 0;//跟踪当前写入缓冲区的位置
	for (; *s; s++) {
		if (format) {
			switch(*s) {
			case 'l': {
				//long
				longarg = 1;
				break;
			}
			case 'p': {
				//pointer
				longarg = 1;
				if (out && pos < n) {
					out[pos] = '0';
				}
				pos++;
				if (out && pos < n) {
					out[pos] = 'x';
				}
				pos++;
			}
			case 'x': {
				//hex
				long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
				int hexdigits = 2*(longarg ? sizeof(long) : sizeof(int))-1;
				for(int i = hexdigits; i >= 0; i--) {
					int d = (num >> (4*i)) & 0xF;
					if (out && pos < n) {
						out[pos] = (d < 10 ? '0'+d : 'a'+d-10);
					}
					pos++;
				}
				longarg = 0;
				format = 0;
				break;
			}
			case 'd': {
				//dec
				long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
				if (num < 0) {
					num = -num;
					if (out && pos < n) {
						out[pos] = '-';
					}
					pos++;
				}
				long digits = 1;
				for (long nn = num; nn /= 10; digits++);
				for (int i = digits-1; i >= 0; i--) {
					if (out && pos + i < n) {
						out[pos + i] = '0' + (num % 10);
					}
					num /= 10;
				}
				pos += digits;
				longarg = 0;
				format = 0;
				break;
			}
			case 's': {
				//string
				const char* s2 = va_arg(vl, const char*);
				while (*s2) {
					if (out && pos < n) {
						out[pos] = *s2;
					}
					pos++;
					s2++;
				}
				longarg = 0;
				format = 0;
				break;
			}
			case 'c': {
				//char
				if (out && pos < n) {
					out[pos] = (char)va_arg(vl,int);
				}
				pos++;
				longarg = 0;
				format = 0;
				break;
			}
			default:
				break;
			}
		} else if (*s == '%') {
			format = 1;
		} else {
			if (out && pos < n) {
				//如果不是占位符，直接输出
				out[pos] = *s;
			}
			pos++;
		}
    	}
	if (out && pos < n) {
		//添加终止字符串
		out[pos] = 0;
	} else if (out && n) {
		//如果缓冲区已满，确保最后一个是空字符
		out[n-1] = 0;
	}
	return pos;
}

static char out_buf[1000]; // buffer for _vprintf()

static int _vprintf(const char* s, va_list vl)
{
	int res = _vsnprintf(NULL, -1, s, vl);
	//计算字格式化字符串的长度(res)
	if (res+1 >= sizeof(out_buf)) {
		uart_puts("error: output string size overflow\n");
		while(1) {}
	}
	//检查计算出的长度是否超出了预定义的缓冲区out_buf的大小
	//如果超出，输出错误信息并进入无限循环
	_vsnprintf(out_buf, res + 1, s, vl);
	//将格式化的字符串写入out_buf
	uart_puts(out_buf);
	//使用uart_puts将格式化后的字符串输出到串口
	return res;
}

int printf(const char* s, ...)
{
	int res = 0;
	va_list vl;
	//va_list 是一个列表，用于处理可变参数，允许函数在编译时不知道参数数量和类型
	va_start(vl, s);
	//使用宏vl指向可变参数列表
	res = _vprintf(s, vl);
	//将格式化字符串混合参数列表传递给_vprintf函数
	va_end(vl);//结束对可变参数的处理
	return res;
}

void panic(char *s)
//panic是一个常用的错误处理函数
//前三行用于打印错误信息，最后用一个死循环卡住程序，等待手动解决
//panic不应在可恢复的错误上使用
{
	printf("panic: ");
	printf(s);
	printf("\n");
	while(1){};
}
