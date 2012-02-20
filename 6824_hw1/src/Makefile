CC=g++
CFLAGS=-g
MPICC=mpic++
LDFLAGS=-pthread

all: b c

b: b.cpp gen
	$(CC) $(CFLAGS) $(LDFLAGS)  $< -o b 

gen: gen.cpp gen.sh
	$(CC) $(CFLAGS) $(LDFLAGS) gen.cpp -o gen
	sh gen.sh

c: c.cpp
	$(MPICC) $(CFLAGS) $(LDFLAGS) $< -o c 

test: test.sh
	./test.sh 2>&1 | tee eval.txt
