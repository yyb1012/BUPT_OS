#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main()
{
    printf("Please input: ");
    int num=0;
    scanf("%d",&num);
    const int SIZE = 1024;
    const char *name ="OS";  //共享内存的名字 
    const int BUFSIZE = 1024;  //共享内存的大小 
    int shm_fd=shm_open(name,O_CREAT | O_RDWR,0777);
    void *ptr=mmap(0,SIZE,PROT_WRITE,MAP_SHARED,shm_fd,0);
    ftruncate(shm_fd,SIZE);

    pid_t pid;
    pid = fork();
    if(pid < 0)
    {
        printf("Creat Child Process failed\n");
        return -1;
    }
    else if (pid == 0)
    {//child process
        char buffer[BUFSIZE];  //创建buffer存所有字符串 
        memset(buffer,0,sizeof(char)*BUFSIZE);//buffer为字符串，buffer[0]为字符串首字符 
        char *p_buffer = &buffer[0];//将 buffer[0]即字符串首字符的地址赋给p_buffer 

        p_buffer += sprintf(p_buffer,"%d,",num);  
        
        while (num != 1)
	    {
            if (num % 2 == 0)/*Even*/
	        {
                num = num / 2;
                if (num == 1)
                    p_buffer += sprintf(p_buffer,"%d\n",num);
                else
                    p_buffer += sprintf(p_buffer,"%d,",num);
            }
            else
	        {
                num = 3*num + 1;
                p_buffer += sprintf(p_buffer,"%d,",num);
            }
        }
        sprintf( (char *)ptr,"%s",buffer);  //buffer的地址给共享内存的地址ptr 
        printf("Child: Data has been written to shared memory\n");
    }
    else if(pid > 0)
    {
        wait(NULL);
        printf("Parent process is reading the shared memory\n");
        shm_fd = shm_open(name,O_RDONLY, 0777);
        ptr = mmap(0,SIZE,PROT_READ,MAP_SHARED,shm_fd,0);
        printf("%s",(char *)ptr);
        shm_unlink(name); //移除共享内存段 
    }
    return 0;
}

