#include "essentials.h"

void action(int sockfd)
{
	ull ans;

	printf("Sending 20 requests..........\n");
	for(int i=1; i<=20; i++)
	{
		send(sockfd, &i, sizeof(i), 0);
		recv(sockfd, &ans, sizeof(ans), 0);
		printf("Factorial of %d is %llu\n", i, ans);
	}
	printf("Done sending all requests........\n");
}

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2)
	{
		printf("Provide IP address of server.");
		exit(0);
	}

	for(int i=1; i<=10; i++)
	{
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("socket error");
			exit(0);
		}

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port   = htons(SERV_PORT);
		if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		{
			printf("inet_pton error for %s", argv[1]);
			exit(0);
		}

		if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		{
			printf("connect error");
			exit(0);
		}
		action(sockfd);

		close(sockfd);
	}
	exit(0);
}