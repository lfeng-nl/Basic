# 本文介绍unix环境编程相关知识

> cpu工作原理,自动取址执行-->管理cpu就是设置PC值-->多道程序，交替执行 --> 并发 -->  如何实现并发？（切换PC、记录返回地址) --> PCB记录信息（切出去时程序执行的上下文：进程标识符、处理机的信息（通用寄存器，指令计数器，PSW，用户的栈指针、进程调度信息（进程状态，进程的优先级...)、进程控制信息（进程同步和通信机制，链接指针...)
>
> 中断->将寄存器、PC、程序状态字、等压栈 --> 跳转到中断向量所指示的地址
>
> 进程：中断->保存上下文到PCB--> 找到目标PCB  --> 恢复PCB中信息 --> 跳转执行
>
> 进程包括一个地址空间和一个控制线程；
>
> 1.并行实体共享一个地址空间和所有可用数据的能力；2.线程更轻，比进程快10~100倍；3.加快程序执行的速度；
>
> 线程：每个线程中的内容：程序计数器，寄存器，**堆**，状态，

## 一.进程

进程定义：一个其中运行着一个或多个线程的地址空间和这些线程所需的系统资源；

进程有自己的栈空间，用于保存函数中的局部变量和控制函数的调用和返回，进程之间会共享函数程序代码和系统函数库；

### 1.进程环境

- `main(int argc, char *argv[])`函数：`argc`参数个数，`argv`参数列表`argv[0]`为函数名；
- 进程表：保存当前进程的所有相关信息，包括PID，进程状态，命令字符串，和其他一些`ps`命令输出的各类信息。


- 进程终止：`exit()`先执行清理处理（用户登记的清理函数，IO库的清理和关闭），然后返回内核；`_exit()`和`_Exit()`立即进入内核；`atexit()`登记处理函数，执行时按登记顺序的逆序执行；

  > 在`main()`中调用`exit(0)`等价于`return(0)` ；

- 环境表：每个进程都会有一张环境表，包含为这个进程建立的环境变量；全局变量`environ`包含了该指针数组的地址；如需使用则需要声明`extern char **environ;`一般用户可以用`getenv()`和`putenv()`使用特定的环境变量；

  - `putenv(char *str)` ：传入`name=value`的字符串；
  - `setenv(const char *name, const char *value, int overwrite)` ：overwrite是否重写；

- 程序空间布局，从低地址向上依次为：

  - **text**：文本段，只读，CPU执行的机器指令，共享的，存储器中通常只有一个副本；
  - **data**：数据段，存储初始化后全局变量，
  - **bss**：未初始化数据段，
  - **堆**：动态存储分配；
  - **栈**：自动变量，函数调用信息；
  - **命令行参数和环境变量**：

  >函数参数入栈顺序：从右到左；
  >
  >栈帧：函数调用时，将参数、返回地址(当前PC值的下一个值)压栈，跳转到被调函数`func()`中；跳转后`func()`的栈栈就已经形成，首先会把调用者的帧指针压栈；

- 共享库：静态库( .a)和动态库( .so);

  - `dlopen()` ：打开动态链接库，返回一个句柄给调用进程；
  - `dlsym()` ：通过**句柄**和**连接符号名称**获取函数名或变量名；
  - `dlclose()` ：将已装载库引用计数减一，当减至零时，库被卸载；

- 存储空间分配：`malloc(),free()` , `calloc()`空间中每位初始化为0；

- 查询和更改资源限制：`getrlimit()` ：

### 2.进程控制

- 进程标识：每个进程都有一个非负的唯一进程ID，类型`pid_t` ；`getpid(),getppid()`

#### a.创建

- 一个现有的进程调用`fork()`创建一个新的进程，子进程为父进程的一个副本（拷贝）：

  ```c
  pid_t fork(void);             	//子进程返回0，父进程返回子进程ID，出错返回-1
  pid_t vfork(void);				//创建子进程，并阻塞父进程直到子进程终止exit()或调用exec();
  ```

#### b.进程退出

