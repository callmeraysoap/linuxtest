#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>



int main(int argc, char *argv[])
{
    const char *fifo_name = "/tmp/myfifo";
    int error;
    error = mkfifo(fifo_name, 0666);
    if (error == - 1) 
    {
        printf("mkfifo error\n");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        // 子进程
        int fd = open(fifo_name, O_RDONLY, 0666);
        if (fd == -1)
        {
            printf("open error\n");
            return 1;
        }
        char buf[100];
        int bytes_read = read(fd, buf, sizeof(buf));
        if (bytes_read == -1) {
            printf("read error\n");
        }
        printf(" child received = %s\n", buf);
        close(fd);
        usleep(3000*1000);
        return 1;
    }
    else if (pid > 0)
    {
        // 父进程
        int fd = open(fifo_name, O_WRONLY, 0666);
        if (fd == -1)
        {
            printf("open error\n");
            return 1;
        }
        char *msg = "Hello World!";
        write(fd, msg, strlen(msg)+1);
        printf("father pid = %d child pid = %d\n", getpid(), pid);
        close(fd);
        wait(NULL);//等待子进程
    }
    return 0;
}