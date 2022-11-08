#/bin/sh

mpicc n_queens_gmp.c -o n_queens -lgmp && mpirun -host k8s-0,k8s-1,k8s-2,k8s-3,k8s-4,k8s-5,k8s-6,k8s-7 ./n_queens 