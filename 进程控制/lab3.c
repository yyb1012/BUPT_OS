#include<sys/types.h>
#include<string.h>
#include<unistd.h>
#include<wait.h>
#include<stdio.h>
#define BUFSIZE 256
int main()
{
    int fd1[2],fd2[2];
    if(pipe(fd1)==-1 || pipe(fd2) == -1)
    {
        perror("creat pipe failed\n");
        return -1;
    }

    pid_t pid=fork();
    if(pid < 0)
    {
        perror("Creat child process failed\n");
        return -1;
    }
    else if(pid > 0)//Parent process
    {
        char sendBuf1[BUFSIZE];
        char recvBuf1[BUFSIZE];
        printf("Parent send the origin message(Please input): ");
        scanf("%[^\n]",sendBuf1);
        //wirte ,close the read
        close(fd1[0]);
        write(fd1[1],sendBuf1,strlen(sendBuf1)+1);
        close(fd1[1]);
        //read,close the write
        close(fd2[1]);
		read(fd2[0], recvBuf1, BUFSIZE);
		printf("Parent get the message:%s\n", recvBuf1);
		close(fd2[0]);
    }
    else//Child process
    {
        char sendBuf2[BUFSIZE];
		char recvBuf2[BUFSIZE];
        //read,close the write
		close(fd1[1]);
		read(fd1[0], recvBuf2, BUFSIZE);
		printf("Child get the message:%s\n", recvBuf2);
		close(fd1[0]);

		for(int i = 0; i < strlen(recvBuf2); i++)
        {
			if(recvBuf2[i]>='a' && recvBuf2[i]<='z')
				recvBuf2[i] = recvBuf2[i]-32;
			else if(recvBuf2[i]>='A' && recvBuf2[i]<='Z')
				recvBuf2[i] = recvBuf2[i]+32;
		}
		strcpy(sendBuf2,recvBuf2);
        //write,close the read
		close(fd2[0]);
		printf("Child send the message:%s\n", sendBuf2);
		write(fd2[1], sendBuf2, strlen(sendBuf2)+1);
		close(fd2[1]);
    }
    return 0;
}