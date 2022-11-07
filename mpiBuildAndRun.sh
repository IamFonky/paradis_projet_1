#/bin/sh

mpicc n_queens_gmp.c -o n_queens -lgmp && mpirun ./n_queens 