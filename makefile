#makefile for sockets project
all: clFtp srvFtp
clFtp: clFtp.o
	gcc -o clFtp clFtp.o
srvFtp: srvFtp.o
	gcc -o srvFtp srvFtp.o
clFtp.o: clFtp.c
	gcc -c clFtp.c
srvFtp.o: srvFtp.c
	gcc -c srvFtp.c