# Makefile for client and server

CC = gcc
OBJCLI = client.c 
OBJSRV = server.c
CFLAGS = -Wall 
# setup for system
LIBS =

all:  ftpc ftps 

ftpc:	$(OBJCLI)
	$(CC) $(CFLAGS) -o $@ $(OBJCLI) $(LIBS)

ftps:	$(OBJSRV)
	$(CC) $(CFLAGS) -o $@ $(OBJSRV) $(LIBS)

clean:
	rm cli srv
