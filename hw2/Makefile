all:serv.o cli.o user_list.o
	gcc -o serv -pthread -g serv.o user_list.o
	gcc -o cli cli.o
serv.o:serv.c 
	gcc -pthread -c serv.c
cli.o:cli.c
	gcc -c cli.c
user_list.o:user_list.c
	gcc -c user_list.c
