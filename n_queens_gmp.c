#include "n_queens.h"
#include <stdio.h>
#include <gmp.h>
#include <time.h>
#include <stdlib.h>

#if USE_MPI > 0
#include <unistd.h>
#include <mpi.h>
#endif

#if USE_MPI > 0
int myrank, numprocs, running;
#if MPI_STATS > 0
unsigned long long num_transactions = 0;
unsigned long long num_calculations = 0;
#endif
#endif

#if RUN_SHOW_MAX_STACK > 0
int stack_max = 0;
#endif

mpz_t diagonals1[NB_QUEENS][NB_QUEENS];
mpz_t diagonals2[NB_QUEENS][NB_QUEENS];
mpz_t columns[NB_QUEENS];
mpz_t rows[NB_QUEENS];

// merged columns, rows and diagonals in a single int
mpz_t shots[NB_QUEENS][NB_QUEENS];

void makeColumn(mpz_t column, short startPos)
{
    // set column to 0
    mpz_set_ui(column, 0);

    // init and set mask to 1
    mpz_t mask;
    mpz_init(mask);
    mpz_set_ui(mask, 1);

    // offset mask to start column
    mpz_mul_2exp(mask, mask, startPos);

    for (short i = 0; i < NB_QUEENS; ++i)
    {
        // add mask to colun
        mpz_add(column, column, mask);

        // left shift mask to next row
        mpz_mul_2exp(mask, mask, NB_QUEENS);
    }

    // clear mask
    mpz_clear(mask);
}

void makeRow(mpz_t row, short startPos)
{
    // set row to 0
    mpz_set_ui(row, 0);

    // init and set mask to 1
    mpz_t mask;
    mpz_init(mask);
    mpz_set_ui(mask, 1);

    // offset mask to start row
    mpz_mul_2exp(mask, mask, (startPos * NB_QUEENS));

    for (short i = 0; i < NB_QUEENS; ++i)
    {
        // add mask to row
        mpz_add(row, row, mask);

        // left shift mask to next column
        mpz_mul_2exp(mask, mask, 1);
    }

    // clear masks
    mpz_clear(mask);
}

void makeDiagonal1(mpz_t diagonal, short startPos)
{
    // set diagonal to 0
    mpz_set_ui(diagonal, 0);

    // init mask
    mpz_t mask;
    mpz_init(mask);

    for (short i = 0; i < NB_QUEENS; ++i)
    {
        // set mask to 1
        mpz_set_ui(mask, 1);
        // left shift mask to next column and row
        mpz_mul_2exp(mask, mask, (i * NB_QUEENS + i));
        // add mask to diagonal
        mpz_add(diagonal, diagonal, mask);
    }

    // init row mask
    mpz_t row_mask;
    mpz_init(row_mask);

    if (startPos >= 0)
    {
        // offset diagonal to startPos
        mpz_mul_2exp(diagonal, diagonal, startPos);

        for (short i = 0; i < startPos; i++)
        {
            // generate a row at pos
            makeRow(row_mask, (NB_QUEENS - 1) - i);

            // invert bits (complement)
            mpz_com(row_mask, row_mask);

            // remove row from the diagonal
            mpz_and(diagonal, diagonal, row_mask);
        }
    }
    else
    {
        // offset diagonal to startPos
        mpz_fdiv_q_2exp(diagonal, diagonal, -startPos);

        for (short i = 0; i < -startPos; i++)
        {
            // generate a row at pos
            makeRow(row_mask, i);

            // invert bits (complement)
            mpz_com(row_mask, row_mask);

            // remove row from the diagonal
            mpz_and(diagonal, diagonal, row_mask);
        }
    }

    // clear masks
    mpz_clear(mask);
    mpz_clear(row_mask);
}

