#include "essentials.h"

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

int main(int argc, char **argv)
{
	remove("clients-factorial.txt");
	int	listenfd, connfd;
	struct sockaddr_in servaddr, cliaddr;
	int len = sizeof(SA);
	FILE *fileptr = fopen("clients-factorial.txt", "a+");

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	int count = 0;

	for ( ; ; ) 
	{
		time_t begin = time(NULL);
		count++;

		connfd = accept(listenfd, (SA *) &cliaddr, &len);

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
		if(count<=10)
		{
			time_t end = time(NULL);
			printf("time_spent: %f\n", (double)(end-begin));
		}
	}
	fclose(fileptr);
	close(connfd);
}