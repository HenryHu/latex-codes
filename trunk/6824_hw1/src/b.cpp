#include <pthread.h>
#include <cmath>
#include "stdint.h"
#include <algorithm>
#include "assert.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>

using namespace std;

void *worker(void *args_old)
{
  void **args = (void**) args_old;
  double **a = (double**)args[0];
  int k = (int)(uint64_t) args[1];
  int lower = (int)(uint64_t) args[2];
  int upper = (int)(uint64_t) args[3];
  int n = (int)(uint64_t) args[4];

  for (int i = lower; i <= upper; ++i) 
  {
    for (int j = k + 1; j < n; ++j)
      a[i][j] -= a[i][k] * a[k][j];
    a[i][k] = 0;
  }
  delete [] args;
}
void eliminate(double **a, int n, int np)
{
  for (int k = 0; k < n; ++k)
  {
    for (int j = k + 1; j < n; ++j) a[k][j] /= a[k][k];
    a[k][k] = 1;
    int p = min(np, n - k - 1); //  the number of max avaiable proc
    int block_size = (n - k - 2) / p + 1; //  get block size for each

    pthread_t *th = new pthread_t[p];

    for (int i = 0; i < p; ++i) 
    {
      void* *args = new void*[5];
      args[0] = (void*)a;
      args[1] = (void*)k;
      args[2] = (void*)(k + 1 + i * block_size);
      args[3] = (void*) min(
        k + 1 + (i + 1) * block_size,
        n - 1);
      args[4] = (void*)n;
      pthread_create(th + i, NULL, worker, (void*)args);
    }
    for (int i = 0; i < p; ++i)
      pthread_join(th[i], NULL);
    delete [] th;
  }
}

void usage(int argc, char *argv[])
{
  printf("Usage\n\t%s <# of threads>\n", argv[0]);
  exit(-1);
}

int main(int argc, char *argv[]) 
{
  int n;
  double **a; 
  int np;
  if (argc < 2)
    usage(argc, argv);
  np = atoi(argv[1]);
  cin >> n;
  a = new double*[n];
  for (int i = 0; i < n; ++i) a[i] = new double[n];
  for (int i = 0; i < n; ++i) for (int j = 0; j < n; ++j) cin >> a[i][j];
  eliminate(a, n, np); 
  for (int i = 0; i < n; ++i) 
  {
    for (int j = 0; j < n; ++j) printf("%.2lf ", a[i][j]);
    printf("\n");
  }
  return 0;
}
