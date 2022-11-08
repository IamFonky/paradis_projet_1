#ifndef N_QUEENS_H
#define N_QUEENS_H

#define NB_QUEENS 15
#define INT_SIZE  NB_QUEENS*NB_QUEENS

// VERBOSE DIRECTIVES
#define BUILD_SHOW_COLUMNS 0
#define BUILD_SHOW_ROWS 0
#define BUILD_SHOW_DIAGONALS_1 0
#define BUILD_SHOW_DIAGONALS_2 0
#define BUILD_SHOW_SHOTS 0

#define RUN_SHOW_SOLUTIONS 0
#define RUN_SHOW_STACK_EVOLUTION 0
#define RUN_SHOW_STACK 0
#define RUN_SHOW_MAX_STACK 0

#define USE_MPI 1
#define MPI_AVOID_DUMMY_TRANSACTIONS 1
#define MPI_MAX_ROW_FOR_TRANSACTION (NB_QUEENS - 10)
#define MPI_SHOW_DEBUG 0
#define MPI_SHOW_VISUAL_DEBUG 0
#define MPI_STATS 1
#define MPI_SLOW_PROCESS 0

#define MPI_ASK_TAG 69
#define MPI_ACK_TAG 96
#define MPI_BOARD_TAG 50
#define MPI_ROW_TAG 51
#define MPI_USEDCOL_TAG 52
#define MPI_SERIALIZED_BOARD_LENGTH 100
#define MPI_GMP_N_BIT_SERIALIZING 36


#define push(sp, n) (*((sp)++) = (n))
#define pop(sp) (*--(sp))

#endif // N_QUEENS_H
