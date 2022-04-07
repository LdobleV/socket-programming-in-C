CC=gcc
CFLAGS=-I -gstabs -Wall

all: commonFunctions clFtp srvFtp

clFtp: clFtp.c commonFunctions.o
	gcc -o clFtp clFtp.c commonFunctions.o

srvFtp: srvFtp.c commonFunctions.o
	gcc -o srvFtp srvFtp.c commonFunctions.o

commonFunctions: commonFunctions.c commonFunctions.h
	gcc -c -o commonFunctions.o commonFunctions.c