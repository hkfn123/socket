#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define ERR_EXIT(m) \
	do{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0)

void do_service(int accept_conn,pid_t pid);
void handler(int sig)
{
	printf("recv a sig=%d\n",sig);
	exit(EXIT_SUCCESS);
}

int main(int argc,char* argv[])
{
	signal(SIGCHLD,SIG_IGN);
	int sockfd = 0;
	
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		ERR_EXIT("socket() error.");

	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	/*servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	inet_aton("127.0.0.1",&servaddr,sin_addr);*/
	
	int on = 1;
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0)
		ERR_EXIT("sersockopt() error.");
	
	if(bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
		ERR_EXIT("bind() error.");
	
	if(listen(sockfd,SOMAXCONN) < 0)
		ERR_EXIT("listen() error.");

	struct sockaddr_in cliaddr;
	socklen_t cliaddr_len = sizeof(cliaddr);
	int accept_conn = 0;
	
	if((accept_conn = accept(sockfd,(struct sockaddr*)&cliaddr,&cliaddr_len)) < 0)
		ERR_EXIT("accept() error.");
		printf("recived connect ip:%s port:%d\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));


	pid_t pid;
	while(1)
	{
		pid = fork();
		if(pid == -1)
			ERR_EXIT("fork error");
		else if(pid == 0)
		{
			signal(SIGUSR1,handler);
			do_service(accept_conn,pid);
		}
		else
		{
			char sendbuf[1024] = {0};
			while(fgets(sendbuf,sizeof(sendbuf),stdin) != NULL)
			{
				int ret = write(accept_conn,sendbuf,sizeof(sendbuf));
				if(ret < 0)
					break;
				memset(sendbuf,0,sizeof(sendbuf));
			}

			kill(pid,SIGUSR1);
			close(sockfd);
			exit(EXIT_SUCCESS);
		}
	}

	return 0;
}

void do_service(int accept_conn,pid_t pid)
{
	char recvbuf[1024];
	while(1)
	{
		memset(recvbuf,0,sizeof(recvbuf));
		int ret = read(accept_conn,recvbuf,sizeof(recvbuf));
		if(ret == 0)
		{
			printf("client close\n");
			close(accept_conn);
			break;
		}
		else if(ret == -1)
			ERR_EXIT("read error");
		fputs(recvbuf,stdout);
	}
	exit(EXIT_SUCCESS);
}
