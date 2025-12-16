#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>

#define SHM_NAME "/my_shared_memory"
#define SHM_SIZE 1024

void *thread_function0(void *arg)
{
    int * data = (int *)arg;
    printf("Thread: Data received is %d\n", *data);
    pthread_exit((void *)123);
}

void *thread_function1(void *arg)
{
    int * data = (int *)arg;
    printf("Thread: Data received is %d\n", *data);
    pthread_exit((void *)123);
}

int main() 
{
    int *shared_data;
    pthread_mutex_t *mutex;
    int fd;

    // 创建共享内存
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        printf("shm_open fail");
        exit(1);
    }

    // 设置共享内存大小
    if (ftruncate(fd, SHM_SIZE) == -1) {
        printf("ftruncate fail");
        exit(1);
    }

    // 映射共享内存
    void *addr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        printf("mmap fail");
        exit(1);
    }

    // 初始化共享数据和互斥锁
    shared_data = (int *)addr;
    mutex = (pthread_mutex_t *)(addr + sizeof(int));
    *shared_data = 0;

    // 设置互斥锁属性
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

    // 初始化互斥锁
    if (pthread_mutex_init(mutex, &attr) != 0) {
        printf("pthread_mutex_init fail");
        exit(1);
    }

    // 创建子进程
    pid_t pid = fork();
    if (pid == 0) {
        // 子进程
        for (int i = 0; i < 1000; i++) {
            // 获取互斥锁
            pthread_mutex_lock(mutex);
            (*shared_data)++;
            printf("Child: shared_data = %d\n", *shared_data);
            // 释放互斥锁
            pthread_mutex_unlock(mutex);
            usleep(1);
        }
        exit(1);
    } else {
        // 父进程
        for (int i = 0; i < 1000; i++) {
            pthread_mutex_lock(mutex);
            (*shared_data)++;
            printf("Parent: shared_data = %d\n", *shared_data);
            pthread_mutex_unlock(mutex);
            usleep(1);
        }

        // 等待子进程结束
        wait(NULL);

        // 清理资源
        pthread_mutex_destroy(mutex);
        munmap(addr, SHM_SIZE);
        close(fd);
        shm_unlink(SHM_NAME);
    }

    return 0;
}