void makeDiagonal2(mpz_t diagonal, short startPos)
{
    // set diagonal to 0
    mpz_set_ui(diagonal, 0);

    // init mask
    mpz_t mask;
    mpz_init(mask);

    for (short i = 0; i < NB_QUEENS; ++i)
    {
        // set mask to 1
        mpz_set_ui(mask, 1);
        // left shift mask to next column and row
        mpz_mul_2exp(mask, mask, (i * NB_QUEENS + (NB_QUEENS - 1 - i)));
        // add mask to diagonal
        mpz_add(diagonal, diagonal, mask);
    }

    // init column mask
    mpz_t column_mask;
    mpz_init(column_mask);

    if (startPos >= 0)
    {
        // offset diagonal to startPos
        mpz_mul_2exp(diagonal, diagonal, startPos);
        for (short i = 0; i < startPos; i++)
        {
            // generate a row at pos
            makeColumn(column_mask, i);

            // invert bits (complement)
            mpz_com(column_mask, column_mask);

            // remove row from the diagonal
            mpz_and(diagonal, diagonal, column_mask);
        }
    }
    else
    {
        // offset diagonal to startPos
        mpz_fdiv_q_2exp(diagonal, diagonal, -startPos);

        for (short i = 0; i < -startPos; i++)
        {
            // generate a row at pos
            makeColumn(column_mask, (NB_QUEENS - 1) - i);

            // invert bits (complement)
            mpz_com(column_mask, column_mask);

            // remove row from the diagonal
            mpz_and(diagonal, diagonal, column_mask);
        }
    }

    // clear masks
    mpz_clear(mask);
    mpz_clear(column_mask);
}

void showChessboard(mpz_t board)
{

    char *displayPatern = "%1d";

    if (NB_QUEENS >= 10)
    {
        displayPatern = "%2d";
    }

    // init and set mask to 1
    mpz_t mask;
    mpz_init(mask);
    mpz_set_ui(mask, 1);

    // init and set checker to 0
    mpz_t checker;
    mpz_init(checker);
    mpz_set_ui(checker, 0);

    printf(" ");
    for (int y = 0; y < (NB_QUEENS); ++y)
    {
        printf(" ");
        printf(displayPatern, y);
    }
    printf("\n");
    for (int y = 0; y < NB_QUEENS; ++y)
    {
        printf(displayPatern, y);
        for (int x = 0; x < NB_QUEENS; ++x)
        {
            printf(" ");

            // and compare mask and board
            mpz_and(checker, board, mask);

            printf(mpz_cmp_ui(checker, 0) ? "!" : ".");

            if (NB_QUEENS >= 10)
            {
                printf(" ");
            }

            // left shift mask to next column
            mpz_mul_2exp(mask, mask, 1);
        }
        printf(" ");
        printf(displayPatern, y);
        printf("\n");
    }
    printf(" ");
    for (int y = 0; y < (NB_QUEENS); ++y)
    {
        printf(" ");
        printf(displayPatern, y);
    }
    printf("\n");

    // clear values
    mpz_clear(mask);
    mpz_clear(checker);
}

