#include<stdio.h>
#include<unistd.h>
void main(){
printf("The current process is %d\n",getpid());
printf("The parent process  is %d\n",getppid());
printf("The owner of this process has uid %d\n",getuid());
sleep(60);
}

