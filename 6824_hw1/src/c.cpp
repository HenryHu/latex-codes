#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "mpi.h"

using namespace std;

#define TAG 0

#if 1
#define dprintf(...) ;
#else
#define dprintf printf
#endif

//  global variables;
double **a;   //  matrix
int n;        //  matrix size;
MPI_Status stat;

void main_proc(int myid, int numprocs)
{
  cin >> n;
  a = new double*[n];
  for (int i = 0; i < n; ++i) a[i] = new double[n];
  for (int i = 0; i < n; ++i) for (int j = 0; j < n; ++j) cin >> a[i][j];

  int wp = numprocs - 1;  //  # of worker proc

  //  send out the number of iterations
  for (int i = 0; i < wp; ++i)
    MPI_Send(&n, sizeof(n), MPI_CHAR, i + 1, TAG, MPI_COMM_WORLD);
    
  for (int k = 0; k < n; ++k)
  {
    for (int i = k + 1; i < n; ++i) a[k][i] /= a[k][k];
    a[k][k] = 1;

    int p = min(wp, n - k - 1); //  max # of working proc
    int block_size = !p ? 0 : (n - k - 2) / p + 1;
    dprintf("n = %d, wp = %d\n", n, wp);
    dprintf("row %d: p = %d, block_size = %d\n", k, p, block_size);

    //  decomposition && assignment
    for (int i = 0; i < wp; ++i)
    {
      int args[4];
      args[0] = (int)k;
      if (i < p)
      {
        args[1] = (int)(k + 1 + i * block_size);
        args[2] = (int) min(
          k + 1 + (i + 1) * block_size,
          n - 1);
      } else
        args[1] = args[2] = -1;
      args[3] = (int)n;

      dprintf("sending\n");
      MPI_Send(args, sizeof(args), MPI_CHAR, i + 1, TAG, MPI_COMM_WORLD);
      dprintf("sent\n");
      if (args[1] >= 0 && args[2] >= 0)
      {
        MPI_Send(a[k], sizeof(a[k][0]) * n, MPI_CHAR, i + 1, TAG, 
          MPI_COMM_WORLD);
        for (int j = args[1]; j <= args[2]; ++j)
          MPI_Send(a[j], sizeof(a[k][0]) * n, MPI_CHAR, i + 1, TAG, 
            MPI_COMM_WORLD);
      }
    }

    //  collect data
    for (int i = 0; i < wp; ++i)
    {
      int args[4];
      args[0] = (int)k;
      if (i < p)
      {
        args[1] = (int)(k + 1 + i * block_size);
        args[2] = (int) min(
          k + 1 + (i + 1) * block_size,
          n - 1);
      } else
        args[1] = args[2] = -1;
      args[3] = (int)n;

      if (args[1] >= 0 && args[2] >= 0)
      {
        for (int j = args[1]; j <= args[2]; ++j)
          MPI_Recv(a[j], sizeof(a[k][0]) * n, MPI_CHAR, i + 1, TAG, 
            MPI_COMM_WORLD, &stat);
      }
    }
  }
  for (int i = 0; i < n; ++i) 
  {
    for (int j = 0; j < n; ++j) printf("%.2lf ", a[i][j]);
    printf("\n");
  }
}

void worker_proc(int myid, int numprocs)
{
  int ite;
  bool flag = 0;
  double *x;
  double **y;
  MPI_Recv(&ite, sizeof(ite), MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
  dprintf("ID %d: receive ite = %d\n", myid, ite);

  while (ite--) {
    int args[4];
    MPI_Recv(args, sizeof(args), MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
    if (args[1] >= 0 && args[2] >= 0)
    {
      int k = args[0];
      int n = args[3];
      if (!flag)
      {
        x = new double[n];
        int m = n / (numprocs - 1) + 1;
        y = new double*[m];
        for (int i = 0; i < m; ++i) 
          y[i] = new double[n];
        flag = 1;
      }
      MPI_Recv(x, sizeof(x[0]) * n, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
      dprintf("ID %d: processing row %d to %d\n", myid, args[1], args[2]);
      for (int i = 0; i < args[2] - args[1] + 1; ++i)
        MPI_Recv(y[i], sizeof(x[0]) * n, MPI_CHAR, 0, TAG, 
          MPI_COMM_WORLD, &stat);
      dprintf("ID %d: obtained data from %d rows\n", myid, args[2] - args[1] + 1);
      for (int i = 0; i < args[2] - args[1] + 1; ++i)
      {
        for (int j = k + 1; j < n; ++j)
          y[i][j] -= y[i][k] * x[j];
        y[i][k] = 0;
      }
      dprintf("ID %d: sending back data from row %d to row %d\n", myid, args[1], args[2]);
      for (int i = 0; i < args[2] - args[1] + 1; ++i)
        MPI_Send(y[i], sizeof(x[0]) * n, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
    }
  }
}

void usage(int argc, char *argv[])
{
  printf("Usage\n\t%s <# of threads>\n", argv[0]);
  exit(-1);
}

int main(int argc, char *argv[])
{
  int numprocs;
  int myid;

  if (argc < 2) 
    usage(argc, argv);
  numprocs = atoi(argv[1]);
  dprintf("numprocs = %d, argv[1] = %s\n", numprocs, argv[1]);
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  
  if (myid == 0)
    main_proc(myid, numprocs);
  else
    worker_proc(myid, numprocs);

  MPI_Finalize();
  return 0;
}