/*
    This function will build all the possible lines and put them in arrays so it is
    easy for the user to get which lines goes at witch position
*/
void buildMasks()
{
    for (short row = 0; row < NB_QUEENS; ++row)
    {
        mpz_init(columns[row]);
        makeColumn(columns[row], row);

#if BUILD_SHOW_COLUMNS > 0
        printf("Column %d\n", row);
        showChessboard(columns[row]);
#endif

        mpz_init(rows[row]);
        makeRow(rows[row], row);

#if BUILD_SHOW_ROWS > 0
        printf("Row %d\n", row);
        showChessboard(rows[row]);
#endif

        for (short column = 0; column < NB_QUEENS; ++column)
        {
            mpz_init(diagonals1[row][column]);
            makeDiagonal1(diagonals1[row][column], column - row);

#if BUILD_SHOW_DIAGONALS_1 > 0
            printf("Diagonal1 - row : %d - col : %d\n", row, column);
            showChessboard(diagonals1[row][column]);
#endif

            mpz_init(diagonals2[row][column]);
            makeDiagonal2(diagonals2[row][column], column + row - NB_QUEENS + 1);

#if BUILD_SHOW_DIAGONALS_2 > 0
            printf("Diagonal2 - row : %d - col : %d\n", row, column);
            showChessboard(diagonals2[row][column]);
#endif
        }
    }

    // pretty redondant build but we dont care as the time to make it is insignificant
    for (short row = 0; row < NB_QUEENS; ++row)
    {
        for (short column = 0; column < NB_QUEENS; ++column)
        {
            mpz_init(shots[row][column]);
            mpz_set_ui(shots[row][column], 0);

            // compare line and board
            mpz_ior(shots[row][column], columns[column], rows[row]);
            mpz_ior(shots[row][column], shots[row][column], diagonals1[row][column]);
            mpz_ior(shots[row][column], shots[row][column], diagonals2[row][column]);

#if BUILD_SHOW_SHOTS > 0
            printf("All - row : %d - col : %d\n", row, column);
            showChessboard(shots[row][column]);
#endif
        }
    }
}

void clearMasks()
{
    for (short row = 0; row < NB_QUEENS; ++row)
    {
        mpz_clear(columns[row]);
        mpz_clear(rows[row]);
        for (short column = 0; column < NB_QUEENS; ++column)
        {
            mpz_clear(diagonals1[row][column]);
            mpz_clear(diagonals2[row][column]);
            mpz_clear(shots[row][column]);
        }
    }
}

short isQueenOnLine(mpz_t board, mpz_t line)
{
    // init and set checker to 0
    mpz_t checker;
    mpz_init(checker);
    mpz_set_ui(checker, 0);

    // compare line and board
    mpz_and(checker, board, line);
    short returnValue = mpz_cmp_ui(checker, 0);

    // clear checker
    mpz_clear(checker);
    return returnValue;
}

short isQueenInColumn(mpz_t board, int column)
{
    return isQueenOnLine(board, columns[column]);
}

short isQueenInRow(mpz_t board, int row)
{
    return isQueenOnLine(board, rows[row]);
}

short isQueenInDiagonal1(mpz_t board, int row, int column)
{
    return isQueenOnLine(board, diagonals1[row][column]);
}

short isQueenInDiagonal2(mpz_t board, int row, int column)
{
    return isQueenOnLine(board, diagonals2[row][column]);
}

short isQueenValid(mpz_t board, int row, int column)
{
    // init and set checker to 0
    mpz_t checker;
    mpz_init(checker);
    mpz_set_ui(checker, 0);

    // compare line and board
    mpz_and(checker, board, shots[row][column]);
    short returnValue = mpz_cmp_ui(checker, 0);

    // clear checker
    mpz_clear(checker);
    return !returnValue;
}

void addQueenAt(mpz_t board, int row, int column)
{
    mpz_t new_queen;
    mpz_init(new_queen);
    mpz_set_ui(new_queen, 1);

    // left shift queen to position
    mpz_mul_2exp(new_queen, new_queen, (column + row * NB_QUEENS));
    mpz_ior(board, board, new_queen);

    mpz_clear(new_queen);
}

long long unsigned nb_solutions = 0;

void checkAllQueensRec(mpz_t board, int row)
{
    if (row == NB_QUEENS)
    {
#if RUN_SHOW_SOLUTIONS > 0
        showChessboard(board);
#endif
        ++nb_solutions;
    }
    else
    {
        for (int col = 0; col < NB_QUEENS; ++col)
        {
            if (isQueenValid(board, row, col))
            {
                // create new board that is a copy of the board
                mpz_t test_board;
                mpz_init(test_board);
                mpz_set(test_board, board);

                // put the queen at position (row,col)
                addQueenAt(test_board, row, col);

                // recurse with the new board
                checkAllQueensRec(test_board, row + 1);
            }
        }
    }
}

