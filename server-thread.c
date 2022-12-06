#include "essentials.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct client_info{
    char* IP;
    int port;
    FILE* ptr;
    int connfd;
};

ull get_factorial(int n)
{
	ull ans = 1;
	for(int i=2; i<=n; i++) ans*=i;
	return ans;
}

ull compute(struct client_info* in)
{
	int val = 0;
	int x = recv(in->connfd, &val, sizeof(val), 0);

	if(x<=0) return x;

	ull factorial = get_factorial(val);
	pthread_mutex_lock(&lock);
	fprintf(in->ptr, "Client IP: %s ; Client Port: %d ; Factorial: %llu\n", in->IP, in->port, factorial);
	fflush(in->ptr);
	pthread_mutex_unlock(&lock);

	send(in->connfd, &factorial, sizeof(factorial), 0);
	return factorial;
}

void* process_connection(void* info)
{
    
    struct client_info* in = (struct client_info*)info;
    //in->ptr = fopen("clients-factorial.txt", "a+");
	
	for ( ; ; )
	{
		ull ret = compute(in);
		if(ret == 0) break;
		else if(ret == -1)
		{
			printf("errror with connection.");
			exit(-1);
		}
		// else
		// {
			// pthread_mutex_lock(&lock);
			// fprintf(in->ptr, "Client IP: %s ; Client Port: %d ; Factorial: %llu\n", in->IP, in->port, ret);
			// fflush(in->ptr);
			// pthread_mutex_unlock(&lock);
		// }
	}
	//fclose(fileptr);

}

int main()
{
	remove("clients-factorial.txt");

	int	listenfd, connfd;
	struct sockaddr_in servaddr, cliaddr;
    pthread_t thread_num;
	int len = sizeof(SA);
	FILE *fileptr = fopen("clients-factorial.txt", "a+");


	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

    // if (pthread_mutex_init(&lock, NULL) != 0)
	// {
	//     printf("lock could not be initialized\n");
	//     exit(-1);
	// }

	int count = 0;

	for ( ; ; ) 
	{
		time_t begin = time(NULL);
		count++;

		connfd = accept(listenfd, (SA *) &cliaddr, &len);
        
        char *cliIP = inet_ntoa(cliaddr.sin_addr);
        int cliport = ntohs(cliaddr.sin_port);

        struct client_info* info = malloc(sizeof(struct client_info));
        info->IP = cliIP;
        info->port = cliport;
        info->ptr = fileptr;
        info->connfd = connfd;

        if(pthread_create(&thread_num, NULL, process_connection, (void *)info)<0)
        {
            printf("error in creating thread for client\n");
            exit(-1);
        }
		if(count<=10)
		{
			time_t end = time(NULL);
			printf("time_spent: %f\n", (double)(end-begin));
		}
    }
	fclose(fileptr);
    pthread_join(thread_num, NULL);
    pthread_mutex_destroy(&lock);
	close(connfd);
}