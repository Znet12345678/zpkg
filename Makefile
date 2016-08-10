all:
	cc -c src/zpkg.c -o objs/zpkg.o -Werror -std=gnu99 -Wall
	cc -c src/main.c -o objs/main.o
	cc objs/zpkg.o objs/main.o -o zpkg