void mpi_sender_routine(int *index_stack, mpz_t *boards_stack, int *rows_stack)
{
    int got_message = 0;
    int rcv_rank = -1;
    MPI_Message message;
    MPI_Status status;

    MPI_Improbe(MPI_ANY_SOURCE, MPI_ASK_TAG, MPI_COMM_WORLD, &got_message, &message, &status);
    if (got_message)
    {
        MPI_Mrecv(&rcv_rank, 1, MPI_INT, &message, &status);
#if MPI_SHOW_DEBUG > 0
        printf("Proc %d got query for proc %d\n", myrank, rcv_rank);
#endif
        if ((*index_stack) > 2)
        {
#if MPI_SHOW_DEBUG > 0
            printf("Proc %d send ack with stack %d\n", myrank, (*index_stack));
#endif
            int ack = 1;
            MPI_Send(&ack, 1, MPI_INT, rcv_rank, MPI_ACK_TAG, MPI_COMM_WORLD);

            char *serialized_board = malloc(MPI_SERIALIZED_BOARD_LENGTH);
            mpz_get_str(serialized_board, MPI_GMP_N_BIT_SERIALIZING, boards_stack[(*index_stack)]);
#if MPI_SHOW_DEBUG > 0
            printf("Proc %d send board %s\n", myrank, serialized_board);
#endif
            MPI_Send(serialized_board, MPI_SERIALIZED_BOARD_LENGTH, MPI_BYTE, rcv_rank, MPI_BOARD_TAG, MPI_COMM_WORLD);
#if MPI_SHOW_DEBUG > 0
            printf("Proc %d send row %d\n", myrank, rows_stack[(*index_stack)]);
#endif
            MPI_Send(&rows_stack[(*index_stack)], 1, MPI_INT, rcv_rank, MPI_ROW_TAG, MPI_COMM_WORLD);

            --(*index_stack);

            free(serialized_board);
        }
        else
        {
#if MPI_SHOW_DEBUG > 0
            printf("Proc %d send N-ack\n", myrank);
#endif
            int ack = 0;
            MPI_Send(&ack, 1, MPI_INT, rcv_rank, MPI_ACK_TAG, MPI_COMM_WORLD);
        }
    }
}

void mpi_receiver_initiated_routine(int *index_stack, mpz_t *boards_stack, int *rows_stack)
{
#if MPI_SHOW_DEBUG > 0
    printf("Proc %d asked stack to : 0\n", myrank);
#endif
    int rcv_ack = 0;
    MPI_Ssend(&myrank, 1, MPI_INT, 0, MPI_ASK_TAG, MPI_COMM_WORLD);
    MPI_Recv(&rcv_ack, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ACK_TAG, MPI_COMM_WORLD, NULL);
#if MPI_SHOW_DEBUG > 0
    printf("Proc %d recieved ack : %d\n", myrank, rcv_ack);
#endif
    if (rcv_ack == 1)
    {
        (*index_stack) = 0;
        char *serialized_board = malloc(MPI_SERIALIZED_BOARD_LENGTH);
        MPI_Recv(serialized_board, MPI_SERIALIZED_BOARD_LENGTH, MPI_BYTE, MPI_ANY_SOURCE, MPI_BOARD_TAG, MPI_COMM_WORLD, NULL);
#if MPI_SHOW_DEBUG > 0
        printf("Proc %d recieved board : %s\n", myrank, serialized_board);
#endif
        mpz_set_str(boards_stack[(*index_stack)], serialized_board, MPI_GMP_N_BIT_SERIALIZING);
#if MPI_SHOW_VISUAL_DEBUG > 0
        showChessboard(boards_stack[(*index_stack)]);
#endif

        int row;
        MPI_Recv(&row, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ROW_TAG, MPI_COMM_WORLD, NULL);
#if MPI_SHOW_DEBUG > 0
        printf("Proc %d recieved row : %d\n", myrank, row);
#endif
        rows_stack[(*index_stack)] = row;

        free(serialized_board);

#if MPI_STATS > 0
        num_transactions++;
#endif
    }
    else if (rcv_ack == -1)
    {
        running = 0;
    }
}

