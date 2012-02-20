#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;

void usage(int argc, char *argv[])
{
  printf("Usage\n\t%s <matrix size>\n", argv[0]);
  exit(-1);
}

int main(int argc, char *argv[])
{
  if (argc < 2) 
    usage(argc, argv);

  int n = atoi(argv[1]);

  printf("%d\n", n);  
  for (int i = 0; i < n; ++i) 
  {
    for (int j = 0; j < n; ++j) printf("%d ", rand() % 100);
    printf("\n");
  }
}