- 进程有五种退出方式：1、`main`中`return`,2、调用`exit()`，3、调用`_exit()`， 4，进程最后一个线程中执行`return`语句，5、进程最后一个线程调用`pthread_exit()`;

- 当进程终止时，内核会向其父进程发送`SIGCHLD`信号，终止进程的父进程都能通过`wait()`或`waitpid()` 取得其终止状态；

  ```c
  pid_t wait(int *status);		// 调用者阻塞，直到有一个子进程终止，如果有僵死子进程也立刻返回；
  pid_t waitpid(pid_t pid, int *status, int options);		// 等待指定的进程，可以不阻塞等待；
  int waitid(idtype_t idtype, id_t id, siginfo_t *info, int options);
  /*
  	status 为返回状态，可以用<sys/wait.h>中定义的宏来查看具体内容：
  		WIFEXITED(status):正常终止为真，可以用WEXITSTATUS(status) 获取exit()返回值；
  		WIFSIGNALED(status):
  		WIFSTOPPED(status):
  		WIFCONTINUED(status):
  */
  ```

- 当子进程终止退出时，父进程并未对其发出的`SIGCHLD`信号进行适当处理，导致子进程停留在僵死状态等待其父进程为其收尸，这个状态下的子进程就是僵死进程；

#### c.进程的使用

- 竞争：当多个进程都企图对共享数据进行某种处理，而结果又取决于进程运行的顺序时，我们认为发生了竞争条件；

- `exec()`函数：包括一系列函数：调用一个程序，完全取代该进程；

  | 函数         | pathname | filename |  参数表  | argv[] | anviron | envp[] |
  | ---------- | :------: | :------: | :---: | :----: | :-----: | :----: |
  | `execl()`  |    *     |          |   *   |        |    *    |        |
  | `execlp()` |          |    *     |   *   |        |    *    |        |
  | `execle()` |    *     |          |   *   |        |         |   *    |
  | `execv()`  |    *     |          |       |   *    |    *    |        |
  | `execvp()` |          |    *     |       |   *    |    *    |        |
  | `execve()` |    *     |          |       |   *    |         |   *    |
  | 名字中的字母     |          |    p     | l参数列表 | v参数数组  |         |   e    |

  > path 和 file：如果filename中含有`/`就将其视为路径（pathname）
  >
  > 只有`execve()`是内核系统调用，其余只是库函数，

- `system()`：允许调用一个一个shell命令；

#### d.进程的调度

- 典型的进程调度算法有：
  - 先来先去，FCFS（first come，first service）
  - 短作业（进程）优先，SJP
  - 轮转法
  - 多级反馈队列
- 可以通过调整nice`值来调整调度优先级，值越小，优先级越高；
- 进程时间：也称CPU时间，度量进程使用CPU资源，以时钟滴答计算，类型为`clock_t` ，UNIX系统为进程维护了3个进程时间时，时钟时间、用户CPU时间、系统CPU时间；
  - 时钟时间：又称墙上时钟时间，进程运行的时间总量；
  - 用户CPU时间：执行用户指令所用的时间量，
  - 系统CPU时间：执行内核程序所用时间。
  - 可以用`times(struct tms *buf);` 获取，结构体中包含本身和子进程的用户CPU时间和系统CPU时间；

### 3.进程间通信

#### a.管道

#### b.FIFO

#### c.消息队列

#### d.信号量

#### e.共享内存

## 二.信号



## 三.线程  

每个线程都包含有表示执行环境所需的信息，其中包括：**线程ID，一组寄存器值，栈，调度优先级和策略，信号屏蔽字，errno变量，线程私有数据**；一个进程的所有信息，对该进程的所有线程都是共享的，包括**可执行程序的代码，程序的全局内存和堆内存，栈以及文件描述符号**；

> 可以用==`#ifdef _POSIX_THREADS`==在编译时测试系统是否支持线程；

### 1.线程的生生死死

