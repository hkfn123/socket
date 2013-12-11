#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define ERR_EXIT(m) \
	do{ \
		perror(m); \
		exit(EXIT_FAILURE); \
	}while(0);

int main(int argc,char* argv[])
{
	int sockfd;

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		ERR_EXIT("socket error.");
	
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = inet_addr("10.10.4.92");

	if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
		ERR_EXIT("connect() error.");

	pid_t pid;
	pid = fork();
	if(pid == -1)
		ERR_EXIT("fork error.");
	if(pid == 0)
	{
		char sendbuf[1024] = {0};
		while(fgets(sendbuf,sizeof(sendbuf),stdin) != NULL)
		{
			write(sockfd,sendbuf,strlen(sendbuf));
			memset(sendbuf,0,sizeof(sendbuf));
		}
	}
	else
	{
		while(1)
		{
			char recvbuf[1024] = {0};
			int ret = read(sockfd,recvbuf,sizeof(recvbuf));
			if(ret == 0)
			{
				printf("close by server.\n");
				break;
			}else if(ret == -1){
				printf("read error.\n");
			}else{
				fputs(recvbuf,stdout);
			}
		}
		close(sockfd);
	}

	return 0;
}
