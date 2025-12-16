#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

void *thread_function(void *arg)
{
    int * data = (int *)arg;
    printf("Thread: Data received is %d\n", data);
    pthread_exit((void *)123);
}

int main() 
{
    pthread_t thread;
    int data = 42;
    if (pthread_create(&thread, NULL, thread_function, (void *)&data) != 0) {
        printf("pthread_create fail");
        return 1;
    }
    void *retval;
    if (pthread_join(thread, &retval) != 0) {
        printf("pthread join fail");
        return 1;
    }
    printf("Main: Thread returned with value %d\n", (char *)retval);
}






