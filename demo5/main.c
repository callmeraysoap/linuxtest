#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
// #include <fcntl.h>
// #include <string.h>
#define PROGRAM1 "./hello"

pid_t pid;

void start_program(const char *program)
{
    pid = fork();
    printf("start pid %d\n", pid);
    if (pid == 0)
    {
        execl(program, program, (char *)NULL);
        printf("execl error\n");
        exit(1);
    }
    else if (pid < 0)
    {
        printf("fork error\n");
        exit(1);
    }

}

void monitor_program()
{
    while (1) {
        int status;
        pid_t result;
        usleep(10*1000);
        result = waitpid(pid, &status, WNOHANG);
        if (result == 0)
        {
           // printf("child program running\n");
        }   
        else if (result == -1) 
        {
            printf("waitpid error\n");
            exit(1);
        }     
        else 
        {
            printf("restart process\n");
            usleep(2*1000);
            start_program(PROGRAM1);
        } 
    }
}

int main(int argc, char *argv[])
{
    // printf("argc %d\n",argc);
    // printf("argv %s\n",argv[0]);
    // printf("argv %s\n",argv[1]);
    // printf("argv %s\n",argv[2]);
    // printf("argv %s\n",argv[3]);
    // 启动程序
    start_program(PROGRAM1);
    printf("start %s\n", PROGRAM1);
    // 监控程序
    monitor_program();
    return 0;
}



