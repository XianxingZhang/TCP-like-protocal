#Makefile for driver and timer

CC=gcc
OBJDRI = driver.c
OBJTIM = timer.c
CFLAGS = -Wall

#setup for system
LIBS=

all:	driver	timer

driver:	$(OBJDRI)
	$(CC)	$(CFLAGS) -o $@ $(OBJDRI) $(LIBS)

timer:	$(OBJTIM)
	$(CC)	$(CFLAGS) -o $@ $(OBJTIM) $(LIBS)

clean:	
	rm driver timer
