#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/wait.h>

#define QUEUE_NAME "/my_msg_queue"
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_PRIORITY 1

int main(int argc, char *argv[])
{
    mqd_t mq;
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = MAX_MESSAGES,
        .mq_msgsize = MAX_MSG_SIZE,
        .mq_curmsgs = 0,
    };


    int error;
    mq_unlink(QUEUE_NAME);
    mq = mq_open(QUEUE_NAME, O_CREAT|O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1)
    {
        printf("mq_open error \n");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        // 子进程
        char buf[MAX_MSG_SIZE];
        unsigned int priority;
        ssize_t bytes_read;
        
        if (bytes_read == -1) {
            printf("read error\n");
        }
        printf(" [Child] waitting for message ...\n");
        bytes_read =  mq_receive(mq, buf, MAX_MSG_SIZE, &priority);
        if (bytes_read >= 0)
        {
            buf[bytes_read] = '\0';
            printf("[child] Received %s \n", buf);
        }
    }
    else if (pid > 0)
    {
        // 父进程：发送者
        const char *message = "Hello from parent process!";
        printf("[Parent] Sending message: %s\n", message);
        if (mq_send(mq, message, strlen(message), MSG_PRIORITY) == -1)
        {
            printf("mq_send failed");
        }
        // 等待子进程处理完成
        wait(NULL);
    }
    mq_close(mq);          // 关闭队列
    mq_unlink(QUEUE_NAME); // 删除队列
    return 0;
}