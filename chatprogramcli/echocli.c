#include "unphead.h"

#define ERR_EXIT(m) \
	do \
	{ \
		perror(m); \
		exit(EXIT_FAILURE); \
	}while(0)
//learn how to define a function use macros

void handler1(int sig)
{
	printf("receive a signal %d\n", sig);
	printf("I am client, my send parent process is closed\n");
	exit(EXIT_SUCCESS);
}

void handler2(int sig)
{
	printf("receive a signal %d\n", sig);
	printf("I am client, my receive subprocess is closed\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char ** argv)
{
	int sockfd;
	if((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	//if((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
		ERR_EXIT("socket");

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	//bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);	//this is necessary ????
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	//servaddr.sin_addr.s_addr = inet_addr("192.168.59.129");
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	// inet_aton("127.0.0.1", &servaddr.sin_addr);

	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)	//servaddr is server's socket address structure
		ERR_EXIT("connect");

	pid_t pid;
	pid = fork();

	if(pid == -1)
		ERR_EXIT("fork");
	else if(pid == 0)
	{
		signal(SIGUSR1, handler2);
		char recvbuf[1024] = {0};
		while(1)
		{
			int ret = read(sockfd, recvbuf, sizeof(recvbuf));
			if(ret == -1)
				ERR_EXIT("read");
			else if(ret == 0)
			{
				printf("I am client, the opposite side's send subprocess is closed\n");
				break;
			}
			else
			{
				fputs(recvbuf, stdout);
				bzero(recvbuf, sizeof(recvbuf));
			}
		}
		printf("I am client, my receive subprocess is closed\n");
		kill(getppid(), SIGUSR1);
		//close(sockfd);
		exit(EXIT_SUCCESS);
	}
	else
	{
		signal(SIGUSR1, handler1);
		char sendbuf[1024] = {0};
		while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
		{
			write(sockfd, sendbuf, strlen(sendbuf));
			bzero(sendbuf, sizeof(sendbuf));
		}
	}
	printf("I am client, my send parent process is closed\n");
	kill(pid, SIGUSR1);
	//close(sockfd);
	exit(EXIT_SUCCESS);	//what is the different between close() and exit() ??????

	exit(0);
}
