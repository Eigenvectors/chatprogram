#include "unphead.h"

#define ERR_EXIT(m) \
	do \
	{ \
		perror(m); \
		exit(EXIT_FAILURE); \
	}while(0)

/*
void do_service(int conn)
{
	char recvbuf[1024];

	while(1)
	{
		bzero(recvbuf, sizeof(recvbuf));
		int ret = read(conn, recvbuf, sizeof(recvbuf));
		if(ret == 0)	//ret is zero represent the client close link forwardly
		{
			printf("client closed\n");
			break;
		}
		else if(ret == -1)
			ERR_EXIT("read");
		fputs(recvbuf, stdout);		//output the information that receive by parent process
		write(conn, recvbuf, ret);
	}
}
*/

void handler1(int sig)
{
	printf("Receive a signal %d\n", sig);
	printf("I am server, my receive parent process closed\n");
	exit(EXIT_SUCCESS);
}


void handler2(int sig)
{
	printf("Receive a signal %d\n", sig);
	printf("I am server, my send subprocess closed\n");
	exit(EXIT_SUCCESS);
}


int main(void)
{
	int listenfd;
	if((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	//if((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		ERR_EXIT("socket");

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	//bzero(&servaddr, sizeof(servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);	//open this port in server
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	//the ip is random in server
	//servaddr.sin_addr.s_addr = inet_addr("192.168.59.129"); 
	//inet_aton("127.0.0.1", &servaddr.sin_addr);

	int on = 1;
	if((setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0))	//all the tcp server should set the socket option
		ERR_EXIT("setsockopt");

	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		ERR_EXIT("bind");

	if(listen(listenfd, SOMAXCONN) < 0)
		ERR_EXIT("listen");

	struct sockaddr_in peeraddr;	//store the opposite's information in structure
	socklen_t peerlen = sizeof(peeraddr);	//socklen_t is same to uint32_t
	int conn;

	if((conn = accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen)) < 0)	//extract a connect from queue
		ERR_EXIT("accept");

	printf("Client ip =  %s  port = %d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
	printf("Server ip =  %s  port = %d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));

	pid_t pid;	//conn and pid only have one definition
	pid = fork();	//copy process think in multiprocess method
	if(pid == -1)
		ERR_EXIT("fork");
	else if(pid == 0)	//subprocess should do
	{
		signal(SIGUSR1, handler2);
		char sendbuf[1024] = {0};
		while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)	//ctrl + d represent eof
		{
			write(conn, sendbuf, strlen(sendbuf));
			bzero(sendbuf, sizeof(sendbuf));	//clean old date
		}
		printf("I am server, my send subprocess closed\n");
		kill(getppid(), SIGUSR1);
		exit(EXIT_SUCCESS);	//only exit this process ????????
		//close(conn);
		//close(listenfd);
	}
	else
	{
		signal(SIGUSR1, handler1);	//use the function name as function pointer
		char recvbuf[1024] = {0};
		while(1)
		{
			int ret = read(conn, recvbuf, sizeof(recvbuf));
			if(ret == -1)
				ERR_EXIT("read");
			else if(ret == 0)//ret is zero represent the opposite side's send process is exit. this process should do countermeasures
			{
				printf("I am server, the opposite side send parent process closed\n");
				break;
			}
			else
			{
				fputs(recvbuf, stdout);	//the terminal character wouldn't write into stdout
				bzero(recvbuf, sizeof(recvbuf));
			}
		}
		printf("I am server, my receive parent process closed\n");	//relation is important!!!!!!!!!!!!!!!!
		kill(pid, SIGUSR1);
		exit(EXIT_SUCCESS);	//I want to know the between close() and exit() detail
		//close(conn);
		//close(conn);
	}
	
//how to release the resource legally ?????????????	
	exit(EXIT_SUCCESS);
}
