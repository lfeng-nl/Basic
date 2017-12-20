# 本文介绍unix环境编程相关知识

## 一 .线程  

每个线程都包含有表示执行环境所需的信息，其中包括：**线程ID，一组寄存器值，栈，调度优先级和策略，信号屏蔽字，errno变量，线程私有数据**；一个进程的所有信息，对该进程的所有线程都是共享的，包括**可执行程序的代码，程序的全局内存和堆内存，栈以及文件描述符号**；

### 1.线程的生生死死

- 线程ID：用`pthread_t`类型表示，不同的操作系统实现不同，有的是结构体，有的是长整形；*linux中是无符号长整形*；

  ```c
  #include <pthread.h>
  int pthread_equal(pthread_t tid1, pthread_t tid2);			//比较两个线程id是否相同；
  pthread_t pthread_self(void);								//返回自己的线程id；
  ```

- 线程的创建：

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

#### a.互斥量

#### ｂ.死锁

#### ｃ.读写锁

#### ｄ.自旋锁