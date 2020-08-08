#ifndef __KERN_PROCESS_PROC_H__
#define __KERN_PROCESS_PROC_H__

#include <defs.h>
#include <list.h>
#include <trap.h>
#include <memlayout.h>


// process's state in his life cycle
enum proc_state {
    PROC_UNINIT = 0,  // uninitialized  未初始化
    PROC_SLEEPING,    // sleeping 休眠态
    PROC_RUNNABLE,    // runnable(maybe running)  待绪态/运行态
    PROC_ZOMBIE,      // almost dead, and wait parent proc to reclaim his resource  僵尸态
};

// Saved registers for kernel context switches.
// Don't need to save all the %fs etc. segment registers,
// because they are constant across kernel contexts.
// Save all the regular registers so we don't need to care
// which are caller save, but not the return register %eax.
// (Not saving %eax just simplifies the switching code.)
// The layout of context must match code in switch.S.
// 上下文，存储所有的通用寄存器
struct context {
    uint32_t eip;
    uint32_t esp;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
};

#define PROC_NAME_LEN               15
#define MAX_PROCESS                 4096
#define MAX_PID                     (MAX_PROCESS * 2)

extern list_entry_t proc_list;

struct proc_struct {
    enum proc_state state;                      // Process state  进程状态
    int pid;                                    // Process ID  进程id，一起标识进程
    int runs;                                   // the running times of Proces
    uintptr_t kstack;                           // Process kernel stack  内核栈，对于内核线程，就是执行用的内存堆栈，对于用户进程，是保留中断硬件信息的堆栈
    volatile bool need_resched;                 // bool value: need to be rescheduled to release CPU?  可被调度标志
    struct proc_struct *parent;                 // the parent process  父进程
    struct mm_struct *mm;                       // Process's memory management field 内存映射的信息，包括内存映射列表和页表首地址
    struct context context;                     // Switch here to run process 进程切换的上下文，主要是一些通用寄存器，用于保存下一条被执行的指令地址
    struct trapframe *tf;                       // Trap frame for current interrupt  中断帧，发生特权级切换的时候保留的一些寄存器信息
    uintptr_t cr3;                              // CR3 register: the base addr of Page Directroy Table(PDT) 页表首地址，因为内核线程共用ucore的页表首地址，所以内核线程的等于boot_cr3
    uint32_t flags;                             // Process flag
    char name[PROC_NAME_LEN + 1];               // Process name
    list_entry_t list_link;                     // Process link list 
    list_entry_t hash_link;                     // Process hash list
};

#define le2proc(le, member)         \
    to_struct((le), struct proc_struct, member)

extern struct proc_struct *idleproc, *initproc, *current;

void proc_init(void);
void proc_run(struct proc_struct *proc);
int kernel_thread(int (*fn)(void *), void *arg, uint32_t clone_flags);

char *set_proc_name(struct proc_struct *proc, const char *name);
char *get_proc_name(struct proc_struct *proc);
void cpu_idle(void) __attribute__((noreturn));

struct proc_struct *find_proc(int pid);
int do_fork(uint32_t clone_flags, uintptr_t stack, struct trapframe *tf);
int do_exit(int error_code);

#endif /* !__KERN_PROCESS_PROC_H__ */

