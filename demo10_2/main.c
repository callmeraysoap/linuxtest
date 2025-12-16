#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_function(void *arg) {
    while (1) {
        printf("Thread: Running...\n");
        sleep(1);
    }
    pthread_exit(NULL); // 通常不会到达这里
}

int main() {
    pthread_t thread;

    if (pthread_create(&thread, NULL, thread_function, NULL) != 0) {
        printf("pthread_create fail");
        return 1;
    }

    sleep(3); // 让线程运行一段时间

    if (pthread_cancel(thread) != 0) {
        printf("pthread_cancel fail");
        return 1;
    }

    if (pthread_join(thread, NULL) != 0) {
        printf("pthread_join fail");
        return 1;
    }
    return 0;
}





