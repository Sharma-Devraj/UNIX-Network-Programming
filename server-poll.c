#include	"essentials.h"
#include	<limits.h>

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

    int i, maxi, listenfd, connfd, sockfd;
	int	action;
	ssize_t	n;
	socklen_t clilen;
	struct pollfd client[FOPEN_MAX];
	struct sockaddr_in cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;
	for (i = 1; i < FOPEN_MAX; i++) client[i].fd = -1;	
	maxi = 0;

	int count=0;

	for ( ; ; )
    {
		time_t begin = time(NULL);
		count++;
		action = poll(client, maxi+1, INT_MAX);

		if (client[0].revents & POLLRDNORM)
        {
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (SA *) &cliaddr, &clilen);

			for (i = 1; i < FOPEN_MAX; i++)
            {
				if (client[i].fd < 0)
                {
					client[i].fd = connfd;
					break;
				}
            }
			if (i == FOPEN_MAX)
            {
				printf("too many clients\n");
                exit(-1);
            }

			client[i].events = POLLRDNORM;
			if (i > maxi) maxi = i;				

            --action;
			if (action <= 0) continue;				
		}

		for (i = 1; i <= maxi; i++) 
        {	
			if ((sockfd = client[i].fd) < 0) continue;
			if (client[i].revents & (POLLRDNORM | POLLERR))
            {
                fileptr = fopen("clients-factorial.txt", "a+");
                
                char *cliIP = inet_ntoa(cliaddr.sin_addr);
                int cliport = ntohs(cliaddr.sin_port);
                for ( ; ; )
                {
                	ull ret = compute(sockfd);
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

				--action;
				if (action <= 0) break;
			}
		}
		if(count<=10)
		{
			time_t end = time(NULL);
			printf("time_spent: %f\n", (double)(end-begin));
		}
	}

	close(sockfd);
	fclose(fileptr);
}