# ECE254 lab4 Linux Port
# Makefile: build mqueue sender and receiver example code

CC=gcc
CFLAGS=-Wall -g
LD=gcc
LDFLAGS=-g -lrt -lm


OBJS1=sender.o
OBJS2=main.o
OBJS3=receiver.o

all: produce sender.out receiver.out

sender.out: $(OBJS1)
	$(LD) -o $@ $(OBJS1)  $(LDFLAGS)

sender.o: sender.c
	$(CC) $(CFLAGS) -c  sender.c

produce: $(OBJS2)
	$(LD) -o $@ $(OBJS2)  $(LDFLAGS) -lm

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

receiver.out: $(OBJS3)
	$(LD) -o $@ $(OBJS3)  $(LDFLAGS) -lm

receiver.o: receiver.c
	$(CC) $(CFLAGS) -c receiver.c

.PHONY: clean
clean:
	rm -f *.o *.out produce