void mpi_managed_terminaison_routine()
{
    int got_message = 0;
    int rcv_rank = -1;
    MPI_Message message;
    MPI_Status status;

    short terminated_procs = 0;
    while (terminated_procs != numprocs - 1)
    {

        MPI_Improbe(MPI_ANY_SOURCE, MPI_ASK_TAG, MPI_COMM_WORLD, &got_message, &message, &status);

        if (got_message)
        {
            int rcv_rank = -1;
            MPI_Mrecv(&rcv_rank, 1, MPI_INT, &message, &status);
#if MPI_SHOW_DEBUG > 0
            printf("Proc %d got query for proc %d\n", myrank, rcv_rank);
            printf("Proc %d send termina ack (-1)\n", myrank);
#endif
            int ack = -1;
            MPI_Send(&ack, 1, MPI_INT, rcv_rank, MPI_ACK_TAG, MPI_COMM_WORLD);
            terminated_procs++;
        }
    }
}

void checkAllQueensIt()
{
    int index_stack = 0;
#if USE_MPI > 0
    short *terminated_procs = malloc(numprocs);

    if (myrank != 0)
    {
        // If not the first proc, start with no stack and wait 1 sec for main stack to grow
        index_stack = -1;
        sleep(1);
    }

#endif

    // Initializing and prealocating boards_stack stack
    mpz_t boards_stack[INT_SIZE];
    for (int i = 0; i < INT_SIZE; ++i)
    {
        mpz_init(boards_stack[i]);
        mpz_set_ui(boards_stack[i], 0);
    }
    mpz_t current_board;
    mpz_init(current_board);

    // Declaring row stack
    int rows_stack[INT_SIZE];
    rows_stack[0] = 0;

#if USE_MPI > 0
    while (running)
    {
#endif
        while (index_stack >= 0)
        {
#if USE_MPI > 0
#if MPI_SLOW_PROCESS > 0
            sleep(5);
#endif
            mpi_sender_routine(&index_stack, boards_stack, rows_stack);
#endif
#if RUN_SHOW_MAX_STACK > 0
            stack_max = (index_stack > stack_max) * index_stack + (index_stack <= stack_max) * stack_max;
#endif

            mpz_set(current_board, boards_stack[index_stack]);
            int old_stack = index_stack;
            int row = rows_stack[index_stack];
            --index_stack;
#if RUN_SHOW_STACK > 0
            printf("Stack : %d\n", index_stack);
#endif

            if (row == NB_QUEENS)
            {
#if RUN_SHOW_SOLUTIONS > 0
                showChessboard(current_board);
#endif

                ++nb_solutions;

#if MPI_STATS > 0
                ++num_calculations;
#endif
#if RUN_SHOW_SOLUTIONS_EVOLUTION > 0
                printf("Nb solutions : %llu\n", nb_solutions);
#endif
            }
            else
            {
                for (int col = 0; col < NB_QUEENS; ++col)
                {
                    if (isQueenValid(current_board, row, col))
                    {
                        ++index_stack;
#if RUN_SHOW_STACK > 0
                        printf("Stack : %d\n", index_stack);
#endif
#if RUN_SHOW_STACK_EVOLUTION > 0
                        if (index_stack > stack_max)
                        {
                            printf("New stack max : %d\n", index_stack);
                        }
#endif
                        mpz_set(boards_stack[index_stack], current_board);

                        addQueenAt(boards_stack[index_stack], row, col);

                        rows_stack[index_stack] = row + 1;
                    }
                }
#if MPI_STATS > 0
                ++num_calculations;
#endif
            }
        }
#if USE_MPI > 0
        if (myrank == 0)
        {
            mpi_managed_terminaison_routine();
            running = 0;
        }
        else
        {
            mpi_receiver_initiated_routine(&index_stack, boards_stack, rows_stack);
        }
    }
#endif

    // clearing boards_stack
    for (int i = 0; i < INT_SIZE; ++i)
    {
        mpz_clear(boards_stack[i]);
    }
    mpz_clear(current_board);

#if MPI_SHOW_DEBUG > 0
    printf("Proc %d terminated\n", myrank);
#endif
}

