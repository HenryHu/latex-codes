#include <pthread.h>

#define MAXPROCS 64
pthread_t threads[MAXPROCS];

pthread_mutex_t com_mutex;

void* do_work(void* arg) {
  long id = (long)arg;
  for (int i=0; i<1000000; i++) {
    pthread_mutex_lock(&com_mutex);
    pthread_mutex_unlock(&com_mutex);
  }
}


int main(int argc, char **argv) {
  int nprocs = atoi(argv[1]);
  pthread_mutex_init(&com_mutex, NULL);
  for (int i=0; i<nprocs; i++) {
    pthread_create(&threads[i], NULL, do_work, i);
  }

  for (int i=0; i<nprocs; i++) {
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&com_mutex);
}

