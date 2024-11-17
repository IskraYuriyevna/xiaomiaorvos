#ifndef __USER_API_H__
#define __USER_API_H__

/* user mode syscall APIs */
//这是用户模式下的系统调用API
extern int gethid(unsigned int *hid);
//获取当前硬件线程的 ID 并将其存储在提供的指针指向的位置。

#endif /* __USER_API_H__ */
 