- 线程ID：用`pthread_t`类型表示，不同的操作系统实现不同，有的是**`struc`**，有的是**`long int`**；*linux中是无符号长整形*；

  ```c
  #include <pthread.h>
  int pthread_equal(pthread_t tid1, pthread_t tid2);			//比较两个线程id是否相同；
  pthread_t pthread_self(void);								//返回自己的线程id；
  ```

- 线程的创建：线程创建时并不能保证那个线程会先运行；

  > 进程的创建和终止都能通过`void*`传递的值不止一个，需要注意的是，这个结构体所使用的内存在调用者完成调用以后必须仍然是有效的。可以使用全局结构或者`malloc`函数分配结构

  ```c
  #include <pthread.h>
  int pthread_create(pthread_t *restrict tid, const pthread_attr_t *restrict attr, void *(* start_rtn)(void *), void *restrict arg);
  // tid：用于存放创建进程的进程ID
  // attr：定制线程属性
  // start_rtn：新线程从 start_rtn 函数地址开始运行； 
  // arg：函数参数列表
  // 返回值：成功，返回0，失败，返回错误代码；
  /* restrict：是c99标准引入的，它只用于限定和约束指针，并表明指针是访问一个数据对象的唯一且初始的方式.即它告诉编译器，所有修改该指针所指向内存中内容的操作都必须通过该指针来修改,而不能通过其它途径(其它变量或指针)来修改;
  */
  ```

- 线程终止

  如果进程中的任意线程调用了`exit、_Exit、_exit`，**整个进程就会终止**；线程；

  线程可以通过以下三种方式终止：1.从启动例程中返回；2.被同进程的其他线程取消；3.线程调用`pthread_exit()`

  > exit：会调用**终止处理程序**、**标准I/O清理程序(刷新缓冲区)**，最后调用`_exit`或`_Exit`；
  >
  > `_exit(),_Exit()`：立即终止调用的进程，关闭文件描述符号；
  >
  > 在`main()`中执行`return`，相当于调用`exit()`；
  >
  - `pthread_exit()`：

    ```c
    void pthread_exit(void *rval_ptr);			//终止调用的线程，并返回一个值存放于rval_ptr中
    int pthread_join(pthread_t thread, void **rval_ptr);		//调用线程阻塞，等待指定进程终止，rval_ptr包含返回信息，或终止信息（如目标线程被取消，放置PTHREAD_CANCELED）；如果多个线程同时链接到一个指定线程，结果未定义；
    // void **rval_ptr; 需要一个指针存放返回信息，这里传入指针的地址；
    ```

  - `pthread_cancel()`

    ```c
    int pthread_cancel(pthread_t tid);			//终止指定的线程，使进程返回一个PTHREAD_CANCELED
    ```

  - 进程退出时：

    ```c
    void pthread_cleanup_push(void (*rtn)(void *), void *arg);		//设置一个进程退出时需要执行的函数
    void pthread_cleanup_pop(int execute);			//从栈中删除一个清理函数，execue=0，清理函数不会被调用；
    ```

    当线程执行以下动作是，清理函数被调用：

    - 线程调用`pthread_exit()`;
    - 线程响应了`pthread_cancel()`取消请求；
    - 用非零的`execute`调用`pthread_cleanup_pop()`时；

  - 线程的分离

    默认情况，线程的终止状态会一直保存直到对该线程调用`pthread_join()`；或用`pthread_detach()`对线程分离，分离后，线程的资源可以在线程终止时立刻被回收；分离后的线程不能在被`pthread_join()`;

    ```c
    int pthread_detach(pthread_t tid);
    ```

    > 进程线程相关函数对比
    >
    > | 进程          | 线程                       | 描述           |
    > | ----------- | ------------------------ | ------------ |
    > | `fork()`    | `pthread_create()`       | 创建           |
    > | `exit()`    | `pthread_exit()`         | 主动退出         |
    > | `waitpid()` | `pthread_join()`         | 等待，获得别人的退出状态 |
    > | `atexit()`  | `pthread_cleanup_push()` | 设置清理函数       |
    > | `getpid()`  | `pthread_self()`         | 获得自己的ID      |
    > | `abort()`   | `pthread_cancel()`       | 请求其他人退出      |

