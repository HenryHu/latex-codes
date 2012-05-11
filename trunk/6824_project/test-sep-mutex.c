#include <pthread.h>

#define MAXPROCS 64
pthread_t threads[MAXPROCS];

pthread_mutex_t mutexs[MAXPROCS];

void* do_work(void* arg) {
  long id = (long)arg;
  for (int i=0; i<10000000; i++) {
    pthread_mutex_lock(&mutexs[id]);
    pthread_mutex_unlock(&mutexs[id]);
  }
}


int main(int argc, char **argv) {
  int nprocs = atoi(argv[1]);
  for (int i=0; i<nprocs; i++)
    pthread_mutex_init(&mutexs[i], NULL);
  for (int i=0; i<nprocs; i++) {
    pthread_create(&threads[i], NULL, do_work, i);
  }

  for (int i=0; i<nprocs; i++) {
    pthread_join(threads[i], NULL);
  }

  for (int i=0; i<nprocs; i++)
    pthread_mutex_destroy(&mutexs[i]);
}