int main()
{

#if USE_MPI > 0
    double begin_build;
    double build_time;
    double max_build_time;
    double min_build_time;
    double avg_build_time;

    double total_time;
    double max_total_time;
    double min_total_time;
    double avg_total_time;

    unsigned long long max_solutions;
    unsigned long long min_solutions;
    unsigned long long sum_solutions;

#if MPI_STATS > 0
    unsigned long long total_transactions;
#endif

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Barrier(MPI_COMM_WORLD);
    running = 1;

    if (myrank == 0)
    {
        printf("%2d Queens problem with %2d procs\n", NB_QUEENS, numprocs);
        printf("=================================\n");
    }
#else
    clock_t begin_build;
    clock_t begin_queens;
    double time_spent;
    printf("%d queen problem\n", NB_QUEENS);
    printf("============================\n");
#endif

#if USE_MPI > 0
    begin_build = MPI_Wtime();
#else
    begin_build = clock()
#endif
    buildMasks(0b00000);
#if USE_MPI > 0
    build_time = MPI_Wtime() - begin_build;
#else
    time_spent = (double)(clock() - begin_build) / CLOCKS_PER_SEC;
    printf("Time to buid %g seconds\n", time_spent);
#endif

    checkAllQueensIt();
    clearMasks();

#if USE_MPI > 0
    total_time = MPI_Wtime() - begin_build;

    MPI_Reduce(&nb_solutions, &max_solutions, 1, MPI_UNSIGNED_LONG_LONG, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&nb_solutions, &min_solutions, 1, MPI_UNSIGNED_LONG_LONG, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&nb_solutions, &sum_solutions, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Reduce(&build_time, &max_build_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&build_time, &min_build_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&build_time, &avg_build_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Reduce(&total_time, &max_total_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&total_time, &min_total_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&total_time, &avg_total_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

#if MPI_STATS > 0
    MPI_Reduce(&num_transactions, &total_transactions, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
#endif

    if (myrank == 0)
    {
#if RUN_SHOW_MAX_STACK > 0
        printf("Biggest stack           : %d\n", stack_max);
#endif
#if MPI_STATS > 0
        unsigned long long nb_possibilities = 1;
        for (int i = NB_QUEENS; i > 1; --i)
        {
            nb_possibilities *= i;
        }
        printf("Number of possibilities : %llu\n", nb_possibilities);
        printf("Number of calulation    : %llu\n", num_calculations);
        printf("Number of transactions  : %llu\n", total_transactions);
        printf("Transactions ratio      : %g%%\n", (float)total_transactions * 100.f / (float)num_calculations);
#endif
        printf("Build timings : Min: %lf  Max: %lf  Avg:  %lf\n", min_build_time, max_build_time, avg_build_time / numprocs);
        printf("Run timings : Min: %lf  Max: %lf  Avg:  %lf\n", min_total_time, max_total_time, avg_total_time / numprocs);
        printf("Found %llu solutions (Min : %llu, Max : %llu)\n", sum_solutions, min_solutions, max_solutions);
    }

    MPI_Finalize();
#else
    time_spent = (double)(clock() - begin_build) / CLOCKS_PER_SEC;
    printf("Iterative : Found %llu solutions in %g seconds\n", nb_solutions, time_spent);
    printf("Biggest stack : %d\n", stack_max);
#endif
}
