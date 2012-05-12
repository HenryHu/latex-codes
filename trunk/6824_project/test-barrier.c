#define _XOPEN_SOURCE 600
#include <pthread.h>
#include <stdio.h>

#define MAXPROCS 32
pthread_t threads[MAXPROCS];
pthread_barrier_t barrier;

void* do_work(void* arg) {
    long id = (long)arg;
    for (int i=0; i<10000; i++) {
        pthread_barrier_wait(&barrier);
    }
}

int main(int argc, char **argv) {
    int nprocs = atoi(argv[1]);
    pthread_barrier_init(&barrier, NULL, nprocs);
    for (int i=0; i<nprocs; i++) {
        pthread_create(&threads[i], NULL, do_work, i);
    }
    for (int i=0; i<nprocs; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_barrier_destroy(&barrier);
}

