#include <stdio.h>
#include <unistd.h>
// #include <sys/wait.h>
int main ()
{
    while(1){
        printf("hello world\n");
        usleep(5000 * 1000);
    }

    return 0;

}


