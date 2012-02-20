#define TAG 0

//  global variables;
double **a;   //  matrix
int n;        //  matrix size;
MPI_Status stat;

void main_proc(int myid, int numprocs)
{
  //  TODO do something to obtain data

  int wp = numprocs - 1;  //  # of worker proc

  //  send out the number of iterations
  for (int i = 0; i < wp; ++i)
    MPI_Send(&n, sizeof(n), MPI_CHAR, i + 1, TAG, MPI_COMM_WORLD);
    
  for (int k = 0; k < n; ++k)
  {
    for (int i = k + 1; i < n; ++i) a[k][i] /= a[k][k];
    a[k][k] = 1;

    int p = min(wp, n - k - 1); //  max # of working proc
    int block_size = (n - k - 2) / p + 1;

    //  decomposition && assignment
    for (int i = 0; i < wp; ++i)
    {
      int args[4];
      args[0] = (void*)k;
      if (i < p)
      {
        args[1] = (void*)(k + 1 + i * block_size);
        args[2] = (void*) min(
          k + 1 + (i + 1) * block_size,
          n - 1);
      } else
        args[1] = args[2] = -1;
      args[3] = (void*)n;

      MPI_Send(args, sizeof(args), MPI_CHAR, i + 1, TAG, MPI_COMM_WORLD);
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
      args[0] = (void*)k;
      if (i < p)
      {
        args[1] = (void*)(k + 1 + i * block_size);
        args[2] = (void*) min(
          k + 1 + (i + 1) * block_size,
          n - 1);
      } else
        args[1] = args[2] = -1;
      args[3] = (void*)n;

      if (args[1] >= 0 && args[2] >= 0)
      {
        for (int j = args[1]; j <= args[2]; ++j)
          MPI_Recv(a[j], sizeof(a[k][0]) * n, MPI_CHAR, i + 1, TAG, 
            MPI_COMM_WORLD, &stat);
      }
    }
  }
}

void worker_proc(int myid, int numprocs)
{
  int ite;
  MPI_Recv(&ite, sizeof(ite), MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);

  while (ite--) {
    int args[4];
    MPI_Recv(args, sizeof(args), MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
    if (args[1] >= 0 && args[2] >= 0)
    {
      int k = args[0];
      int n = args[3];
      double []x = new double[n];
      double [][]y = new double[args[2] - args[1] + 1][n];
      MPI_Recv(x, sizeof(x[0]) * n, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
      for (int i = 0; i < args[2] - args[1] + 1; ++i)
        MPI_Recv(y[i], sizeof(x[0]) * n, MPI_CHAR, 0, TAG, 
          MPI_COMM_WORLD, &stat);
      for (int i = 0; i < args[2] - args[1] + 1; ++i)
      {
        for (int j = k + 1; j < n; ++j)
          a[i][j] -= a[i][k] * x[j];
        a[i][k] = 0;
      }
      for (int i = 0; i < args[2] - args[1] + 1; ++i)
        MPI_Send(y[i], sizeof(x[0]) * n, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
    }
  }
}

void main(int argc, char *argv[])
{
  int numprocs;
  int myid;
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
