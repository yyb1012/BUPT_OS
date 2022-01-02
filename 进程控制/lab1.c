#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<wait.h>
int main()
{
    int num=0;
    printf("Please inout the number: ");
    scanf("%d",&num);
    pid_t pid;
    pid=fork();
    if(pid==0)//child process
    {
	printf("Child Process, pid: %d\n",getpid());
        while(num!=1)
        {
            if(num%2==0)
            {
                num=num/2;
                printf("%d ",num);
            }
            else
            {
                num=3*num+1;
                printf("%d ",num);
            }
        }
	printf("\n");
    }
    else if(pid>0)//parent process
    {
	printf("Parent Process, pid: %d\n",getpid());
        wait(NULL);
        //printf("parent process complete\n");
    }
    return 0;
}
