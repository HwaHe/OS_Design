.text
.globl kernel_thread_entry
kernel_thread_entry:        # void kernel_thread(void)  内核线程入口函数kernel_thread_entry的实现

    pushl %edx              # push arg  参数压栈
    call *%ebx              # call fn 调用fn

    pushl %eax              # save the return value of fn(arg) 返回值压栈
    call do_exit            # call do_exit to terminate current thread 调用do_exit退出当前进程

