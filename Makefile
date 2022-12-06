all: sequential fork thread select poll epoll

sequential:
	gcc -o sequential-server sequential-server.c
	gcc -o sequential-client sequential-client.c

fork:
	gcc -pthread -lrt -o server-fork server-fork.c
	gcc -pthread -o concurrent-client concurrent-client.c

thread:
	gcc -pthread -lrt -o server-thread server-thread.c
	gcc -pthread -o concurrent-client concurrent-client.c

select:
	gcc -o server-select server-select.c
	gcc -pthread -o client-select client-select.c

poll:
	gcc -o server-poll server-poll.c
	gcc -pthread -o client-select client-select.c

epoll:
	gcc -o server-epoll server-epoll.c
	gcc -pthread -o client-select client-select.c

clean:
	rm -rf clients-factorial.txt concurrent-client server-fork sequential-client sequential-server
	rm -rf server-thread server-select server-poll client-select
