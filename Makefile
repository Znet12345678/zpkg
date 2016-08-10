all:
	cc -c src/zpkg.c -o objs/zpkg.o -Werror -std=gnu99 -Wall
	cc -c src/main.c -o objs/main.o
	cc -c src/extract.c -o objs/extract.o
	cc objs/zpkg.o objs/main.o -o zpkg
	cc objs/zpkg.o objs/extract.o -o zpkg-extract
