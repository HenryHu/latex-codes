#!/bin/bash

echo "testing small testcases: 5 * 5"
for i in 1 2 3 4
do
  j=`expr $i + 1`
  echo "Running shared-variable approach with $i workers"
  /usr/bin/time -f %e ./b $i < small.in > sv_small.out
  echo "Running mpi-sync with $i workers"
  /usr/bin/time -f %e mpirun -np $j c $j < small.in > mpi_small.out
  echo "Running mpi-async with $i workers"
  #/usr/bin/time -f %e mpirun -np $j c $j < small.in > mpi_small.out
done

echo "testing large testcases: 1000 * 1000"
for i in 1 2 3 4
do
  j=`expr $i + 1`
  echo "Running shared-variable approach with $i workers"
  /usr/bin/time -f %e ./b $i < large.in > sv_large.out
  echo "Running mpi-sync with $i workers"
  /usr/bin/time -f %e mpirun -np $j c $j < large.in > mpi_large.out
  echo "Running mpi-async with $i workers"
  #/usr/bin/time -f %e mpirun -np $j c $j < large.in > mpi_large.out
done
