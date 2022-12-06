#include "essentials.h"
#include<limits.h>

FILE* fileptr;

ull get_factorial(int n)
{
	ull ans = 1;
	for(int i=2; i<=n; i++) ans*=i;
	return ans;
}

ull compute(int connfd)
{
	int val = 0;
	int x = recv(connfd, &val, sizeof(val), 0);

	if(x<=0) return x;

	ull factorial = get_factorial(val);

	send(connfd, &factorial, sizeof(factorial), 0);
	return factorial;
}

int main()
{
	remove("clients-factorial.txt");

	struct epoll_event ev, events[FOPEN_MAX];
	int action, epollfd, listenfd, connfd, sockfd;
	ssize_t	n;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	fileptr = fopen("clients-factorial.txt", "a+");

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	
	listen(listenfd, LISTENQ+1);
	
	epollfd = epoll_create1(0);
	if (epollfd == -1)
	{
	    printf("error with epoll_create1");
	    exit(-1);
	}

	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1)
	{
	    printf("error epoll_ctl: listen_sock");
	    exit(-1);
	}

	for ( ; ; )
	{
		time_t begin = time(NULL);
	    
		action = epoll_wait(epollfd, events, FOPEN_MAX, -1);
	    if (action == -1)
		{
	        printf("epoll_wait");
	        exit(-1);
	    }
	    for (int i = 0; i < FOPEN_MAX; i++)
		{
	        //if((events[i].events & EPOLLIN) == EPOLLIN)
			//{
				if (events[i].data.fd == listenfd)
				{
					clilen = sizeof(cliaddr);
					connfd = accept(listenfd, (SA *) &cliaddr, &clilen);
					if (connfd == -1)
					{
						printf("accept error");
						exit(-1);
					}

					ev.events = EPOLLIN | EPOLLET;
					ev.data.fd = connfd;

					if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1)
					{
						printf("epoll_ctl: connfd");
						exit(-1);
					}
				}
				else
				{
					//fileptr = fopen("clients-factorial.txt", "a+");
					char *cliIP = inet_ntoa(cliaddr.sin_addr);
                	int cliport = ntohs(cliaddr.sin_port);
					for ( ; ; )
                	{
						ull ret = compute(connfd);
						if(ret == 0) break;
						else if(ret == -1)
						{
							printf("errror with connection.");
							exit(-1);
						}
						else
						{
							fprintf(fileptr, "Client IP: %s ; Client Port: %d ; Factorial: %llu\n", cliIP, cliport, ret);
							fflush(fileptr);
						}
                	}
				}
			//}
		}
		time_t end = time(NULL);
		printf("time_spent: %f\n", (double)(end-begin));
	}
	close(sockfd);
	fclose(fileptr);
}