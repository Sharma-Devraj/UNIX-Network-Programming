#include "essentials.h"

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

    int	i, maxi, maxfd, listenfd, connfd, sockfd;
	int	action, client[FD_SETSIZE];
	ssize_t	n;
	fd_set rset, allset;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	maxfd = listenfd;
	maxi = -1;
	for (i = 0; i < FD_SETSIZE; i++) client[i] = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	int count = 0;
	
	for ( ; ; )
    {
		//rset = allset;
		time_t begin = time(NULL);
		count++;
		action = select(maxfd+1, &allset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &allset))
        {	
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (SA *) &cliaddr, &clilen);
            

			for (i = 0; i < FD_SETSIZE; i++)
            {
				if (client[i] < 0)
                {
					client[i] = connfd;
					break;
				}
            }

			if (i == FD_SETSIZE)
            {
				printf("too many clients");
                exit(-1);
            }

			FD_SET(connfd, &allset);

			if (connfd > maxfd) maxfd = connfd;
			if (i > maxi) maxi = i;

            --action;
			if (action <= 0) continue;
		}

		for (i = 0; i <= maxi; i++)
        {	
			if ((sockfd = client[i]) < 0) continue; //connfd
			if (FD_ISSET(sockfd, &allset))
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
        FD_CLR(sockfd, &allset);
	}
    close(sockfd);
	fclose(fileptr);
}