### 2.线程同步

#### a.互斥量mutex

对互斥量加锁以后，其他任何试图再次对互斥量加锁的线程都会被**阻塞**直到当前线程释放该互斥量；互斥量为`pthread_mutex_t`结构体，使用前必须初始化，也可以设置为常量`PTHREAD_MUTEX_INITIALIXZER`

- 互斥量初始化和销毁

  ```c
  #inlcude <pthread.h>
  /*
  	mutex:初始化的互斥量
  	attr：互斥量的属性
  */
  int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *attr);
  int pthread_mutex_destroy(pthread_mutex_t *mutex);
  ```

- 互斥量的上锁解锁

  ```c
  int pthread_mutex_lock(pthread_mutex_t *mutex);				// 上锁，不成功则阻塞
  int pthread_mutex_trylock(pthread_mutex_t *mutex);			// 上锁，不成功不阻塞
  int pthread_mutex_unlock(pthread_mutex_t *mutex);			// 解锁
  int pthread_mutex_timedlock(pthread_mutex_t *restrict mutex,
                 const struct timespec *restrict tsptr);		// 上锁，阻塞到指定（绝对）时间
  															// 成功返回0，失败，返回错误编号
  ```

#### ｂ.死锁

- 如果一个程序试图对一个互斥量加锁两次，那么它自身就会陷入死锁状态；
- 如果一个以上的互斥量时，如果允许一个程序一直占有第一个互斥亮并且在试图锁住第二个互斥量时处于阻塞状态，但是拥有第二个互斥量的进程也试图锁住第一个互斥量。**两个线程都试图获取对方占有的资源，就产生了死锁**
- 避免方式：
  - 控制加锁顺序
  - 使用`pthread_mutex_trylock()`，避免进程阻塞；如果加锁不成功，清理资源，过段时间在试；

#### ｃ.读写锁

类似互斥锁，但比互斥更高效；读写锁有三种模式：读模式锁，写模式锁，不加锁；可以有多线程同时占用读锁，但只有一个线程能占用写锁；读写锁类型为：`pthread_rwlock_t`

- 初始化和销毁

  ```c
  #include <pthread.h>
  int pthread_rwlock_init(pthread_rwlock_t * lock, const pthread_rwlockattr_t *attr);
  int pthread_rwlock_destroy(pthread_rwlock_t *relock);
  ```

- 上锁解锁

  ```c
  int pthread_rwlock_rdlock(pthread_rwlock_t * lock);		// 上读锁，
  int pthread_rwlock_wrlock(pthread_rwlock_t * lock);		// 上写锁
  int pthread_rwlock_unlock(pthread_rwlock_t * lock);		// 解锁
  int pthread_rwlock_tryrdlock(pthread_rwlock_t * lock);
  int pthread_rwlock_trywrlock(pthread_rwlock_t * lock);
  int pthread_rwlock_timedrdlock(pthread_rwlock_t * lock, const struct timespec *tsptr);
  int pthread_rwlock_timedwrlock(pthread_rwlock_t * lock, const struct timespec *tsptr);
  ```

#### ｄ.自旋锁

自旋锁与互斥量类似，但是不通过休眠使进程阻塞，而是在获取锁之前一直处于忙等（自旋）阻塞状态；可以用于锁持有时间短，且线程不希望在重新调度上花时间；在锁空闲时立刻获得；

- 初始化和销毁

  ```c
    int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
    int pthread_spin_destroy(pthread_spinlock_t *lock);
  ```

- 上锁解锁

  ```c
    int pthread_spin_lock(pthread_spinlock_t *lock);
    int pthread_spin_trylock(pthread_spinlock_t *lock);
    int pthread_spin_unlock(pthread_spinlock_t *lock);
  ```

#### e.屏障

屏障（barrier）是用户协调多个线程并行执行工作的同步机制；屏障允许每个线程等待，直到所有的合作线程都到达某一点，然后从该点继续执行；

- 初始化和销毁

  ```c
  int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count);
  ```

## 四.Socket

## 五.高级IO

