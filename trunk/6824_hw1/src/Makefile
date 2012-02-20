CC=g++
CFLAGS=-g
MPICC=mpic++
LDFLAGS=-pthread

all: b c

b: b.cpp
	$(CC) $(CFLAGS) $(LDFLAGS)  $< -o b 

c: c.cpp
	$(MPICC) $(CFLAGS) $(LDFLAGS) $< -o c 
