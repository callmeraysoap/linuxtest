#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>



int main(int argc, char *argv[])
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        printf("pipe error\n");
        return -1;
    }    
    

    pid_t pid = fork();
    if (pid == 0)
    {
        // 子进程
        char buf[100];
        close(pipefd[1]);
        int bytes_read = read(pipefd[0], buf, sizeof(buf));
        if (bytes_read == -1) {
            printf("read error\n");
        }
        printf(" child received = %s\n", buf);
        usleep(3000*1000);
        return 1;
    }
    else if (pid > 0)
    {
        // 父进程
        close(pipefd[0]);
        char *msg = "Hello World!";
        write(pipefd[1], msg, strlen(msg)+1);
        close(pipefd[1]);
        printf("father pid = %d child pid = %d\n", getpid(), pid);
        wait(NULL);
    }
    return 0;
}