#include "essentials.h"

//sem_t lock;
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

void process_connection(int connfd, FILE* fileptr, char* cliIP, int cliport)
{
    //sem_wait(&lock);

    fileptr = fopen("clients-factorial.txt", "a+");
	
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
	//fclose(fileptr);
    //sem_post(&lock);
}

int main()
{
	remove("clients-factorial.txt");

	int	listenfd, connfd;
	struct sockaddr_in servaddr, cliaddr;
    pid_t childpid;
	int len = sizeof(SA);
	FILE *fileptr = fopen("clients-factorial.txt", "a+");


	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

    //sem_init(&lock, 1, 1);
	int count = 0;

	for ( ; ; ) 
	{
		time_t begin = time(NULL);
		count++;

		connfd = accept(listenfd, (SA *) &cliaddr, &len);
        
        if((childpid=fork())==0)
        {
            char *cliIP = inet_ntoa(cliaddr.sin_addr);
            int cliport = ntohs(cliaddr.sin_port);
            process_connection(connfd, fileptr, cliIP, cliport);
            exit(0);
        }
		if(count<=10)
		{
			time_t end = time(NULL);
			printf("time_spent: %f\n", (double)(end-begin));
		}
	}
	fclose(fileptr);
    //sem_destroy(&lock);
	close(connfd);
}