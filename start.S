# include "platform.h"

    # 只有大写的"*.S"文件支持include,其他的汇编文件不支持
    # size of each hart's stack is 1024 bytes
    .equ STACK_SIZE , 1024

    .global _start

    .text

_start:
    # park harts with id != 0
    csrr    t0,mhartid      # read current hart id 
    mv  tp, t0              # keep CPU's hartid in its tp for later uasge.
    bnez    t0  ,park       # if we're not on the hart 0
                            # we park the hart
    # Setup stacks, the stack grows from botton to top ,so we put the
    # stack pointer to the very end of the stack range.
    slli    t0, t0, 10      # 
    la  sp, stacks + STACK_SIZE # 
                    # 
    add sp, sp, t0         #
                    #

    j   start_kernel        # hart 0 jump to c

park:
    wfi
    j   park

.balign 16
stacks:
    .skip    STACK_SIZE * MAXNUM_CPU #
    .end 