#include "essentials.h"

void action(int sockfd, int thread_id)
{
	ull ans;
    printf("For client no. %d\n", thread_id);
	printf("Sending 20 requests..........\n");
	for(int i=1; i<=20; i++)
	{
		send(sockfd, &i, sizeof(i), 0);
		recv(sockfd, &ans, sizeof(ans), 0);
		printf("Factorial of %d is %llu\n", i, ans);
	}
	printf("Done sending all requests........\n");
}

void* connection(void* thread_id)
{
	//pthread_detach(pthread_self);

    int sockfd;
    int id = (int)thread_id;
	struct sockaddr_in servaddr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error");
		return 0;
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(SERV_PORT);
	if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
	{
		printf("inet_pton error");
		return 0;
	}
	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
	{
		printf("connect error");
		return 0;
	}
	action(sockfd,id);
	close(sockfd);

    //pthread_exit(NULL);
	return 0;
}

int main()
{
    pthread_t thread_num;

    for(int i=1; i<=10; i++)
    {
        if(pthread_create(&thread_num, NULL, connection, (void*) i)<0)
        {
            printf("pthread error\n");
            exit(-1);
        }
		//sleep(1);
    }
    pthread_join(thread_num, NULL);
    pthread_exit(NULL);
    exit(0);
}
