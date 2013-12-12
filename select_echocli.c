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

void do_echocli(int sockfd)
{
	fd_set rset;
	FD_ZERO(&rset);

	int nready;
	int maxfd;
	int fd_stdin = fileno(stdin);
	if(fd_stdin > sockfd)
		maxfd = fd_stdin;
	maxfd = sockfd;

	char sendbuf[1024] = {0};
	char recvbuf[1024] = {0};
	while(1)
	{
		FD_SET(fd_stdin,&rset);
		FD_SET(sockfd,&rset);

		nready = select(maxfd+1,&rset,NULL,NULL,NULL);
		if(nready == -1)
			ERR_EXIT("select error.");
		if(nready == 0)
			continue;

		if(FD_ISSET(sockfd,&rset))
		{
			int ret = read(sockfd,recvbuf,sizeof(sockfd));
			if(ret == -1){
				ERR_EXIT("read error");
			}else if(ret == 0)
			{
				printf("server close connection.\n");
				break;
			}
			fputs(recvbuf,stdout);
			memset(recvbuf,0,sizeof(recvbuf));
		}

		if(FD_ISSET(fd_stdin,&rset))
		{
			if(fgets(sendbuf,sizeof(sendbuf),stdin) == NULL)
				break;
			write(sockfd,sendbuf,strlen(sendbuf));
			memset(sendbuf,0,sizeof(sendbuf));
		}

	}
}

int main(int argc,char* argv[])
{
	int sockfd;

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		ERR_EXIT("socket error.");
	
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.109");

	if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
		ERR_EXIT("connect() error.");

	do_echocli(sockfd);
	close(sockfd);

	return 0;
}
